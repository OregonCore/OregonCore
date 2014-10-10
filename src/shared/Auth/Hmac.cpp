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

#include "Auth/Hmac.h"
#include "BigNumber.h"

HmacHash::HmacHash()
{
    uint8 temp[SEED_KEY_SIZE] = { 0x38, 0xA7, 0x83, 0x15, 0xF8, 0x92, 0x25, 0x30, 0x71, 0x98, 0x67, 0xB1, 0x8C, 0x4, 0xE2, 0xAA };
    memcpy(&m_key, &temp, SEED_KEY_SIZE);
    HMAC_CTX_init(&m_ctx);
    HMAC_Init_ex(&m_ctx, &m_key, SEED_KEY_SIZE, EVP_sha1(), NULL);
}

HmacHash::HmacHash(uint32 len, uint8* seed)
{
    HMAC_CTX_init(&m_ctx);
    HMAC_Init_ex(&m_ctx, &m_key, SEED_KEY_SIZE, EVP_sha1(), NULL);
    memset(m_digest, 0, sizeof(m_digest));
}

HmacHash::~HmacHash()
{
    HMAC_CTX_cleanup(&m_ctx);
}

void HmacHash::UpdateBigNumber(BigNumber* bn)
{
    UpdateData(bn->AsByteArray(), bn->GetNumBytes());
}

void HmacHash::UpdateData(const uint8* data, int length)
{
    HMAC_Update(&m_ctx, data, length);
}

void HmacHash::UpdateData(const std::string& str)
{
    UpdateData((uint8 const*)str.c_str(), str.length());
}

void HmacHash::Initialize()
{
    HMAC_Init_ex(&m_ctx, &m_key, SEED_KEY_SIZE, EVP_sha1(), NULL);
}

void HmacHash::Finalize()
{
    uint32 length = 0;
    HMAC_Final(&m_ctx, m_digest, &length);
    ASSERT(length == SHA_DIGEST_LENGTH);
}
