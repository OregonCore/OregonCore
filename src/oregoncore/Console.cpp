
#include "Console.h"
#include "Log.h"
#include "World.h"
#include "TicketMgr.h"
#include "revision.h"

#ifdef __PDCURSES__
# undef A_PROTECT
# define A_PROTECT A_NORMAL
# define getmouse nc_getmouse
# define resizeterm resize_term
# define SCROLLUP_MASK BUTTON4_PRESSED
# define SCROLLDN_MASK BUTTON5_PRESSED
#else
# define SCROLLUP_MASK 0x00080000
# define SCROLLDN_MASK 0x08000080
#endif

#define TermColor(x) COLOR_PAIR(x+1)

INSTANTIATE_SINGLETON_1(Console);

static const char sOregonLogo[] =
{
    "MMMMMMMMMMM                                             "
    "MMP     dMM                                             "
    "M' .mmm. `M                                             "
    "M  MMMMM  M 88d888b. .d8888b. .d8888b. .d8888b. 88d888b."
    "M  MMMMM  M 88'  `88 88ooood8 88'  `88 88'  `88 88'  `88"
    "M. `MMM' .M 88       88.  ... 88.  .88 88.  .88 88    88"
    "MMb     dMM dP       `88888P' `8888P88 `88888P' dP    dP"
    "MMMMMMMMMMM                        .88                  "
    "                               d8888P"
};
static const unsigned int sOregonLogoRows = 9;
static const unsigned int sOregonLogoCols = 56;

static const char* Menu[] =
{
    "Run a Command",
    "Toggle Output",
    "-------------",
    "   Restart   ",
    " Halt Server ",
};
static const int MenuItems = sizeof(Menu) / sizeof(*Menu);

#if PLATFORM == PLATFORM_WINDOWS
BOOL WINAPI HandleConsoleInterrupt(DWORD)
{
    endwin();
    raise(SIGINT);
    return TRUE;
}
#endif

Console::Console() : m_IamEnabled(false), m_loggerFd(0), m_logoWindow(0), m_loadWindow(0)
{
    SetTitle("OregonCore");
    #if PLATFORM == PLATFORM_WINDOWS
    SetConsoleCtrlHandler(&HandleConsoleInterrupt, TRUE);
    #endif
}

Console::~Console()
{
    if (!m_IamEnabled)
        return;

    for (std::set<Window*>::const_iterator it = m_windows.begin(); it != m_windows.end(); )
    {
        DestroyWindow(*it);
        it = m_windows.begin();
    }

    endwin();

    if (m_loggerFd)
    #if PLATFORM == PLATFORM_WINDOWS
        CloseHandle(m_loggerFd);
    #else
        close(m_loggerFd);
    #endif
}

