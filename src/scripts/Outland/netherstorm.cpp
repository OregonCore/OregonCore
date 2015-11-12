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
SDName: Netherstorm
SD%Complete: 90
SDComment: Quest support: 10337, 10438, 10652 (special flight paths), 10299,10321,10322,10323,10329,10330,10338,10365(Shutting Down Manaforge), 10198, 10191, 10924, 10221, 10310.
SDCategory: Netherstorm
EndScriptData */

/* ContentData
npc_manaforge_control_console
go_manaforge_control_console
npc_commander_dawnforge
at_commander_dawnforge
npc_professor_dabiri
npc_phase_hunter
npc_bessy
npc_maxx_a_million
npc_zeppit
npc_dr_boom
npc_boom_bot
npc_drijya
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"
#include "GridNotifiers.h"
#include "Cell.h"
#include "CellImpl.h"

/*######
## npc_manaforge_control_console
######*/

enum eManaforge
{

    EMOTE_START             = -1000211,
    EMOTE_60                = -1000212,
    EMOTE_30                = -1000213,
    EMOTE_10                = -1000214,
    EMOTE_COMPLETE          = -1000215,
    EMOTE_ABORT             = -1000216,

    ENTRY_BNAAR_C_CONSOLE   = 20209,
    ENTRY_CORUU_C_CONSOLE   = 20417,
    ENTRY_DURO_C_CONSOLE    = 20418,
    ENTRY_ARA_C_CONSOLE     = 20440,

    ENTRY_SUNFURY_TECH      = 20218,
    ENTRY_SUNFURY_PROT      = 20436,

    ENTRY_ARA_TECH          = 20438,
    ENTRY_ARA_ENGI          = 20439,
    ENTRY_ARA_GORKLONN      = 20460,

    SPELL_DISABLE_VISUAL    = 35031,
    SPELL_INTERRUPT_1       = 35016,                      //ACID mobs should cast this
    SPELL_INTERRUPT_2       = 35176                       //ACID mobs should cast this (Manaforge Ara-version)
};

struct npc_manaforge_control_consoleAI : public ScriptedAI
{
    npc_manaforge_control_consoleAI(Creature* c) : ScriptedAI(c) {}

    uint32 Event_Timer;
    uint32 Wave_Timer;
    uint32 Phase;
    bool Wave;
    uint64 someplayer;
    uint64 goConsole;
    Creature* add;

    void Reset()
    {
        Event_Timer = 3000;
        Wave_Timer = 0;
        Phase = 1;
        Wave = false;
        someplayer = 0;
        goConsole = 0;
        add = NULL;
    }

    void EnterCombat(Unit* /*who*/)
    {
        return;
    }

    /*void SpellHit(Unit* caster, const SpellEntry *spell)
    {
        //we have no way of telling the creature was hit by spell -> got aura applied after 10-12 seconds
        //then no way for the mobs to actually stop the shutdown as intended.
        if (spell->Id == SPELL_INTERRUPT_1)
            DoSay("Silence! I kill you!",LANG_UNIVERSAL, NULL);
    }*/

    void JustDied(Unit* /*killer*/)
    {
        DoScriptText(EMOTE_ABORT, me);

        if (someplayer)
        {
            Player* p = Unit::GetPlayer(*me, someplayer);
            if (p)
            {
                switch (me->GetEntry())
                {
                case ENTRY_BNAAR_C_CONSOLE:
                    p->FailQuest(10299);
                    p->FailQuest(10329);
                    break;
                case ENTRY_CORUU_C_CONSOLE:
                    p->FailQuest(10321);
                    p->FailQuest(10330);
                    break;
                case ENTRY_DURO_C_CONSOLE:
                    p->FailQuest(10322);
                    p->FailQuest(10338);
                    break;
                case ENTRY_ARA_C_CONSOLE:
                    p->FailQuest(10323);
                    p->FailQuest(10365);
                    break;
                }
            }
        }

        if (goConsole)
        {
            if (GameObject* go = GameObject::GetGameObject((*me), goConsole))
                go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
        }
    }

