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

#ifndef _DYNTREE_H
#define _DYNTREE_H

#include "Platform/Define.h"

namespace G3D
{
    class Vector3;
    class Matrix3;
    class AABox;
    class Ray;
}

class GameObjectModel;

class DynamicMapTree
{
    public:
        DynamicMapTree();
        ~DynamicMapTree();

        bool isInLineOfSight(float x1, float y1, float z1, float x2, float y2, float z2) const;
        bool getIntersectionTime(const G3D::Ray& ray, const G3D::Vector3& endPos, float& maxDist) const;
        bool getObjectHitPos(const G3D::Vector3& pPos1, const G3D::Vector3& pPos2, G3D::Vector3& pResultHitPos, float pModifyDist) const;
        float getHeight(float x, float y, float z, float maxSearchDist) const;

        void insert(const GameObjectModel&);
        void remove(const GameObjectModel&);
        bool contains(const GameObjectModel&) const;
        int size() const;

        void balance();
        void update(uint32 diff);
    private:
        struct DynTreeImpl& impl;
};

#endif // _DYNTREE_H
