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

#ifndef OREGON_SINGLETONIMPL_H
#define OREGON_SINGLETONIMPL_H

#include "Singleton.h"

// avoid the using namespace here cuz
// its a .h file afterall

template
<
typename T,
class ThreadingModel,
class CreatePolicy,
class LifeTimePolicy
>
T&
Oregon::Singleton<T, ThreadingModel, CreatePolicy, LifeTimePolicy >::Instance()
{
    if (!si_instance)
    {
        // double-checked Locking pattern
        Guard();
        if (!si_instance)
        {
            if (si_destroyed)
            {
                si_destroyed = false;
                LifeTimePolicy::OnDeadReference();
            }
            si_instance = CreatePolicy::Create();
            LifeTimePolicy::ScheduleCall(&DestroySingleton);
        }
    }

    return *si_instance;
}

template
<
typename T,
class ThreadingModel,
class CreatePolicy,
class LifeTimePolicy
>
void
Oregon::Singleton<T, ThreadingModel, CreatePolicy, LifeTimePolicy>::DestroySingleton()
{
    CreatePolicy::Destroy(si_instance);
    si_instance = NULL;
    si_destroyed = true;
}

#define INSTANTIATE_SINGLETON_1(TYPE) \
    namespace Oregon \
    { \
        template<> TYPE* Singleton<TYPE, Oregon::SingleThreaded<TYPE>, Oregon::OperatorNew<TYPE>, Oregon::ObjectLifeTime<TYPE> >::si_instance = 0; \
        template<> bool  Singleton<TYPE, Oregon::SingleThreaded<TYPE>, Oregon::OperatorNew<TYPE>, Oregon::ObjectLifeTime<TYPE> >::si_destroyed = false; \
        template   class Singleton<TYPE, Oregon::SingleThreaded<TYPE>, Oregon::OperatorNew<TYPE>, Oregon::ObjectLifeTime<TYPE> >; \
    }

#define INSTANTIATE_SINGLETON_2(TYPE, THREADINGMODEL) \
    namespace Oregon \
    { \
        template<> TYPE* Singleton<TYPE, THREADINGMODEL, Oregon::OperatorNew<TYPE>, Oregon::ObjectLifeTime<TYPE> >::si_instance = 0; \
        template<> bool  Singleton<TYPE, THREADINGMODEL, Oregon::OperatorNew<TYPE>, Oregon::ObjectLifeTime<TYPE> >::si_destroyed = false; \
        template   class Singleton<TYPE, THREADINGMODEL, Oregon::OperatorNew<TYPE>, Oregon::ObjectLifeTime<TYPE> >; \
    }

#define INSTANTIATE_SINGLETON_3(TYPE, THREADINGMODEL, CREATIONPOLICY ) \
    namespace Oregon \
    { \
        template<> TYPE* Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, Oregon::ObjectLifeTime<TYPE> >::si_instance = 0; \
        template<> bool  Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, Oregon::ObjectLifeType<TYPE> >::si_destroyed = false; \
        template   class Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, Oregon::ObjectLifeTime<TYPE> >; \
    }

#define INSTANTIATE_SINGLETON_4(TYPE, THREADINGMODEL, CREATIONPOLICY, OBJECTLIFETIME) \
    namespace Oregon \
    { \
        template<> TYPE* Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, OBJECTLIFETIME >::si_instance = 0; \
        template<> bool  Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, OBJECTLIFETIME >::si_destroyed = false; \
        template   class Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, OBJECTLIFETIME >; \
    }
#endif

