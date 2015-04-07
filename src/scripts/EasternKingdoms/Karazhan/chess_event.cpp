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
SDName: chess_event
SD%Complete: 99%
SDComment: only some small things to research
SDCategory: Karazhan
EndScriptData */

#include "ScriptPCH.h"
#include "karazhan.h"

#define ALLIANCE_CREATURE_FACTION 1690
#define HORDE_CREATURE_FACTION 1689
#define DUST_COVERED_CHEST  185119

#define CHESS_EVENT_BEGIN -1910000
#define CHESS_EVENT_CHEATING_1 -1910001
#define CHESS_EVENT_CHEATING_2 -1910002
#define CHESS_EVENT_CHEATING_3 -1910003
#define CHESS_EVENT_P_L_PAWN_1 -1910004
#define CHESS_EVENT_P_L_PAWN_2 -1910005
#define CHESS_EVENT_P_L_PAWN_3 -1910006
#define CHESS_EVENT_M_L_PAWN_1 -1910007
#define CHESS_EVENT_M_L_PAWN_2 -1910008
#define CHESS_EVENT_M_L_PAWN_3 -1910009
#define CHESS_EVENT_P_L_ROOK_1 -1910010
#define CHESS_EVENT_M_L_ROOK_1 -1910011
#define CHESS_EVENT_P_L_KNIGHT_1 -1910012
#define CHESS_EVENT_M_L_KNIGHT_1 -1910013
#define CHESS_EVENT_P_L_BISHOP_1 -1910014
#define CHESS_EVENT_M_L_BISHOP_1 -1910015
#define CHESS_EVENT_P_L_QUEEN_1 -1910016
#define CHESS_EVENT_M_L_QUEEN_1 -1910017
#define CHESS_EVENT_P_L_KING_1 -1910018
#define CHESS_EVENT_M_L_KING_1 -1910019
#define CHESS_EVENT_P_LOSE_1 -1910020
#define CHESS_EVENT_M_LOSE_1 -1910021

enum CHESS_PIECE_EVENT
{
    START_GAME = 0,
    CHEAT,
    PLAYER_LOST_PAWN,
    PLAYER_LOST_ROOK,
    PLAYER_LOST_QUEEN,
    PLAYER_LOST_KNIGHT,
    PLAYER_LOST_KING,
    PLAYER_LOST_BISHOP,
    MEDIVH_LOST_PAWN,
    MEDIVH_LOST_ROOK,
    MEDIVH_LOST_QUEEN,
    MEDIVH_LOST_KNIGHT,
    MEDIVH_LOST_KING,
    MEDIVH_LOST_BISHOP,
    PLAYER_LOST,
    MEDIVH_LOST
};

#define NPC_MEDIVH 16816

#define NPC_PAWN_H 17469
#define NPC_PAWN_A 17211

#define NPC_KNIGHT_H 21748
#define NPC_KNIGHT_A 21664

#define NPC_QUEEN_H 21750
#define NPC_QUEEN_A 21683

#define NPC_BISHOP_H 21747
#define NPC_BISHOP_A 21682

#define BISHOP_HEAL_H 37456
#define BISHOP_HEAL_A 37455

#define NPC_ROOK_H 21726
#define NPC_ROOK_A 21160

#define NPC_KING_H 21752
#define NPC_KING_A 21684

#define NPC_MOVE_MARKER 22519
#define NPC_WHITE_SQUARE 17208
#define NPC_BLACK_SQUARE 17305
#define NPC_VICTORY_CONTROLER 22524
#define NPC_VICTORY_DUMMY 22523
#define NPC_PIECE_BAR 22520
#define NPC_FURY_OF_MEDIVH_VISUAL 22521

#define SPELL_MOVE_1 37146 //Pawn
#define SPELL_MOVE_2 30012
#define SPELL_MOVE_3 37144 //Knight
#define SPELL_MOVE_4 37148 //Queens
#define SPELL_MOVE_5 37151 //Rook
#define SPELL_MOVE_6 37152 //Bishop
#define SPELL_MOVE_7 37153 //king

#define SPELL_CHANGE_FACING 30284
#define SPELL_MOVE_MARKER 32261
#define SPELL_TRANSFORM_FIELD 32745
#define SPELL_CONTROL_PIECE 30019
#define SPELL_COMMAND_MOVE 32306
#define SPELL_DEACTIVATE_OWN_FIELD 32260
#define SPELL_POST_MOVE_FACING 38011
#define SPELL_ATTACK_TIMER 32226 //required since AI of playercontrolled pieces gets deactivated
#define SPELL_FACE_NEARBY_ENEMY 37787
#define SPELL_WIN 39395
#define SPELL_KILL_CHEST_BUNNY 45259
#define SPELL_GAME_OVER 39401
#define SPELL_CAMERA_SHAKE 44762

//MedivhCheats
#define SPELL_HAND_OF_MEDIVH 39339 //berserk spell
#define SPELL_FURY_OF_MEDIVH 39383 //aoe spell
//and we have another cheat medivh heals the target to max.(me->sethealth(me->getmaxhealth()))

#define SEARCH_RANGE 5

const float CHESS_BASE_VECTOR[3] = { -11049.5527f, -1879.8188f, 220.667f };
const float CHESS_RIGHT_VECTOR[3] = { -27.665f, 34.6075f, 0.0f };
const float CHESS_UP_VECTOR[3] = { -34.6375f, -27.3475f, 0.0f };

const float CHESS_PIECEBAR_ALLIANCE[3] = { -11082.1709f, -1910.06103f, 221.07f };
const float CHESS_PIECEBAR_HORDE[3] = { -11079.038f, -1842.5085, 221.07f };

uint32 ChessPieceEntrysHorde[6] = { NPC_PAWN_H, NPC_ROOK_H, NPC_QUEEN_H, NPC_KNIGHT_H, NPC_BISHOP_H, NPC_KING_H };
uint32 ChessPieceEntrysAlliance[6] = { NPC_PAWN_A, NPC_ROOK_A, NPC_QUEEN_A, NPC_BISHOP_A, NPC_KNIGHT_A, NPC_KING_A };


