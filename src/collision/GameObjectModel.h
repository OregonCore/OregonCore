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

#ifndef _GAMEOBJECT_MODEL_H
#define _GAMEOBJECT_MODEL_H

#include <G3D/Matrix3.h>
#include <G3D/Vector3.h>
#include <G3D/AABox.h>
#include <G3D/Ray.h>

#include "Platform/Define.h"

namespace VMAP
{
    class WorldModel;
}

class GameObject;
struct GameObjectDisplayInfoEntry;

class GameObjectModel /*, public Intersectable*/
{
    bool collision_enabled;
    G3D::AABox iBound;
    G3D::Matrix3 iInvRot;
    G3D::Vector3 iPos;
    //G3D::Vector3 iRot;
    float iInvScale;
    float iScale;
    VMAP::WorldModel* iModel;

    GameObjectModel() : collision_enabled(false), iModel(nullptr) {}
    bool initialize(const GameObject& go, const GameObjectDisplayInfoEntry& info);

    public:
        std::string name;

        const G3D::AABox& getBounds() const { return iBound; }

        ~GameObjectModel();

        const G3D::Vector3& getPosition() const { return iPos;}

        /**    Enables\disables collision. */
        void disable() { collision_enabled = false;}
        void enable(bool enable) { collision_enabled = enable;}

        bool intersectRay(const G3D::Ray& Ray, float& MaxDist, bool StopAtFirstHit) const;

        static GameObjectModel* Create(const GameObject& go);

        bool Relocate(GameObject const& go); 
};

#endif // _GAMEOBJECT_MODEL_H
