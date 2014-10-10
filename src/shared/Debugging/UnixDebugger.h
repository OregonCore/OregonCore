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

// This is fix for libiberty because ACE has already declared it.
#define HAVE_DECL_BASENAME 1
// And this one is fix for bfd.h
#ifndef PACKAGE
#define PACKAGE
#endif

#include <libiberty.h>
#include <bfd.h>

#ifdef HAVE_DEMANGLE_H
#include <demangle.h>
#else
// Some distributios of libiberty doesn't provide demangle.h
// so this is a workaround - declaring needed function explicitly

/* Options passed to cplus_demangle (in 2nd parameter). */
#define DMGL_NO_OPTS     0
#define DMGL_PARAMS  (1 << 0)
#define DMGL_ANSI    (1 << 1)
#define DMGL_JAVA    (1 << 2)
#define DMGL_VERBOSE     (1 << 3
#define DMGL_TYPES   (1 << 4)
#define DMGL_RET_POSTFIX (1 << 5)
#define DMGL_RET_DROP    (1 << 6)
extern char*
cplus_demangle (const char* mangled, int options);
#endif

namespace UnixDebugger
{
void RegisterDeadlySignalHandler();
void DumpDebugInfo(const char* sig, const char* reason);

class Resolver
{
    public:

        Resolver(const char* executable);
        ~Resolver();

        bool Resolve(unsigned long addr);

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
    private:
        bfd* abfd;
        asymbol** syms;
        asection* text;

        std::string function;
        std::string filename;
        unsigned int line;

        static bool initialized;
};
};

#endif // _UNIX_DEBUGGER_H_
#endif // PLATFORM_UNIX
