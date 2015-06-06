
#ifndef OREGON_CONSOLE_H
#define OREGON_CONSOLE_H

#include "Common.h"
#include "Policies/Singleton.h"
#include <curses.h>

typedef WINDOW Window;
class Master;
class UnixDebugger;

class Console
{
    public:
        Console();
        ~Console();

    protected:
        friend class World;
        friend class Master;
        friend class Log;
        friend class UnixDebugger;
        friend void LoadSQLUpdates();

        void Initialize();

        /* Start-up Loading Stuff */
        void SetLoading(bool show, const char* caption = "Initializing");
        void SetLoadingLabel(const char* label, bool br = true);
        void FatalError(const char* msg);

        /* Main Loops */
        void MainLoop();
        void RunCommandLoop();
        void RunLogViewLoop();

        void DrawLogo();
        bool IsEnabled() const
        {
            return m_IamEnabled;
        }

        void Beep()
        {
            beep();
        }

        class CliRunnable : public ACE_Based::Runnable
        {
            public:
                void run();
        };

        void Restore();

    private:

        void SetTitle(const char* title)
        {
            #if PLATFORM == PLATFORM_WINDOWS
            SetConsoleTitle("OregonCore");
            #elif PLATFORM == PLATFORM_UNIX
            printf("\033]0;%s\007", title);
            fflush(stdout);
            #endif
        }

        void Refresh();

        int GetChar();

        void DrawLoading();
        void UpdateLog();

        Window* MakeWindow(int h, int w, int y, int x);
        void ResizeWindow(Window* win, int h, int w, int y, int x);
        void DestroyWindow(Window* win);

        std::set<Window*> m_windows;
        bool m_IamEnabled;

        typedef std::list<std::string> History;
        #if PLATFORM == PLATFORM_WINDOWS
        typedef HANDLE PipeType;
        #else
        typedef int PipeType;
        #endif

        History m_cmdHistory;
        PipeType m_loggerFd;

        Window* m_logoWindow;
        Window* m_loadWindow;
        Window* m_logViewer;
        Window* m_cmdOutput;
};

#define sConsole Oregon::Singleton<Console>::Instance()

#endif
