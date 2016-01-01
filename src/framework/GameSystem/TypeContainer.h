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

#ifndef OREGON_TYPECONTAINER_H
#define OREGON_TYPECONTAINER_H

/*
 * Here, you'll find a series of containers that allow you to hold multiple
 * types of object at the same time.
 */

#include <map>
#include <vector>
#include "Platform/Define.h"
#include "Utilities/TypeList.h"
#include "GameSystem/GridRefManager.h"

/*
 * ContainerMapList is a mulit-type container for map elements
 * By itself its meaningless but collaborate along with TypeContainers,
 * it become the most powerfully container in the whole system.
 */

template<class OBJECT> struct ContainerMapList
{
    GridRefManager<OBJECT> _element;
};

template<> struct ContainerMapList<TypeNull>                /* nothing is in type null */
{
};
template<class H, class T> struct ContainerMapList<TypeList<H, T> >
{
    ContainerMapList<H> _elements;
    ContainerMapList<T> _TailElements;
};


#include "TypeContainerFunctions.h"

/*
 * TypeMapContainer contains a fixed number of types and is
 * determined at compile time.  This is probably the most complicated
 * class and do its simplest thing, that is, holds objects
 * of different types.
 */

template<class OBJECT_TYPES>
class TypeMapContainer
{
    public:
        template<class SPECIFIC_TYPE> size_t Count() const
        {
            return Oregon::Count(i_elements, (SPECIFIC_TYPE*)NULL);
        }

        // inserts a specific object into the container
        template<class SPECIFIC_TYPE> bool insert(SPECIFIC_TYPE* obj)
        {
            SPECIFIC_TYPE* t = Oregon::Insert(i_elements, obj);
            return (t != NULL);
        }

        ContainerMapList<OBJECT_TYPES>& GetElements(void)
        {
            return i_elements;
        }
        const ContainerMapList<OBJECT_TYPES>& GetElements(void) const
        {
            return i_elements;
        }

    private:
        ContainerMapList<OBJECT_TYPES> i_elements;
};
#endif

