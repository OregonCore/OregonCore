/*
 * Copyright (C) 2005-2008 MaNGOS <http://www.mangosproject.org/>
 *
 * Copyright (C) 2008 Trinity <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010 Oregon <http://www.oregoncore.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
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
    template class Oregon::Singleton<TYPE, Oregon::SingleThreaded<TYPE>, Oregon::OperatorNew<TYPE>, Oregon::ObjectLifeTime<TYPE> >; \
    template<> TYPE* Oregon::Singleton<TYPE, Oregon::SingleThreaded<TYPE>, Oregon::OperatorNew<TYPE>, Oregon::ObjectLifeTime<TYPE> >::si_instance = 0; \
    template<> bool Oregon::Singleton<TYPE, Oregon::SingleThreaded<TYPE>, Oregon::OperatorNew<TYPE>, Oregon::ObjectLifeTime<TYPE> >::si_destroyed = false

#define INSTANTIATE_SINGLETON_2(TYPE, THREADINGMODEL) \
    template class Oregon::Singleton<TYPE, THREADINGMODEL, Oregon::OperatorNew<TYPE>, Oregon::ObjectLifeTime<TYPE> >; \
    template<> TYPE* Oregon::Singleton<TYPE, THREADINGMODEL, Oregon::OperatorNew<TYPE>, Oregon::ObjectLifeTime<TYPE> >::si_instance = 0; \
    template<> bool Oregon::Singleton<TYPE, THREADINGMODEL, Oregon::OperatorNew<TYPE>, Oregon::ObjectLifeTime<TYPE> >::si_destroyed = false

#define INSTANTIATE_SINGLETON_3(TYPE, THREADINGMODEL, CREATIONPOLICY ) \
    template class Oregon::Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, Oregon::ObjectLifeTime<TYPE> >; \
    template<> TYPE* Oregon::Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, Oregon::ObjectLifeTime<TYPE> >::si_instance = 0; \
    template<> bool Oregon::Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, Oregon::ObjectLifeType<TYPE> >::si_destroyed = false

#define INSTANTIATE_SINGLETON_4(TYPE, THREADINGMODEL, CREATIONPOLICY, OBJECTLIFETIME) \
    template class Oregon::Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, OBJECTLIFETIME >; \
    template<> TYPE* Oregon::Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, OBJECTLIFETIME >::si_instance = 0; \
    template<> bool Oregon::Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, OBJECTLIFETIME >::si_destroyed = false
#endif