void Console::Initialize()
{
    m_IamEnabled = true;

    initscr();              // init curses
    cbreak();               // turn off line buffering
    noecho();               // turn off echoing of chars by terminal
    keypad(stdscr, TRUE);   // enable use of F1-F12 keys, arrows, etc.
    curs_set(0);            // try to make the cursor invisible
    idlok(stdscr, TRUE);    // allow terminal controls line ins/del
    idcok(stdscr, TRUE);    // allow terminal controls char ins/del
    scrollok(stdscr, TRUE); // allow scrolling
    leaveok(stdscr, TRUE);  // dont move cursor (performance)
    nonl();                 // disable new line feed translating
    start_color();          // initialize colors

    use_default_colors();
    if (has_colors())
        for (int i = 1; i <= COLORS; i++)
            init_pair(i + 1, i, -1);

    mousemask(ALL_MOUSE_EVENTS & ~REPORT_MOUSE_POSITION, NULL); // allow mouse interaction
    mouseinterval(0);

    int width = sConfig.GetIntDefault("Console.Width", 0);
    int height = sConfig.GetIntDefault("Console.Height", 0);

    if (width > 0 && height > 0)
        resizeterm(height, width);

    if (!m_logoWindow)
    {
        // win                       height         width                          y                              x
        m_logoWindow = MakeWindow(sOregonLogoRows, sOregonLogoCols, 1,                               COLS / 2 - sOregonLogoCols / 2);
        m_loadWindow = MakeWindow(3,               sOregonLogoCols, 2 + sOregonLogoRows,             COLS / 2 - sOregonLogoCols / 2);

        int rows = 0xFFFF / COLS;
        m_logViewer = newpad(rows, COLS);
        idlok(m_logViewer, TRUE);
        idcok(m_logViewer, TRUE);
        scrollok(m_logViewer, TRUE);
        wmove(m_logViewer, rows - 1, COLS - 1);
        wprintw(m_logViewer, " ");

        m_cmdOutput = dupwin(m_logViewer);
    }

    Refresh();

    // we abuse stderr to bring us data from logger
    #if PLATFORM == PLATFORM_WINDOWS
    HANDLE writer, reader;
    char pipeName[MAX_PATH];
    sprintf(pipeName, "\\\\.\\Pipe\\LocalOCAnon.%u", GetCurrentProcessId());
    SetLastError(0);
    reader = CreateNamedPipeA(pipeName, PIPE_ACCESS_INBOUND, PIPE_TYPE_BYTE, 1, 4096, 0xFFFF, 1000, NULL);
    writer = CreateFileA(pipeName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH, NULL);

    if (GetLastError())
    {
        if (!reader || reader == INVALID_HANDLE_VALUE)
            CloseHandle(reader);
        if (!writer || writer == INVALID_HANDLE_VALUE)
            CloseHandle(writer);
        fclose(stderr);
        return;
    }

    SetStdHandle(STD_ERROR_HANDLE, writer);
    //stderr->_file = _open_osfhandle((long) writer, _O_TEXT);
    dup2(_open_osfhandle((long) writer, 0), fileno(stderr));
    m_loggerFd = reader;
    #else
    int fds[2];
    if (!pipe(fds))
    {
        // 0 - reader, 1 - writer
        if (-1 == dup2(fds[1], STDERR_FILENO))
            fclose(stderr);
        else
        {
            fcntl(fds[0], F_SETFL, fcntl(fds[0], F_GETFL) | O_NONBLOCK | O_CLOEXEC);
            fcntl(fds[1], F_SETFL, fcntl(fds[1], F_GETFL) | O_NONBLOCK | O_CLOEXEC);
            m_loggerFd = fds[0];

            setvbuf(stderr, NULL, _IOLBF, 0);
        }
    }
    else
        fclose(stderr);
    #endif
}

void Console::Refresh()
{
    wrefresh(stdscr);

    for (std::set<Window*>::const_iterator it = m_windows.begin(); it != m_windows.end(); it++)
        wrefresh(*it);
}

void Console::Restore()
{
    endwin();
    m_IamEnabled = false;
}

Window* Console::MakeWindow(int h, int w, int y, int x)
{
    Window* win = newwin(h, w, y, x);
    leaveok(win, TRUE);
    m_windows.insert(win);
    return win;
}

void Console::DestroyWindow(Window* win)
{
    if (!win)
        return;

    werase(win);   // make it blank
    wrefresh(win); // apply
    m_windows.erase(win);
    delwin(win);
}

void Console::ResizeWindow(Window* win, int h, int w, int y, int x)
{
    mvwin(win, y, x);
    wresize(win, h, w);
}

int Console::GetChar()
{
    int ch = getch();
    if (ch == KEY_MOUSE)
    {
        MEVENT event;
        if (getmouse(&event) == OK)
        {
            if (event.bstate & SCROLLUP_MASK)
                ch = KEY_SR; // scroll up
            else if (event.bstate & SCROLLDN_MASK)
                ch = KEY_SF; // scroll down
        }
    }
    return ch;
}

void Console::CliRunnable::run()
{
    if (sConfig.GetBoolDefault("BeepAtStart", true))
        sConsole.Beep();

    sConsole.MainLoop();
}