Unit* FindCreatureWithEntryInRange(Unit* source, uint32 entry, float range)
{
    std::list<Unit*> UnitList;
    Oregon::AllCreaturesOfEntryInRange u_check(source, entry, range);
    Oregon::UnitListSearcher<Oregon::AllCreaturesOfEntryInRange> searcher(UnitList, u_check);
    source->GetMap()->VisitAll(source->GetPositionX(), source->GetPositionY(), source->GetPositionZ(), searcher);

    for (std::list<Unit*>::iterator itr = UnitList.begin(); itr != UnitList.end(); itr++)
    {
        if ((*itr) && (*itr)->GetDistance(source) <= 1)
        {
            return (*itr);
        }
    }
    return NULL;
}

void DoMedivhEventSay(ScriptedInstance* pInstance, Unit* who, uint32 chess_action)
{
    uint32 textID = 0;
    if (pInstance)
    {
        uint64 MedivhGUID = pInstance->GetData64(DATA_ECHO_OF_MEDIVH);
        Unit* Medivh = Unit::GetUnit(*who, MedivhGUID);
        if (Medivh)
        {
            switch (chess_action)
            {
            case START_GAME:
                textID = CHESS_EVENT_BEGIN;
                break;
            case CHEAT:
                switch (urand(0, 2))
                {
                case 0:
                    textID = CHESS_EVENT_CHEATING_1;
                    break;
                case 1:
                    textID = CHESS_EVENT_CHEATING_2;
                    break;
                case 2:
                    textID = CHESS_EVENT_CHEATING_3;
                    break;
                }
                break;
            case PLAYER_LOST_PAWN:
                switch (urand(0, 2))
                {
                case 0:
                    textID = CHESS_EVENT_P_L_PAWN_1;
                    break;
                case 1:
                    textID = CHESS_EVENT_P_L_PAWN_2;
                    break;
                case 2:
                    textID = CHESS_EVENT_P_L_PAWN_3;
                    break;
                }
                break;
            case PLAYER_LOST_ROOK:
                textID = CHESS_EVENT_P_L_ROOK_1;
                break;
            case PLAYER_LOST_QUEEN:
                textID = CHESS_EVENT_P_L_ROOK_1;
                break;
            case PLAYER_LOST_KNIGHT:
                textID = CHESS_EVENT_P_L_ROOK_1;
                break;
            case PLAYER_LOST_KING:
                textID = CHESS_EVENT_P_L_ROOK_1;
                break;
            case PLAYER_LOST_BISHOP:
                textID = CHESS_EVENT_P_L_ROOK_1;
                break;
            case MEDIVH_LOST_PAWN:
                switch (urand(0, 2))
                {
                case 0:
                    textID = CHESS_EVENT_M_L_PAWN_1;
                    break;
                case 1:
                    textID = CHESS_EVENT_M_L_PAWN_2;
                    break;
                case 2:
                    textID = CHESS_EVENT_M_L_PAWN_3;
                    break;
                }
                break;
            case MEDIVH_LOST_ROOK:
                textID = CHESS_EVENT_M_L_ROOK_1;
                break;
            case MEDIVH_LOST_QUEEN:
                textID = CHESS_EVENT_M_L_QUEEN_1;
                break;
            case MEDIVH_LOST_KNIGHT:
                textID = CHESS_EVENT_M_L_KNIGHT_1;
                break;
            case MEDIVH_LOST_KING:
                textID = CHESS_EVENT_M_L_KING_1;
                break;
            case MEDIVH_LOST_BISHOP:
                textID = CHESS_EVENT_M_L_BISHOP_1;
                break;
            case PLAYER_LOST:
                textID = CHESS_EVENT_P_LOSE_1;
                break;
            case MEDIVH_LOST:
                textID = CHESS_EVENT_M_LOSE_1;
                break;
            }
            if (textID)
                DoScriptText(textID, Medivh);
        }
    }
}

struct ChessPieceSpell
{
    ChessPieceSpell(uint32 Entry, bool IsPositiv) : m_spellEntry(Entry), m_positiv(IsPositiv){}
    ChessPieceSpell() { m_spellEntry = 0; m_positiv = false; }
    uint32 m_spellEntry;
    bool m_positiv;
};

struct move_markerAI : public ScriptedAI
{
    ScriptedInstance* pInstance;
    uint32 check_timer;

    move_markerAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
        Reset();
    }

    void Reset()
    {
        check_timer = 1000;
    }

    void Aggro(Unit *)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (pInstance->GetData(TYPE_CHESS) != IN_PROGRESS)
            CAST_SUM(me)->UnSummon();

        if (check_timer < diff)
        {
            if (me->isSummon() && (!CAST_SUM(me)->GetSummoner()->IsAlive() || CAST_SUM(me)->GetSummoner()->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE)))
            {
                CAST_SUM(me)->UnSummon();
            }

            check_timer = 1000;
        }
        else
            check_timer -= diff;
    }
};

struct move_triggerAI : public ScriptedAI
{
    ScriptedInstance* pInstance;
    uint32 search_timer;

    move_triggerAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
        Reset();
    }

    void Reset()
    {
        search_timer = 4500;
    }

    void Aggro(Unit *)
    {
    }

    bool HasMoveMarker()
    {
        std::list<Unit*> MarkerList;
        Oregon::AllCreaturesOfEntryInRange u_check(me, 22519, 2);
        Oregon::UnitListSearcher<Oregon::AllCreaturesOfEntryInRange> searcher(MarkerList, u_check);
        me->GetMap()->VisitAll(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), searcher);
        return !MarkerList.empty();
    }

    void UpdateAI(const uint32 diff)
    {
        if (pInstance->GetData(TYPE_CHESS) != IN_PROGRESS)
            return;

        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE))
        {
            if (search_timer < diff)
            {
                if (!HasMoveMarker())
                    me->ToCreature()->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

                search_timer = 2000;
            }
            else
                search_timer -= diff;
        }
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (spell->Id == SPELL_TRANSFORM_FIELD)
        {
            search_timer = 3000;
            me->ToCreature()->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
        }
    }
};

