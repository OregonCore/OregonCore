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

#ifndef OREGON_CREATIONPOLICY_H
#define OREGON_CREATIONPOLICY_H

#include <stdlib.h>
#include "Platform/Define.h"

namespace Oregon
{
// OperatorNew policy creates an object on the heap using new.
template <class T>
class OperatorNew
{
    public:
        static T* Create(void)
        {
            return (new T);
        }
        static void Destroy(T* obj)
        {
            delete obj;
        }
};

/*
 * LocalStaticCreation policy creates an object on the stack
 * the first time call Create.
 */
template <class T>
class LocalStaticCreation
{
        union MaxAlign
        {
            char t_[sizeof(T)];
            short int shortInt_;
            int int_;
            long int longInt_;
            float float_;
            double double_;
            long double longDouble_;
            struct Test;
            int Test::* pMember_;
            int (Test::*pMemberFn_)(int);
        };
    public:
        static T* Create(void)
        {
            static MaxAlign si_localStatic;
            return new(&si_localStatic) T;
        }

        static void Destroy(T* obj)
        {
            obj->~T();
        }
};

/*
 * CreateUsingMalloc by pass the memory manger.
 */
template<class T>
class CreateUsingMalloc
{
    public:
        static T* Create()
        {
            void* p = ::malloc(sizeof(T));
            if (!p) return 0;
            return new(p) T;
        }

        static void Destroy(T* p)
        {
            p->~T();
            ::free(p);
        }
};

/*
 * CreateOnCallBack creates the object base on the call back.
 */
template<class T, class CALL_BACK>
class CreateOnCallBack
{
    public:
        static T* Create()
        {
            return CALL_BACK::createCallBack();
        }

        static void Destroy(T* p)
        {
            CALL_BACK::destroyCallBack(p);
        }
};
}
#endif

