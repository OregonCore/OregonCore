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

#ifndef OREGONCORE_DYNAMICOBJECT_H
#define OREGONCORE_DYNAMICOBJECT_H

#include "Object.h"

class Unit;
struct SpellEntry;

class DynamicObject : public WorldObject, public GridObject<DynamicObject>
{
    public:
        typedef std::set<Unit*> AffectedSet;
        explicit DynamicObject(bool isWorldObject);

        void AddToWorld() override;
        void RemoveFromWorld() override;

        bool CreateDynamicObject(uint32 guidlow, Unit* caster, uint32 spellId, uint32 effIndex, const Position& pos, int32 duration, float radius);
        void Update(uint32 p_time) override;
        void Remove();
        uint32 GetSpellId() const
        {
            return m_spellId;
        }
        uint32 GetEffIndex() const
        {
            return m_effIndex;
        }
        uint32 GetDuration() const
        {
            return m_aliveDuration;
        }
        uint64 GetCasterGUID() const
        {
            return m_casterGuid;
        }
        Unit* GetCaster() const;
        float GetRadius() const
        {
            return m_radius;
        }
        bool IsAffecting(Unit* unit) const
        {
            return m_affected.find(unit) != m_affected.end();
        }
        void AddAffected(Unit* unit)
        {
            m_affected.insert(unit);
        }
        void RemoveAffected(Unit* unit)
        {
            m_affected.erase(unit);
        }
        void Delay(int32 delaytime);

        void Say(const char* text, uint32 language, uint64 TargetGuid)
        {
            MonsterSay(text, language, TargetGuid);
        }
        void Yell(const char* text, uint32 language, uint64 TargetGuid)
        {
            MonsterYell(text, language, TargetGuid);
        }
        void TextEmote(const char* text, uint64 TargetGuid)
        {
            MonsterTextEmote(text, TargetGuid);
        }
        void Whisper(const char* text, uint64 receiver)
        {
            MonsterWhisper(text, receiver);
        }
        void Say(int32 textId, uint32 language, uint64 TargetGuid)
        {
            MonsterSay(textId, language, TargetGuid);
        }
        void Yell(int32 textId, uint32 language, uint64 TargetGuid)
        {
            MonsterYell(textId, language, TargetGuid);
        }
        void TextEmote(int32 textId, uint64 TargetGuid)
        {
            MonsterTextEmote(textId, TargetGuid);
        }
        void Whisper(int32 textId, uint64 receiver)
        {
            MonsterWhisper(textId, receiver);
        }

    protected:
        uint64 m_casterGuid;
        uint32 m_spellId;
        uint32 m_effIndex;
        int32 m_aliveDuration;
        uint32 m_updateTimer;
        time_t m_nextThinkTime;
        float m_radius;
        AffectedSet m_affected;
};
#endif