void Console::MainLoop()
{
    DestroyWindow(m_loadWindow);

    int MenuActiveItem = 0;
    time_t startTime = time(0); // we use our own timer due to thread-safety

    //                                height            width                    y                         x
    Window* menuWindow = MakeWindow(MenuItems + 2,     19,              1 + sOregonLogoRows + 1,         COLS / 2 - 10 - 2);
    Window* infoWindow = MakeWindow(4,               sOregonLogoCols, 5 + sOregonLogoRows + MenuItems, COLS / 2 - sOregonLogoCols / 2);

    flushinp();

    wattron(menuWindow, A_BOLD | TermColor(COLOR_CYAN));
    wborder(menuWindow, '|', '|', '-', '-', '+', '+', '+', '+');
    wattroff(menuWindow, A_BOLD | TermColor(COLOR_CYAN));

    DrawLogo();

    while (!World::IsStopped())
    {
        // draw menu
        for (int i = 0; i < MenuItems; i++)
        {
            if (MenuActiveItem == i)
            {
                wattron(menuWindow, A_BOLD | TermColor(COLOR_GREEN));
                mvwprintw(menuWindow, i + 1, 1, "* %s *", Menu[i]);
                wattroff(menuWindow, A_BOLD | TermColor(COLOR_GREEN));
            }
            else if (Menu[i][0] == '-')
                mvwprintw(menuWindow, i + 1, 1, "--%s--", Menu[i]);
            else
                mvwprintw(menuWindow, i + 1, 1, "  %s  ", Menu[i]);
        }

        // draw info
        enum InfoValue
        {
            INFO_EMPTY,

            INFO_PLAYERS_ONLINE,
            INFO_PLAYERS_QUEUED,
            INFO_PLAYERS_MAX,

            INFO_REVISION,
            INFO_LOGMASK,
            INFO_UPTIME,
            INFO_TICKETS
        };

        struct
        {
            int y;
            int x;
            const char* type;
            InfoValue value;
        } InfoTable[]
        =
        {
            { 0,    0,    "Players:",    INFO_PLAYERS_ONLINE },
            { 1,    0,    "        ",    INFO_PLAYERS_QUEUED },
            { 2,    0,    "        ",    INFO_PLAYERS_MAX,   },
            { 3,    0,    "        ",    INFO_EMPTY,         },

            { 0,   34,    "Revision:",   INFO_REVISION,      },
            { 1,   34,    "LogMask:",    INFO_LOGMASK,      },
            { 2,   34,    "Uptime:  ",   INFO_UPTIME,        },
            { 3,   34,    "Tickets: ",   INFO_TICKETS,       }
        };

        for (uint32 i = 0; i < sizeof(InfoTable) / sizeof(*InfoTable); i++)
        {
            wmove(infoWindow, InfoTable[i].y, InfoTable[i].x);
            wattron(infoWindow, A_BOLD | TermColor(COLOR_GREEN));
            wprintw(infoWindow, "%s", InfoTable[i].type);
            wattroff(infoWindow, A_BOLD | TermColor(COLOR_GREEN));
            switch (InfoTable[i].value)
            {
            case INFO_PLAYERS_ONLINE:
                wprintw(infoWindow, " %u (online)", sWorld.GetActiveSessionCount());
                break;
            case INFO_PLAYERS_QUEUED:
                wprintw(infoWindow, " %u (queued)", sWorld.GetQueuedSessionCount());
                break;
            case INFO_PLAYERS_MAX:
                wprintw(infoWindow, " %u (max)", sWorld.GetMaxActiveSessionCount() + sWorld.GetMaxQueuedSessionCount());
                break;
            case INFO_REVISION:
                wprintw(infoWindow, " %s", _REVISION);
                break;
            case INFO_LOGMASK:
                wprintw(infoWindow, " %lu", sLog.GetLogMask());
                break;
            case INFO_UPTIME:
                {
                    time_t diff = time(0) - startTime;
                    uint32 mins = diff / 60;
                    uint32 hours = mins / 60;
                    uint32 days = hours / 24;
                    wprintw(infoWindow, " %ud %uh %um", days, hours % 24, mins % 60);
                }
                break;
            case INFO_TICKETS:
                wprintw(infoWindow, " %lu", ticketmgr.GM_TicketList.size());
            default:
                break;
            }
        }

        wrefresh(m_logoWindow);
        wrefresh(menuWindow);
        wrefresh(infoWindow);

        UpdateLog();

        // align timeout to the next minute
        timeout((60 - ((time(0) - startTime) % 60)) * 1000);
        int ch = GetChar();

        switch (ch)
        {
        case KEY_UP:
            MenuActiveItem = (MenuActiveItem == 0) ? MenuItems - 1 : (MenuActiveItem - 1);
            if (Menu[MenuActiveItem][0] == '-')
                MenuActiveItem--;
            break;
        case KEY_DOWN:
            MenuActiveItem = (MenuActiveItem >= MenuItems - 1) ? 0 : (MenuActiveItem + 1);
            if (Menu[MenuActiveItem][0] == '-')
                MenuActiveItem++;
            break;
        case KEY_ENTER:
        case '\n':
        case '\r':

            switch (MenuActiveItem)
            {
            case 0: /* Run a Command */
                RunCommandLoop();
                break;
            case 1: /* Real-time logs */
                RunLogViewLoop();
                break;
            case 3:  /* Restart Server*/
                sWorld.ShutdownServ(sWorld.getConfig(CONFIG_CONSOLE_RESTART_TIMER), SHUTDOWN_MASK_RESTART, RESTART_EXIT_CODE);
                RunLogViewLoop(); // Need to view logs for it to correctly restart
                break;
            case 4: /* shutdown */
                endwin();
                raise(SIGINT);
                break;
            }

        /* Fallthrough */
        case KEY_RESIZE:

            erase();

            //           win             height         width                 y                            x
            ResizeWindow(m_logoWindow, sOregonLogoRows, sOregonLogoCols, 1,                       COLS / 2 - sOregonLogoCols / 2);
            ResizeWindow(menuWindow, MenuItems + 2,     19,              1 + sOregonLogoRows + 1, std::max<int>(0, COLS / 2 - 10 - 2));
            ResizeWindow(infoWindow, 4,               sOregonLogoCols, 5 + sOregonLogoRows + MenuItems, COLS / 2 - sOregonLogoCols / 2);

            wattron(menuWindow, A_BOLD | TermColor(COLOR_CYAN));
            wborder(menuWindow, '|', '|', '-', '-', '+', '+', '+', '+');
            wattroff(menuWindow, A_BOLD | TermColor(COLOR_CYAN));

            Refresh();
            DrawLogo();
            break;
        default:
            break;;
        }
    }

    DestroyWindow(menuWindow);
    DestroyWindow(infoWindow);
}