struct Echo_of_MedivhAI : public ScriptedAI
{
    ScriptedInstance* pInstance;
    uint32 cheat_timer;
    uint32 cheat_block;
    std::list<uint64> ControlledCreatureGuid;
    uint32 PlayerControlledFaction;

    Echo_of_MedivhAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
        Reset();
    }

    std::list<Unit*> FindEnemyChessCreatures(uint32 count, uint32 enemyfaction)
    {
        std::list<Unit*> PieceList;
        uint32 SearchEntry;
        for (int i = 0; i < 6; i++)
        {
            SearchEntry = enemyfaction == ALLIANCE_CREATURE_FACTION ? ChessPieceEntrysAlliance[i] : ChessPieceEntrysHorde[i];
            std::list<Unit*> UnitList;
            Oregon::AllCreaturesOfEntryInRange u_check(me, SearchEntry, 150);
            Oregon::UnitListSearcher<Oregon::AllCreaturesOfEntryInRange> searcher(UnitList, u_check);
            me->GetMap()->VisitAll(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), searcher);

            for (std::list<Unit*>::iterator itr = UnitList.begin(); itr != UnitList.end(); itr++)
            if ((*itr) && (*itr)->IsAlive() && !(*itr)->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                PieceList.push_back(*itr);
        }
        Oregon::RandomResizeList<Unit*>(PieceList, count);
        return PieceList;
    }

    uint32 GetPlayerControlledFaction()
    {
        if (PlayerControlledFaction)
            return PlayerControlledFaction;

        Map::PlayerList const &PlList = me->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
        {
            if (i->getSource())
            {
                if (i->getSource()->GetTeam() == ALLIANCE)
                    PlayerControlledFaction = ALLIANCE_CREATURE_FACTION;
                else
                    PlayerControlledFaction = HORDE_CREATURE_FACTION;
            }
        }
        return 0;
    }

    void Reset()
    {
        cheat_timer = 5000;
        cheat_block = 60000; //first 60s no cheat
        ControlledCreatureGuid.clear();
        PlayerControlledFaction = 0;
    }

    void UpdateAI(const uint32 diff)
    {
        if (pInstance->GetData(TYPE_CHESS) == IN_PROGRESS)
        {
            if (cheat_block < diff)
            {
                if (cheat_timer < diff)
                {
                    if (urand(0, 100) < 20) //5% chance orginal
                    {
                        std::list<Unit*> Targets;
                        uint32 spellid = 0;
                        //handle cheat here
                        switch (urand(0, 2))
                        {
                            case 0: //fury of medivh
                                {
                                    if (GetPlayerControlledFaction() == ALLIANCE_CREATURE_FACTION)
                                        Targets = FindEnemyChessCreatures(1, ALLIANCE_CREATURE_FACTION);
                                    else
                                        Targets = FindEnemyChessCreatures(1, HORDE_CREATURE_FACTION);
                                    spellid = SPELL_FURY_OF_MEDIVH;
                                    break;
                                }
                            case 1: //hand of medivh
                                {
                                    if (GetPlayerControlledFaction() == ALLIANCE_CREATURE_FACTION)
                                        Targets = FindEnemyChessCreatures(1, HORDE_CREATURE_FACTION);
                                    else
                                        Targets = FindEnemyChessCreatures(1, ALLIANCE_CREATURE_FACTION);
                                    spellid = SPELL_HAND_OF_MEDIVH;
                                    break;
                                }
                            case 2: //heal of medivh
                                {
                                    if (GetPlayerControlledFaction() == ALLIANCE_CREATURE_FACTION)
                                        Targets = FindEnemyChessCreatures(1, HORDE_CREATURE_FACTION);
                                    else
                                        Targets = FindEnemyChessCreatures(1, ALLIANCE_CREATURE_FACTION);
                                    if (!Targets.empty())
                                        (*Targets.begin())->SetHealth((*Targets.begin())->GetMaxHealth());
                                    break;
                                }
                        }
                        if (spellid && !Targets.empty())
                        {
                            for (std::list<Unit*>::iterator itr = Targets.begin(); itr != Targets.end(); itr++)
                            {
                                if (spellid == SPELL_FURY_OF_MEDIVH)
                                {
                                    Unit* tmpSummon = me->SummonCreature(NPC_FURY_OF_MEDIVH_VISUAL, (*itr)->GetPositionX(), (*itr)->GetPositionY(), (*itr)->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
                                    tmpSummon->setFaction(GetPlayerControlledFaction() == ALLIANCE_CREATURE_FACTION ? HORDE_CREATURE_FACTION : ALLIANCE_CREATURE_FACTION);
                                    tmpSummon->CastSpell(me, SPELL_FURY_OF_MEDIVH, true);
                                }
                                else
                                    me->CastSpell(*itr, spellid, true);
                            }
                        }
                        DoMedivhEventSay(pInstance, me, CHEAT);
                        cheat_block = 30000; //allow 30s no cheat
                    }
                    cheat_timer = 5000;
                }
                else
                    cheat_timer -= diff;
            }
            else
                cheat_block -= diff;
        }
    }
};

struct chess_npcAI : public Scripted_NoMovementAI
{
    Creature *npc_medivh;
    bool initSpells;
    bool initFlags;
    bool setAITimer;
    bool won;
    uint32 m_checkTimer;
    uint32 m_AITimer;
    ScriptedInstance* pInstance;
    Unit* pCharmer;
    ChessPieceSpell m_spells[2];
    std::list<uint32> AvailableSpells;

    chess_npcAI(Creature *c) : Scripted_NoMovementAI(c)
    {
        pInstance = c->GetInstanceData();
        pCharmer = NULL;
    }

    void Reset()
    {
        me->setActive(true);
        me->SetReactState(REACT_PASSIVE);
        m_AITimer = 5000;
        initSpells = false;
        initFlags = false;
        setAITimer = false;
    }

