/*
 * Copyright (C) 2010-2014 OregonCore <http://www.oregoncore.com/>
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2012 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: Stormwind_City
SD%Complete: 100
SDComment: Quest support: 1640, 1447, 4185, 11223, 434. Receive emote General Marcus
SDCategory: Stormwind City
EndScriptData */

/* ContentData
npc_archmage_malin
npc_bartleby
npc_dashel_stonefist
npc_general_marcus_jonathan
npc_lady_katrana_prestor
npc_tyrion
npc_tyrion_spybot
npc_lord_gregor_lescovar
npc_marzon_silent_blade
npc_stormwind_royal_guard
EndContentData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"

/*######
## npc_archmage_malin
######*/

#define GOSSIP_ITEM_MALIN "Can you send me to Theramore? I have an urgent message for Lady Jaina from Highlord Bolvar."

bool GossipHello_npc_archmage_malin(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(11223) == QUEST_STATUS_COMPLETE && !pPlayer->GetQuestRewardStatus(11223))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_MALIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_archmage_malin(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer, 42711, true);
    }

    return true;
}

/*######
## npc_bartleby
######*/

struct npc_bartlebyAI : public ScriptedAI
{
    npc_bartlebyAI(Creature *c) : ScriptedAI(c) {}

    uint64 PlayerGUID;

    void Reset()
    {
        me->setFaction(11);
        me->setEmoteState(7);

        PlayerGUID = 0;
    }

    void JustDied(Unit *)
    {
        me->setFaction(11);
    }

    void DamageTaken(Unit *done_by, uint32 & damage)
    {
        if (damage > me->GetHealth() || ((me->GetHealth() - damage)*100 / me->GetMaxHealth() < 15))
        {
            //Take 0 damage
            damage = 0;

            if (done_by->GetTypeId() == TYPEID_PLAYER && done_by->GetGUID() == PlayerGUID)
            {
                CAST_PLR(done_by)->AttackStop();
                CAST_PLR(done_by)->AreaExploredOrEventHappens(1640);
            }
            me->CombatStop();
            EnterEvadeMode();
        }
    }
};

bool QuestAccept_npc_bartleby(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == 1640)
    {
        pCreature->setFaction(168);
        ((npc_bartlebyAI*)pCreature->AI())->PlayerGUID = pPlayer->GetGUID();
        pCreature->AI()->AttackStart(pPlayer);
    }
    return true;
}

CreatureAI* GetAI_npc_bartleby(Creature* pCreature)
{
    return new npc_bartlebyAI(pCreature);
}

/*######
## npc_dashel_stonefist
######*/

struct npc_dashel_stonefistAI : public ScriptedAI
{
    npc_dashel_stonefistAI(Creature *c) : ScriptedAI(c) {}

    void Reset()
    {
        me->setFaction(11);
        me->setEmoteState(7);
    }

    void DamageTaken(Unit *done_by, uint32 & damage)
    {
        if ((damage > me->GetHealth()) || (me->GetHealth() - damage)*100 / me->GetMaxHealth() < 15)
        {
            //Take 0 damage
            damage = 0;

            if (done_by->GetTypeId() == TYPEID_PLAYER)
            {
                CAST_PLR(done_by)->AttackStop();
                CAST_PLR(done_by)->AreaExploredOrEventHappens(1447);
            }
            EnterEvadeMode();
        }
    }
};

bool QuestAccept_npc_dashel_stonefist(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == 1447)
    {
        pCreature->setFaction(168);
        CAST_AI(npc_dashel_stonefistAI, pCreature->AI())->AttackStart(pPlayer);
    }
    return true;
}

CreatureAI* GetAI_npc_dashel_stonefist(Creature* pCreature)
{
    return new npc_dashel_stonefistAI(pCreature);
}

/*######
## npc_general_marcus_jonathan
######*/

struct npc_general_marcus_jonathanAI : public ScriptedAI
{
    npc_general_marcus_jonathanAI(Creature *c) : ScriptedAI(c) {}