enum
{
    CMD_FINISHED = (1 << 0),
    CMD_SUCCESS  = (1 << 1)
};

struct CBArg
{
    Window* win;
    volatile long status;
    bool colored;
};

static void PrintCommandResultCallback(void* data, const char* msg)
{
    Window* output = reinterpret_cast<CBArg*>(data)->win;
    if (reinterpret_cast<CBArg*>(data)->colored)
        wattrset(output, A_PROTECT | A_BOLD | TermColor(COLOR_WHITE));
    else
        wattrset(output, A_PROTECT);
    wprintw(output, "%s", msg);
    wattrset(output, A_NORMAL);
}

static void CommandFinishedCallback(void* data, bool success)
{
    // we set first bit to true - command finished
    // we set second bit to result (success)
    // we try to do this atomically ('long' should be of native size)

    if (success)
        reinterpret_cast<CBArg*>(data)->status = CMD_FINISHED | CMD_SUCCESS;
    else
        reinterpret_cast<CBArg*>(data)->status = CMD_FINISHED;
}

void Console::RunCommandLoop()
{
    History::iterator historyCur = m_cmdHistory.begin();
    if (m_cmdHistory.size())
        std::advance(historyCur, m_cmdHistory.size() - 1);

    int no = 1;

    int lineWidth = COLS - (sizeof("Oregon>") - 1);
    Window* cmdLine = MakeWindow(1, lineWidth, LINES - 1, (sizeof("Oregon>") - 1));

    erase();
    curs_set(1);

    attron(A_BOLD | TermColor(COLOR_GREEN));
    mvprintw(LINES - 1, 0, "Oregon> ");
    attroff(A_BOLD | TermColor(COLOR_GREEN));
    wattrset(cmdLine, A_PROTECT | TermColor(COLOR_WHITE));

    int y, mx, my;
    int pageSize = LINES - 1;
    getmaxyx(m_cmdOutput, my, mx);
    y = my - 1 - pageSize;

    leaveok(cmdLine, FALSE);
    wmove(cmdLine, 0, 0);

    pnoutrefresh(m_cmdOutput, y, 0, 0, 0, LINES - 2, COLS);
    wrefresh(stdscr);
    wrefresh(cmdLine);

    bool update = true;

    std::string buffer = "";

    while (!World::IsStopped())
    {
        if (update)
        {
            pnoutrefresh(m_cmdOutput, y, 0, 0, 0, LINES - 2, COLS);

            attron(A_BOLD | TermColor(COLOR_GREEN));
            mvprintw(LINES - 1, 0, "Oregon> ");
            attroff(A_BOLD | TermColor(COLOR_GREEN));

            wrefresh(stdscr);

            update = false;
        }

        werase(cmdLine);
        wrefresh(cmdLine);
        mvwprintw(cmdLine, 0, 0, "%s", buffer.c_str() + std::max<int>(0, (buffer.size() - (lineWidth - 1))));
        wmove(cmdLine, 0, std::min<size_t>(lineWidth - 1, buffer.size()));
        wrefresh(cmdLine);

        int ch = GetChar();
        if (ch >= 0x20 && ch < 0x7F)
        {
            if (buffer.empty() && ch != '.')
                buffer.append(1, '.');
            buffer.append(1, ch);
        }
        else if (ch == KEY_UP)
        {
            historyCur--;
            if (historyCur != m_cmdHistory.end())
                buffer = *historyCur;
            else
                buffer.clear();
        }
        else if (ch == KEY_DOWN)
        {
            historyCur++;
            if (historyCur != m_cmdHistory.end())
                buffer = *historyCur;
            else
                buffer.clear();
        }
        else if (ch == KEY_BACKSPACE || ch == 0x7F || ch == 0x08)
        {
            if (buffer.size())
                buffer.resize(buffer.size() - 1);
        }
        else if (ch == KEY_PPAGE) // page up
        {
            update = true;
            y = std::max<int>(0, y - pageSize);
        }
        else if (ch == KEY_NPAGE) // page down
        {
            update = true;
            y = std::min<int>(my - 1 - pageSize, y + pageSize);
        }
        else if (ch == KEY_SR) // scroll up
        {
            update = true;
            y = std::max<int>(0, y - 1);
        }
        else if (ch == KEY_SF) // scroll  down
        {
            update = true;
            y = std::min<int>(my - 1 - pageSize, y + 1);
        }
        else if ((ch == KEY_ENTER || ch == '\n' || ch == '\r'))
        {
            if (buffer.empty() || buffer == ".exit" || buffer == ".quit")
                break;

            curs_set(0);

            CBArg result;
            result.win = m_cmdOutput;
            result.status = 0;
            result.colored = (no ^= 1);

            sWorld.QueueCliCommand(new CliCommandHolder(&result, buffer.c_str(), &PrintCommandResultCallback, &CommandFinishedCallback));

            /* command is issued we now block until command is executed, to prevent crash in PrintCommandResultCallback:
               when server shuts down and we destroy output window before last World::Update() which calls PrintCmdResCB;
               also this ensures thread-safety between curses calls */
            do
                ACE_Based::Thread::Sleep(50);
            while (!(result.status & CMD_FINISHED) && !World::IsStopped());

            #if OREGON_DEBUG
            if (!(result.status & CMD_SUCCESS))
            {
                wattron(m_cmdOutput, A_BOLD | TermColor(COLOR_RED));
                wprintw(m_cmdOutput, "Command failed :(\n");
                wattroff(m_cmdOutput, A_BOLD | TermColor(COLOR_RED));
            }
            #endif

            update = true;

            m_cmdHistory.push_back(buffer);
            historyCur = m_cmdHistory.begin();
            std::advance(historyCur, m_cmdHistory.size() - 1);

            buffer.clear();
            curs_set(1);
        }
        else if (ch == KEY_RESIZE)
        {
            lineWidth = COLS - (sizeof("Oregon>") - 1);

            wresize(m_cmdOutput, 0xFFFF / COLS, COLS);

            ResizeWindow(cmdLine, 1, lineWidth, LINES - 1, (sizeof("Oregon>") - 1));
            wmove(m_cmdOutput, LINES - 2, COLS - 1);

            werase(stdscr);

            pageSize = LINES - 4;
            getmaxyx(m_cmdOutput, my, mx);
            wmove(m_cmdOutput, my - 1, mx - 1);
            waddch(m_cmdOutput, ' ');

            y = my - pageSize;

            update = true;
        }

    }

    curs_set(0);
    DestroyWindow(cmdLine);
}