    Echo_of_MedivhAI* GetMedivh()
    {
        if (!pInstance->GetData64(DATA_ECHO_OF_MEDIVH))
            return NULL;

        Creature* Medivh = me->GetCreature(*me, pInstance->GetData64(DATA_ECHO_OF_MEDIVH));

        if (!Medivh)
            return NULL;

        return ((Echo_of_MedivhAI*)Medivh->AI());
    }

    void JustRespawned()
    {
        if (pInstance && pInstance->GetData(TYPE_CHESS) == IN_PROGRESS)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            if (me->getFaction() == ALLIANCE_CREATURE_FACTION)
                me->Relocate(CHESS_PIECEBAR_ALLIANCE[0], CHESS_PIECEBAR_ALLIANCE[1], CHESS_PIECEBAR_ALLIANCE[2]);
            else
                me->Relocate(CHESS_PIECEBAR_HORDE[0], CHESS_PIECEBAR_HORDE[1], CHESS_PIECEBAR_HORDE[2]);

            Unit* Statusbar = me->FindNearestCreature(NPC_PIECE_BAR, 100, true);
            if (Statusbar)
            {
                me->Relocate(Statusbar->GetPositionX(), Statusbar->GetPositionY(), Statusbar->GetPositionZ());
                me->SetOrientation(Statusbar->GetOrientation());
                me->Kill(Statusbar, false);
            }

            me->GetMap()->CreatureRelocation(me, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());

            //notify clients for relocation
            me->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());

            /* old code */
            //notify clients for relocation
            //WorldPacket data;
            //me->BuildTeleportAckMsg(&data, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
            //me->SendMessageToSet(&data, false);

