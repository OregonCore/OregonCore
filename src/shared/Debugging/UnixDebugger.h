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

#if PLATFORM == PLATFORM_UNIX
#ifndef _UNIX_DEBUGGER_H_
#define _UNIX_DEBUGGER_H_

#include "Common.h"
#include "Policies/Singleton.h"

// bfd.h requires PACKAGE to be defined
#ifndef PACKAGE
#define PACKAGE
#endif

#include <bfd.h>

/* Options passed to Resolver::Demangle (in 3rd parameter). */
#ifndef DMGL_PARAMS
#define DMGL_NO_OPTS     0
#define DMGL_PARAMS      (1 << 0)
#define DMGL_ANSI        (1 << 1)
#define DMGL_JAVA        (1 << 2)
#define DMGL_VERBOSE     (1 << 3)
#define DMGL_TYPES       (1 << 4)
#define DMGL_RET_POSTFIX (1 << 5)
#define DMGL_RET_DROP    (1 << 6)
#endif

class UnixDebugger
{
    public:

    /// don't intantiate this ever, it has public visiblity in order
    /// to be able to instantiate implicitly when program starts
    UnixDebugger();
    ~UnixDebugger();

    /// writes backtrace of the calling thread into ss
    static void WriteBacktrace(std::stringstream& ss);

    /// Writes crash log
    static void DumpDebugInfo(const char* sig, const char* reason);

    /// Writes backtrace of all threads ino ss
    void WriteBacktraceForAllThreads(std::stringstream& ss);

    /// Call this when new thread spawns
    static void InsertThread(ACE_thread_t thread);

    /// Call this when thread exits
    static void RemoveThread(ACE_thread_t thread);

    class Resolver
    {
        public:
            Resolver(const char* executable);
            ~Resolver();

            bool Resolve(unsigned long addr);
            const char* Demangle(const char* str, int options);

            const std::string& GetFunction() const
            {
                return function;
            }
            const std::string& GetFile() const
            {
                return filename;
            }
            unsigned int GetLine() const
            {
                return line;
            }
            time_t GetModificationTime() const
            {
                return mtime;
            }
        private:
            bfd* abfd;
            asymbol** syms;
            asection* text;

            std::string function;
            std::string filename;
            unsigned int line;

            time_t mtime;

            static bool initialized;
    };

    private:

    static void SignalHandler(int num, siginfo_t* info, void* ctx);

    // helpers for WriteBacktraceForAllThreads(), other functions
    static ACE_sema_t s_AllBacktracesSemaphore;
    static std::stringstream s_AllBacktraces;

    static std::set<ACE_thread_t> s_Threads;
    static ACE_Thread_Mutex s_Threads_Lock;
    static bool s_InstanceExists;
};

#endif // _UNIX_DEBUGGER_H_
#endif // PLATFORM_UNIX