void Console::RunLogViewLoop()
{
    const char info[] = "Use arrows, PgUp/PgDn to scroll. Enter to return. (%lu%%)";
    size_t len = sizeof(info) - 1;

    int y, mx, my;
    int pageSize = LINES - 4;
    getmaxyx(m_logViewer, my, mx);
    my--;

    y = my - pageSize;

    Window* infoWindow = MakeWindow(2, len, LINES - 2, COLS / 2 - len / 2);
    wattron(infoWindow, A_BOLD | TermColor(COLOR_GREEN));

    erase();
    refresh();

    wrefresh(infoWindow);
    timeout(1000);

    while (!World::IsStopped())
    {
        if (y == my - pageSize)
        {
            UpdateLog();
            prefresh(m_logViewer, y, 0, 0, 0, pageSize + 1, COLS);
        }
        else
            prefresh(m_logViewer, y, 0, 0, 0, pageSize, COLS);

        werase(infoWindow);
        mvwprintw(infoWindow, 0, 0, info, (100 * y) / (my - pageSize));
        wrefresh(infoWindow);

        int ch = GetChar();

        if (ch == KEY_ENTER || ch == '\r' || ch == '\n')
            break;
        else if (ch == KEY_PPAGE) // page up
            y = std::max<int>(0, y - pageSize);
        else if (ch == KEY_NPAGE) // page down
            y = std::min<int>(my - pageSize, y + pageSize);
        else if (ch == KEY_UP || ch == KEY_SR)
            y = std::max<int>(0, y - 1);
        else if (ch == KEY_DOWN || ch == KEY_SF)
            y = std::min<int>(my - pageSize, y + 1);
        else if (ch == KEY_RESIZE)
        {
            ResizeWindow(infoWindow, 2, len, LINES - 2, COLS / 2 - len / 2);

            wresize(m_logViewer, 0xFFFF / COLS, COLS);

            pageSize = LINES - 4;
            getmaxyx(m_logViewer, my, mx);
            wmove(m_logViewer, my - 1, mx - 1);
            waddch(m_logViewer, ' ');

            y = my - pageSize;
        }
    }

    DestroyWindow(infoWindow);
}