            if (me->HasAura(SPELL_ATTACK_TIMER, 0))
                me->RemoveAurasDueToSpell(SPELL_ATTACK_TIMER);
        }
    }

    void JustDied(Unit *Killer)
    {
        HandleMedivh_PieceKilledUnit(me);

        if (me->GetEntry() == NPC_KING_H || me->GetEntry() == NPC_KING_A)
        {
            if (pInstance)
                pInstance->SetData(TYPE_CHESS, DONE);

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            //Killer->CastSpell(Killer, SPELL_WIN, true);
            //Killer->CastSpell(Killer, SPELL_GAME_OVER, true);
            DespawnChess();
            Killer->SummonCreature(NPC_VICTORY_CONTROLER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 60000);

            //me->ForcedDespawn();
            RemoveControlledPieceForAllPlayers();
        }

        if (pInstance && pInstance->GetData(TYPE_CHESS) == IN_PROGRESS)
            me->Respawn();
    }

    void EnterEvadeMode()
    {
    }

    void OnCharmed(bool apply)
    {
        if (apply)
        {
            me->InitCharmInfo();
            me->GetCharmInfo()->InitPossessCreateSpells(false);
            pCharmer = me->GetCharmer();

            Echo_of_MedivhAI* Medivh = GetMedivh();
            if (Medivh)
                Medivh->ControlledCreatureGuid.insert(Medivh->ControlledCreatureGuid.end(), me->GetGUID());
        }
        else
        {
            if (pCharmer)
                pCharmer->RemoveAurasDueToSpellByCancel(SPELL_CONTROL_PIECE);

            pCharmer = NULL;

            Echo_of_MedivhAI* Medivh = GetMedivh();
            if (Medivh)
                Medivh->ControlledCreatureGuid.remove(me->GetGUID());
        }
    }

    void KilledUnit(Unit* victim)
    {
    }

    void DespawnChess()
    {
        uint32 searchEntry;
        std::list<Unit*> UnitList;
        for (int i = 0; i < 12; i++)
        {
            if (i < 6)
                searchEntry = ChessPieceEntrysAlliance[i];
            else
                searchEntry = ChessPieceEntrysHorde[i - 6];

            Oregon::AllCreaturesOfEntryInRange u_check(me, searchEntry, 100);
            Oregon::UnitListSearcher<Oregon::AllCreaturesOfEntryInRange> searcher(UnitList, u_check);
            me->GetMap()->VisitAll(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), searcher);
            for (std::list<Unit*>::iterator itr = UnitList.begin(); itr != UnitList.end(); itr++)
            {
                //(*itr)->RemoveAurasDueToSpell(32226);
                (*itr)->ToCreature()->ForcedDespawn(urand(10, 20) * 1000);
            }
        }
    }

    bool IsMedivhControlled(Unit* who)
    {
        Echo_of_MedivhAI* Medivh = GetMedivh();

        if (!Medivh)
            return false;

        if (Medivh->GetPlayerControlledFaction() == ALLIANCE_CREATURE_FACTION && who->getFaction() == HORDE_CREATURE_FACTION)
            return true;

        if (Medivh->GetPlayerControlledFaction() == HORDE_CREATURE_FACTION && who->getFaction() == ALLIANCE_CREATURE_FACTION)
            return true;

        return false;
    }

    void HandleMedivh_PieceKilledUnit(Unit* who)
    {
        bool PlayerUnit = !IsMedivhControlled(who);
        switch (who->GetEntry())
        {
        case NPC_KING_A:
        case NPC_KING_H:
            DoMedivhEventSay(pInstance, me, PlayerUnit ? PLAYER_LOST_KING : MEDIVH_LOST_KING);
            break;
        case NPC_ROOK_H:
        case NPC_ROOK_A:
            DoMedivhEventSay(pInstance, me, PlayerUnit ? PLAYER_LOST_ROOK : MEDIVH_LOST_ROOK);
            break;
        case NPC_BISHOP_H:
        case NPC_BISHOP_A:
            DoMedivhEventSay(pInstance, me, PlayerUnit ? PLAYER_LOST_BISHOP : MEDIVH_LOST_BISHOP);
            break;
        case NPC_QUEEN_A:
        case NPC_QUEEN_H:
            DoMedivhEventSay(pInstance, me, PlayerUnit ? PLAYER_LOST_QUEEN : MEDIVH_LOST_QUEEN);
            break;
        case NPC_KNIGHT_A:
        case NPC_KNIGHT_H:
            DoMedivhEventSay(pInstance, me, PlayerUnit ? PLAYER_LOST_KNIGHT : MEDIVH_LOST_KNIGHT);
            break;
        case NPC_PAWN_A:
        case NPC_PAWN_H:
            DoMedivhEventSay(pInstance, me, PlayerUnit ? PLAYER_LOST_PAWN : MEDIVH_LOST_PAWN);
            break;
        }
    }

    void RemoveControlledPieceForAllPlayers()
    {
        Map::PlayerList const &PlList = me->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
        {
            i->getSource()->RemoveAurasDueToSpellByCancel(SPELL_CONTROL_PIECE);
        }
    }

    Unit* FindNearestEnemyChessPiece()
    {
        float d = 100.0f;
        Unit* target = NULL;
        Unit* tester = NULL;
        uint32 searchEntry;
        uint32 npcFaction;

        if (me->getFaction() == HORDE_CREATURE_FACTION)
            npcFaction = ALLIANCE_CREATURE_FACTION;
        else
            npcFaction = HORDE_CREATURE_FACTION;

        //handle AI units
        for (int i = 0; i < 6; i++)
        {
            if (npcFaction == ALLIANCE_CREATURE_FACTION)
                searchEntry = ChessPieceEntrysAlliance[i];
            else
                searchEntry = ChessPieceEntrysHorde[i];

            tester = GetClosestCreatureWithEntry(me, searchEntry, 100);
            if (tester)
            {
                if ((!target || me->GetDistance(tester) < d) && !tester->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                {
                    target = tester;
                    d = me->GetDistance(target);
                }
            }
        }
        // handle possessed units
        Echo_of_MedivhAI* Medivh = GetMedivh();
        if (Medivh)
        {
            for (std::list<uint64>::iterator itr = Medivh->ControlledCreatureGuid.begin(); itr != Medivh->ControlledCreatureGuid.end(); itr++)
            {
                tester = Unit::GetUnit(*me, *itr);
                if (tester)
                {
                    if (Medivh->GetPlayerControlledFaction() == npcFaction)
                    {
                        if ((!target || me->GetDistance(tester) < d) && !tester->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                        {
                            target = tester;
                            d = me->GetDistance(target);
                        }
                    }
                }
            }
        }

        return target;
    }

    Unit* FindFriendlyChessPieceForHeal(uint32 hpdiv)
    {
        uint32 searchEntry;
        std::list<Unit*> UnitList;
        for (int i = 0; i < 6; i++)
        {
            if (me->getFaction() == ALLIANCE_CREATURE_FACTION)
                searchEntry = ChessPieceEntrysAlliance[i];
            else
                searchEntry = ChessPieceEntrysHorde[i];

            Oregon::AllCreaturesOfEntryInRange u_check(me, searchEntry, 100);
            Oregon::UnitListSearcher<Oregon::AllCreaturesOfEntryInRange> searcher(UnitList, u_check);
            me->GetMap()->VisitAll(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), searcher);
            for (std::list<Unit*>::iterator itr = UnitList.begin(); itr != UnitList.end(); itr++)
            {
                if ((*itr)->IsAlive() && (*itr)->GetMaxHealth() - (*itr)->GetHealth() >= hpdiv && (*itr)->getFaction() == me->getFaction())
                    return (*itr);
            }
        }
        return NULL;
    }

    bool SpellIsInRange(int idx, Unit* target)
    {
        uint32 spellID;
        const SpellEntry* proto;
        const SpellRangeEntry* EffectRange;
        const SpellRadiusEntry* EffectRadius;

        spellID = m_spells[idx].m_spellEntry;
        if (spellID && !m_spells[idx].m_positiv) //only check negativ targets here
        if (proto = sSpellStore.LookupEntry(spellID))
        {
            for (int effIdx = 0; effIdx < 3; effIdx++)
            {
                if (proto->SchoolMask == SPELL_SCHOOL_MASK_NORMAL && me->GetDistance(target) < 4)
                    return true;
                if (proto->SchoolMask != SPELL_SCHOOL_MASK_NORMAL)
                if (EffectRange = sSpellRangeStore.LookupEntry(proto->rangeIndex))
                {
                    if (proto->Effect[effIdx] != 0 && proto->rangeIndex != 13 && me->GetDistance(target) <= EffectRange->maxRange)
                        return true;
                    else if (proto->Effect[effIdx] != 0 && proto->rangeIndex == 13)
                    if (EffectRadius = sSpellRadiusStore.LookupEntry(proto->EffectRadiusIndex[effIdx]))
                    if (me->GetDistance(target) <= EffectRadius->radiusHostile)
                        return true;
                }
            }
        }
        return false;
    }

    bool IsWithinSpellRange(Unit* target)
    {
        if (!target)
            return false;

        for (int i = 0; i <= 1; i++)
        {
            if (SpellIsInRange(i, target))
                return true;
        }

        return false;
    }

    void InitializePieceSpells()
    {
        bool positiv = false;
        for (int i = 2; i < CREATURE_MAX_SPELLS; i++) //check all abilitys (except move and change facing || 0 & 1)
        {
            switch (me->m_spells[i])
            {
                //positiv spells : default false
            case 37432:
            case 37434:
            case 37414:
            case 37416:
            case 37472:
            case 37471:
            case 37456:
            case 37455:
                positiv = true;
                break;
            default:
                positiv = false;
            }
            m_spells[i - 2].m_positiv = positiv;
            m_spells[i - 2].m_spellEntry = me->m_spells[i];
        }
    }

    void AddSpellCooldown(uint32 Entry)
    {
        if (const SpellEntry* proto = sSpellStore.LookupEntry(Entry))
        {
            uint32 cooldown = proto->RecoveryTime;
            time_t cd = time(NULL) + cooldown / IN_MILLISECONDS;
            me->ToCreature()->_AddCreatureSpellCooldown(Entry, cd);
            me->ToCreature()->_AddCreatureCategoryCooldown(Entry, cd);
        }
    }

    void UpdateSpellList(Unit* target)
    {
        AvailableSpells.clear();
        for (int i = 0; i <= 1; i++)
        {
            if (SpellIsInRange(i, target) && !me->ToCreature()->HasSpellCooldown(m_spells[i].m_spellEntry))
            {
                AvailableSpells.push_back(m_spells[i].m_spellEntry);
            }
        }
    }

    bool HasPositivAvailableSpell()
    {
        for (int i = 0; i < 1; i++)
        if (m_spells[i].m_positiv && !me->ToCreature()->HasSpellCooldown(m_spells[i].m_spellEntry))
            return true;

        return false;
    }

    void HandleCastHeal(Unit* who)
    {
        if (me->GetEntry() == NPC_BISHOP_A || me->GetEntry() == NPC_BISHOP_H)
        {
            if (!who)
                who = FindFriendlyChessPieceForHeal(10000);

            if (who)
            {
                me->CastSpell(who, m_spells[0].m_spellEntry, false); //heal is 3rd spell so 0 in our store (leftout move and face)
                AddSpellCooldown(m_spells[0].m_spellEntry);
            }
        }
    }

    void CastPositivSpell()
    {
        //only have 1 positiv Spell
        for (int i = 0; i < 1; i++)
        {
            if (m_spells[i].m_positiv)
            {
                if (me->GetEntry() == NPC_BISHOP_H || me->GetEntry() == NPC_BISHOP_A)
                    HandleCastHeal(NULL);
                else
                    me->CastSpell(me, m_spells[i].m_spellEntry, false);
                break;
            }
        }
    }

    void HandleCastSpellForPiece(Unit* who)
    {
        //handle all negativ spells here propably get available spells, do rnd of max size and target nearest creature
        if (!AvailableSpells.empty())
        {
            std::list<uint32>::iterator itr = AvailableSpells.begin();
            advance(itr, urand(0, AvailableSpells.size() - 1));
            uint32 SpellEntry = (*itr);
            if (SpellEntry && !me->HasSpellCooldown(SpellEntry))
            {
                me->CastSpell(who, SpellEntry, false);
                AddSpellCooldown(SpellEntry);
            }
        }
    }

    bool SpecialAI()
    {
        //for instance handle Bishop Heal here
        switch (me->GetEntry())
        {
            case NPC_BISHOP_H:
            case NPC_BISHOP_A:
                {
                    Unit* HealTarget = FindFriendlyChessPieceForHeal(20000);
                    if (HealTarget && HasPositivAvailableSpell())
                    {
                        HandleCastHeal(HealTarget);
                        return true; //handled Action here
                    }
                }
        }
        return false;
    }

    Unit* FindChessFieldForMovement(Unit* target)
    {
        const SpellEntry* MoveSpell = sSpellStore.LookupEntry(me->m_spells[0]);
        float SpellRange = sSpellRangeStore.LookupEntry(MoveSpell->rangeIndex)->maxRange - sSpellRadiusStore.LookupEntry(MoveSpell->EffectRadiusIndex[0])->radiusHostile;
        std::list<Unit*> BlackFieldList;
        //Search for Black Fields
        Oregon::AllCreaturesOfEntryInRange u_check(me, NPC_BLACK_SQUARE, SpellRange);
        Oregon::UnitListSearcher<Oregon::AllCreaturesOfEntryInRange> searcher(BlackFieldList, u_check);
        me->GetMap()->VisitAll(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), searcher);

        //Search for White Fields
        std::list<Unit*> WhiteFieldList;
        Oregon::AllCreaturesOfEntryInRange check(me, NPC_WHITE_SQUARE, SpellRange);
        Oregon::UnitListSearcher<Oregon::AllCreaturesOfEntryInRange> _searcher(WhiteFieldList, check);
        me->GetMap()->VisitAll(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), _searcher);

        Unit* Victim = NULL;
        float d = 100.0f;

        for (std::list<Unit*>::iterator itr = BlackFieldList.begin(); itr != BlackFieldList.end(); itr++)
        {
            if ((*itr)->GetDistance(target) < d && !(*itr)->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE))
            {
                Victim = (*itr);
                d = (*itr)->GetDistance(target);
            }
        }
        //Now Check for closer White Fields
        for (std::list<Unit*>::iterator itr = WhiteFieldList.begin(); itr != WhiteFieldList.end(); itr++)
        {
            if ((*itr)->GetDistance(target) < d && !(*itr)->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE))
            {
                Victim = (*itr);
                d = (*itr)->GetDistance(target);
            }
        }
        return Victim;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!me->IsInCombat() && pInstance->GetData(TYPE_CHESS) == IN_PROGRESS)
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT); //always stay in combat, don't reset health

        if (pInstance->GetData(TYPE_CHESS) == IN_PROGRESS && !initSpells)  //do here before we handle ai
        {
            InitializePieceSpells();
            initSpells = true;
        }

        if (pInstance->GetData(TYPE_CHESS) != IN_PROGRESS && !initFlags)
        {
            if (me->isCharmed())
                me->RemoveCharmAuras();

            if (me->HasAura(SPELL_ATTACK_TIMER, 0))
                me->RemoveAurasDueToSpell(SPELL_ATTACK_TIMER);

            if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            initFlags = true;
        }

        if (pInstance->GetData(TYPE_CHESS) == IN_PROGRESS && !me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
        {
            initFlags = false;

            if (!me->HasAura(SPELL_ATTACK_TIMER, 0))
                me->CastSpell(me, SPELL_ATTACK_TIMER, true); //do this here since it buggs a bit in StartChessEvent function

            //Handle this in deactivate own field - with map scripts
            if (!setAITimer)
            {
                m_AITimer = urand(0, 5) * 1000;
                setAITimer = true;
            }

            //############################# AI ##################################
            //# Current logic - every 5 Seconds Chess Piece can do 1 Action     #
            //# If we have a target in Range of one of our Abbilities use it    #
            //# If we don't have a target in Range, do rnd and 25% we'll move   #
            //# Search for nearest target, and than find ChessField in Range    #
            //# Of our Movement Spell, which is closest to our target and move  #
            //#                                                                 #
            //# Ofc - if we have Unit in Range and it's not in front of us turn #
            //# How does AI of player side behaves? For sure no self movement   #
            //###################################################################

            if (me->isCharmed())
                return;

            Echo_of_MedivhAI* Medivh = GetMedivh();

            if (Medivh && Medivh->GetPlayerControlledFaction() != me->getFaction() && m_AITimer < diff) //5 Second timer
            {
                Unit* target = FindNearestEnemyChessPiece(); //Handles both sides
                if (target) //found target
                {
                    if (IsWithinSpellRange(target) && !SpecialAI()) //do cast spell -- only check negativ spells, don't have to be in range for heal or buffs we'll handel this later
                    {
                        //at least one spell is in range to cast
                        if (me->GetDistance(target) < 3 && !me->ToCreature()->HasSpellCooldown(SPELL_CHANGE_FACING))
                        {
                            me->CastSpell(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), SPELL_CHANGE_FACING, false); //target nearby enemy
                            AddSpellCooldown(SPELL_CHANGE_FACING);
                        }

                        UpdateSpellList(target);

                        if (HasPositivAvailableSpell() && urand(0, 100) < 10)
                            CastPositivSpell();
                        else if (urand(0, 100) < 25)
                            HandleCastSpellForPiece(target);
                    }
                    else
                    {
                        if (urand(0, 100) < 20) //20% Chance to move
                        {
                            Unit* ChessField = FindChessFieldForMovement(target);
                            //apply find enemy aura here for spell visual
                            if (ChessField)
                            {
                                if (!me->ToCreature()->HasSpellCooldown(me->m_spells[0]) && ChessField->GetDistance(target) < me->GetDistance(target))
                                {
                                    me->CastSpell(ChessField->GetPositionX(), ChessField->GetPositionY(), ChessField->GetPositionZ(), me->m_spells[0], false);
                                }
                            }
                        }

                        if (urand(0, 100) < 10) //10% chance to do positiv spell
                        {
                            if (HasPositivAvailableSpell())
                                CastPositivSpell();
                        }
                    }
                }
                m_AITimer = 5000;
            }
            else
                m_AITimer -= diff;
        }
    }

    void AttackStart(Unit *)
    {
    }
};

