/*
 * Copyright (C) 2010-2012 OregonCore <http://www.oregoncore.com/>
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
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

#include "Base.h"

Base::~Base ()
{
}

/*
 * Decrement object's reference count; as soon as the last reference
 * to the object is removed, it is destroyed.
 */

void Base::DecRef ()
{
    if (!--RefCount)
        delete this;
}

/*
 * Object initialization. The initial reference count is set to one;
 * this means if you call DecRef() immediately after creating the object,
 * it will be destroyed.
 */
Base::Base ()
{
    RefCount = 1;
}

/*
 * Increment reference count.
 * Every time when you copy a pointer to a object and store it for
 * later use you MUST call IncRef() on it; this will allow to keep
 * objects as long as they are referenced by some entity.
 */
void Base::IncRef ()
{
    ++RefCount;

}

/*
 * Query number of references to this object.
 * I would rather prefer to have the reference counter strictly private,
 * but sometimes, mostly for debugging, such a function can help.
 */
int Base::GetRefCount ()
{
    return RefCount;
}

