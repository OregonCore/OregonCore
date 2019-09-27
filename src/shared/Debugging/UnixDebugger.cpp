/*
 * This file is part of the OregonCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#if PLATFROM == PLATFORM_UNIX

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "UnixDebugger.h"
#include "SystemConfig.h"
#include "Log.h"
#include "Console.h"

#include <sys/resource.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/ucontext.h>
#include <execinfo.h>
#include <fcntl.h>
#include <unistd.h>

#include <ace/Semaphore.h>
#include <fstream>
#include <iomanip>
#include <dlfcn.h>

#include <execinfo.h>
#include <bfd.h>

extern char** environ;

#define CRASH_DIR "Crashes"
#define BACKTRACE_SIZE 256

bool UnixDebugger::s_InstanceExists = false;

ACE_sema_t UnixDebugger::s_AllBacktracesSemaphore;
std::stringstream UnixDebugger::s_AllBacktraces;

std::set<ACE_thread_t> UnixDebugger::s_Threads;
ACE_Thread_Mutex UnixDebugger::s_Threads_Lock;

/// our global, ensures implicit instantiation
UnixDebugger g_UnixDebugger;

UnixDebugger::UnixDebugger()
{
    s_InstanceExists = true;

    // Register Handler for Deadly Signals
    struct sigaction sa;
    sa.sa_sigaction = SignalHandler;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;

    ACE_Thread::sigsetmask(SIG_UNBLOCK, &sa.sa_mask, NULL);

    // assuming this is (should be) main thread, add it to thread list here
    InsertThread(ACE_Based::Thread::currentId());

    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGBUS,  &sa, NULL);
    sigaction(SIGILL,  &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);
    sigaction(SIGFPE,  &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
}

UnixDebugger::~UnixDebugger()
{
    s_InstanceExists = false;
}

void UnixDebugger::SignalHandler(int num, siginfo_t* info, void* ctx)
{
    const char* strsig = NULL;
    std::stringstream reason("");
    bool segv = true;

    switch (num)
    {
    case SIGSEGV:
        strsig = "SIGSEGV";
        break;
    case SIGBUS:
        strsig = "SIGBUG";
        break;
    case SIGILL:
        strsig = "SIGILL";
        break;
    case SIGFPE:
        strsig = "SIGFPE";
        break;
    case SIGABRT:
        strsig = "SIGABRT";
        segv = false;
        break;
    case SIGUSR1:
        // requested backtrace
        WriteBacktrace(s_AllBacktraces);
        ACE_OS::sema_post(&s_AllBacktracesSemaphore);
        return;
    }

    if (segv)
    {
        reason << "Fault at "
               << std::hex << "0x"
               << (unsigned long) info->si_addr
               << " referenced from: 0x"
               #ifdef __x86_64__
               << std::hex << reinterpret_cast<struct sigcontext*> (&reinterpret_cast<ucontext_t*>(ctx)->uc_mcontext)->rip
               #else
               << std::hex << reinterpret_cast<struct sigcontext*> (&reinterpret_cast<ucontext_t*>(ctx)->uc_mcontext)->eip
               #endif
               << ": ";

        switch (num)
        {
        /* We can't use just switch(info->si_code), because
           there would be many duplicates and compiler would refuse to compile */
        case SIGSEGV:
            switch (info->si_code)
            {
            case SEGV_MAPERR:
                reason << "address not mapped to object";
                break;
            case SEGV_ACCERR:
                reason << "invalid permissions for mapped object";
                break;
            }
            break;
        case SIGBUS:
            switch (info->si_code)
            {
            case BUS_ADRALN:
                reason << "invalid address alignment";
                break;
            case BUS_ADRERR:
                reason << "nonconsistent physical address";
                break;
            case BUS_OBJERR:
                reason << "object-specific hardware error";
                break;
            }
            break;
        case SIGILL:
            switch (info->si_code)
            {
            case ILL_ILLOPC:
                reason << "illegal opcode";
                break;
            case ILL_ILLOPN:
                reason << "illegal operand";
                break;
            case ILL_ILLADR:
                reason << "illegal addressing mode";
                break;
            case ILL_ILLTRP:
                reason << "illegal trap";
                break;
            case ILL_PRVOPC:
                reason << "privileged opcode";
                break;
            case ILL_PRVREG:
                reason << "privileged register";
                break;
            case ILL_COPROC:
                reason << "coprocessor error";
                break;
            case ILL_BADSTK:
                reason << "internal stack error";
                break;
            }
            break;
        case SIGFPE:
            switch (info->si_code)
            {
            case FPE_INTDIV:
                reason << "integer divide b zero";
                break;
            case FPE_INTOVF:
                reason << "integer overflow";
                break;
            case FPE_FLTDIV:
                reason << "floating-point divide by zero";
                break;
            case FPE_FLTOVF:
                reason << "floating-point overflow";
                break;
            case FPE_FLTUND:
                reason << "floating-point underflow";
                break;
            case FPE_FLTRES:
                reason << "floating-point inexact result";
                break;
            case FPE_FLTINV:
                reason << "floating-point invalid operand";
                break;
            case FPE_FLTSUB:
                reason << "subscript out of range";
                break;
            }
            break;
        }
    }

    DumpDebugInfo(strsig, reason.str().c_str());

    struct sigaction sa;
    sa.sa_handler = SIG_DFL;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    sigaction(num, &sa, NULL);
    raise(num);
}