struct victory_controlerAI : ScriptedAI
{
    ScriptedInstance* pInstance;
    uint32 check_timer;
    bool checkedForChest;
    uint32 say_timer;
    bool said;

    victory_controlerAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
        Reset();
    }

    bool PlayerWon()
    {
        if (me->isSummon())
        {
            //player killed opposite king
            if (CAST_SUM(me)->GetSummoner() && CAST_SUM(me)->GetSummoner()->isCharmed())
                return true;

            if (!pInstance->GetData64(DATA_ECHO_OF_MEDIVH))
                return false;

            Creature* Medivh = me->GetCreature(*me, pInstance->GetData64(DATA_ECHO_OF_MEDIVH));

            if (!Medivh)
                return false;

            Echo_of_MedivhAI* MedivhAI = ((Echo_of_MedivhAI*)Medivh->AI());

            if (!MedivhAI)
                return false;

            //killer was from players side
            if (CAST_SUM(me)->GetSummoner() && CAST_SUM(me)->GetSummoner()->getFaction() == MedivhAI->GetPlayerControlledFaction())
                return true;
        }
        return false;
    }

    void Reset()
    {
        check_timer = 500;
        checkedForChest = false;
        say_timer = 20000;
        said = false;
    }

    void spawnChessVictoryDummy(int count)
    {
        for (int i = 0; i < count; i++)
        {
            float fac_x = urand(0, 1000) / 1000.0f;
            float fac_y = urand(0, 1000) / 1000.0f;
            float x = CHESS_BASE_VECTOR[0] + fac_x*CHESS_RIGHT_VECTOR[0] + fac_y*CHESS_UP_VECTOR[0];
            float y = CHESS_BASE_VECTOR[1] + fac_x*CHESS_RIGHT_VECTOR[1] + fac_y*CHESS_UP_VECTOR[1];
            float z = CHESS_BASE_VECTOR[2] + fac_x*CHESS_RIGHT_VECTOR[2] + fac_y*CHESS_UP_VECTOR[2];
            me->SummonCreature(NPC_VICTORY_DUMMY, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN, 4000);
            me->CastSpell(me, SPELL_CAMERA_SHAKE, true);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!checkedForChest)
        {
            if (PlayerWon() && me->FindNearestCreature(25213, 100, true))
                me->Kill(me->FindNearestCreature(25213, 100, true), false);

            /*if (!PlayerWon())
            pInstance->SetData(TYPE_CHESS, FAIL);*/

            checkedForChest = true;
        }

        if (check_timer < diff)
        {
            spawnChessVictoryDummy(1);
            check_timer = 150;
        }
        else
            check_timer -= diff;

        if (say_timer < diff && !said)
        {
            DoMedivhEventSay(pInstance, me, PlayerWon() ? MEDIVH_LOST : PLAYER_LOST);
            said = true;
        }
        else
            say_timer -= diff;
    }
};

