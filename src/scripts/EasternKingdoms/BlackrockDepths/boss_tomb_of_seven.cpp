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

/* ScriptData
SDName: Boss_Tomb_Of_Seven
SD%Complete: 90
SDComment: Learning Smelt Dark Iron if tribute quest rewarded. Missing event.
SDCategory: Blackrock Depths
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "blackrock_depths.h"

enum Spells
{
    SPELL_SMELT_DARK_IRON                                  = 14891,
    SPELL_LEARN_SMELT                                      = 14894,
};

enum Quests
{
    QUEST_SPECTRAL_CHALICE                                 = 4083
};

enum Misc
{
    DATA_SKILLPOINT_MIN                                    = 230
};

#define GOSSIP_ITEM_TEACH_1 "Teach me the art of smelting dark iron"
#define GOSSIP_ITEM_TEACH_2 "Continue..."
#define GOSSIP_ITEM_TEACH_3 "[PH] Continue..."
#define GOSSIP_ITEM_TRIBUTE "I want to pay tribute"

bool GossipHello_boss_gloomrel(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestRewardStatus(QUEST_SPECTRAL_CHALICE) == 1 && pPlayer->GetSkillValue(SKILL_MINING) >= DATA_SKILLPOINT_MIN && !pPlayer->HasSpell(SPELL_SMELT_DARK_IRON))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TEACH_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    if (pPlayer->GetQuestRewardStatus(QUEST_SPECTRAL_CHALICE) == 0 && pPlayer->GetSkillValue(SKILL_MINING) >= DATA_SKILLPOINT_MIN)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TRIBUTE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_boss_gloomrel(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF+1:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TEACH_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
        pPlayer->SEND_GOSSIP_MENU(2606, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+11:
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, SPELL_LEARN_SMELT, false);
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TEACH_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 22);
        pPlayer->SEND_GOSSIP_MENU(2604, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+22:
        pPlayer->CLOSE_GOSSIP_MENU();
        if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
        {
            //are 5 minutes expected? go template may have data to despawn when used at quest
            pInstance->DoRespawnGameObject(pInstance->GetData64(DATA_GO_CHALICE), MINUTE * 5);
        }
        break;
    }
    return true;
}

enum DoomrelSpells
{
    SPELL_SHADOWBOLTVOLLEY                                 = 15245,
    SPELL_IMMOLATE                                         = 12742,
    SPELL_CURSEOFWEAKNESS                                  = 12493,
    SPELL_DEMONARMOR                                       = 13787,
    SPELL_SUMMON_VOIDWALKERS                               = 15092
};

struct boss_doomrelAI : public ScriptedAI
{
    boss_doomrelAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;
    uint32 ShadowVolley_Timer;
    uint32 Immolate_Timer;
    uint32 CurseOfWeakness_Timer;
    uint32 DemonArmor_Timer;
    bool Voidwalkers;

    void Reset()
    {
        ShadowVolley_Timer = 10000;
        Immolate_Timer = 18000;
        CurseOfWeakness_Timer = 5000;
        DemonArmor_Timer = 16000;
        Voidwalkers = false;

        me->setFaction(FACTION_FRIEND);

        // was set before event start, so set again
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);

        if (pInstance)
        {
            if (pInstance->GetData(DATA_GHOSTKILL) >= 7)
                me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
            else
                me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }
    }

    void EnterCombat(Unit* /*who*/)
    {
    }

    void EnterEvadeMode()
    {
        me->RemoveAllAuras();
        me->DeleteThreatList();
        me->CombatStop(true);
        me->LoadCreaturesAddon();
        if (me->IsAlive())
            me->GetMotionMaster()->MoveTargetedHome();
        me->SetLootRecipient(NULL);
        if (pInstance)
            pInstance->SetData64(DATA_EVENSTARTER, 0);
    }

    void JustDied(Unit* /*who*/)
    {
        if (pInstance)
            pInstance->SetData(DATA_GHOSTKILL, 1);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //ShadowVolley_Timer
        if (ShadowVolley_Timer <= diff)
        {
            DoCastVictim( SPELL_SHADOWBOLTVOLLEY);
            ShadowVolley_Timer = 12000;
        }
        else ShadowVolley_Timer -= diff;

        //Immolate_Timer
        if (Immolate_Timer <= diff)
        {
            if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                DoCast(pTarget, SPELL_IMMOLATE);

            Immolate_Timer = 25000;
        }
        else Immolate_Timer -= diff;

        //CurseOfWeakness_Timer
        if (CurseOfWeakness_Timer <= diff)
        {
            DoCastVictim( SPELL_CURSEOFWEAKNESS);
            CurseOfWeakness_Timer = 45000;
        }
        else CurseOfWeakness_Timer -= diff;

        //DemonArmor_Timer
        if (DemonArmor_Timer <= diff)
        {
            DoCast(me, SPELL_DEMONARMOR);
            DemonArmor_Timer = 300000;
        }
        else DemonArmor_Timer -= diff;

        //Summon Voidwalkers
        if (!Voidwalkers && HealthBelowPct(50))
        {
            DoCastVictim( SPELL_SUMMON_VOIDWALKERS, true);
            Voidwalkers = true;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_doomrel(Creature* pCreature)
{
    return new boss_doomrelAI (pCreature);
}

#define GOSSIP_ITEM_CHALLENGE   "Your bondage is at an end, Doom'rel. I challenge you!"
#define GOSSIP_SELECT_DOOMREL   "[PH] Continue..."

bool GossipHello_boss_doomrel(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_CHALLENGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    pPlayer->SEND_GOSSIP_MENU(2601, pCreature->GetGUID());

    return true;
}

bool GossipSelect_boss_doomrel(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF+1:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT_DOOMREL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->SEND_GOSSIP_MENU(2605, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        pPlayer->CLOSE_GOSSIP_MENU();
        //start event here
        pCreature->setFaction(FACTION_HOSTILE);
        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
        pCreature->AI()->AttackStart(pPlayer);
        ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        if (pInstance)
            pInstance->SetData64(DATA_EVENSTARTER, pPlayer->GetGUID());
        break;
    }
    return true;
}

void AddSC_boss_tomb_of_seven()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_gloomrel";
    newscript->pGossipHello = &GossipHello_boss_gloomrel;
    newscript->pGossipSelect = &GossipSelect_boss_gloomrel;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_doomrel";
    newscript->GetAI = &GetAI_boss_doomrel;
    newscript->pGossipHello = &GossipHello_boss_doomrel;
    newscript->pGossipSelect = &GossipSelect_boss_doomrel;
    newscript->RegisterSelf();
}
