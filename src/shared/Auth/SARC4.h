/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
 *
 * Copyright (C) 2008-2010 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010 Oregon <http://www.oregoncore.com/>
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

#ifndef _AUTH_SARC4_H
#define _AUTH_SARC4_H

#include "Common.h"
#include <openssl/evp.h>

class SARC4
{
    public:
        SARC4(uint8 len);
        SARC4(uint8 *seed, uint8 len);
        ~SARC4();
        void Init(uint8 *seed);
        void UpdateData(int len, uint8 *data);
    private:
        EVP_CIPHER_CTX m_ctx;
};
#endif