    void DoWaveSpawnForCreature(Creature* creature)
    {
        switch (creature->GetEntry())
        {
        case ENTRY_BNAAR_C_CONSOLE:
            if (rand() % 2)
            {
                add = me->SummonCreature(ENTRY_SUNFURY_TECH, 2933.68f, 4162.55f, 164.00f, 1.60f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                if (add) add->GetMotionMaster()->MovePoint(0, 2927.36f, 4212.97f, 164.00f);
            }
            else
            {
                add = me->SummonCreature(ENTRY_SUNFURY_TECH, 2927.36f, 4212.97f, 164.00f, 4.94f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                if (add) add->GetMotionMaster()->MovePoint(0, 2933.68f, 4162.55f, 164.00f);
            }
            Wave_Timer = 30000;
            break;
        case ENTRY_CORUU_C_CONSOLE:
            add = me->SummonCreature(ENTRY_SUNFURY_TECH, 2445.21f, 2765.26f, 134.49f, 3.93f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
            if (add) add->GetMotionMaster()->MovePoint(0, 2424.21f, 2740.15f, 133.81f);
            add = me->SummonCreature(ENTRY_SUNFURY_TECH, 2429.86f, 2731.85f, 134.53f, 1.31f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
            if (add) add->GetMotionMaster()->MovePoint(0, 2435.37f, 2766.04f, 133.81f);
            Wave_Timer = 20000;
            break;
        case ENTRY_DURO_C_CONSOLE:
            add = me->SummonCreature(ENTRY_SUNFURY_TECH, 2986.80f, 2205.36f, 165.37f, 3.74f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
            if (add) add->GetMotionMaster()->MovePoint(0, 2985.15f, 2197.32f, 164.79f);
            add = me->SummonCreature(ENTRY_SUNFURY_TECH, 2952.91f, 2191.20f, 165.32f, 0.22f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
            if (add) add->GetMotionMaster()->MovePoint(0, 2060.01f, 2185.27f, 164.67f);
            Wave_Timer = 15000;
            break;
        case ENTRY_ARA_C_CONSOLE:
            if (rand() % 2)
            {
                add = me->SummonCreature(ENTRY_ARA_TECH, 4035.11f, 4038.97f, 194.27f, 2.57f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                if (add) add->GetMotionMaster()->MovePoint(0, 4003.42f, 4040.19f, 193.49f);
                add = me->SummonCreature(ENTRY_ARA_TECH, 4033.66f, 4036.79f, 194.28f, 2.57f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                if (add) add->GetMotionMaster()->MovePoint(0, 4003.42f, 4040.19f, 193.49f);
                add = me->SummonCreature(ENTRY_ARA_TECH, 4037.13f, 4037.30f, 194.23f, 2.57f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                if (add) add->GetMotionMaster()->MovePoint(0, 4003.42f, 4040.19f, 193.49f);
            }
            else
            {
                add = me->SummonCreature(ENTRY_ARA_TECH, 3099.59f, 4049.30f, 194.22f, 0.05f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                if (add) add->GetMotionMaster()->MovePoint(0, 4028.01f, 4035.17f, 193.59f);
                add = me->SummonCreature(ENTRY_ARA_TECH, 3999.72f, 4046.75f, 194.22f, 0.05f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                if (add) add->GetMotionMaster()->MovePoint(0, 4028.01f, 4035.17f, 193.59f);
                add = me->SummonCreature(ENTRY_ARA_TECH, 3996.81f, 4048.26f, 194.22f, 0.05f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                if (add) add->GetMotionMaster()->MovePoint(0, 4028.01f, 4035.17f, 193.59f);
            }
            Wave_Timer = 15000;
            break;
        }
    }
    void DoFinalSpawnForCreature(Creature* creature)
    {
        switch (creature->GetEntry())
        {
        case ENTRY_BNAAR_C_CONSOLE:
            add = me->SummonCreature(ENTRY_SUNFURY_TECH, 2946.52f, 4201.42f, 163.47f, 3.54f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
            if (add) add->GetMotionMaster()->MovePoint(0, 2927.49f, 4192.81f, 163.00f);
            break;
        case ENTRY_CORUU_C_CONSOLE:
            add = me->SummonCreature(ENTRY_SUNFURY_TECH, 2453.88f, 2737.85f, 133.27f, 2.59f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
            if (add) add->GetMotionMaster()->MovePoint(0, 2433.96f, 2751.53f, 133.85f);
            add = me->SummonCreature(ENTRY_SUNFURY_TECH, 2441.62f, 2735.32f, 134.49f, 1.97f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
            if (add) add->GetMotionMaster()->MovePoint(0, 2433.96f, 2751.53f, 133.85f);
            add = me->SummonCreature(ENTRY_SUNFURY_TECH, 2450.73f, 2754.50f, 134.49f, 3.29f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
            if (add) add->GetMotionMaster()->MovePoint(0, 2433.96f, 2751.53f, 133.85f);
            break;
        case ENTRY_DURO_C_CONSOLE:
            add = me->SummonCreature(ENTRY_SUNFURY_TECH, 2956.18f, 2202.85f, 165.32f, 5.45f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
            if (add) add->GetMotionMaster()->MovePoint(0, 2972.27f, 2193.22f, 164.48f);
            add = me->SummonCreature(ENTRY_SUNFURY_TECH, 2975.30f, 2211.50f, 165.32f, 4.55f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
            if (add) add->GetMotionMaster()->MovePoint(0, 2972.27f, 2193.22f, 164.48f);
            add = me->SummonCreature(ENTRY_SUNFURY_PROT, 2965.02f, 2217.45f, 164.16f, 4.96f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
            if (add) add->GetMotionMaster()->MovePoint(0, 2972.27f, 2193.22f, 164.48f);
            break;
        case ENTRY_ARA_C_CONSOLE:
            add = me->SummonCreature(ENTRY_ARA_ENGI, 3994.51f, 4020.46f, 192.18f, 0.91f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
            if (add) add->GetMotionMaster()->MovePoint(0, 4008.35f, 4035.04f, 192.70f);
            add = me->SummonCreature(ENTRY_ARA_GORKLONN, 4021.56f, 4059.35f, 193.59f, 4.44f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
            if (add) add->GetMotionMaster()->MovePoint(0, 4016.62f, 4039.89f, 193.46f);
            break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (Event_Timer <= diff)
        {
            switch (Phase)
            {
            case 1:
                if (someplayer)
                {
                    Unit* u = Unit::GetUnit((*me), someplayer);
                    if (u && u->GetTypeId() == TYPEID_PLAYER) DoScriptText(EMOTE_START, me, u);
                }
                Event_Timer = 60000;
                Wave = true;
                ++Phase;
                break;
            case 2:
                DoScriptText(EMOTE_60, me);
                Event_Timer = 30000;
                ++Phase;
                break;
            case 3:
                DoScriptText(EMOTE_30, me);
                Event_Timer = 20000;
                DoFinalSpawnForCreature(me);
                ++Phase;
                break;
            case 4:
                DoScriptText(EMOTE_10, me);
                Event_Timer = 10000;
                Wave = false;
                ++Phase;
                break;
            case 5:
                DoScriptText(EMOTE_COMPLETE, me);
                if (someplayer)
                {
                    Player* player = Unit::GetPlayer(*me, someplayer);
                    if (player)
                        player->KilledMonsterCredit(me->GetEntry(), me->GetGUID());
                    DoCast(me, SPELL_DISABLE_VISUAL);
                }
                if (goConsole)
                {
                    if (GameObject* go = GameObject::GetGameObject((*me), goConsole))
                        go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
                }
                ++Phase;
                break;
            }
        }
        else Event_Timer -= diff;

        if (Wave)
        {
            if (Wave_Timer <= diff)
                DoWaveSpawnForCreature(me);
            else Wave_Timer -= diff;
        }
    }
};
CreatureAI* GetAI_npc_manaforge_control_console(Creature* pCreature)
{
    return new npc_manaforge_control_consoleAI (pCreature);
}

/*######
## go_manaforge_control_console
######*/

//@todo clean up this workaround when Oregon adds support to do it properly (with gossip selections instead of instant summon)
bool GOHello_go_manaforge_control_console(Player* player, GameObject* _GO)
{
    if (_GO->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER)
    {
        player->PrepareQuestMenu(_GO->GetGUID());
        player->SendPreparedQuest(_GO->GetGUID());
    }

    Creature* manaforge;
    manaforge = NULL;

    switch (_GO->GetAreaId())
    {
    case 3726:                                          //b'naar
        if ((player->GetQuestStatus(10299) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(10329) == QUEST_STATUS_INCOMPLETE) &&
            player->HasItemCount(29366, 1))
            manaforge = player->SummonCreature(ENTRY_BNAAR_C_CONSOLE, 2918.95f, 4189.98f, 161.88f, 0.34f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 125000);
        break;
    case 3730:                                          //coruu
        if ((player->GetQuestStatus(10321) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(10330) == QUEST_STATUS_INCOMPLETE) &&
            player->HasItemCount(29396, 1))
            manaforge = player->SummonCreature(ENTRY_CORUU_C_CONSOLE, 2426.77f, 2750.38f, 133.24f, 2.14f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 125000);
        break;
    case 3734:                                          //duro
        if ((player->GetQuestStatus(10322) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(10338) == QUEST_STATUS_INCOMPLETE) &&
            player->HasItemCount(29397, 1))
            manaforge = player->SummonCreature(ENTRY_DURO_C_CONSOLE, 2976.48f, 2183.29f, 163.20f, 1.85f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 125000);
        break;
    case 3722:                                          //ara
        if ((player->GetQuestStatus(10323) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(10365) == QUEST_STATUS_INCOMPLETE) &&
            player->HasItemCount(29411, 1))
            manaforge = player->SummonCreature(ENTRY_ARA_C_CONSOLE, 4013.71f, 4028.76f, 192.10f, 1.25f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 125000);
        break;
    }

    if (manaforge)
    {
        ((npc_manaforge_control_consoleAI*)manaforge->AI())->someplayer = player->GetGUID();
        ((npc_manaforge_control_consoleAI*)manaforge->AI())->goConsole = _GO->GetGUID();
        _GO->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
    }
    return true;
}

/*######
## npc_commander_dawnforge
######*/

// The Speech of Dawnforge, Ardonis & Pathaleon
enum eDawnforge
{
    SAY_COMMANDER_DAWNFORGE_1           = -1000128,
    SAY_ARCANIST_ARDONIS_1              = -1000129,
    SAY_COMMANDER_DAWNFORGE_2           = -1000130,
    SAY_PATHALEON_CULATOR_IMAGE_1       = -1000131,
    SAY_COMMANDER_DAWNFORGE_3           = -1000132,
    SAY_PATHALEON_CULATOR_IMAGE_2       = -1000133,
    SAY_PATHALEON_CULATOR_IMAGE_2_1     = -1000134,
    SAY_PATHALEON_CULATOR_IMAGE_2_2     = -1000135,
    SAY_COMMANDER_DAWNFORGE_4           = -1000136,
    SAY_ARCANIST_ARDONIS_2              = -1000136,
    SAY_COMMANDER_DAWNFORGE_5           = -1000137,

    QUEST_INFO_GATHERING                = 10198,
    SPELL_SUNFURY_DISGUISE              = 34603
};

// Entries of Arcanist Ardonis, Commander Dawnforge, Pathaleon the Curators Image
int CreatureEntry[3][1] =
{
    {19830},                                                // Ardonis
    {19831},                                                // Dawnforge
    {31001}                                                 // Pathaleon
};

struct npc_commander_dawnforgeAI : public ScriptedAI
{
    npc_commander_dawnforgeAI(Creature* c) : ScriptedAI(c)
    {
        Reset ();
    }


    uint64 playerGUID;
    uint64 ardonisGUID;
    uint64 pathaleonGUID;


    uint32 Phase;
    uint32 PhaseSubphase;
    uint32 Phase_Timer;
	uint32 spellbreaker_timer;
	uint32 cleave_timer;
    bool isEvent;

    float angle_dawnforge;
    float angle_ardonis;

    void Reset()
    {
        playerGUID = 0;
        ardonisGUID = 0;
        pathaleonGUID = 0;

        Phase = 1;
        PhaseSubphase = 0;
        Phase_Timer = 4000;
        isEvent = false;

		cleave_timer = 7000;
		spellbreaker_timer = 3000;
    }

    void EnterCombat(Unit* /*who*/) { }

    //Select any creature in a grid
    Creature* SelectCreatureInGrid(uint32 entry, float range)
    {
        Creature* pCreature = NULL;

        CellCoord pair(Oregon::ComputeCellCoord(me->GetPositionX(), me->GetPositionY()));
        Cell cell(pair);
        cell.SetNoCreate();

        Oregon::NearestCreatureEntryWithLiveStateInObjectRangeCheck creature_check(*me, entry, true, range);
        Oregon::CreatureLastSearcher<Oregon::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pCreature, creature_check);
        TypeContainerVisitor<Oregon::CreatureLastSearcher<Oregon::NearestCreatureEntryWithLiveStateInObjectRangeCheck>, GridTypeMapContainer> creature_searcher(searcher);
        cell.Visit(pair, creature_searcher, *(me->GetMap()), *me, me->GetGridActivationRange());

        return pCreature;
    }

    void JustSummoned(Creature* summoned)
    {
        pathaleonGUID = summoned->GetGUID();
    }

    // Emote Ardonis and Pathaleon
    void Turn_to_Pathaleons_Image()
    {
        Creature* ardonis = Unit::GetCreature(*me, ardonisGUID);
        Creature* pathaleon = Unit::GetCreature(*me, pathaleonGUID);
        Player* player = Unit::GetPlayer(*me, playerGUID);

        if (!ardonis || !pathaleon || !player)
            return;

        //Calculate the angle to Pathaleon
        angle_dawnforge = me->GetAngle(pathaleon->GetPositionX(), pathaleon->GetPositionY());
        angle_ardonis = ardonis->GetAngle(pathaleon->GetPositionX(), pathaleon->GetPositionY());

        //Turn Dawnforge and update
        me->SetOrientation(angle_dawnforge);
        me->SendUpdateToPlayer(player);
        //Turn Ardonis and update
        ardonis->SetOrientation(angle_ardonis);
        ardonis->SendUpdateToPlayer(player);

        //Set them to kneel
        me->SetStandState(UNIT_STAND_STATE_KNEEL);
        ardonis->SetStandState(UNIT_STAND_STATE_KNEEL);
    }

    //Set them back to each other
    void Turn_to_eachother()
    {
        if (Unit* ardonis = Unit::GetUnit(*me, ardonisGUID))
        {
            Player* player = Unit::GetPlayer(*me, playerGUID);

            if (!player)
                return;

            angle_dawnforge = me->GetAngle(ardonis->GetPositionX(), ardonis->GetPositionY());
            angle_ardonis = ardonis->GetAngle(me->GetPositionX(), me->GetPositionY());

            //Turn Dawnforge and update
            me->SetOrientation(angle_dawnforge);
            me->SendUpdateToPlayer(player);
            //Turn Ardonis and update
            ardonis->SetOrientation(angle_ardonis);
            ardonis->SendUpdateToPlayer(player);

            //Set state
            me->SetStandState(UNIT_STAND_STATE_STAND);
            ardonis->SetStandState(UNIT_STAND_STATE_STAND);
        }
    }

    bool CanStartEvent(Player* player)
    {
        if (!isEvent)
        {
            Creature* ardonis = SelectCreatureInGrid(CreatureEntry[0][0], 10.0f);
            if (!ardonis)
                return false;

            ardonisGUID = ardonis->GetGUID();
            playerGUID = player->GetGUID();

            isEvent = true;

            Turn_to_eachother();
            return true;
        }

        debug_log("OSCR: npc_commander_dawnforge event already in progress, need to wait.");
        return false;
    }

    void UpdateAI(const uint32 diff)
    {
        //Is event even running?
        if (!isEvent)
            return;

        //Phase timing
        if (Phase_Timer >= diff)
        {
            Phase_Timer -= diff;
            return;
        }

        Unit* ardonis = Unit::GetUnit(*me, ardonisGUID);
        Unit* pathaleon = Unit::GetUnit(*me, pathaleonGUID);
        Player* player = Unit::GetPlayer(*me, playerGUID);

        if (!ardonis || !player)
        {
            Reset();
            return;
        }

        if (Phase > 4 && !pathaleon)
        {
            Reset();
            return;
        }

        //Phase 1 Dawnforge say
        switch (Phase)
        {
        case 1:
            DoScriptText(SAY_COMMANDER_DAWNFORGE_1, me);
            ++Phase;
            Phase_Timer = 16000;
            break;
        //Phase 2 Ardonis say
        case 2:
            DoScriptText(SAY_ARCANIST_ARDONIS_1, ardonis);
            ++Phase;
            Phase_Timer = 16000;
            break;
        //Phase 3 Dawnforge say
        case 3:
            DoScriptText(SAY_COMMANDER_DAWNFORGE_2, me);
            ++Phase;
            Phase_Timer = 16000;
            break;
        //Phase 4 Pathaleon spawns up to phase 9
        case 4:
            //spawn pathaleon's image
            me->SummonCreature(CreatureEntry[2][0], 2325.851563f, 2799.534668f, 133.084229f, 6.038996f, TEMPSUMMON_TIMED_DESPAWN, 90000);
            ++Phase;
            Phase_Timer = 500;
            break;
        //Phase 5 Pathaleon say
        case 5:
            DoScriptText(SAY_PATHALEON_CULATOR_IMAGE_1, pathaleon);
            ++Phase;
            Phase_Timer = 6000;
            break;
        //Phase 6
        case 6:
            switch (PhaseSubphase)
            {
            //Subphase 1: Turn Dawnforge and Ardonis
            case 0:
                Turn_to_Pathaleons_Image();
                ++PhaseSubphase;
                Phase_Timer = 8000;
                break;
            //Subphase 2 Dawnforge say
            case 1:
                DoScriptText(SAY_COMMANDER_DAWNFORGE_3, me);
                PhaseSubphase = 0;
                ++Phase;
                Phase_Timer = 8000;
                break;
            }
            break;
        //Phase 7 Pathaleons say 3 Sentence, every sentence need a subphase
        case 7:
            switch (PhaseSubphase)
            {
            //Subphase 1
            case 0:
                DoScriptText(SAY_PATHALEON_CULATOR_IMAGE_2, pathaleon);
                ++PhaseSubphase;
                Phase_Timer = 12000;
                break;
            //Subphase 2
            case 1:
                DoScriptText(SAY_PATHALEON_CULATOR_IMAGE_2_1, pathaleon);
                ++PhaseSubphase;
                Phase_Timer = 16000;
                break;
            //Subphase 3
            case 2:
                DoScriptText(SAY_PATHALEON_CULATOR_IMAGE_2_2, pathaleon);
                PhaseSubphase = 0;
                ++Phase;
                Phase_Timer = 10000;
                break;
            }
            break;
        //Phase 8 Dawnforge & Ardonis say
        case 8:
            DoScriptText(SAY_COMMANDER_DAWNFORGE_4, me);
            DoScriptText(SAY_ARCANIST_ARDONIS_2, ardonis);
            ++Phase;
            Phase_Timer = 4000;
            break;
        //Phase 9 Pathaleons Despawn, Reset Dawnforge & Ardonis angle
        case 9:
            Turn_to_eachother();
            //hide pathaleon, unit will despawn shortly
            pathaleon->SetVisible(false);
            PhaseSubphase = 0;
            ++Phase;
            Phase_Timer = 3000;
            break;
        //Phase 10 Dawnforge say
        case 10:
            DoScriptText(SAY_COMMANDER_DAWNFORGE_5, me);
            player->AreaExploredOrEventHappens(QUEST_INFO_GATHERING);
            Reset();
            break;
        }

		if (UpdateVictim())
		{
			if (cleave_timer <= diff)
			{
				DoCastVictim(35473);
				cleave_timer = 7000;
			}
			else cleave_timer -= diff;

			if (spellbreaker_timer <= diff)
			{
				DoCastVictim(35871);
				spellbreaker_timer = 12000;
			}
			else spellbreaker_timer -= diff;

		}

		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_commander_dawnforge(Creature* pCreature)
{
    return new npc_commander_dawnforgeAI(pCreature);
}

bool AreaTrigger_at_commander_dawnforge(Player* player, const AreaTriggerEntry* /*at*/)
{
    //if player lost aura or not have at all, we should not try start event.
    if (!player->HasAura(SPELL_SUNFURY_DISGUISE, 0))
        return false;

    if (player->IsAlive() && player->GetQuestStatus(QUEST_INFO_GATHERING) == QUEST_STATUS_INCOMPLETE)
    {
        Creature* Dawnforge = player->FindNearestCreature(CreatureEntry[1][0], 30.0f);

        if (!Dawnforge)
            return false;

        if (((npc_commander_dawnforgeAI*)Dawnforge->AI())->CanStartEvent(player))
            return true;
    }
    return false;
}

/*######
## npc_professor_dabiri
######*/

#define SPELL_PHASE_DISTRUPTOR  35780
#define GOSSIP_ITEM "I need a new phase distruptor, Professor"
#define WHISPER_DABIRI -1000302

#define QUEST_DIMENSIUS 10439
#define QUEST_ON_NETHERY_WINGS 10438

bool GossipHello_npc_professor_dabiri(Player* player, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        player->PrepareQuestMenu(pCreature->GetGUID());

    if (player->GetQuestStatus(QUEST_ON_NETHERY_WINGS) == QUEST_STATUS_INCOMPLETE && !player->HasItemCount(29778, 1))
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_professor_dabiri(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pCreature->CastSpell(player, SPELL_PHASE_DISTRUPTOR, false);
        player->CLOSE_GOSSIP_MENU();
    }

    return true;
}

bool QuestAccept_npc_professor_dabiri(Player* player, Creature* creature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_DIMENSIUS)
        DoScriptText(WHISPER_DABIRI, creature, player);

    return true;
}

/*######
## npc_phase_hunter
######*/

enum phaseHunter
{
    // NPCs
    NPC_DRAINED_PHASE_HUNTER    = 19595,
    NPC_PHASE_HUNTER            = 18879,

    // Models
    MODEL_PHASE_HUNTER          = 19435,
    MODEL_DRAINED_PHASE_HUNTER  = 19436,

    // Spells
    SPELL_PHASE_SLIP            = 36574,
    SPELL_MANA_BURN             = 13321,
    SPELL_MATERIALIZE           = 34804,
    SPELL_RECHARGING_BATTERY    = 34219,

    // Quest
    QUEST_RECHARGING_BATTERIES  = 10190,

    // Misc
    WEAK_PERCENT                = 25
};

#define EMOTE_WEAK          "is very weak."

struct npc_phase_hunterAI : public ScriptedAI
{
    npc_phase_hunterAI(Creature* c) : ScriptedAI(c) {}

    bool Weak;
    bool Materialize;
    bool Drained;

    uint64 PlayerGUID;
    uint32 ManaBurnTimer;

    void Reset()
    {
        Weak = false;
        Materialize = false;
        Drained = false;

        PlayerGUID = 0;
        ManaBurnTimer = urand(5000, 8000);

        me->SetEntry(NPC_PHASE_HUNTER);
        me->SetDisplayId(MODEL_PHASE_HUNTER);
    }

    void EnterCombat(Unit* who)
    {
        if (Player* player = who->GetCharmerOrOwnerPlayerOrPlayerItself())
            PlayerGUID = player->GetGUID();
    }

    void SpellHit(Unit* /*caster*/, const SpellEntry* /*spell*/)
    {
        DoCast(me, SPELL_MATERIALIZE);
    }

    void JustDied(Unit* killer)
    {
        if (me->GetEntry() == NPC_DRAINED_PHASE_HUNTER)
            if (Player* plWho = killer->GetCharmerOrOwnerPlayerOrPlayerItself())
                if (plWho->GetQuestStatus(QUEST_RECHARGING_BATTERIES) == QUEST_STATUS_INCOMPLETE)
                        plWho->KilledMonsterCredit(NPC_DRAINED_PHASE_HUNTER);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!Materialize)
        {
            DoCast(me, SPELL_MATERIALIZE);
            Materialize = true;
        }

        if (me->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED) || me->HasUnitState(UNIT_STATE_ROOT))
            DoCast(me, SPELL_PHASE_SLIP);

        if (!UpdateVictim())
            return;

        if (ManaBurnTimer <= diff)
        {
            if (me->getVictim()->getPowerType() == POWER_MANA && me->getVictim()->GetPower(POWER_MANA) > 0)
            {
                DoCastVictim(SPELL_MANA_BURN);
                urand(8000, 18000);
            }
        }
        else ManaBurnTimer -= diff;

        if (PlayerGUID)
        {
            Player* pTarget = Unit::GetPlayer(*me, PlayerGUID);

            if (pTarget && !Weak && me->GetHealthPct() < WEAK_PERCENT
                && pTarget->GetQuestStatus(QUEST_RECHARGING_BATTERIES) == QUEST_STATUS_INCOMPLETE)
            {
                me->MonsterTextEmote(EMOTE_WEAK, 0);
                Weak = true;
            }
            if (Weak && !Drained && me->HasAura(SPELL_RECHARGING_BATTERY))
            {
                Drained = true;

                me->SetEntry(NPC_DRAINED_PHASE_HUNTER);
                me->SetDisplayId(MODEL_DRAINED_PHASE_HUNTER);
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_phase_hunter(Creature* c)
{
    return new npc_phase_hunterAI (c);
}

/*######
## npc_bessy
######*/

#define Q_ALMABTRIEB    10337
#define N_THADELL       20464
#define SPAWN_FIRST     20512
#define SPAWN_SECOND    19881
#define SAY_BESSY_1   -1910187
#define SAY_THADELL_1   -1910188

struct npc_bessyAI : public npc_escortAI
{

    npc_bessyAI(Creature* c) : npc_escortAI(c) {}

    void JustDied(Unit* /*killer*/)
    {
        if (Player* pPlayer = GetPlayerForEscort())
            pPlayer->FailQuest(Q_ALMABTRIEB);
    }

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (i)
        {
        case 3: //first spawn
            me->SummonCreature(SPAWN_FIRST, 2449.67f, 2183.11f, 96.85f, 6.20f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            me->SummonCreature(SPAWN_FIRST, 2449.53f, 2184.43f, 96.36f, 6.27f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            me->SummonCreature(SPAWN_FIRST, 2449.85f, 2186.34f, 97.57f, 6.08f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            break;

        case 7:
            me->SummonCreature(SPAWN_SECOND, 2309.64f, 2186.24f, 92.25f, 6.06f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            me->SummonCreature(SPAWN_SECOND, 2309.25f, 2183.46f, 91.75f, 6.22f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            break;

        case 12:
            if (pPlayer)
                pPlayer->GroupEventHappens(Q_ALMABTRIEB, me);
            if (Unit* Thadell = me->FindNearestCreature(N_THADELL, 30))
                DoScriptText(SAY_BESSY_1, me);
            break;
        case 13:
            if (Creature* Thadell = me->FindNearestCreature(N_THADELL, 30))
				DoScriptText(SAY_THADELL_1, Thadell, pPlayer);
            break;
        }
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->AI()->AttackStart(me);
    }

    void EnterCombat(Unit* /*who*/) {}

    void Reset()
    {
        me->RestoreFaction();
    }

};

bool QuestAccept_npc_bessy(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == Q_ALMABTRIEB)
    {
        pCreature->setFaction(113);
        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        CAST_AI(npc_escortAI, (pCreature->AI()))->Start(true, false, pPlayer->GetGUID());
    }
    return true;
}

CreatureAI* GetAI_npc_bessy(Creature* pCreature)
{
    return new npc_bessyAI(pCreature);
}

/*######
## npc_maxx_a_million
######*/

enum
{
    QUEST_MARK_V_IS_ALIVE   = 10191,
    GO_DRAENEI_MACHINE      = 183771
};

struct npc_maxx_a_million_escortAI : public npc_escortAI
{
    npc_maxx_a_million_escortAI(Creature* pCreature) : npc_escortAI(pCreature) {}

    bool bTake;
    uint32 uiTakeTimer;

    void Reset()
    {
        bTake = false;
        uiTakeTimer = 3000;
    }

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (i)
        {
        case 7:
        case 17:
        case 29:
            //Find Object and "work"
            if (me->FindNearestGameObject(GO_DRAENEI_MACHINE, INTERACTION_DISTANCE))
            {
                // take the GO -> animation
                me->HandleEmoteCommand(428);
                SetEscortPaused(true);
                bTake = true;
            }
            break;
        case 36: //return and quest_complete
            if (pPlayer)
                pPlayer->CompleteQuest(QUEST_MARK_V_IS_ALIVE);
            break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        npc_escortAI::UpdateAI(uiDiff);

        if (bTake)
        {
            if (uiTakeTimer < uiDiff)
            {
                me->HandleEmoteCommand(EMOTE_STATE_NONE);
                if (GameObject* pGO = me->FindNearestGameObject(GO_DRAENEI_MACHINE, INTERACTION_DISTANCE))
                {
                    SetEscortPaused(false);
                    bTake = false;
                    uiTakeTimer = 3000;
                    pGO->Delete();
                }
            }
            else
                uiTakeTimer -= uiDiff;
        }
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_maxx_a_million_escort(Creature* pCreature)
{
    return new npc_maxx_a_million_escortAI(pCreature);
}

bool QuestAccept_npc_maxx_a_million_escort(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_MARK_V_IS_ALIVE)
    {
        if (npc_maxx_a_million_escortAI* pEscortAI = CAST_AI(npc_maxx_a_million_escortAI, pCreature->AI()))
        {
            pCreature->setFaction(113);
            pEscortAI->Start(false, false, pPlayer->GetGUID(), pQuest);
        }
    }
    return true;
}

/*######
## npc_zeppit
######*/

enum
{
    EMOTE_GATHER_BLOOD         = -1000625,

    NPC_WARP_CHASER            = 18884,
    SPELL_GATHER_WARP_BLOOD    = 39244,
    QUEST_BLOODY               = 10924
};

struct npc_zeppitAI : public ScriptedAI
{
    npc_zeppitAI(Creature* pCreature) : ScriptedAI(pCreature) {}

    uint32 uiCheckTimer;
    uint64 uiWarpGUID;

    void Reset()
    {
        uiCheckTimer = 8000;
        uiWarpGUID = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (uiCheckTimer <= uiDiff)
        {
            if (Creature* pWarp = me->FindNearestCreature(NPC_WARP_CHASER, 9.0f, false))
            {
                if (pWarp->GetGUID() != uiWarpGUID && CAST_PLR(me->GetOwner())->GetQuestStatus(QUEST_BLOODY) == QUEST_STATUS_INCOMPLETE)
                {
                    uiWarpGUID = pWarp->GetGUID();
                    DoScriptText(EMOTE_GATHER_BLOOD, me);
                    me->CastSpell(me->GetOwner(), SPELL_GATHER_WARP_BLOOD, false);
                }
            }
            uiCheckTimer = 8000;
        }
        else uiCheckTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_zeppit(Creature* pCreature)
{
    return new npc_zeppitAI (pCreature);
}

/*######
## npc_dr_boom
######*/

enum
{
    THROW_DYNAMITE    = 35276,
    BOOM_BOT          = 19692,
    BOOM_BOT_TARGET   = 20392
};

struct npc_dr_boomAI : public ScriptedAI
{
    npc_dr_boomAI(Creature* pCeature) : ScriptedAI(pCeature) {}

    std::vector<uint64> targetGUID;

    uint32 SummonTimer;

    void Reset()
    {
        SummonTimer = 1500;

        std::list<Creature*> targets;
        me->GetCreatureListWithEntryInGrid(targets, BOOM_BOT_TARGET, 30.0f);

        targetGUID.clear();

        for (std::list<Creature*>::iterator it = targets.begin(); it != targets.end(); it++)
            targetGUID.push_back((*it)->GetGUID());
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (SummonTimer <= uiDiff)
        {
            if (targetGUID.size())
            {
                if (Unit* target = Unit::GetUnit(*me, targetGUID[rand() % targetGUID.size()]))
                {
                    if (Unit* bot = DoSpawnCreature(BOOM_BOT, 0, 0, 0, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 20000))
                        bot->GetMotionMaster()->MovePoint(0, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
                }
            }

            SummonTimer = 1500;
        }
        else SummonTimer -= uiDiff;

        if (!UpdateVictim())
            return;

        if (!me->IsWithinDistInMap(me->getVictim(), 30.0f))
        {
            EnterEvadeMode();
            return;
        }

        if (me->isAttackReady() && me->IsWithinDistInMap(me->getVictim(), 13.0f))
        {
            DoCastVictim( THROW_DYNAMITE, true);
            me->resetAttackTimer();
        }
    }

};

CreatureAI* GetAI_npc_dr_boom(Creature* pCreature)
{
    return new npc_dr_boomAI (pCreature);
}

/*######
## npc_boom_bot
######*/

#define    SPELL_BOOM    35132

struct npc_boom_botAI : public ScriptedAI
{
    npc_boom_botAI(Creature* pCreature) : ScriptedAI(pCreature) {}

    bool Boom;

    uint32 BoomTimer;

    void Reset()
    {
        Boom = false;
        BoomTimer = 800;
        me->SetWalk(true);
    }

    void AttackedBy(Unit* /*pWho*/) {}
    void AttackStart(Unit* /*pWho*/) {}

    void MovementInform(uint32 type, uint32 /*id*/)
    {
        if (type != POINT_MOTION_TYPE)
            return;

        DoCast(SPELL_BOOM);
        Boom = true;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho->isCharmedOwnedByPlayerOrPlayer())
            return;

        if (me->IsWithinDistInMap(pWho, 4.0f, false))
        {
            DoCast(SPELL_BOOM);
            Boom = true;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (Boom)
        {
            if (BoomTimer <= uiDiff)
                me->setDeathState(CORPSE);
            else BoomTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_boom_bot(Creature* pCreature)
{
    return new npc_boom_botAI (pCreature);
}

/*######
## npc_drijya
######*/

enum
{
    SAY_DR_START           = -1900156,
    SAY_DR_1               = -1900157,
    SAY_DR_2               = -1900158,
    SAY_DR_3               = -1900159,
    SAY_DR_4               = -1900160,
    SAY_DR_5               = -1900161,
    SAY_DR_6               = -1900162,
    SAY_DR_7               = -1900163,
    SAY_DR_COMPLETE        = -1900164,

    QUEST_WARP_GATE        = 10310,

    MAX_TROOPER            = 9,
    MAX_IMP                = 6,

    NPC_EXPLODE_TRIGGER    = 20296,
    NPC_IMP                = 20399,
    NPC_TROOPER            = 20402,
    NPC_DESTROYER          = 20403,

    GO_SMOKE               = 185318,
    GO_FIRE                = 185317,
    GO_BIG_FIRE            = 185319
};

struct npc_drijyaAI : public npc_escortAI
{
    npc_drijyaAI(Creature* pCreature) : npc_escortAI(pCreature) {}

    bool Destroy;
    bool SummonImp;
    bool SummonTrooper;
    bool SummonDestroyer;

    uint32 Count;
    uint32 SpawnTimer;
    uint32 StartSpawnTimer;
    uint32 DestroyingTimer;

    void Reset()
    {
        Destroy = false;
        SummonImp = false;
        SummonTrooper = false;
        SummonDestroyer = false;
        Count = 0;
        SpawnTimer = 3500;
        StartSpawnTimer = 15000;
        DestroyingTimer = 60000;
    }

    void AttackedBy(Unit* /*pWho*/) {}
    void AttackStart(Unit* /*pWho*/) {}

    void SpawnImp()
    {
        ++Count;
        me->SummonCreature(NPC_IMP, 3025.752f, 2715.122, 113.758, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
    }

    void SpawnTrooper()
    {
        ++Count;
        me->SummonCreature(NPC_TROOPER, 3025.752f, 2715.122, 113.758, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
    }

    void SpawnDestroyer()
    {
        me->SummonCreature(NPC_DESTROYER, 3019.741f, 2720.757, 115.189, 2.5f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_IMP)
        {
            if (Player* pPlayer = GetPlayerForEscort())
                pSummoned->AI()->AttackStart(pPlayer);
        }
        if (pSummoned->GetEntry() == NPC_TROOPER)
        {
            if (Player* pPlayer = GetPlayerForEscort())
                pSummoned->AI()->AttackStart(pPlayer);
        }
        else
        {
            if (pSummoned->GetEntry() == NPC_DESTROYER)
            {
                if (Player* pPlayer = GetPlayerForEscort())
                    pSummoned->AI()->AttackStart(pPlayer);
            }
        }
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch (uiPointId)
        {
        case 0:
            DoScriptText(SAY_DR_START, me);
            SetRun();
            break;
        case 1:
            DoScriptText(SAY_DR_1, me);
            break;
        case 5:
            DoScriptText(SAY_DR_2, me);
            break;
        case 7:
            SetEscortPaused(true);
            Destroy = true;
            SummonImp = true;
            break;
        case 8:
            me->SummonGameObject(GO_SMOKE, 3049.354f, 2726.431f, 113.922f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 215);
            DoScriptText(SAY_DR_4, me);
            break;
        case 12:
            SetEscortPaused(true);
            Destroy = true;
            SummonTrooper = true;
            break;
        case 13:
            me->SummonGameObject(GO_SMOKE, 3020.842f, 2697.501f, 113.368f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 140);
            DoScriptText(SAY_DR_5, me);
            break;
        case 17:
            SetEscortPaused(true);
            Destroy = true;
            SummonDestroyer = true;
            break;
        case 18:
            DoScriptText(SAY_DR_6, me);
            if (Creature* pTrigger = me->FindNearestCreature(NPC_EXPLODE_TRIGGER, 20.0f))
                pTrigger->CastSpell(pTrigger, SPELL_BOOM , false);
            me->SummonGameObject(GO_SMOKE, 3008.503f, 2729.432f, 114.350f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 60);
            me->SummonGameObject(GO_FIRE, 3026.163f, 2723.538f, 113.681f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 60);
            me->SummonGameObject(GO_BIG_FIRE, 3021.556f, 2718.887f, 115.055f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 60);
            break;
        case 19:
            DoScriptText(SAY_DR_7, me);
            break;
        case 22:
            SetRun(false);
            break;
        case 26:
            if (Player* pPlayer = GetPlayerForEscort())
            {
                DoScriptText(SAY_DR_COMPLETE, me, pPlayer);
                pPlayer->GroupEventHappens(QUEST_WARP_GATE, me);
            }
            break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        npc_escortAI::UpdateAI(uiDiff);

        if (SummonImp)
        {
            if (StartSpawnTimer <= uiDiff)
            {
                if (SpawnTimer <= uiDiff)
                {
                    if (Count >= MAX_IMP)
                    {
                        DoScriptText(SAY_DR_3, me);
                        SummonImp = false;
                        StartSpawnTimer = 15000;
                    }
                    SpawnTimer = 3500;
                    SpawnImp();
                }
                else SpawnTimer -= uiDiff;
            }
            else StartSpawnTimer -= uiDiff;
        }

        if (SummonTrooper)
        {
            if (StartSpawnTimer <= uiDiff)
            {
                if (SpawnTimer <= uiDiff)
                {
                    if (Count >= MAX_TROOPER)
                    {
                        SummonTrooper = false;
                        StartSpawnTimer = 15000;
                    }
                    SpawnTimer = 3500;
                    SpawnTrooper();
                }
                else SpawnTimer -= uiDiff;
            }
            else StartSpawnTimer -= uiDiff;
        }

        if (SummonDestroyer)
        {
            if (StartSpawnTimer <= uiDiff)
            {
                SpawnDestroyer();
                SummonDestroyer = false;
                StartSpawnTimer = 15000;
            }
            else StartSpawnTimer -= uiDiff;
        }

        if (Destroy)
        {
            if (DestroyingTimer <= uiDiff)
            {
                SetEscortPaused(false);
                Destroy = false;
                DestroyingTimer = 60000;
            }
            else DestroyingTimer -= uiDiff;

            me->HandleEmoteCommand(173);
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_drijya(Creature* pCreature)
{
    return new npc_drijyaAI(pCreature);
}

bool QuestAccept_npc_drijya(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_WARP_GATE)
    {
        if (npc_drijyaAI* pEscortAI = dynamic_cast<npc_drijyaAI*>(pCreature->AI()))
        {
            pCreature->setFaction(113);
            pEscortAI->Start(true, false, pPlayer->GetGUID(), pQuest);
        }
    }
    return true;
}

/*######
## npc_mana_wraith
######*/

#define SPELL_MANA_DEVOUR 29054

struct npc_mana_wraithAI : public ScriptedAI
{
	npc_mana_wraithAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);

		manadevour_timer = 9000;
	}

	uint32 manadevour_timer;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (manadevour_timer <= diff)
		{
			DoCastVictim(SPELL_MANA_DEVOUR);
			manadevour_timer = 13000;
		}
		else manadevour_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_mana_wraith(Creature* pCreature)
{
	return new npc_mana_wraithAI(pCreature);
}

/*######
## npc_warp_abberation
######*/

#define SPELL_WARP_STORM 36577
#define SPELL_ARCANE_SHIELD 36640

struct npc_warp_abberationAI : public ScriptedAI
{
	npc_warp_abberationAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);

		warpstorm_timer = urand(2500, 5000);

		DoCast(me, SPELL_ARCANE_SHIELD);
	}

	uint32 warpstorm_timer;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (warpstorm_timer <= diff)
		{
			DoCastVictim(SPELL_WARP_STORM);
			warpstorm_timer = urand(20000, 25000);
		}
		else warpstorm_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_warp_abberation(Creature* pCreature)
{
	return new npc_warp_abberationAI(pCreature);
}

/*######
## npc_scrapped_fel_reaver
######*/

#define NPC_ZAXXIS_AMBUSHER 20287
#define NPC_SCRAPPER_FEL 20243
#define NPC_SCRAPPER 22509
#define EMOTE_SCRAPPED "The Scrapped Fel Reaver's mechanical heart begins to beat softly."

struct npc_scrapped_fel_reaverAI : public ScriptedAI
{
	npc_scrapped_fel_reaverAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{		
		if (me->GetEntry() == NPC_SCRAPPER)
			me->UpdateEntry(NPC_SCRAPPER_FEL);

		me->setFaction(16);

		spellHit = false;
		CanSummon = false;

		summon_timer = 0;
	}

	uint32 summon_timer;

	bool spellHit;
	bool CanSummon;

	void SpellHit(Unit* Hitter, const SpellEntry* Spellkind)
	{
		if (Spellkind->Id == 35282 && !spellHit)
		{
			if (me->GetEntry() == NPC_SCRAPPER_FEL)
				me->UpdateEntry(NPC_SCRAPPER);

			summon_timer = 5000;
			me->MonsterTextEmote(EMOTE_SCRAPPED, 0, false);
			CanSummon = true;
			spellHit = true;		
		}
	}

	void JustSummoned(Creature* pSummoned)
	{	
		pSummoned->GetMotionMaster()->MovePoint(0, 2528.6f, 3983.1f, 130.4f);
		pSummoned->SetSpeed(MOVE_RUN, 1.2f, true);
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (CanSummon == true)
		{
			if (summon_timer <= diff)
			{
				me->SummonCreature(NPC_ZAXXIS_AMBUSHER, 2551.7f, 3928.1f, 135.9f, 1.88f, TEMPSUMMON_TIMED_DESPAWN, 120000);
				summon_timer = 20000;
			}
			else summon_timer -= diff;
		}
	}
};

CreatureAI* GetAI_npc_scrapped_fel_reaver(Creature* pCreature)
{
	return new npc_scrapped_fel_reaverAI(pCreature);
}

/*######
## npc_mana_seeker
######*/

struct npc_mana_seekerAI : public ScriptedAI
{
	npc_mana_seekerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);

		// Poisons 
		me->ApplySpellImmune(0, IMMUNITY_ID, 27282, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 26892, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 26786, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 27283, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 26969, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 25347, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11343, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 13230, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11358, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11400, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11342, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 3421, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 13229, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11357, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11341, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 13228, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8694, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 2837, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8691, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 13220, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 2835, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8687, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 5763, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8681, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 3420, true);

		slow_timer = 0;
		manaburn_timer = urand(3000, 5000);
	}

	uint32 slow_timer;
	uint32 manaburn_timer;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (manaburn_timer <= diff)
		{
			DoCastVictim(11981);
			manaburn_timer = urand(6000, 9000);
		}
		else manaburn_timer -= diff;

		if (slow_timer <= diff)
		{
			DoCastVictim(36843);
			slow_timer = urand(16000, 20000);
		}
		else slow_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_mana_seeker(Creature* pCreature)
{
	return new npc_mana_seekerAI(pCreature);
}

/*######
## npc_mageslayer
######*/

struct npc_mageslayerAI : public ScriptedAI
{
	npc_mageslayerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);

		// Poisons 
		me->ApplySpellImmune(0, IMMUNITY_ID, 27282, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 26892, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 26786, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 27283, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 26969, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 25347, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11343, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 13230, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11358, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11400, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11342, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 3421, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 13229, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11357, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11341, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 13228, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8694, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 2837, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8691, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 13220, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 2835, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8687, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 5763, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8681, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 3420, true);

		reflect_timer = urand(3000, 5000);
		blink_timer = urand(8000, 12000);
	}

	uint32 reflect_timer;
	uint32 blink_timer;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (blink_timer <= diff)
		{
			DoCast(36097);
			blink_timer = urand(16000, 20000);
		}
		else blink_timer -= diff;

		if (reflect_timer <= diff)
		{
			DoCast(me, 36096);
			reflect_timer = urand(16000, 20000);
		}
		else reflect_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_mageslayer(Creature* pCreature)
{
	return new npc_mageslayerAI(pCreature);
}

/*######
## npc_naberius
######*/

#define SPELL_FROSTBOLT 15497
#define SPELL_CHILL_NOVA 36148
#define SPELL_CHAINS_OF_NABERIUS 36145

struct npc_naberiusAI : public ScriptedAI
{
	npc_naberiusAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		// Poisons 
		me->ApplySpellImmune(0, IMMUNITY_ID, 27282, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 26892, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 26786, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 27283, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 26969, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 25347, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11343, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 13230, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11358, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11400, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11342, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 3421, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 13229, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11357, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11341, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 13228, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8694, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 2837, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8691, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 13220, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 2835, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8687, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 5763, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8681, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 3420, true);

		chains_timer = urand(15000, 23000);
		frostbolt_timer = urand(5000, 8000);
		nova_timer = urand(10000, 14500);
	}

	uint32 frostbolt_timer;
	uint32 chains_timer;
	uint32 nova_timer;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (frostbolt_timer <= diff)
		{
			DoCastVictim(SPELL_FROSTBOLT);
			frostbolt_timer = urand(5000, 8000);
		}
		else frostbolt_timer -= diff;

		if (nova_timer <= diff)
		{
			DoCast(SPELL_CHILL_NOVA);
			nova_timer = urand(15000, 18500);
		}
		else nova_timer -= diff;

		if (chains_timer <= diff)
		{
			Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
			DoCast(pTarget, SPELL_CHAINS_OF_NABERIUS);
			chains_timer = urand(25000, 30000);
		}
		else chains_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_naberius(Creature* pCreature)
{
	return new npc_naberiusAI(pCreature);
}

/*######
## npc_towercurse_trigger
######*/

#define SPELL_CURSE_OF_THE_VIOLET_TOWER 34102


struct npc_towercurse_triggerAI : public ScriptedAI
{
	npc_towercurse_triggerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->SetReactState(REACT_AGGRESSIVE);
	}

	void MoveInLineOfSight(Unit *pWho)
	{
		if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetDistance(me) < 3.0f)
			{
				switch (me->GetEntry())
				{
				case 61016:	
					plWho->CastSpell(plWho, SPELL_CURSE_OF_THE_VIOLET_TOWER, true);
					break;
				}
			}
		}
	}
};

CreatureAI* GetAI_npc_towercurse_trigger(Creature* pCreature)
{
	return new npc_towercurse_triggerAI(pCreature);
}

/*######
## npc_towerchannel_trigger
######*/

#define SPELL_ETHEREAL_CHANNEL 35518


struct npc_towerchannel_triggerAI : public ScriptedAI
{
	npc_towerchannel_triggerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (Creature* channeler = me->FindNearestCreature(61018, 8.0f, true))
			{
				DoCast(channeler, SPELL_ETHEREAL_CHANNEL);
			}
		}
	}
};

CreatureAI* GetAI_npc_towerchannel_trigger(Creature* pCreature)
{
	return new npc_towerchannel_triggerAI(pCreature);
}

struct npc_towerchanneler_triggerAI : public ScriptedAI
{
	npc_towerchanneler_triggerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{

	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (Creature* channel = me->FindNearestCreature(19656, 12.0f, true))
			{
				DoCast(channel, SPELL_ETHEREAL_CHANNEL);
			}
		}
	}
};

CreatureAI* GetAI_npc_towerchanneler_trigger(Creature* pCreature)
{
	return new npc_towerchanneler_triggerAI(pCreature);
}

/*######
## QUEST_YOU_ROBOT!
######*/

#define QUEST_YOU_ROBOT 10248
#define NPC_NEGATRON 19851
#define NPC_DOCTOR_VOMISA 19832
#define VOMISA_TEXT "Oh no! What's that!? Quickly, defend us with the Scrap Reaver X6000!!!"
#define NEGATRON_TEXT "HAHAHA! YOUR FEEBLE ROCKET IS DESTROYED! I'LL RETURN LATER TO FINISH OFF THE REST OF YOUR PUNY TOWN!"
#define NEGATRON_TEXT_2 "I AM DEATH! PREPARE YOUR TOWN FOR ANNIHILATION!"

#define SPELL_FRENZY 34624
#define SPELL_EARTHQUAKE 35565
#define SPELL_CHARGE 35570
#define SPELL_DEMOLISH 34625

struct npc_doctor_vomisaAI : public ScriptedAI
{
	npc_doctor_vomisaAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset() 
	{
		said = false;
	}

	bool said;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (!said)
			{
				if (Creature* negatron = me->FindNearestCreature(NPC_NEGATRON, 30.0f, true))
				{
					me->MonsterSay(VOMISA_TEXT, LANG_UNIVERSAL, 0);
					said = true;
				}
			}
		}
	}
};

CreatureAI* GetAI_npc_doctor_vomisa(Creature* pCreature)
{
	return new npc_doctor_vomisaAI(pCreature);
}

bool QuestAccept_npc_doctor_vomisa(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_YOU_ROBOT)
	{
		pCreature->SummonCreature(NPC_NEGATRON, 3113.6f, 3316.8f, 109.4f, 1.84f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
	}
	return true;
}

struct npc_negatronAI : public ScriptedAI
{
	npc_negatronAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->GetMotionMaster()->MovePath(601601605, false);
		me->MonsterYell(NEGATRON_TEXT, LANG_UNIVERSAL, 0);
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		me->SetReactState(REACT_DEFENSIVE);
		me->setFaction(35);

		yelled = false;
		start_timer = 0;
		frenzy_timer = 0;
		charge_timer = 0;
		demolish_timer = 0;
		earthquake_timer = urand(15000, 19000);
	}

	bool yelled;
	uint32 start_timer;

	bool frenzy;
	uint32 frenzy_timer;
	uint32 demolish_timer;
	uint32 charge_timer;
	uint32 earthquake_timer;

	void JustDied(Unit* killer)
	{
		if (Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself())
			player->CompleteQuest(QUEST_YOU_ROBOT);
	}

	void UpdateAI(const uint32 diff)
	{
		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (!UpdateVictim())
		{
			if (!yelled)
			{
				if (Creature* vomisa = me->FindNearestCreature(NPC_DOCTOR_VOMISA, 15.0f, true))
				{
					me->MonsterYell(NEGATRON_TEXT_2, LANG_UNIVERSAL, 0);
					start_timer = 8000;
					yelled = true;
				}
			}

			if (yelled == true && start_timer <= diff)
			{
				me->setFaction(14);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				me->SetReactState(REACT_AGGRESSIVE);
			}
			else start_timer -= diff;
		}

		if (UpdateVictim())
		{
			if (!frenzy && HealthBelowPct(80))
			{			
				DoCast(me, SPELL_FRENZY);
				frenzy_timer = urand(16000, 22000);
				frenzy = true;
			}

			if (frenzy_timer <= diff && frenzy == true)
			{
				DoCast(me, SPELL_FRENZY);
				frenzy_timer = urand(16000, 22000);				
			}
			else frenzy_timer -= diff;

			if (earthquake_timer <= diff)
			{
				DoCast(SPELL_EARTHQUAKE);
				earthquake_timer = urand(21000, 25000);
			}
			else earthquake_timer -= diff;

			if (demolish_timer <= diff)
			{
				DoCastVictim(SPELL_DEMOLISH);
				demolish_timer = urand(8000, 12000);
			}
			else demolish_timer -= diff;

			if (charge_timer <= diff)
			{
				Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 4);
				DoCast(pTarget, SPELL_CHARGE);
				charge_timer = urand(15000, 21000);
			}
			else charge_timer -= diff;
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_negatron(Creature* pCreature)
{
	return new npc_negatronAI(pCreature);
}

/*######
## npc_farahlon_lasher
######*/

#define NPC_MUTATED_FARAHLON_LASHER 20983

struct npc_farahlon_lasherAI : public ScriptedAI
{
	npc_farahlon_lasherAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		spellHit = false;
		frenzied = false;
		summoned = false;

		roots_timer = urand(8000, 10000);
	}

	bool spellHit;
	bool frenzied;
	bool summoned;

	uint32 roots_timer;

	void SpellHit(Unit* Hitter, const SpellEntry* Spellkind)
	{
		if (Spellkind->Id == 35772 && !spellHit)
		{
			me->SummonCreature(NPC_MUTATED_FARAHLON_LASHER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
			me->DisappearAndDie();

			spellHit = true;
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (HealthBelowPct(40) && !frenzied)
		{
			DoCast(me, 3019);
			frenzied = true;
		}

		if (HealthBelowPct(75) && !summoned)
		{
			DoCast(me, 36604);
			summoned = true;
		}

		if (roots_timer <= diff)
		{
			DoCastVictim(12747);
			roots_timer = urand(18000, 22000);
		}
		else roots_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_farahlon_lasher(Creature* pCreature)
{
	return new npc_farahlon_lasherAI(pCreature);
}

/*######
## npc_talbuk_sire
######*/

#define EMOTE_TALBUK_SIRE "Talbuk Sire seems to have weakened."

struct npc_talbuk_sireAI : public ScriptedAI
{
	npc_talbuk_sireAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		spellHit = false;

		me->RemoveAllAuras();
		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		me->SetStandState(UNIT_STAND_STATE_STAND);
		me->setFaction(14);
		reset_timer = 0;
		stun_timer = urand(8000, 10000);
	}

	bool spellHit;

	uint32 stun_timer;
	uint32 reset_timer;

	void SpellHit(Unit* Hitter, const SpellEntry* Spellkind)
	{
		if (Spellkind->Id == 35771 && !spellHit)
		{		
			if (Player* player = Hitter->GetCharmerOrOwnerPlayerOrPlayerItself())
			{
				DoCast(player, 40347);
				player->ClearInCombat();
			}
			
			me->MonsterTextEmote(EMOTE_TALBUK_SIRE, 0, false);
			me->CombatStop();
			me->setFaction(35);
			DoCast(me, 20373);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			me->SetStandState(UNIT_STAND_STATE_DEAD);
			reset_timer = 120000;
			spellHit = true;
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (reset_timer <= diff)
			{
				Reset();
			}
			else reset_timer -= diff;
		}

		if (stun_timer <= diff)
		{
			DoCastVictim(32023);
			stun_timer = urand(18000, 22000);
		}
		else stun_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_talbuk_sire(Creature* pCreature)
{
	return new npc_talbuk_sireAI(pCreature);
}

/*######
## npc_talbuk_doe
######*/

#define EMOTE_TALBUK_DOE "Talbuk Doe seems to have weakened."

struct npc_talbuk_doeAI : public ScriptedAI
{
	npc_talbuk_doeAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		spellHit = false;

		me->RemoveAllAuras();
		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		me->SetStandState(UNIT_STAND_STATE_STAND);
		me->setFaction(15);
		reset_timer = 0;
		gore_timer = urand(4000, 7000);
	}

	bool spellHit;

	uint32 gore_timer;
	uint32 reset_timer;

	void SpellHit(Unit* Hitter, const SpellEntry* Spellkind)
	{
		if (Spellkind->Id == 35771 && !spellHit)
		{
			if (Player* player = Hitter->GetCharmerOrOwnerPlayerOrPlayerItself())
			{
				DoCast(player, 40347);
				player->ClearInCombat();
			}

			me->MonsterTextEmote(EMOTE_TALBUK_DOE, 0, false);
			me->CombatStop();
			me->setFaction(35);
			DoCast(me, 20373);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			me->SetStandState(UNIT_STAND_STATE_DEAD);
			reset_timer = 120000;
			spellHit = true;
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (reset_timer <= diff)
			{
				Reset();
			}
			else reset_timer -= diff;
		}

		if (gore_timer <= diff)
		{
			DoCastVictim(32019);
			gore_timer = urand(18000, 22000);
		}
		else gore_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_talbuk_doe(Creature* pCreature)
{
	return new npc_talbuk_doeAI(pCreature);
}

/*######
## npc_warp_monstrosity
######*/

#define SPELL_WARP_STORM 36577
#define SPELL_ARCANE_BOLT 13901

struct npc_warp_monstrosityAI : public ScriptedAI
{
	npc_warp_monstrosityAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);

		warpstorm_timer = urand(7500, 12000);
		arcanebolt_timer = 5000;
	}

	uint32 warpstorm_timer;
	uint32 arcanebolt_timer;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (warpstorm_timer <= diff)
		{
			DoCastVictim(SPELL_WARP_STORM);
			warpstorm_timer = urand(20000, 25000);
		}
		else warpstorm_timer -= diff;

		if (arcanebolt_timer <= diff)
		{
			DoCastVictim(SPELL_ARCANE_BOLT);
			arcanebolt_timer = urand(7000, 9000);
		}
		else arcanebolt_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_warp_monstrosity(Creature* pCreature)
{
	return new npc_warp_monstrosityAI(pCreature);
}

/*######
## QUEST_FEL_REAVER'S NO THANKS
######*/

#define QUEST_NETHER_GAS_IN_FEL_FIRE_ENGINE 10850
#define QUEST_FEL_REAVER_NO_THANKS 10855
#define INACTIVE_TEXT "Inactive Fel Reaver begins to sputter as it's engine malfunctions."

bool ChooseReward_npc_inactive_fel_reaver(Player* pPlayer, Creature* pCreature, const Quest* pQuest, uint32 /*item*/)
{
	if (pQuest->GetQuestId() == QUEST_NETHER_GAS_IN_FEL_FIRE_ENGINE)
	{	
		pCreature->MonsterTextEmote(INACTIVE_TEXT, 0, false);
		pPlayer->CompleteQuest(QUEST_FEL_REAVER_NO_THANKS);
	}
	return true;
}

/*######
## QUEST_TELEPORT_THIS! TRIGGER
		gehenna_trigger
######*/

struct gehenna_triggerAI : public ScriptedAI
{
	gehenna_triggerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset() 
	{
		spellHit = false;
	}

	bool spellHit;

	void SpellHit(Unit* Hitter, const SpellEntry* Spellkind)
	{
		if (Spellkind->Id == 38920 && !spellHit)
		{
			if (Player* player = Hitter->GetCharmerOrOwnerPlayerOrPlayerItself())
			{
				player->Kill(me);
			}

			spellHit = true;
		}
	}
};

CreatureAI* GetAI_gehenna_trigger(Creature* pCreature)
{
	return new gehenna_triggerAI(pCreature);
}

/*######
## QUEST_DEATHBLOW_TO_THE_LEGION! ALDOR
######*/

enum AldorDeathblowLegion
{
	QUEST_DEATHBLOW_TO_THE_LEGION = 10409,
	NPC_ADYEN_THE_LIGHTWARDEN = 61021,
	NPC_ANCHORITE_KARJA = 61022,
	NPC_EXARCH_ORELIS = 19466,
	NPC_ISHANAH = 18538,
	NPC_SOCRETHAR = 20132,
	NPC_KAYLAAN = 20794,

	ADYEN_TEXT_1 = -1910189,

	SPELL_POWER_OF_THE_LEGION = 35596
};

struct legion_aldor_triggerAI : public ScriptedAI
{
	legion_aldor_triggerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->SetReactState(REACT_AGGRESSIVE);

		summoned = false;
	}

	bool summoned;

	void MoveInLineOfSight(Unit *pWho)
	{
		if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetQuestStatus(QUEST_DEATHBLOW_TO_THE_LEGION) == QUEST_STATUS_INCOMPLETE && plWho->GetDistance(me) < 10.0f)
			{
				switch (me->GetEntry())
				{
				case 61020:
					if (!summoned)
					{
						me->SummonCreature(NPC_ADYEN_THE_LIGHTWARDEN, 4804.8f, 3772.0f, 210.5f, 5.3f, TEMPSUMMON_MANUAL_DESPAWN, 0);
						me->SummonCreature(NPC_ANCHORITE_KARJA, 4802.2f, 3772.0f, 210.5f, 5.3f, TEMPSUMMON_MANUAL_DESPAWN, 0);
						me->SummonCreature(NPC_EXARCH_ORELIS, 4805.7f, 3775.3f, 210.5f, 5.3f, TEMPSUMMON_MANUAL_DESPAWN, 0);
						me->DisappearAndDie();
						summoned = true;
					}
					break;
				}
			}
		}
	}
};

CreatureAI* GetAI_legion_aldor_trigger(Creature* pCreature)
{
	return new legion_aldor_triggerAI(pCreature);
}

#define QUEST_TURNING_POINT 10507

#define SOCRETHAR_TEXT_1 "Do not make me laugh. Is this the mighty Aldor army that's come to defeat me?"
#define SOCRETHAR_TEXT_2 "Yes, let us settle this. Before we begin, however, there's somebody I'd like you to meet."
#define SOCRETHAR_TEXT_3 "Slay these dogs, Kaylaan! Earn your place in the Burning Legion!"
#define SOCRETHAR_TEXT_4 "What are you waiting for? Finish them, young one. Let your hatred burn!"

#define SPELL_SB_VOLLEY 28448
#define SPELL_FB_BARRAGE 37540
#define SPELL_ANTI_MAGIC 37538
#define SPELL_SOCRETHAR_CLEAVE 15496
#define SPELL_BACKLASH 37537

struct npc_socretharAI : public ScriptedAI
{
	npc_socretharAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->setFaction(35);

		// Event
		EventStarted = false;
		summoned = false;
		text1 = false;
		text2 = false;
		text3 = false;
		text4 = false;
		scryer_start = false;

		summon_timer = 0;
		wait_timer = 0;
		wait_timer1 = 0;
		combat_timer = 0;
		combat_timer1 = 0;

		// Skills
		barrage_timer = urand(20000, 25000);
		sbv_timer = urand(10000, 15000);
		backlash_timer = 9000;
		antimagic_timer = 5000;
		cleave_timer = 7500;
	}

	// Event
	bool EventStarted;
	bool summoned;
	bool text1; 
	bool text2;
	bool text3;
	bool text4;
	bool scryer_start;

	uint32 summon_timer;
	uint32 wait_timer;
	uint32 wait_timer1;
	uint32 combat_timer;
	uint32 combat_timer1;

	// Skills
	uint32 barrage_timer;
	uint32 sbv_timer;
	uint32 backlash_timer;
	uint32 antimagic_timer;
	uint32 cleave_timer;

	void JustSummoned(Creature* summoned)
	{
		summoned->GetMotionMaster()->MovePath(601601609, false);
	}

	void MoveInLineOfSight(Unit *pWho)
	{
		if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetQuestStatus(QUEST_TURNING_POINT) == QUEST_STATUS_INCOMPLETE && plWho->GetDistance(me) < 25.0f && plWho->HasItemCount(30259, 1, false))
			{
				switch (me->GetEntry())
				{
				case 20132:
					if (!scryer_start)
					{
						me->setFaction(14);	
						scryer_start = true;
					}
					break;
				}
			}
		}
	}

	void JustDied(Unit* killer)
	{
		if (Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (player->GetQuestStatus(QUEST_DEATHBLOW_TO_THE_LEGION) == QUEST_STATUS_INCOMPLETE)
			{
				player->CompleteQuest(QUEST_DEATHBLOW_TO_THE_LEGION);
			}

			if (Creature* adveyn = me->FindNearestCreature(NPC_ADYEN_THE_LIGHTWARDEN, 40.0f, true))
				adveyn->DisappearAndDie();

			if (Creature* karja = me->FindNearestCreature(NPC_ANCHORITE_KARJA, 40.0f, true))
				karja->DisappearAndDie();

			if (Creature* orelis = me->FindNearestCreature(NPC_EXARCH_ORELIS, 40.0f, true))
				orelis->DisappearAndDie();
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (EventStarted == true)
			{
				if (wait_timer <= diff && !text1)
				{
					me->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
					me->MonsterYell(SOCRETHAR_TEXT_1, LANG_UNIVERSAL, 0);				
					text1 = true;
				}
				else wait_timer -= diff;

				if (wait_timer1 <= diff && text1 == true && !text2)
				{
					me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
					me->MonsterYell(SOCRETHAR_TEXT_2, LANG_UNIVERSAL, 0);
					summon_timer = 5000;
					text2 = true;
				}
				else wait_timer1 -= diff;

				if (summon_timer <= diff && !summoned && text2 == true)
				{
					me->SummonCreature(NPC_KAYLAAN, 4953.0f, 3901.9f, 211.3f, 4.66f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 180000);
					summoned = true;
				}
				else summon_timer -= diff;

				if (combat_timer <= diff && summoned == true && !text3)
				{
					me->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
					me->MonsterYell(SOCRETHAR_TEXT_3, LANG_UNIVERSAL, 0);
					text3 = true;

					if (Creature* kaylaan = me->FindNearestCreature(NPC_KAYLAAN, 20.0f, true))
					{
						DoCast(kaylaan, SPELL_POWER_OF_THE_LEGION);
						kaylaan->setFaction(14);

						if (Creature* adyen = me->FindNearestCreature(NPC_ADYEN_THE_LIGHTWARDEN, 50.0f, true))
						{
							kaylaan->AI()->AttackStart(adyen);
							adyen->AI()->AttackStart(kaylaan);
						}

						if (Creature* karja = me->FindNearestCreature(NPC_ANCHORITE_KARJA, 50.0f, true))
							karja->AI()->AttackStart(kaylaan);

						if (Creature* orelis = me->FindNearestCreature(NPC_EXARCH_ORELIS, 50.0f, true))
							orelis->AI()->AttackStart(kaylaan);
					}
				}
				else combat_timer -= diff;
			}

			if (combat_timer1 <= diff && text3 == true && !text4)
			{
				me->MonsterYell(SOCRETHAR_TEXT_4, LANG_UNIVERSAL, 0);
				text4 = true;
			}
			else combat_timer1 -= diff;
		}

		if (UpdateVictim())
		{
			if (backlash_timer <= diff)
			{
				DoCastVictim(SPELL_BACKLASH);
				backlash_timer = urand(12000, 15000);
			}
			else backlash_timer -= diff;

			if (cleave_timer <= diff)
			{
				DoCastVictim(SPELL_SOCRETHAR_CLEAVE);
				cleave_timer = 7500;
			}
			else cleave_timer -= diff;

			if (sbv_timer <= diff)
			{
				DoCastVictim(SPELL_SB_VOLLEY);
				sbv_timer = urand(18000, 20000);
			}
			else sbv_timer -= diff;

			if (barrage_timer <= diff)
			{
				DoCast(me, SPELL_FB_BARRAGE);
				barrage_timer = urand(25000, 30000);
			}
			else barrage_timer -= diff;

			if (antimagic_timer <= diff)
			{
				DoCast(me, SPELL_ANTI_MAGIC);
				antimagic_timer = 31000;
			}
			else antimagic_timer -= diff;
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_socrethar(Creature* pCreature)
{
	return new npc_socretharAI(pCreature);
}

#define ORELIS_TEXT_1 "How... how could you?!"
#define KAYLAN_TEXT_1 "My heart has been filled with hate since our sworn enemies were allowed into our city. I knew the one known as Voren'thal before he was called a Seer. It was by his hand that my brother was slain."
#define KAYLAN_TEXT_2 "I turned that hate on the Illidari and the Burning Legion... but they weren't the ones who betrayed us. We were the naaru's chosen! We lived and died for them!"
#define KAYLAN_TEXT_3 "Once the hatred in my heart became focused, everything became clear to me. Shattrath must be destroyed and the naaru with it."
#define KAYLAN_TEXT_4 "You're wrong, Adyen. My mind has never been clearer."
#define KAYLAN_TEXT_5 "Yes... master."
#define ADYEN_TEXT_3 "Socrethar is clouding your mind, Kaylaan! You do not mean these words! I remember training you when you were but a youngling. Your will was stron even then!"

#define SPELL_HOLY_SLAM 37572
#define SPELL_DIVINE_SHIELD 13874
#define SPELL_BURNING_LIGHT 37552
#define SPELL_AVENGERS_SHIELD 37554
#define SPELL_RESSURECTION 35599

struct npc_kaylaanAI : public ScriptedAI
{
	npc_kaylaanAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->setFaction(35);
		me->SetReactState(REACT_AGGRESSIVE);
		me->SetStandState(UNIT_STAND_STATE_STAND);

		// Event
		kneeled = false;
		summon_ishanah = false;
		adyen_spoke = false;
		stood = false;
		conv1 = false;
		conv2 = false;
		conv3 = false;
		conv4 = false;
		conv5 = false;
		combatstop = false;
		CombatEnd = false;
		reapllied = false;
		spellHit = false;

		// Skills
		divine_shield_timer = 25000;
		avenger_timer = 4000;
		holy_slam_timer = 9000;
		burning_timer = 6000;
	}

	// Event
	bool spellHit;
	bool kneeled;
	bool summon_ishanah;
	bool adyen_spoke;
	bool stood;
	bool conv1;
	bool conv2;
	bool conv3;
	bool conv4;
	bool conv5;
	bool reapllied;
	bool combatstop;
	bool CombatEnd;

	uint32 stand_timer;
	uint32 talk1_timer;
	uint32 talk1_timer1;
	uint32 talk1_timer2;
	uint32 talk1_timer3;
	uint32 talk1_timer4;
	uint32 summon_ishanah_timer;
	uint32 adyen_timer;
	uint32 reapply_buff_timer;

	//Skills
	
	uint32 divine_shield_timer;
	uint32 avenger_timer;
	uint32 burning_timer;
	uint32 holy_slam_timer;

	void SpellHit(Unit* Hitter, const SpellEntry* Spellkind)
	{
		if (Spellkind->Id == 35598 && !spellHit)
		{
			me->Kill(me);
			spellHit = true;
		}
	}

	void JustSummoned(Creature* summoned)
	{
		summoned->GetMotionMaster()->MovePoint(0, 4943.6f, 3839.6f, 211.5f);
	}

	void AdyenConversation()
	{
		if (Creature* adyen = me->FindNearestCreature(NPC_ADYEN_THE_LIGHTWARDEN, 60.0f, true))
		{
			adyen->MonsterSay(ADYEN_TEXT_3, LANG_UNIVERSAL, 0);
		}
	}

	void OrelisConversation()
	{
		if (Creature* orelis = me->FindNearestCreature(NPC_EXARCH_ORELIS, 60.0f, true))
		{
			orelis->MonsterSay(ORELIS_TEXT_1, LANG_UNIVERSAL, 0);
		}
	}

	void SocretharCombatMe()
	{
		if (Creature* socrethar = me->FindNearestCreature(NPC_SOCRETHAR, 60.0f, true))
		{
			CAST_AI(npc_socretharAI, socrethar->AI())->combat_timer = 10000;
		}
	}

	void SocretharStopMe()
	{
		if (Creature* socrethar = me->FindNearestCreature(NPC_SOCRETHAR, 60.0f, true))
		{
			CAST_AI(npc_socretharAI, socrethar->AI())->combat_timer1 = 3000;
		}
	}

	void LeaveCombat()
	{
		me->SummonCreature(61023, 4937.8f, 3849.4f, 211.5f, 4.2f, TEMPSUMMON_TIMED_DESPAWN, 10000);

		me->AI()->EnterEvadeMode();
		
		if (Creature* adveyn = me->FindNearestCreature(NPC_ADYEN_THE_LIGHTWARDEN, 40.0f, true))		
			adveyn->AI()->EnterEvadeMode();
		
		if (Creature* karja = me->FindNearestCreature(NPC_ANCHORITE_KARJA, 40.0f, true))
			karja->AI()->EnterEvadeMode();

		if (Creature* orelis = me->FindNearestCreature(NPC_EXARCH_ORELIS, 40.0f, true))
			orelis->AI()->EnterEvadeMode();	
	}

	void UpdateAI(const uint32 diff)
	{
		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (!UpdateVictim())
		{		
			if (!kneeled && !CombatEnd)
			{
				if (Creature* socrethar = me->FindNearestCreature(NPC_SOCRETHAR, 1.5f, true))
				{				
					me->SetStandState(UNIT_STAND_STATE_KNEEL);	
					OrelisConversation();
					stand_timer = 9000;
					kneeled = true;
				}
			}

			if (!stood && kneeled == true && !CombatEnd)
			{
				if (stand_timer <= diff)
				{
					if (Creature* adveyn = me->FindNearestCreature(NPC_ADYEN_THE_LIGHTWARDEN, 30.0f, true))
					{					
						me->SetStandState(UNIT_STAND_STATE_STAND);
						stood = true;
						talk1_timer = 5000;
					}
				}
				else stand_timer -= diff;
			}

			if (stood == true && !conv1 && !CombatEnd)
			{
				if (talk1_timer <= diff)
				{
					me->MonsterSay(KAYLAN_TEXT_1, LANG_UNIVERSAL, 0);
					conv1 = true;
					talk1_timer1 = 9000;
				}
				else talk1_timer -= diff;
			}

			if (conv1 == true && !conv2)
			{
				if (talk1_timer1 <= diff)
				{
					me->MonsterSay(KAYLAN_TEXT_2, LANG_UNIVERSAL, 0);
					conv2 = true;
					talk1_timer2 = 9000;
				}
				else talk1_timer1 -= diff;
			}

			if (stood == true && conv2 == true && !conv3)
			{
				if (talk1_timer2 <= diff)
				{
					me->MonsterSay(KAYLAN_TEXT_3, LANG_UNIVERSAL, 0);
					conv3 = true;				
					adyen_timer = 8000;				
				}
				else talk1_timer2 -= diff;
			}

			if (!adyen_spoke && conv3 == true)
			{
				if (adyen_timer <= diff)
				{
					AdyenConversation();
					adyen_spoke = true;
					talk1_timer3 = 7000;
				}
				else adyen_timer -= diff;
			}

			if (adyen_spoke == true && !conv4)
			{
				if (talk1_timer3 <= diff)
				{
					me->MonsterSay(KAYLAN_TEXT_4, LANG_UNIVERSAL, 0);
					conv4 = true;	
					SocretharCombatMe();
					talk1_timer4 = 6000;
				}
				else talk1_timer3 -= diff;
			}

			if (combatstop == true && !conv5)
			{
				if (talk1_timer4 <= diff)
				{
					me->MonsterSay(KAYLAN_TEXT_5, LANG_UNIVERSAL, 0);
					conv5 = true;							
					summon_ishanah_timer = 4000;
				}
				else talk1_timer4 -= diff;
			}

			if (conv5 == true && !summon_ishanah)
			{
				if (summon_ishanah_timer <= diff)
				{
					me->SummonCreature(NPC_ISHANAH, 4907.5f, 3823.1f, 211.4f, 0.5f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
					summon_ishanah = true;
				}
				else summon_ishanah_timer -= diff;
			}

			if (combatstop == true && reapply_buff_timer <= diff && !reapllied)
			{
				DoCast(me, SPELL_POWER_OF_THE_LEGION);
				reapllied = true;
			}
			else reapply_buff_timer -= diff;
		}

		if (UpdateVictim())
		{
			if (!combatstop && HealthBelowPct(5))
			{						
				LeaveCombat();			
				reapply_buff_timer = 1500;
				SocretharStopMe();
				combatstop = true;
			}

			if (burning_timer <= diff)
			{
				DoCastVictim(SPELL_BURNING_LIGHT);
				burning_timer = urand(9000, 12000);
			}
			else burning_timer -= diff;

			if (avenger_timer <= diff)
			{
				Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 4);
				DoCast(pTarget, SPELL_AVENGERS_SHIELD);
				avenger_timer = urand(15000, 20000);
			}
			avenger_timer -= diff;

			if (holy_slam_timer <= diff)
			{
				Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
				DoCast(pTarget, SPELL_HOLY_SLAM);
				holy_slam_timer = urand(5000, 10000);
			}
			else holy_slam_timer -= diff;

			if (divine_shield_timer <= diff)
			{
				DoCast(me, SPELL_DIVINE_SHIELD);
				divine_shield_timer = urand(30000, 45000);
			}
			else divine_shield_timer -= diff;
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_kaylaan(Creature* pCreature)
{
	return new npc_kaylaanAI(pCreature);
}

#define GOSSIP_ITEM_ALDOR_EVENT_START "I'm ready, Adyen."
#define ADYEN_TEXT_2 "We may be few, Socrethar, but our faith is strong. Something you will never understand. Now that custom has been served, prepare to meet your end."

struct adyen_lightwardenAI : public ScriptedAI
{
	adyen_lightwardenAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{		
		EventStart = false;
		EventStarted1 = true;
		started = false;
		text1 = false;

		dshield_timer = urand(30000, 60000);
		hl_timer = urand(20000, 28000);
		hoj_timer = urand(5000, 15000);
		crusader_timer = urand(6000, 10000);
	}

	bool EventStart;
	bool EventStarted1;
	bool started;
	bool text1;

	uint64 PlayerGUID;

	uint32 talk_timer;

	uint32 dshield_timer;
	uint32 hl_timer;
	uint32 hoj_timer;
	uint32 crusader_timer;

	void JustSummoned(Creature* summoned)
	{
		summoned->AI()->AttackStart(me);
		me->AI()->AttackStart(summoned);

		if (Creature* karja = me->FindNearestCreature(NPC_ANCHORITE_KARJA, 10.0f))
		{
			karja->AI()->AttackStart(summoned);
		}

		if (Creature* orelis = me->FindNearestCreature(NPC_EXARCH_ORELIS, 10.0f))
		{
			orelis->AI()->AttackStart(summoned);
		}
	}

	void SocretharEvent()
	{
		if (Creature* socrethar = me->FindNearestCreature(NPC_SOCRETHAR, 25.0f, true))
		{
			CAST_AI(npc_socretharAI, socrethar->AI())->EventStarted = true;
			CAST_AI(npc_socretharAI, socrethar->AI())->wait_timer = 15000;
		}
	}

	void SocretharEvent1()
	{
		if (Creature* socrethar = me->FindNearestCreature(NPC_SOCRETHAR, 25.0f, true))
		{
			CAST_AI(npc_socretharAI, socrethar->AI())->wait_timer1 = 12000;
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (!started && EventStarted1 == true)
			{
				if (Creature* sctrigger = me->FindNearestCreature(23491, 3.0f, true))
				{
					sctrigger->DisappearAndDie();
					DoScriptText(ADYEN_TEXT_1, me);
					talk_timer = 23000;
					SocretharEvent();
					started = true;
				}
			}

			if (EventStart == true)
			{
				me->GetMotionMaster()->MovePath(601601606, false);
				me->SummonCreature(20929, 4847.8f, 3777.6f, 204.8f, 3.3f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000);

				if (Creature* karja = me->FindNearestCreature(NPC_ANCHORITE_KARJA, 10.0f, true))
				{
					karja->GetMotionMaster()->MovePath(601601607, false);
				}

				if (Creature* orelis = me->FindNearestCreature(NPC_EXARCH_ORELIS, 10.0f, true))
				{
					orelis->GetMotionMaster()->MovePath(601601608, false);
				}
			}

			if (started == true && !text1 && EventStarted1 == true)
			{
				if (talk_timer <= diff)
				{
					me->MonsterSay(ADYEN_TEXT_2, LANG_UNIVERSAL, 0);
					SocretharEvent1();
					text1 = true;
				}
				else talk_timer -= diff;
			}
		}

		if (UpdateVictim())
		{
			if (hoj_timer <= diff)
			{
				DoCastVictim(13005);
				hoj_timer = urand(30000, 45000);
			}
			else hoj_timer -= diff;

			if (crusader_timer <= diff)
			{
				DoCastVictim(14518);
				crusader_timer = urand(10000, 22000);
			}
			else crusader_timer -= diff;

			if (dshield_timer <= diff)
			{
				DoCast(me, 13874);
				dshield_timer = urand(45000, 60000);
			}
			else dshield_timer -= diff;

			if (HealthBelowPct(50) && hl_timer <= diff)
			{
				DoCast(me, 13952);
				hl_timer = urand(20000, 25000);
			}
			else hl_timer -= diff;
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_adyen_lightwarden(Creature* pCreature)
{
	return new adyen_lightwardenAI(pCreature);
}

bool GossipHello_adyen_lightwarden(Player* player, Creature* creature)
{
	if (creature->isQuestGiver())
		player->PrepareQuestMenu(creature->GetGUID());

	if (player->GetQuestStatus(QUEST_DEATHBLOW_TO_THE_LEGION) == QUEST_STATUS_INCOMPLETE)
		player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_ALDOR_EVENT_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
	player->SEND_GOSSIP_MENU(23022, creature->GetGUID());

	return true;
}

bool GossipSelect_adyen_lightwarden(Player* player, Creature* creature, uint32 sender, uint32 action)
{
	switch (action)
	{
	case GOSSIP_ACTION_INFO_DEF + 1:
		CAST_AI(adyen_lightwardenAI, creature->AI())->EventStart = true;
		CAST_AI(adyen_lightwardenAI, creature->AI())->EventStarted1 = false;
		CAST_AI(adyen_lightwardenAI, creature->AI())->PlayerGUID = player->GetGUID();
		break;
	}
	return true;
}

struct npc_socre_helperAI : public ScriptedAI
{
	npc_socre_helperAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		SocStart = false;
	}

	bool SocStart;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (!SocStart)
			{
				if (Creature* kaylaan = me->FindNearestCreature(NPC_KAYLAAN, 50.0f, true))
				{
					CAST_AI(npc_kaylaanAI, kaylaan->AI())->CombatEnd = true;
				}
			}
		}
	}
};

CreatureAI* GetAI_npc_socre_helper(Creature* pCreature)
{
	return new npc_socre_helperAI(pCreature);
}

/*######
## legion_beam_trigger 
######*/

struct legion_beam_triggerAI : public ScriptedAI
{
	legion_beam_triggerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		if (Creature* target = me->FindNearestCreature(61025, 50.0f, true))
		{
			DoCast(target, 40227);
		}

		reCast_timer = 1000;
	}

	uint32 reCast_timer;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (reCast_timer <= diff)
			{
				if (Creature* target = me->FindNearestCreature(61025, 50.0f, true))
				{
					DoCast(target, 40227);
					reCast_timer = 30000;
				}
			}
			else reCast_timer -= diff;
		}
	}
};

CreatureAI* GetAI_legion_beam_trigger(Creature* pCreature)
{
	return new legion_beam_triggerAI(pCreature);
}

/*######
## arena_event_controller
######*/

struct arena_event_controllerAI : public ScriptedAI
{
	arena_event_controllerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		if (Creature* gladiator = me->FindNearestCreature(20854, 50.0f, true))
		{
			if (Creature* questgiver = me->FindNearestCreature(20763, 50.0f, true))
			{
				gladiator->AI()->AttackStart(questgiver);
				questgiver->AI()->AttackStart(gladiator);
			}
		}

		reCast_timer = 1000;
	}

	uint32 reCast_timer;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (reCast_timer <= diff)
			{
				if (Creature* gladiator = me->FindNearestCreature(20854, 50.0f, true))
				{
					if (Creature* questgiver = me->FindNearestCreature(20763, 50.0f, true))
					{
						gladiator->AI()->AttackStart(questgiver);
						questgiver->AI()->AttackStart(gladiator);
						reCast_timer = 120000;
					}
				}					
			}
			else reCast_timer -= diff;
		}
	}
};

CreatureAI* GetAI_arena_event_controller(Creature* pCreature)
{
	return new arena_event_controllerAI(pCreature);
}

/*######
## npc_windtrader_marid
######*/

#define QUEST_TROUBLESOME_DISTRACTIONS 10273
#define GOSSIP_ITEM_MARID_EVENT_START "Wind Trader Marid. I've returned with more information on the nether drakes. I'm prepared to be your business partner, and for an extra sum, I'll take care of that troublesome elf and her human friend."
#define MARID_START_SAY -1910190
#define MARID_SAY_1 "You didn't really think I'd do business with you again, did you? Sometimes, the best way to return to profitability is to know when to cut your losses."
#define MARID_WP 601601610

struct npc_windtrader_maridAI : public ScriptedAI
{
	npc_windtrader_maridAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		me->setFaction(1731);
		me->SetReactState(REACT_DEFENSIVE);

		PlayerGUID = 0;
		MaridEventStart = false;
		marid_say_1 = false;
		start_attack = false;
		summoned = false;
	}

	uint64 PlayerGUID;

	bool MaridEventStart;
	bool marid_say_1;
	bool start_attack;
	bool summoned;

	uint32 attack_timer;

	void JustSummoned(Creature* summoned)
	{
		summoned->AI()->AttackStart(me);
		me->AI()->AttackStart(summoned);
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (MaridEventStart == true)
			{
				me->GetMotionMaster()->MovePath(MARID_WP, false);

				if (!summoned)
				{
					me->SummonCreature(20606, 4308.0f, 2187.5f, 114.5f, 2.0f, TEMPSUMMON_TIMED_DESPAWN, 1000);
					summoned = true;
				}
			}

			if (!marid_say_1)
			{
				if (Creature* netherTrigger = me->FindNearestCreature(19656, 15.0f, true))
				{
					me->MonsterSay(MARID_SAY_1, LANG_UNIVERSAL, 0);
					attack_timer = 8000;
					marid_say_1 = true;					
				}
			}

			if (marid_say_1 == true && !start_attack && attack_timer <= diff)
			{
				me->setFaction(14);
				me->SetReactState(REACT_AGGRESSIVE);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				start_attack = true;
			}
			else attack_timer -= diff;
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_windtrader_marid(Creature* pCreature)
{
	return new npc_windtrader_maridAI(pCreature);
}

bool GossipHello_npc_windtrader_marid(Player* player, Creature* creature)
{
	if (creature->isQuestGiver())
		player->PrepareQuestMenu(creature->GetGUID());

	if (player->GetQuestStatus(QUEST_TROUBLESOME_DISTRACTIONS) == QUEST_STATUS_INCOMPLETE)
		player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_MARID_EVENT_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
	player->SEND_GOSSIP_MENU(9971, creature->GetGUID());

	return true;
}

bool GossipSelect_npc_windtrader_marid(Player* player, Creature* creature, uint32 sender, uint32 action)
{
	switch (action)
	{
	case GOSSIP_ACTION_INFO_DEF + 1:
		DoScriptText(MARID_START_SAY, creature, player);
		CAST_AI(npc_windtrader_maridAI, creature->AI())->MaridEventStart = true;
		CAST_AI(npc_windtrader_maridAI, creature->AI())->PlayerGUID = player->GetGUID();
		player->CLOSE_GOSSIP_MENU();
		break;
	}
	return true;
}

/*######
## npc_captured_protectorate
######*/

#define QUEST_ESCAPE_FROM_THE_STAGING_GROUNDS 10425
#define ESCORT_SAY "Let's get out of here, before more of them will come!"

struct npc_captured_protectorateAI : public npc_escortAI
{
	npc_captured_protectorateAI(Creature* pCreature) : npc_escortAI(pCreature) {}

	void Reset()
	{
		me->SetReactState(REACT_AGGRESSIVE);

		PlayerGUID = 0;
	}

	uint64 PlayerGUID;

	void WaypointReached(uint32 i)
	{
		Player* pPlayer = GetPlayerForEscort();

		if (!pPlayer)
			return;

		switch (i)
		{
		case 0:
			me->MonsterSay(ESCORT_SAY, LANG_UNIVERSAL, 0);
			break;
		case 15: //return and quest_complete
			if (pPlayer)
				pPlayer->CompleteQuest(QUEST_ESCAPE_FROM_THE_STAGING_GROUNDS);
			me->ForcedDespawn(15000);
			break;
		}
	}

	void UpdateAI(const uint32 uiDiff)
	{
		npc_escortAI::UpdateAI(uiDiff);

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_captured_protectorate(Creature* pCreature)
{
	npc_captured_protectorateAI* protectorateAI = new npc_captured_protectorateAI(pCreature);

	protectorateAI->AddWaypoint(0, 4072.77, 2300.59, 111.727, 0);
	protectorateAI->AddWaypoint(1, 4107.44, 2295.81, 106.518, 0);
	protectorateAI->AddWaypoint(2, 4132.99, 2296.5, 103.481, 0);
	protectorateAI->AddWaypoint(3, 4153.95, 2297.79, 102.345, 0);
	protectorateAI->AddWaypoint(4, 4169.84, 2302.15, 104.521, 0); 
	protectorateAI->AddWaypoint(5, 4188.36, 2302.81, 111.182, 0);
	protectorateAI->AddWaypoint(6, 4207.3, 2297.2, 118.857, 0);
	protectorateAI->AddWaypoint(7, 4227.99, 2295.01, 126.434, 0);
	protectorateAI->AddWaypoint(8, 4246.5, 2292.62, 129.425, 0);
	protectorateAI->AddWaypoint(9, 4263.98, 2290.41, 126.291, 0);
	protectorateAI->AddWaypoint(10, 4277.22, 2278.08, 125.221, 0);
	protectorateAI->AddWaypoint(11, 4284.31, 2252.51, 123.878, 0);
	protectorateAI->AddWaypoint(12, 4286.75, 2234.23, 124.217, 0);
	protectorateAI->AddWaypoint(13, 4276.17, 2217.82, 125.582, 0);
	protectorateAI->AddWaypoint(14, 4263.86, 2196.86, 135.513, 0);
	protectorateAI->AddWaypoint(15, 4254.55, 2180.13, 137.052, 0);

	return protectorateAI;
}

bool QuestAccept_npc_captured_protectorate(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
	if (quest->GetQuestId() == QUEST_ESCAPE_FROM_THE_STAGING_GROUNDS)
	{
		if (npc_captured_protectorateAI* pEscortAI = CAST_AI(npc_captured_protectorateAI, pCreature->AI()))
		{		
			CAST_AI(npc_captured_protectorateAI, pCreature->AI())->PlayerGUID = pPlayer->GetGUID();
			pEscortAI->Start(true, true, pPlayer->GetGUID(), quest);
		}
	}
		
	return true;
}

/*######
## npc_protectorate_demolitionist
######*/

#define QUEST_DELIVERING_THE_MESSAGE 10406
#define QUEST_START_SAY "Hostiles detected. Ending transmision."
#define ESCORT_DEMOLITIONIST "Let's do this... just keep me covered and I'll deliver the package."
#define ESCORT_DEMOLITIONIST_1 "By the second sun of K'aresh, look at this place! I can only imagine what Salhadaar is planning. Come on, let's keep going."
#define ESCORT_DEMOLITIONIST_2 "With this much void waste and run-off, a toxic void horror can't be too far behind."
#define ESCORT_DEMOLITIONIST_3 "Look there, fleshling! Salhadaar's conduits! He's keeping well fed...."
#define ESCORT_DEMOLITIONIST_4 "Alright, keep me protected while I plant this disruptor. This shouldn't take very long."
#define ESCORT_DEMOLITIONIST_5 "Done! Back up! Back up!"
#define ESCORT_DEMOLITIONIST_6 "Looks like my work here is done. Report back to the holo-image of Ameer over at the transponder."

struct npc_protectorate_demolitionistAI : public npc_escortAI
{
	npc_protectorate_demolitionistAI(Creature* pCreature) : npc_escortAI(pCreature) {}

	void Reset()
	{
		me->SetReactState(REACT_DEFENSIVE);		
		me->MonsterSay(ESCORT_DEMOLITIONIST, LANG_UNIVERSAL, 0);

		PlayerGUID = 0;
	}

	uint64 PlayerGUID;

	void JustDied()
	{
		Player* pPlayer = GetPlayerForEscort();

		if (!pPlayer)
			return;

		if (pPlayer)
			pPlayer->FailQuest(QUEST_DELIVERING_THE_MESSAGE);
	}

	void WaypointReached(uint32 i)
	{
		Player* pPlayer = GetPlayerForEscort();

		if (!pPlayer)
			return;

		switch (i)
		{
		case 0:
			me->MonsterSay(ESCORT_DEMOLITIONIST_1, LANG_UNIVERSAL, 0);
			me->HandleEmoteCommand(EMOTE_STATE_POINT);
			break;
		case 1:
			me->MonsterSay(ESCORT_DEMOLITIONIST_2, LANG_UNIVERSAL, 0);
			break;
		case 4:
			me->MonsterSay(ESCORT_DEMOLITIONIST_3, LANG_UNIVERSAL, 0);
			me->HandleEmoteCommand(EMOTE_STATE_POINT);
			break;
		case 7:
			me->MonsterSay(ESCORT_DEMOLITIONIST_4, LANG_UNIVERSAL, 0);
			me->HandleEmoteCommand(EMOTE_STATE_WORK);
			break;
		case 8:
			me->SummonCreature(20474, 3880.6f, 2323.4f, 112.9f, 3.23f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
			me->SummonCreature(20474, 3864.8f, 2320.4f, 112.9f, 0.43f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
			me->HandleEmoteCommand(EMOTE_STATE_WORK);
			break;
		case 9:
			me->MonsterSay(ESCORT_DEMOLITIONIST_5, LANG_UNIVERSAL, 0);
			SetRun();
			break;
		case 10:  //quest_complete		
			if (pPlayer)
				pPlayer->CompleteQuest(QUEST_DELIVERING_THE_MESSAGE);
			break;
		case 11:
			me->MonsterSay(ESCORT_DEMOLITIONIST_6, LANG_UNIVERSAL, 0);
			break;
		case 12:
			DoCast(me, 41232);
			break;
		case 13:
			me->DisappearAndDie();
			break;
		}
	}

	void UpdateAI(const uint32 uiDiff)
	{
		npc_escortAI::UpdateAI(uiDiff);

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_protectorate_demolitionist(Creature* pCreature)
{
	npc_protectorate_demolitionistAI* protectorateAI = new npc_protectorate_demolitionistAI(pCreature);

	protectorateAI->AddWaypoint(0, 3982.6, 2330.1, 113.8, 7000);
	protectorateAI->AddWaypoint(1, 3953.7, 2330.1, 113.9, 0);
	protectorateAI->AddWaypoint(2, 3918.4, 2338.6, 113.5, 0);
	protectorateAI->AddWaypoint(3, 3883.5, 2373.3, 114.0, 0);
	protectorateAI->AddWaypoint(4, 3876.7, 2383.2, 113.9, 7000);
	protectorateAI->AddWaypoint(5, 3854.6, 2349.8, 114.7, 0);
	protectorateAI->AddWaypoint(6, 3867.1, 2334.6, 115.1, 0);
	protectorateAI->AddWaypoint(7, 3872.9, 2321.5, 114.5, 5000);
	protectorateAI->AddWaypoint(8, 3872.9, 2321.5, 114.5, 12000);
	protectorateAI->AddWaypoint(9, 3872.9, 2321.5, 114.5, 0);
	protectorateAI->AddWaypoint(10, 3861.2, 2349.6, 115.0, 5000);
	protectorateAI->AddWaypoint(11, 3861.2, 2349.6, 115.0, 6000);
	protectorateAI->AddWaypoint(12, 3861.2, 2349.6, 115.0, 1000);
	protectorateAI->AddWaypoint(13, 3861.2, 2349.6, 115.0, 0);

	return protectorateAI;
}

struct demolitionist_triggerAI : public ScriptedAI
{
	demolitionist_triggerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		demo_found = false;

		PlayerGUID = 0;
	}

	bool demo_found;

	uint64 PlayerGUID;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (!demo_found)
			{
				if (Creature* demolitionist = me->FindNearestCreature(20802, 10.0f, true))
				{
					if (npc_protectorate_demolitionistAI* pEscortAI = CAST_AI(npc_protectorate_demolitionistAI, demolitionist->AI()))
					{
						CAST_AI(npc_protectorate_demolitionistAI, demolitionist->AI())->PlayerGUID;
						pEscortAI->Start(false, false, PlayerGUID);

						demo_found = true;
						me->DisappearAndDie();
					}
				}
			}
		}
	}
};

CreatureAI* GetAI_demolitionist_trigger(Creature* pCreature)
{
	return new demolitionist_triggerAI(pCreature);
}

bool QuestAccept_npc_image_of_commander_ameer(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
	if (quest->GetQuestId() == QUEST_DELIVERING_THE_MESSAGE)
	{
		pCreature->MonsterSay(QUEST_START_SAY, LANG_UNIVERSAL, 0);
		pCreature->DisappearAndDie();
		pPlayer->CastSpell(pPlayer, 35679, true);	

		if (Creature* demotrigger = pCreature->FindNearestCreature(61027, 30.0f, true))
		{
			CAST_AI(demolitionist_triggerAI, demotrigger->AI())->PlayerGUID = pPlayer->GetGUID();
		}
	}

	return true;
}

/*######
## npc_salhadaar
######*/

#define YELL_SALH_AGGRO "Prepare to enter oblivions, meddlers. You have unleashed god!"

struct npc_salhadaarAI : public ScriptedAI
{
	npc_salhadaarAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->setFaction(35);

		Count = 0;
		summoncopy = false;
		arcaneflux_timer = 8000;
		statis_timer = 18000;

	}

	uint8 Count;
	uint32 arcaneflux_timer;
	uint32 statis_timer;
	bool summoncopy;

	void Activate()
	{
		++Count;

		if (Count == 3)
		{
			if (me->HasAura(40225))
				me->RemoveAurasDueToSpell(40225);

			me->setFaction(14);
			me->MonsterYell(YELL_SALH_AGGRO, LANG_UNIVERSAL, 0);
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (!summoncopy && HealthBelowPct(50))
		{
			DoCast(36847);
			DoCast(36848);
			summoncopy = true;
		}

		if (statis_timer <= diff)
		{
			Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0);
			DoCast(target, 36527);
			statis_timer = urand(14000, 18000);
		}
		else statis_timer -= diff;

		if (arcaneflux_timer <= diff)
		{
			DoCast(36533);
			arcaneflux_timer = 35000;
		}
		else arcaneflux_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_salhadaar(Creature* pCreature)
{
	return new npc_salhadaarAI(pCreature);
}

/*######
## npc_energy_ball
######*/

struct npc_energy_ballAI : public ScriptedAI
{
	npc_energy_ballAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		if (Creature* salahaad = me->FindNearestCreature(20454, 100.0f, true))
		{
			me->CastSpell(salahaad, 40225, true);
		}

		object_found = false;
		beam_timer = 1000;
	}

	bool object_found;

	uint32 beam_timer;

	void UpdateAI(const uint32 diff)
	{
		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (!UpdateVictim())
		{
			if (!object_found)
			{
				if (GameObject* box = me->FindNearestGameObject(184561, 15.0f))
				{					
					me->ForcedDespawn(1000);
					object_found = true;		

					if (Creature* salahaad = me->FindNearestCreature(20454, 100.0f, true))
					{
						CAST_AI(npc_salhadaarAI, salahaad->AI())->Activate();
					}
				}
			}			

			if (beam_timer <= diff)
			{
				if (Creature* salahaad = me->FindNearestCreature(20454, 100.0f, true))
				{
					me->CastSpell(salahaad, 40225, true);
					beam_timer = 2000;				
				}
			}
			else beam_timer -= diff;
		}
	}
};

CreatureAI* GetAI_npc_energy_ball(Creature* pCreature)
{
	return new npc_energy_ballAI(pCreature);
}

/*######
## npc_captain_saeed
######*/

#define	NPC_DIMENSIUS 19554
#define ENTRY_DEFENDER_1 20984
#define ENTRY_DEFENDER_2 62000
#define ENTRY_DEFENDER_3 62001

#define ENTRY_REGENERATOR_1 21783
#define ENTRY_REGENERATOR_2 63000

#define ENTRY_AVENGER_1 21805
#define ENTRY_AVENGER_2 64000
#define ENTRY_AVENGER_3 64001
#define ENTRY_AVENGER_4 64002

#define ENTRY_SAEED 20985

#define QUEST_DIMENSIUS_THE_DEVOURER 10439
#define GOSSIP_ITEM_SAEED_EVENT_START "I am that fleshling, Saeed. Let's go!"
#define GOSSIP_ITEM_SAEED_EVENT_START_1 "I am ready. Let's make history!"
#define SAAED_START "You heard the fleshling! MOVE OUT!"
#define SAY_SAEED_PAUSE "Tell me when you are ready, $n. We will attack on your command."
#define SAY_SAEED_2 "It's now or never, soldiers! Let's do this! For K'aresh! For the Protectorate!"
#define SAY_SAEED_START_DIMENSIUS "The time for your destruction has finally come, Dimensius!"
#define SAY_SAEED_DEFEAT_DIMENSIUS "We are victorious! Dimensius is no more! Report back to the Professor, hero."

struct npc_captain_saeedAI : public npc_escortAI
{
	npc_captain_saeedAI(Creature* pCreature) : npc_escortAI(pCreature) {}

	void Reset()
	{
		playerGUID = 0;
		me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
	}

	uint64 playerGUID;

	void WaypointReached(uint32 i)
	{
		Player* pPlayer = GetPlayerForEscort();

		if (!pPlayer)
			return;

		switch (i)
		{
		case 0:
			me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
			break;
		case 1:
			me->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);

			if (Creature* def1 = me->FindNearestCreature(ENTRY_DEFENDER_1, 30.0f, true))
			{
				def1->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
			}

			if (Creature* def2 = me->FindNearestCreature(ENTRY_DEFENDER_2, 30.0f, true))
			{
				def2->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
			}

			if (Creature* def3 = me->FindNearestCreature(ENTRY_DEFENDER_3, 30.0f, true))
			{
				def3->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
			}

			if (Creature* reg1 = me->FindNearestCreature(ENTRY_REGENERATOR_1, 30.0f, true))
			{
				reg1->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
			}

			if (Creature* reg2 = me->FindNearestCreature(ENTRY_REGENERATOR_2, 30.0f, true))
			{
				reg2->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
			}

			if (Creature* aven1 = me->FindNearestCreature(ENTRY_AVENGER_1, 30.0f, true))
			{
				aven1->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
			}

			if (Creature* aven2 = me->FindNearestCreature(ENTRY_AVENGER_2, 30.0f, true))
			{
				aven2->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
			}

			if (Creature* aven3 = me->FindNearestCreature(ENTRY_AVENGER_3, 30.0f, true))
			{
				aven3->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
			}

			if (Creature* aven4 = me->FindNearestCreature(ENTRY_AVENGER_4, 30.0f, true))
			{
				aven4->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
			}
			break;
		case 2:
			if (Creature* def1 = me->FindNearestCreature(ENTRY_DEFENDER_1, 30.0f, true))
			{
				def1->GetMotionMaster()->MoveFollow(me, 3.0f, 0);

				if (Creature* reg1 = me->FindNearestCreature(ENTRY_REGENERATOR_1, 30.0f, true))
				{
					reg1->GetMotionMaster()->MoveFollow(def1, 2.0f, 0);
				}
			}

			if (Creature* def2 = me->FindNearestCreature(ENTRY_DEFENDER_2, 30.0f, true))
			{
				def2->GetMotionMaster()->MoveFollow(me, 5.0f, 0);

				if (Creature* reg2 = me->FindNearestCreature(ENTRY_REGENERATOR_2, 30.0f, true))
				{
					reg2->GetMotionMaster()->MoveFollow(def2, 2.0f, 0);
				}
			}

			if (Creature* def3 = me->FindNearestCreature(ENTRY_DEFENDER_3, 30.0f, true))
			{
				def3->GetMotionMaster()->MoveFollow(me, 7.0f, 0);

				if (Creature* av1 = me->FindNearestCreature(ENTRY_AVENGER_1, 30.0f, true))
				{
					av1->GetMotionMaster()->MoveFollow(def3, 2.0f, 0);
				}
			}

			if (Creature* av1 = me->FindNearestCreature(ENTRY_AVENGER_1, 30.0f, true))
			{			
				if (Creature* av2 = me->FindNearestCreature(ENTRY_AVENGER_2, 30.0f, true))
				{
					av2->GetMotionMaster()->MoveFollow(me, 2.0f, 0);

					if (Creature* av3 = me->FindNearestCreature(ENTRY_AVENGER_3, 30.0f, true))
					{
						av3->GetMotionMaster()->MoveFollow(av2, 2.0f, 0);

						if (Creature* av4 = me->FindNearestCreature(ENTRY_AVENGER_4, 30.0f, true))
						{
							av4->GetMotionMaster()->MoveFollow(av3, 2.0f, 0);
						}
					}
				}
			}
			break;
		case 12:
			me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
			me->MonsterSay(SAY_SAEED_PAUSE, LANG_UNIVERSAL, playerGUID);
			SetEscortPaused(true);	
			break;
		case 13:
			me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
			me->SetWalk(true);
			me->MonsterSay(SAY_SAEED_2, LANG_UNIVERSAL, 0);

			if (Creature* def1 = me->FindNearestCreature(ENTRY_DEFENDER_1, 30.0f, true))
			{
				def1->GetMotionMaster()->MoveFollow(me, 2.0f, 0);
				def1->SetWalk(true);

				if (Creature* def2 = me->FindNearestCreature(ENTRY_DEFENDER_2, 30.0f, true))
				{
					def2->GetMotionMaster()->MoveFollow(def1, 2.0f, 0);
					def2->SetWalk(true);

					if (Creature* def3 = me->FindNearestCreature(ENTRY_DEFENDER_3, 30.0f, true))
					{
						def3->GetMotionMaster()->MoveFollow(def2, 2.0f, 0);
						def3->SetWalk(true);

						if (Creature* av1 = me->FindNearestCreature(ENTRY_AVENGER_1, 30.0f, true))
						{
							av1->GetMotionMaster()->MoveFollow(def3, 2.0f, 0);
							av1->SetWalk(true);

							if (Creature* av2 = me->FindNearestCreature(ENTRY_AVENGER_2, 30.0f, true))
							{
								av2->GetMotionMaster()->MoveFollow(av1, 2.0f, 0);
								av2->SetWalk(true);

								if (Creature* av3 = me->FindNearestCreature(ENTRY_AVENGER_3, 30.0f, true))
								{
									av3->GetMotionMaster()->MoveFollow(av2, 2.0f, 0);
									av3->SetWalk(true);

									if (Creature* av4 = me->FindNearestCreature(ENTRY_AVENGER_4, 30.0f, true))
									{
										av4->GetMotionMaster()->MoveFollow(av3, 2.0f, 0);
										av4->SetWalk(true);

										if (Creature* reg1 = me->FindNearestCreature(ENTRY_REGENERATOR_1, 30.0f, true))
										{
											reg1->GetMotionMaster()->MoveFollow(av4, 2.0f, 0);
											reg1->SetWalk(true);

											if (Creature* reg2 = me->FindNearestCreature(ENTRY_REGENERATOR_2, 30.0f, true))
											{
												reg2->GetMotionMaster()->MoveFollow(reg1, 2.0f, 0);
												reg2->SetWalk(true);
											}
										}
									}
								}
							}
						}
					}
				}
			}
			break;
		case 17:
			me->MonsterSay(SAY_SAEED_START_DIMENSIUS, LANG_UNIVERSAL, 0);
			SetRun();

			if (Creature* dimension = me->FindNearestCreature(21035, 25.0f, true))
			{
				dimension->DisappearAndDie();
				me->SummonCreature(NPC_DIMENSIUS, 3936.4f, 2003.6f, 255.6f, 0.9f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
			}				
			break;
		case 18:
			if (Creature* dimensius = me->FindNearestCreature(NPC_DIMENSIUS, 50.0f, true))
			{
				me->AI()->AttackStart(dimensius);
			}
			break;
		case 20:
			me->MonsterSay(SAY_SAEED_DEFEAT_DIMENSIUS, LANG_UNIVERSAL, 0);
			break;
		case 21:
			if (Creature* def1 = me->FindNearestCreature(ENTRY_DEFENDER_1, 30.0f, true))
			{
				def1->DisappearAndDie();

				if (Creature* def2 = me->FindNearestCreature(ENTRY_DEFENDER_2, 30.0f, true))
				{
					def2->DisappearAndDie();

					if (Creature* def3 = me->FindNearestCreature(ENTRY_DEFENDER_3, 30.0f, true))
					{
						def3->DisappearAndDie();

						if (Creature* av1 = me->FindNearestCreature(ENTRY_AVENGER_1, 30.0f, true))
						{
							av1->DisappearAndDie();

							if (Creature* av2 = me->FindNearestCreature(ENTRY_AVENGER_2, 30.0f, true))
							{
								av2->DisappearAndDie();

								if (Creature* av3 = me->FindNearestCreature(ENTRY_AVENGER_3, 30.0f, true))
								{
									av3->DisappearAndDie();

									if (Creature* av4 = me->FindNearestCreature(ENTRY_AVENGER_4, 30.0f, true))
									{
										av4->DisappearAndDie();

										if (Creature* reg1 = me->FindNearestCreature(ENTRY_REGENERATOR_1, 30.0f, true))
										{
											reg1->DisappearAndDie();

											if (Creature* reg2 = me->FindNearestCreature(ENTRY_REGENERATOR_2, 30.0f, true))
											{
												reg2->DisappearAndDie();
											}
										}
									}
								}
							}
						}
					}
				}
			}	
			me->DisappearAndDie();
			break;
		}
	}

	void UpdateAI(const uint32 uiDiff)
	{
		npc_escortAI::UpdateAI(uiDiff);

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_captain_saeed(Creature* pCreature)
{
	npc_captain_saeedAI* captain_saeedAI = new npc_captain_saeedAI(pCreature);

	captain_saeedAI->AddWaypoint(0, 4264.2, 2102.7, 140.8, 3000);
	captain_saeedAI->AddWaypoint(1, 4264.2, 2102.7, 140.8, 2000);
	captain_saeedAI->AddWaypoint(2, 4264.2, 2102.7, 140.8, 500);
	captain_saeedAI->AddWaypoint(3, 4188.8, 2140.9, 154.8, 0);
	captain_saeedAI->AddWaypoint(4, 4186.5, 2089.6, 159.7, 0);
	captain_saeedAI->AddWaypoint(5, 4171.3, 2060.7, 168.0, 0);
	captain_saeedAI->AddWaypoint(6, 4188.0, 2032.7, 181.0, 0);
	captain_saeedAI->AddWaypoint(7, 4176.2, 1997.5, 201.6, 0);
	captain_saeedAI->AddWaypoint(8, 4124.0, 1975.5, 221.9, 0);
	captain_saeedAI->AddWaypoint(9, 4089.0, 2033.5, 238.2, 0);
	captain_saeedAI->AddWaypoint(10, 4054.4, 2063.2, 251.4, 0);
	captain_saeedAI->AddWaypoint(11, 4002.6, 2097.1, 254.2, 0);
	captain_saeedAI->AddWaypoint(12, 3995.4, 2091.9, 254.3, 0);      // Entrance to Dimensius
	captain_saeedAI->AddWaypoint(13, 3995.4, 2091.9, 254.3, 10000);  // Make new formation // SetWalk
	captain_saeedAI->AddWaypoint(14, 3978.9, 2067.8, 256.4, 0);
	captain_saeedAI->AddWaypoint(15, 3958.5, 2037.2, 257.8, 0); 
	captain_saeedAI->AddWaypoint(16, 3948.4, 2021.4, 256.4, 5000);   // End of Road -> wait 5s
	captain_saeedAI->AddWaypoint(17, 3948.4, 2021.4, 256.4, 0);
	captain_saeedAI->AddWaypoint(18, 3948.4, 2021.4, 256.4, 7000);   // wait 3s. for AttackStart
	captain_saeedAI->AddWaypoint(19, 3948.4, 2021.4, 256.4, 3000);
	captain_saeedAI->AddWaypoint(20, 3948.4, 2021.4, 256.4, 6000);   // Say Kill Dimensius
	captain_saeedAI->AddWaypoint(21, 3948.4, 2021.4, 256.4, 0);	     // Disappear with all adds

	return captain_saeedAI;
}

bool GossipHello_npc_captain_saeed(Player* player, Creature* creature)
{
	if (creature->isQuestGiver())
		player->PrepareQuestMenu(creature->GetGUID());

	if (creature->FindNearestCreature(61028, 10.0f, true))
	{
		if (player->GetQuestStatus(QUEST_DIMENSIUS_THE_DEVOURER) == QUEST_STATUS_INCOMPLETE)
			player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_SAEED_EVENT_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		player->SEND_GOSSIP_MENU(10229, creature->GetGUID());
	}

		if (creature->FindNearestCreature(19563, 10.0f, true))
		{
			if (player->GetQuestStatus(QUEST_DIMENSIUS_THE_DEVOURER) == QUEST_STATUS_INCOMPLETE)
				player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_SAEED_EVENT_START_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
			player->SEND_GOSSIP_MENU(10232, creature->GetGUID());
		}

	return true;
}

bool GossipSelect_npc_captain_saeed(Player* player, Creature* creature, uint32 sender, uint32 action)
{
	switch (action)
	{
	case GOSSIP_ACTION_INFO_DEF + 1:
		creature->MonsterSay(SAAED_START, LANG_UNIVERSAL, 0);

		CAST_AI(npc_captain_saeedAI, creature->AI())->playerGUID = player->GetGUID();
		if (npc_captain_saeedAI* pEscortAI = CAST_AI(npc_captain_saeedAI, creature->AI()))
			pEscortAI->Start(true, true, player->GetGUID());
		
		player->TalkedToCreature(creature->GetEntry(), creature->GetGUID());
		player->CLOSE_GOSSIP_MENU();
		break;

	case GOSSIP_ACTION_INFO_DEF + 2:			
		CAST_AI(npc_captain_saeedAI, creature->AI())->SetEscortPaused(false);
		player->CLOSE_GOSSIP_MENU();
		break;
	}
	return true;
}

/*######
## npc_dimensius_devourer
######*/

#define DIMENSIUS_LAUGH_EMOTE "Dimensius the All-Devouring laughs."
#define DIMENSIUS_YELL "Time only has meaning to mortals, insect. Dimensius is infinite!"
#define DIMENSIUS_SUMMON_SPAWNS -1910191
#define DIMENSIUS_SUMMON_SPAWNS1 -1910192
#define SPAWN_EMOTE "Spawn of Dimensius begins channeling power into Dimensius"
#define ENTRY_SPAWNS 21780
#define SPELL_SHADOW_SPIRAL 37500
#define SPELL_SHADOW_VAULT 37412
#define SPELL_DIMENSIUS_FEEDING 37450
#define SPELL_SHADOW_RAIN_35 37409
#define SPELL_SHADOW_RAIN_25 37396 // 25
#define SPELL_SHADOW_RAIN_15 37397 // 15
#define SPELL_SHADOW_RAIN_5  37399 // 5
#define SPELL_SHADOW_RAIN_10 37405 // 10

struct npc_dimensius_devourerAI : public ScriptedAI
{
	npc_dimensius_devourerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	bool start_combat;
	uint32 start_combat_timer;
	uint32 shadow_spiral_timer;
	uint32 shadow_vault_timer;
	uint32 summon_spawns_timer;

	void Reset()
	{
		me->SetReactState(REACT_AGGRESSIVE);
		me->setFaction(35);

		summon_spawns_timer = 20000;
		shadow_spiral_timer = 11000;
		shadow_vault_timer = 15000;
		start_combat_timer = 2000;
		start_combat = false;
	}

	void JustSummoned(Creature* summoned)
	{
		summoned->MonsterTextEmote(SPAWN_EMOTE, 0, false);
		summoned->CastSpell(me, SPELL_DIMENSIUS_FEEDING, true);
	}

	void MoveInLineOfSight(Unit* pWho)
	{
		if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (me->HasAura(SPELL_DIMENSIUS_FEEDING))
			{
				if (me->IsWithinDistInMap(plWho, 5.0f, false))
				{
					DoCast(SPELL_SHADOW_RAIN_5);
				}
			}
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (!UpdateVictim())
		{
			if (start_combat_timer <= diff && !start_combat)
			{
				me->setFaction(14);
				me->MonsterTextEmote(DIMENSIUS_LAUGH_EMOTE, 0, false);
				me->MonsterYell(DIMENSIUS_YELL, LANG_UNIVERSAL, 0);
				start_combat = true;

				if (Creature* saeed = me->FindNearestCreature(ENTRY_SAEED, 50.0f, true))
				{
					me->AI()->AttackStart(saeed);
					saeed->AI()->AttackStart(me);
				}
			}
			else start_combat_timer -= diff;
		}

		if (UpdateVictim())
		{
			if (summon_spawns_timer <= diff)
			{
				DoScriptText(RAND(DIMENSIUS_SUMMON_SPAWNS, DIMENSIUS_SUMMON_SPAWNS1), me);

				me->SummonCreature(ENTRY_SPAWNS, 3958.3f, 1995.3f, 256.6f, 2.88f, TEMPSUMMON_TIMED_DESPAWN, 60000); // LOC 1
				me->SummonCreature(ENTRY_SPAWNS, 3943.2f, 2025.4f, 256.5f, 4.45f, TEMPSUMMON_TIMED_DESPAWN, 60000); // LOC 2
				me->SummonCreature(ENTRY_SPAWNS, 3914.0f, 2011.5f, 256.6f, 5.81f, TEMPSUMMON_TIMED_DESPAWN, 60000); // LOC 3
				me->SummonCreature(ENTRY_SPAWNS, 3929.3f, 1980.8f, 256.6f, 1.18f, TEMPSUMMON_TIMED_DESPAWN, 60000); // LOC 4

				summon_spawns_timer = 60000;
			}
			else summon_spawns_timer -= diff;

			if (shadow_vault_timer <= diff)
			{
				Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0);
				DoCast(target, SPELL_SHADOW_VAULT);
				shadow_vault_timer = urand(15000, 20000);
			}
			else shadow_vault_timer -= diff;

			if (shadow_spiral_timer <= diff)
			{
				Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0);
				DoCast(target, SPELL_SHADOW_SPIRAL);
				shadow_spiral_timer = urand(10000, 15000);
			}
			else shadow_spiral_timer -= diff;

			DoMeleeAttackIfReady();
		}
	}
};

CreatureAI* GetAI_npc_dimensius_devourer(Creature* pCreature)
{
	return new npc_dimensius_devourerAI(pCreature);
}

void AddSC_netherstorm()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "go_manaforge_control_console";
    newscript->pGOHello = &GOHello_go_manaforge_control_console;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_manaforge_control_console";
    newscript->GetAI = &GetAI_npc_manaforge_control_console;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_commander_dawnforge";
    newscript->GetAI = &GetAI_npc_commander_dawnforge;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "at_commander_dawnforge";
    newscript->pAreaTrigger = &AreaTrigger_at_commander_dawnforge;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_professor_dabiri";
    newscript->pGossipHello =   &GossipHello_npc_professor_dabiri;
    newscript->pGossipSelect =  &GossipSelect_npc_professor_dabiri;
    newscript->pQuestAccept = &QuestAccept_npc_professor_dabiri;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_phase_hunter";
    newscript->GetAI = &GetAI_npc_phase_hunter;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_bessy";
    newscript->GetAI = &GetAI_npc_bessy;
    newscript->pQuestAccept = &QuestAccept_npc_bessy;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_maxx_a_million_escort";
    newscript->GetAI = &GetAI_npc_maxx_a_million_escort;
    newscript->pQuestAccept = &QuestAccept_npc_maxx_a_million_escort;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_zeppit";
    newscript->GetAI = &GetAI_npc_zeppit;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_dr_boom";
    newscript->GetAI = &GetAI_npc_dr_boom;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_boom_bot";
    newscript->GetAI = &GetAI_npc_boom_bot;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_drijya";
    newscript->GetAI = &GetAI_npc_drijya;
    newscript->pQuestAccept = &QuestAccept_npc_drijya;
    newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_mana_wraith";
	newscript->GetAI = &GetAI_npc_mana_wraith;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_warp_abberation";
	newscript->GetAI = &GetAI_npc_warp_abberation;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_scrapped_fel_reaver";
	newscript->GetAI = &GetAI_npc_scrapped_fel_reaver;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_mana_seeker";
	newscript->GetAI = &GetAI_npc_mana_seeker;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_mageslayer";
	newscript->GetAI = &GetAI_npc_mageslayer;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_naberius";
	newscript->GetAI = &GetAI_npc_naberius;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_towercurse_trigger";
	newscript->GetAI = &GetAI_npc_towercurse_trigger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_towerchannel_trigger";
	newscript->GetAI = &GetAI_npc_towerchannel_trigger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_towerchanneler_trigger";
	newscript->GetAI = &GetAI_npc_towerchanneler_trigger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_doctor_vomisa";
	newscript->GetAI = &GetAI_npc_doctor_vomisa;
	newscript->pQuestAccept = &QuestAccept_npc_doctor_vomisa;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_negatron";
	newscript->GetAI = &GetAI_npc_negatron;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_farahlon_lasher";
	newscript->GetAI = &GetAI_npc_farahlon_lasher;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_talbuk_doe";
	newscript->GetAI = &GetAI_npc_talbuk_doe;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_talbuk_sire";
	newscript->GetAI = &GetAI_npc_talbuk_sire;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_warp_monstrosity";
	newscript->GetAI = &GetAI_npc_warp_monstrosity;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_inactive_fel_reaver";
	newscript->pChooseReward = &ChooseReward_npc_inactive_fel_reaver;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "gehenna_trigger";
	newscript->GetAI = &GetAI_gehenna_trigger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "legion_aldor_trigger";
	newscript->GetAI = &GetAI_legion_aldor_trigger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "adyen_lightwarden";
	newscript->GetAI = &GetAI_adyen_lightwarden;
	newscript->pGossipHello = &GossipHello_adyen_lightwarden;
	newscript->pGossipSelect = &GossipSelect_adyen_lightwarden;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_socrethar";
	newscript->GetAI = &GetAI_npc_socrethar;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_kaylaan";
	newscript->GetAI = &GetAI_npc_kaylaan;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_socre_helper";
	newscript->GetAI = &GetAI_npc_socre_helper;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "legion_beam_trigger";
	newscript->GetAI = &GetAI_legion_beam_trigger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "arena_event_controller";
	newscript->GetAI = &GetAI_arena_event_controller;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_windtrader_marid";
	newscript->GetAI = &GetAI_npc_windtrader_marid;
	newscript->pGossipHello = &GossipHello_npc_windtrader_marid;
	newscript->pGossipSelect = &GossipSelect_npc_windtrader_marid;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_captured_protectorate";
	newscript->GetAI = &GetAI_npc_captured_protectorate;
	newscript->pQuestAccept = &QuestAccept_npc_captured_protectorate;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_image_of_commander_ameer";
	newscript->pQuestAccept = &QuestAccept_npc_image_of_commander_ameer;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_protectorate_demolitionist";
	newscript->GetAI = &GetAI_npc_protectorate_demolitionist;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "demolitionist_trigger";
	newscript->GetAI = &GetAI_demolitionist_trigger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_energy_ball";
	newscript->GetAI = &GetAI_npc_energy_ball;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_salhadaar";
	newscript->GetAI = &GetAI_npc_salhadaar;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_captain_saeed";
	newscript->GetAI = &GetAI_npc_captain_saeed;
	newscript->pGossipHello = &GossipHello_npc_captain_saeed;
	newscript->pGossipSelect = &GossipSelect_npc_captain_saeed;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_dimensius_devourer";
	newscript->GetAI = &GetAI_npc_dimensius_devourer;
    newscript->RegisterSelf();
}