bool GossipHello_chess_npc(Player* player, Creature* _Creature)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)_Creature->GetInstanceData());
    // TODO: show text if controlled a piece debuff

    std::string Gossip("Control ");
    Gossip += _Creature->GetName();

    bool CanControl;
    CanControl = true;

    if (!pInstance->GetData64(DATA_ECHO_OF_MEDIVH))
        return false;

    Creature* Medivh = _Creature->GetCreature(*_Creature, pInstance->GetData64(DATA_ECHO_OF_MEDIVH));

    if (!Medivh)
        return false;

    Echo_of_MedivhAI* MedivhAI = ((Echo_of_MedivhAI*)Medivh->AI());

    if (!MedivhAI)
        return false;

    if (player->GetAura(30529, 0)) //shortly controlled a piece debuff is up
        CanControl = false;

    if (pInstance->GetData(TYPE_CHESS) != IN_PROGRESS  && _Creature->GetEntry() != NPC_KING_H && _Creature->GetEntry() != NPC_KING_A)
        CanControl = false;

    if (MedivhAI->GetPlayerControlledFaction() == ALLIANCE_CREATURE_FACTION && _Creature->getFaction() != ALLIANCE_CREATURE_FACTION)
        CanControl = false;

    if (MedivhAI->GetPlayerControlledFaction() == HORDE_CREATURE_FACTION && _Creature->getFaction() != HORDE_CREATURE_FACTION)
        CanControl = false;

    if (_Creature->isPossessedByPlayer())
        CanControl = false;

    if (CanControl)
        player->ADD_GOSSIP_ITEM(0, Gossip, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    player->SEND_GOSSIP_MENU(8990, _Creature->GetGUID());

    return true;
}

