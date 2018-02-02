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

#ifndef OREGONCORE_ERRORS_H
#define OREGONCORE_ERRORS_H

#include "Common.h"
#include <ace/Stack_Trace.h>

#define WPAssert(assertion) { if (!(assertion)) { ACE_Stack_Trace st; fprintf(stderr, "\n%s:%i in %s ASSERTION FAILED:\n  %s\n%s\n", __FILE__, __LINE__, __FUNCTION__, #assertion, st.c_str()); *((volatile int*)NULL) = 0; } }
#define WPError(assertion, errmsg) { if (!(assertion)) { sLog.outError("%\n%s:%i in %s ERROR:\n  %s\n", __FILE__, __LINE__, __FUNCTION__, (char *)errmsg); *((volatile int*)NULL) = 0; } }
#define WPWarning(assertion, errmsg) { if (!(assertion)) { sLog.outError("\n%s:%i in %s WARNING:\n  %s\n", __FILE__, __LINE__, __FUNCTION__, (char *)errmsg); } }
#define WPFatal(assertion, errmsg) { if (!(assertion)) { sLog.outError("\n%s:%i in %s FATAL ERROR:\n  %s\n", __FILE__, __LINE__, __FUNCTION__, (char *)errmsg); ACE_OS::sleep(10); *((volatile int*)NULL) = 0; } }

#define ASSERT WPAssert
#endif