void Console::SetLoading(bool on, const char* caption)
{
    if (!m_loadWindow || !m_IamEnabled)
        return;

    if (on)
    {
        const char stop[] = "Press <CTRL-C> to stop";

        wattron(m_loadWindow, A_BOLD);

        wprintw(m_loadWindow, "%s ", caption);
        mvwprintw(m_loadWindow, 0, sOregonLogoCols - (sizeof(stop) - 1), "%s", stop);

        wattroff(m_loadWindow, A_BOLD);
        wrefresh(m_loadWindow);
    }
    else
    {
        DestroyWindow(m_loadWindow);
        m_loadWindow = NULL;
    }
}

void Console::SetLoadingLabel(const char* label, bool br)
{
    if (m_IamEnabled)
    {
        if (!m_loadWindow)
            return;

        wmove(m_loadWindow, 2, 0);
        wclrtoeol(m_loadWindow);
        mvwprintw(m_loadWindow, 2, 0, "%s", label);
        wrefresh(m_loadWindow);
    }
    else
    {
        if (br)
            sLog.outString();
        sLog.outString("%s", label);
    }
}

void Console::FatalError(const char* msg)
{
    ASSERT(m_IamEnabled);

    erase();

    Window* errWin = MakeWindow(LINES - (2 + sOregonLogoRows), sOregonLogoCols, 2 + sOregonLogoRows, COLS / 2 - sOregonLogoCols / 2);
    wattrset(errWin, A_BOLD | TermColor(COLOR_RED));
    wprintw(errWin, "FATAL ERROR:"
            "\n\n"
            "%s"
            "\n\n"
            "Press enter to exit", msg);
    wrefresh(stdscr);
    DrawLogo();
    wrefresh(errWin);

    timeout(-1);
    for (int c = GetChar(); (c != KEY_ENTER && c != '\n' && c != '\r'); c = GetChar())
        ;

    DestroyWindow(errWin);

    endwin();
}