void UnixDebugger::WriteBacktraceForAllThreads(std::stringstream& ss)
{
    static ACE_Thread_Mutex s_AllBacktracesMutex;

    using ACE_Based::Thread;

    s_AllBacktracesMutex.acquire();

    ACE_OS::sema_init(&s_AllBacktracesSemaphore, 0);

    WriteBacktrace(s_AllBacktraces);

    s_Threads_Lock.acquire();
    for (std::set<ACE_thread_t>::iterator itr = s_Threads.begin(); itr != s_Threads.end(); ++itr)
    {
        if (!ACE_OS::thr_equal(*itr, Thread::currentId()))
        {
            if (!ACE_OS::thr_kill(*itr, SIGUSR1))
                ACE_OS::sema_wait(&s_AllBacktracesSemaphore);
            else
                ss << "\nCouldn't examine backtrace of a thread!\n" << std::endl;
        }
    }
    s_Threads_Lock.release();

    ACE_OS::sema_destroy(&s_AllBacktracesSemaphore);

    ss << s_AllBacktraces.str();
    s_AllBacktraces.str().clear();
    
    s_AllBacktracesMutex.release();
}

void UnixDebugger::WriteBacktrace(std::stringstream& ss)
{
    void* buffer[BACKTRACE_SIZE];
    int size = backtrace(buffer, BACKTRACE_SIZE);
    char** symbols = backtrace_symbols(buffer, size);

    std::string module, func, offset, addr;
    const char* demangled;

    ss << "BackTrace ";
    if (size == BACKTRACE_SIZE)
        ss << "(Might be truncated) ";
    ss << "Thread LWP " << syscall(SYS_gettid);
    
    ss << std::endl << "=====================================" << std::endl;

    std::map<std::string, Resolver*> atlm;
    std::map<std::string, Resolver*>::iterator atlmIt;

    int skip = 0;

    for (int i = 0; i < size; i++)
    {
        module = func = offset = addr = "";

        // ./module(function+0x15c) [0x8048a6d]
        for (char* p = symbols[i]; *p; ++p)
        {
            if (*p == '(')
                module.append(symbols[i] + 0, p - symbols[i]);
            else if (*p == '+' && module.size())
                func.append(symbols[i] + module.size() + 1, p - (symbols[i] + module.size() + 1));
            else if (*p == ')' && module.size())
                offset.append(symbols[i] + module.size() + func.size() + 1, p - (symbols[i] + module.size() + func.size() + 1));
            else if (*p == '[')
            {
                addr = p + 1;
                addr.resize(addr.size() - 1);
            }
        }

        unsigned long addrl = strtoul(addr.c_str(), NULL, 0);

        atlmIt = atlm.find(module);
        Resolver* atl;

        if (atlmIt == atlm.end())
        {
            atl = new Resolver(module.c_str());
            atlm[module] = atl;
        }
        else
            atl = atlmIt->second;

        bool resolved = atl->Resolve(addrl);

        if (func.size())
        {
            if ((demangled = atl->Demangle(func.c_str(), DMGL_PARAMS | DMGL_ANSI)))
                func = demangled;
            // else func is a C symbol and it needs not to be demangled
        }
        else
            func = atl->GetFunction();

        /* We don't need to print frames after SignalHandler */
        if (!skip)
        {
            //if (func == "__kernel_sigreturn")
            if ((void*) (addrl - strtoul(offset.c_str(), 0, 0)) == &SignalHandler)
                skip = i + 2;
            continue;
        }
        else if (i < skip)
            continue;

        /* ## 0x## in func +offset
             from module
                at file:line */

        ss << '#' << (i - skip) << ' ' << addr << " in " << func << ' ' << offset << std::endl
           << "  from " << module << std::endl;

        if (!resolved)
            continue;

        //const std::string& func = atl->GetFunction(); // not used
        const std::string& file = atl->GetFile();
        unsigned int line = atl->GetLine();

        ss << "    at " << file << ':' << line << std::endl;

        do
        {
            if (file == "??" || !line)
                break;

            std::string codeline;
            std::ifstream ifs(file.c_str());
            if (!ifs.is_open())
                break;

            struct stat status;
            if (0 == lstat(file.c_str(), &status))
                if (status.st_mtime > atl->GetModificationTime())
                    ss << "Source file is newer than the executable!\n";

            for (unsigned int j = 1; j < line - 1; j++)
                std::getline(ifs, codeline);

            if (ifs.eof())
                break;

            codeline.clear();

            for (unsigned int j = (line == 1) ? 1 : 0; j < 3; j++)
            {
                std::getline(ifs, codeline);
                if (ifs.eof())
                    break;
                ss << ((line - 1) + j) << ": " << codeline << std::endl;
                codeline.clear();
            }

            ifs.close();
        }
        while (0);

        ss << std::endl;
    }

    for (atlmIt = atlm.begin(); atlmIt != atlm.end(); atlmIt++)
        delete atlmIt->second;

    free(symbols);
}

