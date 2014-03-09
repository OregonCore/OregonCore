/*
 * Copyright (C) 2010-2014 OregonCore <http://www.oregoncore.com/>
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

#ifndef OREGON_SINGLETON_H
#define OREGON_SINGLETON_H

#include "CreationPolicy.h"
#include "ThreadingModel.h"
#include "ObjectLifeTime.h"

namespace Oregon
{
    template
        <
        typename T,
        class ThreadingModel = Oregon::SingleThreaded<T>,
        class CreatePolicy = Oregon::OperatorNew<T>,
        class LifeTimePolicy = Oregon::ObjectLifeTime<T>
        >
        class Singleton
    {
        public:
            static T& Instance();

        protected:
            Singleton() {};

        private:

            // Prohibited actions...this does not prevent hijacking.
            Singleton(const Singleton &);
            Singleton& operator=(const Singleton &);

            // Singleton Helpers
            static void DestroySingleton();

            // data structure
            typedef typename ThreadingModel::Lock Guard;
            static T *si_instance;
            static bool si_destroyed;
    };
}
#endif