Creature* GetClosestChessPiece(Unit* source)
{
    Creature* tar = NULL;
    for (int i = 0; i < 6; i++)
    {
        tar = GetClosestCreatureWithEntry(source, ChessPieceEntrysHorde[i], 0);
        if (!tar)
            tar = GetClosestCreatureWithEntry(source, ChessPieceEntrysAlliance[i], 0);
        if (tar)
            break;
    }
    return tar;
}

void HandleChessEmoteAndTransformForEntry(uint32 Id, Unit* source)
{
    std::list<Unit*> UnitList;
    Oregon::AllCreaturesOfEntryInRange u_check(source, Id, 100);
    Oregon::UnitListSearcher<Oregon::AllCreaturesOfEntryInRange> searcher(UnitList, u_check);
    source->GetMap()->VisitAll(source->GetPositionX(), source->GetPositionY(), source->GetPositionZ(), searcher);
    for (std::list<Unit*>::iterator itr = UnitList.begin(); itr != UnitList.end(); ++itr)
    {
        Unit* curr;
        if (curr = (*itr))
        {
            Creature* target = GetClosestChessPiece(curr);
            if (target)
            {
                target->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK1H);
                curr->CastSpell(curr, SPELL_DEACTIVATE_OWN_FIELD, true, 0, 0, target->GetGUID());
            }
        }
    }
}

bool GossipSelect_chess_npc(Player* player, Creature* _Creature, uint32 sender, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        ScriptedInstance* pInstance = ((ScriptedInstance*)_Creature->GetInstanceData());
        if (pInstance && _Creature)
        {
            player->CastSpell(_Creature, SPELL_CONTROL_PIECE, true);
            _Creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            if ((_Creature->GetEntry() == NPC_KING_A || _Creature->GetEntry() == NPC_KING_H) && pInstance->GetData(TYPE_CHESS) == NOT_STARTED)
            {
                pInstance->SetData(TYPE_CHESS, IN_PROGRESS);
                HandleChessEmoteAndTransformForEntry(NPC_WHITE_SQUARE, _Creature);
                HandleChessEmoteAndTransformForEntry(NPC_BLACK_SQUARE, _Creature);
                DoMedivhEventSay(pInstance, _Creature, START_GAME);
            }
        }
    }

    player->CLOSE_GOSSIP_MENU();
    return true;
}

/*void ReSpawnChess(Creature* me)
{
    uint32 searchEntry;
    std::list<Unit*> UnitList;
    for(int i = 0; i < 12; i++)
    {
        if(i < 6)
            searchEntry = ChessPieceEntrysAlliance[i];
        else
            searchEntry = ChessPieceEntrysHorde[i-6];

        Oregon::AllCreaturesOfEntryInRange u_check(me, searchEntry, 100);
        Oregon::UnitListSearcher<Oregon::AllCreaturesOfEntryInRange> searcher(UnitList, u_check);
        me->GetMap()->VisitAll(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(),searcher);
        for(std::list<Unit*>::iterator itr = UnitList.begin(); itr != UnitList.end(); itr++)
        {
            (*itr)->ToCreature()->Respawn();
        }
    }
}

bool GossipHello_echo_of_medivh(Player* player, Creature* _Creature)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)_Creature->GetInstanceData());

    std::string Gossip("Restart Chess Event");

    if(pInstance->GetData(TYPE_CHESS) == FAIL)
    player->ADD_GOSSIP_ITEM(0, Gossip, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    player->SEND_GOSSIP_MENU(8990, _Creature->GetGUID());

    return true;
}

bool GossipSelect_echo_of_medivh(Player* player, Creature* _Creature, uint32 sender, uint32 action)
{
    if(action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        ScriptedInstance* pInstance = ((ScriptedInstance*)_Creature->GetInstanceData());
        if(pInstance)
        {
            pInstance->SetData(TYPE_CHESS, NOT_STARTED);
            ReSpawnChess(_Creature);
        }
    }

    player->CLOSE_GOSSIP_MENU();
    return true;
}*/

CreatureAI* GetAI_chess_npc(Creature *_Creature)
{
    return new chess_npcAI(_Creature);
}

CreatureAI* GetAI_move_trigger(Creature *_Creature)
{
    return new move_triggerAI(_Creature);
}

CreatureAI* GetAI_move_marker(Creature *_Creature)
{
    return new move_markerAI(_Creature);
}

CreatureAI* GetAI_victory_controler(Creature *_Creature)
{
    return new victory_controlerAI(_Creature);
}

CreatureAI* GetAI_echo_of_medivh(Creature *_Creature)
{
    return new Echo_of_MedivhAI(_Creature);
}

void AddSC_chess_event()
{
    Script* newscript;

    newscript = new Script;
    newscript->GetAI = GetAI_chess_npc;
    newscript->Name = "chess_npc";
    newscript->pGossipHello = GossipHello_chess_npc;
    newscript->pGossipSelect = GossipSelect_chess_npc;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_echo_of_medivh";
    newscript->GetAI = GetAI_echo_of_medivh;
    //newscript->pGossipHello = GossipHello_echo_of_medivh;
    //newscript->pGossipSelect = GossipSelect_echo_of_medivh;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "chess_move_trigger";
    newscript->GetAI = GetAI_move_trigger;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "chess_move_marker";
    newscript->GetAI = GetAI_move_marker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "chess_victory_controler";
    newscript->GetAI = GetAI_victory_controler;
    newscript->RegisterSelf();
}