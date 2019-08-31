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
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _AUTH_SHA1_H
#define _AUTH_SHA1_H

#include "Common.h"
#include <openssl/sha.h>
#include <openssl/crypto.h>
#include "Auth/BigNumber.h"

class Sha1Hash
{
    public:
        Sha1Hash();
        ~Sha1Hash();

        void UpdateFinalizeBigNumbers(BigNumber* bn0, ...);
        void UpdateBigNumbers(BigNumber* bn0, ...);

        void UpdateData(uint8 const* dta, int len);
        void UpdateData(std::string const& str);

        void Initialize();
        void Finalize();

        uint8* GetDigest(void)
        {
            return mDigest;
        };
        int GetLength(void)
        {
            return SHA_DIGEST_LENGTH;
        };

        BigNumber GetBigNumber();

    private:
        SHA_CTX* mC;
        uint8 mDigest[SHA_DIGEST_LENGTH];
};
#endif

