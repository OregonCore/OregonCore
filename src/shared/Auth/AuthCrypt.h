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

#ifndef _AUTHCRYPT_H
#define _AUTHCRYPT_H

#include <Common.h>
#include <vector>

class BigNumber;

class AuthCrypt
{
    public:
        AuthCrypt();
        ~AuthCrypt();

        const static size_t CRYPTED_SEND_LEN = 4;
        const static size_t CRYPTED_RECV_LEN = 6;

        void Init();

        void SetKey(BigNumber*);

        void DecryptRecv(uint8*, size_t);
        void EncryptSend(uint8*, size_t);

        bool IsInitialized()
        {
            return _initialized;
        }

        static void GenerateKey(uint8*, BigNumber*);

    private:
        std::vector<uint8> _key;
        uint8 _send_i, _send_j, _recv_i, _recv_j;
        bool _initialized;
};
#endif