void UnixDebugger::DumpDebugInfo(const char* sig, const char* reason)
{
    bool coredump = false;
    time_t now = time(0);

    // Check if we can (possibly) have a coredump file
    // if not try to change that
    struct rlimit rlim;
    if (!getrlimit(RLIMIT_CORE, &rlim))
    {
        if (rlim.rlim_cur > 0)
            coredump = true;
        else
        {
            rlim.rlim_cur = -1;
            rlim.rlim_max = -1;
            coredump = !setrlimit(RLIMIT_CORE, &rlim);
        }
    }

    char dateString[255];
    strftime(dateString, 255, "%d-%b-%y_%T", localtime(&now));

    std::stringstream ss;

    ss << "OregonCore Crash Report" << std::endl;
    ss << "=======================" << std::endl;
    ss << std::endl;

    struct utsname info;
    if (!uname(&info))
    {
        ss << info.sysname << " (" << info.release << ") ";
        ss << info.version << ' ' << info.machine << std::endl;;
    }

    ss << "Date: " << dateString << std::endl;
    ss << "Version: " << _FULLVERSION << std::endl;
    #if _BUILD_DIRECTIVE == Debug
    ss << "Build Type: Debug" << std::endl;
    #elif _BUILD_DIRECTIVE == Release
    ss << "Build Type: Release" << std::endl;
    #endif

    #if COMPILER == COMPILER_BORLAND
    ss << "Compiler: Borland" << std::endl;
    #elif COMPILER == COMPILER_GNU
    #ifdef __clang__
    ss << "Compiler: clang" << std::endl;
    #else
    ss << "Compiler: GNU (or compatible)" << std::endl;
    #endif
    #elif COMPILER == COMPILER_INTEL
    ss << "Compiler: Intel" << std::endl;
    #endif
    ss << "ACE version: " << ACE_VERSION << std::endl;

    ss << std::endl;

    struct sysinfo si;
    if (!sysinfo(&si))
    {
        ss << "Memory: " << (si.totalram - si.freeram) << " / " << si.totalram
           << " (" << ((float(si.totalram) - float(si.freeram))/float(si.totalram)) * 100.f << "% used)" << std::endl;
        ss << "SWAP:   " << (si.totalswap - si.freeswap) << " / " << si.totalswap
           << " (" << ((float(si.totalswap) - float(si.freeswap))/float(si.totalswap)) * 100.f << "% used)" << std::endl;
    }

    ss << std::endl;

    ss << "Coredump file MAY ";
    if (!coredump)
        ss << "NOT ";
    ss << "be available." << std::endl;

    if (sig)
        ss << "Caught deadly signal: " << sig << std::endl;
    if (*reason)
        ss << reason << std::endl;

    ss << std::endl;

    g_UnixDebugger.WriteBacktraceForAllThreads(ss);

    // We got all info needed now we just need to log it
    #ifdef linux
    char execdir[PATH_MAX] = {0};
    ssize_t size = readlink("/proc/self/exe", execdir, PATH_MAX - 1);
    execdir[size - (execdir + size - strrchr(execdir, '/'))] = '\0';
    chdir(execdir);
    printf("%s\n", execdir);
    #endif

    mkdir(CRASH_DIR, 0777);
    // now we try to chdir() to the newly created directory,
    // so coredump can also be here written if not disabled,
    // or set to absolute path
    chdir(CRASH_DIR);

    std::string filename = "./oregon-core_" _REVISION "_";
    filename.append(dateString);
    filename.append(".txt");
    if (FILE* fp = fopen(filename.c_str(), "w"))
    {
        // locking shouldn't be neccessary but one never knows...
        flock(fileno(fp), LOCK_EX);

        fprintf(fp, "%s", ss.str().c_str());
        fflush(fp);

        flock(fileno(fp), LOCK_UN);
        fclose(fp);
    }
    else
    {
        // Failed to create file... perhaps no write permissions
        // try to use our sLog.outError instead
        try
        {
            sLog.outError("Failed to create crash file: %s\n", strerror(errno));
            sLog.outError("%s", ss.str().c_str());
            return;
        }
        catch (...)
        {
            // sLog was already destroyed ("Dead Reference" exception)
        }
    }

    // safely end curses (restore terminal)
    sConsole.Restore();

    // We always use classic stdio instead of streams for output
    fprintf(stdout, "%s\n", ss.str().c_str());
    fflush(stdout);
}