void Console::DrawLogo()
{
    static const char FavString[] = "Your Favourite TBC Server";

    if (m_IamEnabled)
    {
        werase(m_logoWindow);
        scrollok(m_logoWindow, TRUE);
        wattrset(m_logoWindow, A_BOLD | TermColor(COLOR_GREEN));
        mvwprintw(m_logoWindow, 0, 0, "%s", sOregonLogo);
        wattroff(m_logoWindow, TermColor(COLOR_GREEN));
        wattron(m_logoWindow, TermColor(COLOR_CYAN));

        mvwprintw(m_logoWindow, 1, sOregonLogoCols / 2 - (sizeof(FavString) - 1) / 2, "%s", FavString);
        mvwprintw(m_logoWindow, sOregonLogoRows - 1, 0, "%s", "https://www.oregon-core.net");

        wattroff(m_logoWindow, TermColor(COLOR_CYAN));
        wrefresh(m_logoWindow);
    }
    else
    {
        sLog.outString();
        for (uint32 i = 0; i < sOregonLogoRows; i++)
            sLog.outString("%.*s", sOregonLogoCols, &sOregonLogo[i * sOregonLogoCols]);
        sLog.outString("https://www.oregon-core.net\n");
    }
}

void Console::UpdateLog()
{
    /* Before you start optimizing or "repairing" this functions bear in mind:
       * we read from pipe (we try to read atomic chunks)
       * read() calls are used in non-blocking I/O
       * ReadFile() calls are used with PeekNamedPipe() to achieve non-blocking I/O
       * read()/ReadFile() don't guarantee to read exactly *size* bytes
       * we call this function to *free* bytes so logger can send another data

       So to properly handle colors we use our own special escapes:
       * to apply a color we send 0xFF byte following with the color value
       * to unapply color we send 0xFE byte
       * 0xFF and 0xFE are chosen because they are invalid for utf8
       * one call to this function may read 0xFF byte but not the actual color
         so we use *static* control to check if the previous byte was 0xFF

       I admit this function is terrible for performance, although:
       * it's run from separate thread (cliThread)
       * user probably won't notice a delay
    */

    static long control = 0;
    unsigned char buffer[PIPE_BUF];

    #if PLATFORM == PLATFORM_WINDOWS
    unsigned long len, i, total;
    while (PeekNamedPipe(m_loggerFd, NULL, 0, NULL, &total, NULL) &&
           total &&
           ReadFile(m_loggerFd, buffer, std::min<unsigned long>(total, sizeof(buffer)), &len, NULL))
    #else
    ssize_t len, i;
    while ((len = read(m_loggerFd, (char*)buffer, sizeof(buffer))) > 0)
    #endif
    {
        for (i = 0; i < len; i++)
        {
            switch (buffer[i])
            {
            case 0xFF:
                control = 1;
                continue;
            case 0xFE:
                wattrset(m_logViewer, A_NORMAL);
                continue;
            default:
                if (control)
                {
                    control = 0;
                    if (buffer[i] > 7)
                        wattrset(m_logViewer, A_BOLD | TermColor(buffer[i] - 8));
                    else
                        wattrset(m_logViewer, TermColor(buffer[i]));
                    continue;
                }

                waddch(m_logViewer, buffer[i]);
            }
        }
    }
}