    void ReceiveEmote(Player* pPlayer, uint32 emote)
    {
        if (pPlayer->GetTeam() == ALLIANCE)
        {
            if (emote == TEXTEMOTE_SALUTE)
            {
                me->SetOrientation(me->GetAngle(pPlayer));
                me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
            }
            if (emote == TEXTEMOTE_WAVE)
            {
                me->MonsterSay("Greetings citizen", LANG_COMMON, 0);
            }
        }
    }
};

CreatureAI* GetAI_npc_general_marcus_jonathan(Creature* pCreature)
{
    return new npc_general_marcus_jonathanAI(pCreature);
}

/*######
## npc_lady_katrana_prestor
######*/

#define GOSSIP_ITEM_KAT_1 "Pardon the intrusion, Lady Prestor, but Highlord Bolvar suggested that I seek your advice."
#define GOSSIP_ITEM_KAT_2 "My apologies, Lady Prestor."
#define GOSSIP_ITEM_KAT_3 "Begging your pardon, Lady Prestor. That was not my intent."
#define GOSSIP_ITEM_KAT_4 "Thank you for your time, Lady Prestor."

bool GossipHello_npc_lady_katrana_prestor(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(4185) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(2693, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_lady_katrana_prestor(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(2694, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(2695, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(2696, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->AreaExploredOrEventHappens(4185);
            break;
    }
    return true;
}

/*#######
## Quest: The Attack!
#######*/

enum
{
    SAY_LESCOVAR_2            = -1000657,
    SAY_GUARD_2               = -1000658,
    SAY_LESCOVAR_3            = -1000659,
    SAY_MARZON_1              = -1000660,
    SAY_LESCOVAR_4            = -1000661,
    SAY_TYRION_2              = -1000662,
    SAY_MARZON_2              = -1000663,

    SAY_QUEST_ACCEPT_ATTACK   = -1000664,
    SAY_TYRION_1              = -1000665,
    SAY_SPYBOT_1              = -1000666,
    SAY_GUARD_1               = -1000667,
    SAY_SPYBOT_2              = -1000668,
    SAY_SPYBOT_3              = -1000669,
    SAY_LESCOVAR_1            = -1000670,
    SAY_SPYBOT_4              = -1000671,

    NPC_LORD_GREGOR_LESCOVAR  = 1754,
    NPC_MARZON_BLADE          = 1755,
    NPC_STORMWIND_ROYAL       = 1756,
    NPC_TYRION                = 7766,
    NPC_PRIESTESS_TYRIONA     = 7779,
    NPC_TYRION_SPYBOT         = 8856,

    //@todo need to implement script support for Illusion spells
    SPELL_ILLUSION_BISHOP_TYRIONA = 11048,
    SPELL_ILLUSION_TYRION         = 11067,
    SPELL_FADED                   = 6408,

    QUEST_THE_ATTACK          = 434
};

/*######
## npc_stormwind_royal_guard
######*/

struct Point
{
    float m_fX, m_fY, m_fZ;
};

Point P1[] =
{
    {-8407.58f, 463.477f, 123.761f},
    {-8408.53f, 464.716f, 123.761f}
};
Point P2[] =
{
    {-8434.93f, 441.242f, 121.922f},
    {-8436.16f, 442.804f, 121.923f}
};

struct npc_stormwind_royal_guardAI : public ScriptedAI
{
    npc_stormwind_royal_guardAI(Creature* pCreature) : ScriptedAI(pCreature) {}

    uint32 uiSpybotTimer;
    uint32 uiSpybotPhase;
    uint8 PointID;
    uint64 SpybotGUID;
    uint8 MovePoint;
    bool IsMove;

    void Reset()
    {
        uiSpybotTimer = 0;
        uiSpybotPhase = 0;
        PointID = 0;
        MovePoint = 0;
        SpybotGUID = 0;
        IsMove = false;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (uiSpybotPhase)
        {
            if (uiSpybotTimer <= uiDiff)
            {
                switch(uiSpybotPhase)
                {
                case 1:
                    if ( Unit *pSpybot = me->GetUnit(*me, SpybotGUID))
                        me->SetFacingToObject(pSpybot);
                    uiSpybotTimer = 1000;
                    ++uiSpybotPhase;
                    break;
                case 2:
                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                    uiSpybotTimer = 3000;
                    ++uiSpybotPhase;
                    break;
                case 3:
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    uiSpybotTimer = 5000;
                    ++uiSpybotPhase;
                    break;
                case 4:
                    me->GetMotionMaster()->MoveTargetedHome();
                    me->SendMovementFlagUpdate();
                    uiSpybotPhase = 0;
                }
            }
            else uiSpybotTimer -= uiDiff;
        }

        if (!IsMove && MovePoint)
        {
            IsMove = true;

            switch(MovePoint)
            {
            case 1:
                me->GetMotionMaster()->MovePoint(1, P1[PointID].m_fX, P1[PointID].m_fY, P1[PointID].m_fZ);
                me->SetReactState(REACT_PASSIVE);
                break;
            case 2:
                me->GetMotionMaster()->MovePoint(2, P2[PointID].m_fX, P2[PointID].m_fY, P2[PointID].m_fZ);
                break;
            }
        }
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE)
            return;

        switch(id)
        {
        case 1:
            IsMove = false;
            MovePoint = 2;
            break;
        case 2:
            me->ForcedDespawn(60000); //Despawn after 1 min
            break;
        }
    }
};

CreatureAI* GetAI_npc_stormwind_royal_guard(Creature* pCreature)
{
    return new npc_stormwind_royal_guardAI(pCreature);
}

/*######
## npc_lord_gregor_lescovar
######*/

struct npc_lord_gregor_lescovarAI : public npc_escortAI
{
    npc_lord_gregor_lescovarAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        pCreature->RestoreFaction();
    }

    uint32 uiTimer;
    uint32 uiPhase;
    uint64 MarzonGUID;

    void Reset()
    {
        uiTimer = 0;
        uiPhase = 0;
        MarzonGUID = 0;
    }

    void EnterEvadeMode()
    {
        me->ForcedDespawn();

        if (Creature *pMarzon = Unit::GetCreature(*me, MarzonGUID))
        {
            if (pMarzon->isAlive())
                pMarzon->DisappearAndDie();
        }
    }

    void DoGuardList()
    {
        float x, y, z;
        uint8 PointID;
        PointID = 0;
        std::list<Creature*> lCreatureList;

        me->GetPosition(x, y, z);
        CellPair pair(Oregon::ComputeCellPair(x, y));
        Cell cell(pair);
        cell.data.Part.reserved = ALL_DISTRICT;
        cell.SetNoCreate();

        Oregon::AllCreaturesOfEntryInRange check(me, NPC_STORMWIND_ROYAL, 10);
        Oregon::CreatureListSearcher<Oregon::AllCreaturesOfEntryInRange> searcher(lCreatureList, check);
        TypeContainerVisitor<Oregon::CreatureListSearcher<Oregon::AllCreaturesOfEntryInRange>, GridTypeMapContainer> cSearcher(searcher);
        cell.Visit(pair, cSearcher, *(me->GetMap()));

        if (!lCreatureList.empty())
        {
            for (std::list<Creature*>::iterator itr = lCreatureList.begin(); itr != lCreatureList.end(); ++itr)
            {
                CAST_AI(npc_stormwind_royal_guardAI,(*itr)->AI())->MovePoint = 1;
                CAST_AI(npc_stormwind_royal_guardAI,(*itr)->AI())->PointID = PointID;
                PointID++;
            }
        }
    }

    void EnterCombat(Unit* pWho)
    {
        if (Creature *pMarzon = Unit::GetCreature(*me, MarzonGUID))
        {
            if (pMarzon->isAlive() && !pMarzon->isInCombat())
                pMarzon->AI()->AttackStart(pWho);
        }
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 4:
                SetEscortPaused(true);
                DoScriptText(SAY_LESCOVAR_2, me);
                if (Creature* pGuard = me->FindNearestCreature(NPC_STORMWIND_ROYAL, 8.0f, true))
                     DoScriptText(SAY_GUARD_2, pGuard);
                DoGuardList();
                uiTimer = 3000;
                uiPhase = 1;
                break;
            case 5:
                SetEscortPaused(true);
                if (Creature *pMarzon = me->SummonCreature(NPC_MARZON_BLADE,-8411.360352f, 480.069733f, 123.760895f, 4.941504f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000))
                {
                    pMarzon->GetMotionMaster()->MovePoint(0,-8406.08f, 469.349f, 123.76f);
                    MarzonGUID = pMarzon->GetGUID();
                }
                uiTimer = 2000;
                uiPhase = 5;
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (uiPhase)
        {
            if (uiTimer <= uiDiff)
            {
                switch(uiPhase)
                {
                    case 1:
                        uiTimer = 3000;
                        uiPhase = 2;
                        break;
                    case 2:
                        DoGuardList();
                        uiTimer = 2000;
                        uiPhase = 3;
                        break;
                    case 3:
                        SetEscortPaused(false);
                        uiTimer = 0;
                        uiPhase = 0;
                        break;
                    case 4:
                        DoScriptText(SAY_LESCOVAR_3, me);
                        uiTimer = 0;
                        uiPhase = 0;
                        break;
                    case 5:
                        if (Creature *pMarzon = Unit::GetCreature(*me, MarzonGUID))
                            DoScriptText(SAY_MARZON_1, pMarzon);
                        uiTimer = 3000;
                        uiPhase = 6;
                        break;
                    case 6:
                        DoScriptText(SAY_LESCOVAR_4, me);
                        if (Player* pPlayer = GetPlayerForEscort())
                            pPlayer->GroupEventHappens(QUEST_THE_ATTACK, me);
                        uiTimer = 2000;
                        uiPhase = 7;
                        break;
                    case 7:
                        if (Creature* pTyrion = me->FindNearestCreature(NPC_TYRION, 20.0f, true))
                            DoScriptText(SAY_TYRION_2, pTyrion);
                        if (Creature *pMarzon = Unit::GetCreature(*me, MarzonGUID))
                            pMarzon->setFaction(14);
                        me->setFaction(14);
                        uiTimer = 0;
                        uiPhase = 0;
                        break;
                }
            } else uiTimer -= uiDiff;
        }
        npc_escortAI::UpdateAI(uiDiff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_lord_gregor_lescovar(Creature* pCreature)
{
    return new npc_lord_gregor_lescovarAI(pCreature);
}

/*######
## npc_marzon_silent_blade
######*/

struct npc_marzon_silent_bladeAI : public ScriptedAI
{
    npc_marzon_silent_bladeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        me->AddUnitMovementFlag(MOVEFLAG_WALK_MODE);
    }

    void Reset()
    {
        me->RestoreFaction();
    }

    void EnterCombat(Unit*)
    {
        DoScriptText(SAY_MARZON_2, me);
    }

    void EnterEvadeMode()
    {
        me->ForcedDespawn();
    }

    void MovementInform(uint32 uiType, uint32 /*uiId*/)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (Creature* pLescovar = me->FindNearestCreature(NPC_LORD_GREGOR_LESCOVAR, 20.0f))
        {
            CAST_AI(npc_lord_gregor_lescovarAI, pLescovar->AI())->uiTimer = 2000;
            CAST_AI(npc_lord_gregor_lescovarAI, pLescovar->AI())->uiPhase = 5;
        }
    }

    void UpdateAI(const uint32 /*diff*/)
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_marzon_silent_blade(Creature* pCreature)
{
    return new npc_marzon_silent_bladeAI(pCreature);
}

/*######
## npc_tyrion_spybot
######*/

struct npc_tyrion_spybotAI : public npc_escortAI
{
    npc_tyrion_spybotAI(Creature* pCreature) : npc_escortAI(pCreature) {}

    uint32 uiTimer;
    uint32 uiPhase;

    void Reset()
    {
        uiTimer = 0;
        uiPhase = 0;
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 3:
                DoGuardList();
                SetEscortPaused(true);
                DoScriptText(SAY_SPYBOT_1, me);
                uiTimer = 5000;
                uiPhase = 5;
                break;
            case 9:
                if (Creature* pLescovar = me->FindNearestCreature(NPC_LORD_GREGOR_LESCOVAR,10.0f))
                    pLescovar->SetFacingToObject(me);
                SetEscortPaused(true);
                DoScriptText(SAY_SPYBOT_3, me);
                uiTimer = 3000;
                uiPhase = 8;
                break;
            case 16:
                me->UpdateEntry(NPC_TYRION_SPYBOT);
                me->RemoveUnitMovementFlag(MOVEFLAG_WALK_MODE);
                DoCast(SPELL_FADED);
                break;
        }
    }

    void DoGuardList()
    {
        float x, y, z;
        std::list<Creature*> lCreatureList;

        me->GetPosition(x, y, z);
        CellPair pair(Oregon::ComputeCellPair(x, y));
        Cell cell(pair);
        cell.data.Part.reserved = ALL_DISTRICT;
        cell.SetNoCreate();

        Oregon::AllCreaturesOfEntryInRange check(me, NPC_STORMWIND_ROYAL, 10);
        Oregon::CreatureListSearcher<Oregon::AllCreaturesOfEntryInRange> searcher(lCreatureList, check);
        TypeContainerVisitor<Oregon::CreatureListSearcher<Oregon::AllCreaturesOfEntryInRange>, GridTypeMapContainer> cSearcher(searcher);
        cell.Visit(pair, cSearcher, *(me->GetMap()));

        if (!lCreatureList.empty())
        {
            for (std::list<Creature*>::iterator itr = lCreatureList.begin(); itr != lCreatureList.end(); ++itr)
            {
                CAST_AI(npc_stormwind_royal_guardAI,(*itr)->AI())->SpybotGUID = me->GetGUID();
                CAST_AI(npc_stormwind_royal_guardAI,(*itr)->AI())->uiSpybotPhase = 1;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (uiPhase)
        {
            if (uiTimer <= uiDiff)
            {
                switch(uiPhase)
                {
                case 1:
                    DoScriptText(SAY_QUEST_ACCEPT_ATTACK, me);
                    uiTimer = 3000;
                    uiPhase = 2;
                    break;
                case 2:
                    if (Creature* pTyrion = me->FindNearestCreature(NPC_TYRION,10.0f))
                        DoScriptText(SAY_TYRION_1, pTyrion);
                    uiTimer = 3000;
                    uiPhase = 3;
                    break;
                case 3:
                    me->SetName("Priestess Tyriona");
                    me->SetDisplayId(6703);
                    me->SetEntry(7779);
                    uiTimer = 2000;
                    uiPhase = 4;
                    break;
                case 4:
                    if (Creature* pTyrion = me->FindNearestCreature(NPC_TYRION,10.0f))
                        pTyrion->GetMotionMaster()->MoveTargetedHome();
                    SetEscortPaused(false);
                    uiPhase = 0;
                    uiTimer = 0;
                   break;
                case 5:
                    if (Creature* pGuard = me->FindNearestCreature(NPC_STORMWIND_ROYAL, 10.0f, true))
                        DoScriptText(SAY_GUARD_1, pGuard);
                    uiTimer = 3000;
                    uiPhase = 6;
                    break;
                case 6:
                    DoScriptText(SAY_SPYBOT_2, me);
                    uiTimer = 3000;
                    uiPhase = 7;
                    break;
                case 7:
                    SetEscortPaused(false);
                    uiTimer = 0;
                    uiPhase = 0;
                    break;
                case 8:
                    if (Creature* pLescovar = me->FindNearestCreature(NPC_LORD_GREGOR_LESCOVAR,10.0f))
                    {
                        DoScriptText(SAY_LESCOVAR_1, pLescovar);
                        me->SetFacingToObject(pLescovar);
                        pLescovar->SetFacingToObject(me);
                    }
                    uiTimer = 3000;
                    uiPhase = 9;
                    break;
                case 9:
                    DoScriptText(SAY_SPYBOT_4, me);
                    uiTimer = 1000;
                    uiPhase = 10;
                    break;
                case 10:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                    uiPhase = 11;
                    uiTimer = 2000;
                    break;
                case 11:
                    SetEscortPaused(false);
                    if (Creature* pLescovar = me->FindNearestCreature(NPC_LORD_GREGOR_LESCOVAR,10.0f))
                    {
                        if (Player* pPlayer = GetPlayerForEscort())
                        {
                            CAST_AI(npc_lord_gregor_lescovarAI,pLescovar->AI())->Start(false, false, pPlayer->GetGUID());
                            CAST_AI(npc_lord_gregor_lescovarAI, pLescovar->AI())->SetMaxPlayerDistance(200.0f);
                        }
                        uiTimer = 0;
                        uiPhase = 0;
                        break;
                    }
                }
            }
            else uiTimer -= uiDiff;
        }

        npc_escortAI::UpdateAI(uiDiff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_tyrion_spybot(Creature* pCreature)
{
    return new npc_tyrion_spybotAI(pCreature);
}

/*######
## npc_tyrion
######*/

bool QuestAccept_npc_tyrion(Player* pPlayer, Creature* pCreature, Quest const *pQuest)
{
    if (pQuest->GetQuestId() == QUEST_THE_ATTACK)
    {
        if (Creature* pSpybot = pCreature->FindNearestCreature(NPC_TYRION_SPYBOT, 5.0f, true))
        {
            pCreature->SetFacingToObject(pSpybot);
            CAST_AI(npc_tyrion_spybotAI,pSpybot->AI())->Start(false, false, pPlayer->GetGUID());
            CAST_AI(npc_tyrion_spybotAI,pSpybot->AI())->SetEscortPaused(true);
            CAST_AI(npc_tyrion_spybotAI,pSpybot->AI())->uiPhase = 1;
            CAST_AI(npc_tyrion_spybotAI,pSpybot->AI())->uiTimer = 200;
            CAST_AI(npc_tyrion_spybotAI,pSpybot->AI())->SetMaxPlayerDistance(200.0f);
        }
        return true;
    }
    return false;
}

void AddSC_stormwind_city()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_archmage_malin";
    newscript->pGossipHello = &GossipHello_npc_archmage_malin;
    newscript->pGossipSelect = &GossipSelect_npc_archmage_malin;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_bartleby";
    newscript->GetAI = &GetAI_npc_bartleby;
    newscript->pQuestAccept = &QuestAccept_npc_bartleby;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_dashel_stonefist";
    newscript->GetAI = &GetAI_npc_dashel_stonefist;
    newscript->pQuestAccept = &QuestAccept_npc_dashel_stonefist;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_general_marcus_jonathan";
    newscript->GetAI = &GetAI_npc_general_marcus_jonathan;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_lady_katrana_prestor";
    newscript->pGossipHello = &GossipHello_npc_lady_katrana_prestor;
    newscript->pGossipSelect = &GossipSelect_npc_lady_katrana_prestor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tyrion";
    newscript->pQuestAccept = &QuestAccept_npc_tyrion;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tyrion_spybot";
    newscript->GetAI = &GetAI_npc_tyrion_spybot;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_lord_gregor_lescovar";
    newscript->GetAI = &GetAI_npc_lord_gregor_lescovar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_marzon_silent_blade";
    newscript->GetAI = &GetAI_npc_marzon_silent_blade;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_stormwind_royal_guard";
    newscript->GetAI = &GetAI_npc_stormwind_royal_guard;
    newscript->RegisterSelf();
}
