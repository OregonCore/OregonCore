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

#ifndef OREGONCORE_PROGRESSBAR_H
#define OREGONCORE_PROGRESSBAR_H

#include <stdio.h>
#include "Platform/Define.h"

class barGoLink
{
    public: // constructors
        barGoLink(uint64 row_count);
        ~barGoLink();

    public: // modifiers
        void step( void );

        static void SetOutputState(bool on) { m_showOutput = on; }
    private:
        static bool m_showOutput; // not recommended change with existed active bar
        static char const * const empty;
        static char const * const full;

        uint64 rec_no;
        uint64 rec_pos;
        uint64 num_rec;
        uint64 indic_len;
};
#endif

