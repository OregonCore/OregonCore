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

#ifndef WDTFILE_H
#define WDTFILE_H

#include "mpq_libmpq.h"
#include "wmo.h"
#include <string>
#include "stdlib.h"

class ADTFile;

class WDTFile
{
    public:
        WDTFile(char* file_name, char* file_name1);
        ~WDTFile(void);
        bool init(char* map_id, unsigned int mapID);

        string* gWmoInstansName;
        int gnWMO, nMaps;

        ADTFile* GetMap(int x, int z);

    private:
        MPQFile WDT;
        bool maps[64][64];
        string filename;
};

#endif