void UnixDebugger::InsertThread(ACE_thread_t thread)
{
    if (!s_InstanceExists)
        return;

    s_Threads_Lock.acquire();
    s_Threads.insert(thread);
    s_Threads_Lock.release();
}

void UnixDebugger::RemoveThread(ACE_thread_t thread)
{
    if (!s_InstanceExists)
        return;
    
    s_Threads_Lock.acquire();
    s_Threads.erase(thread);
    s_Threads_Lock.release();
}

/* Following code is a C++ wrapper for code written by an unknown author and
   unknown license, we're very thankful for it. Hopefully no copyrights
   are violated */

bool UnixDebugger::Resolver::initialized = false;

UnixDebugger::Resolver::Resolver(const char* executable)
    :
    abfd(0), syms(0), text(0), function("??"), filename("??"), line(0), mtime(0)
{
    if (!initialized)
    {
        bfd_init();
        initialized = true;
    }

    abfd = bfd_openr(executable, 0);
    if (!abfd)
        return;

    #if linux
    struct stat st;
    if (0 == lstat("/proc/self/exe", &st))
        mtime = st.st_mtime;
    #endif

    /* oddly, this is required for it to work... */
    bfd_check_format(abfd, bfd_object);

    unsigned storage_needed = bfd_get_symtab_upper_bound(abfd);
    syms = (asymbol**) malloc(storage_needed);
    /*unsigned cSymbols = */bfd_canonicalize_symtab(abfd, syms);

    text = bfd_get_section_by_name(abfd, ".text");
}

UnixDebugger::Resolver::~Resolver()
{
    if (abfd)
        bfd_close(abfd);
    if (syms)
        free(syms);
}

bool UnixDebugger::Resolver::Resolve(unsigned long address)
{
    if (!abfd || !syms)
        return false;

    if (text->vma >= address)
        return false;

    bfd_vma offset = address - text->vma;

    const char* file;
    const char* func;

    bfd_find_nearest_line(abfd, text, syms, offset, &file, &func, &line);

    if (func)
    {
        if (const char* demangled = Demangle(func, DMGL_PARAMS | DMGL_ANSI))
            func = demangled;
        else
            function = func; // doesn't need demangling
    }
    else
        func = "??";
    filename = file && strlen(file) ? file : "??";
    // line has been already written

    return true;
}

const char* UnixDebugger::Resolver::Demangle(const char* mangled, int options)
{
    static char buff[255];
    char* demangled = bfd_demangle(abfd, mangled, options);

    if (demangled)
    {
        strcpy(buff, demangled);
        free (demangled);
        return buff;
    }
    
    return NULL;
}

#endif // PLATFORM_UNIX
