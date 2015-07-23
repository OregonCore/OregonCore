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
SDName: Hellfire_Peninsula
SD%Complete: 99
 SDComment: Quest support: 9375, 9418, 10129, 10146, 10162, 10163, 10340, 10346, 10347, 10382 (Special flight paths), 9410, 10909, 10935, 9545, 10838, 11516, 10351.
SDCategory: Hellfire Peninsula
EndScriptData */

/* ContentData
npc_aeranas
npc_ancestral_wolf
go_haaleshi_altar
npc_naladu
npc_tracy_proudwell
npc_trollbane
npc_fel_guard_hound
npc_wing_commander_dabiree
npc_gryphoneer_leafbeard
npc_wing_commander_brack
npc_wounded_blood_elf
npc_hand_berserker
npc_anchorite_relic_bunny
npc_anchorite_barada
npc_darkness_released
npc_foul_purge
npc_sedai_quest_credit_marker
npc_vindicator_sedai
npc_demoniac_scryer
npc_magic_sucker_device_spawner
npc_living_flare
npc_pathaleon_image
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"

/*######
## npc_aeranas
######*/
enum eAeranas
{
    SAY_SUMMON                      = -1000138,
    SAY_FREE                        = -1000139,

    FACTION_HOSTILE                 = 16,
    FACTION_FRIENDLY                = 35,

    SPELL_ENVELOPING_WINDS          = 15535,
    SPELL_SHOCK                     = 12553,

    C_AERANAS                       = 17085
};

struct npc_aeranasAI : public ScriptedAI
{
    npc_aeranasAI(Creature* c) : ScriptedAI(c) {}

    uint32 Faction_Timer;
    uint32 EnvelopingWinds_Timer;
    uint32 Shock_Timer;

    void Reset()
    {
        Faction_Timer = 8000;
        EnvelopingWinds_Timer = 9000;
        Shock_Timer = 5000;

        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        me->setFaction(FACTION_FRIENDLY);

        DoScriptText(SAY_SUMMON, me);
    }

    void EnterCombat(Unit*) {}

    void UpdateAI(const uint32 diff)
    {
        if (Faction_Timer)
        {
            if (Faction_Timer <= diff)
            {
                me->setFaction(FACTION_HOSTILE);
                Faction_Timer = 0;
            }
            else Faction_Timer -= diff;
        }

        if (!UpdateVictim())
            return;

        if ((me->GetHealth() * 100) / me->GetMaxHealth() < 30)
        {
            me->setFaction(FACTION_FRIENDLY);
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            me->RemoveAllAuras();
            me->DeleteThreatList();
            me->CombatStop();
            DoScriptText(SAY_FREE, me);
            return;
        }

        if (Shock_Timer <= diff)
        {
            DoCastVictim(SPELL_SHOCK);
            Shock_Timer = 10000;
        }
        else Shock_Timer -= diff;

        if (EnvelopingWinds_Timer <= diff)
        {
            DoCastVictim(SPELL_ENVELOPING_WINDS);
            EnvelopingWinds_Timer = 25000;
        }
        else EnvelopingWinds_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_aeranas(Creature* pCreature)
{
    return new npc_aeranasAI (pCreature);
}

/*######
## npc_ancestral_wolf
######*/

enum
{
    EMOTE_WOLF_LIFT_HEAD         = -1000496,
    EMOTE_WOLF_HOWL              = -1000497,
    SAY_WOLF_WELCOME             = -1000498,

    SPELL_ANCESTRAL_WOLF_BUFF    = 29981,

    NPC_RYGA                     = 17123
};

struct npc_ancestral_wolfAI : public npc_escortAI
{
    npc_ancestral_wolfAI(Creature* pCreature) : npc_escortAI(pCreature)  {}

    void Reset()
    {
        me->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
    }

    void AttackedBy(Unit*) {}
    void AttackStart(Unit*) {}

    void WaypointReached(uint32 uiPointId)
    {
        switch (uiPointId)
        {
        case 0:
            DoScriptText(EMOTE_WOLF_LIFT_HEAD, me);
            DoCast(me, SPELL_ANCESTRAL_WOLF_BUFF, true);
            break;
        case 2:
            DoScriptText(EMOTE_WOLF_HOWL, me);
            break;
        case 5:
            SetRun();
            break;
        case 15:
            SetRun(false);
            break;
        case 23:
            SetRun();
            break;
        case 42:
            SetRun(false);
            DoCast(me, SPELL_ANCESTRAL_WOLF_BUFF, true);
            break;
        case 50:
            if (Creature* pRyga = me->FindNearestCreature(NPC_RYGA, 15.0f))
                if (pRyga && pRyga->IsAlive() && !pRyga->IsInCombat())
                    DoScriptText(SAY_WOLF_WELCOME, pRyga);
            break;
        }
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        if (pWho->GetTypeId() == TYPEID_PLAYER)
            Start(false, false, pWho->GetGUID());
    }
};

CreatureAI* GetAI_npc_ancestral_wolf(Creature* pCreature)
{
    return new npc_ancestral_wolfAI(pCreature);
}

/*######
## go_haaleshi_altar
######*/

bool GOHello_go_haaleshi_altar(Player*, GameObject* _GO)
{
    _GO->SummonCreature(C_AERANAS, -1321.79f, 4043.80f, 116.24f, 1.25f, TEMPSUMMON_TIMED_DESPAWN, 180000);
    return false;
}


/*######
## npc_naladu
######*/

#define GOSSIP_NALADU_ITEM1 "Why don't you escape?"

enum eNaladu
{
    GOSSIP_TEXTID_NALADU1   = 9788
};

bool GossipHello_npc_naladu(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_NALADU_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_naladu(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_NALADU1, pCreature->GetGUID());

    return true;
}

/*######
## npc_tracy_proudwell
######*/

#define GOSSIP_TEXT_REDEEM_MARKS        "I have marks to redeem!"
#define GOSSIP_TRACY_PROUDWELL_ITEM1    "I heard that your dog Fei Fei took Klatu's prayer beads..."
#define GOSSIP_TRACY_PROUDWELL_ITEM2    "<back>"

enum eTracy
{
    GOSSIP_TEXTID_TRACY_PROUDWELL1       = 10689,
    QUEST_DIGGING_FOR_PRAYER_BEADS       = 10916
};

bool GossipHello_npc_tracy_proudwell(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pCreature->isVendor())
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_REDEEM_MARKS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    if (pPlayer->GetQuestStatus(QUEST_DIGGING_FOR_PRAYER_BEADS) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TRACY_PROUDWELL_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_tracy_proudwell(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF+1:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TRACY_PROUDWELL_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_TRACY_PROUDWELL1, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_TRADE:
        pPlayer->SEND_VENDORLIST(pCreature->GetGUID());
        break;
    }

    return true;
}

/*######
## npc_trollbane
######*/

#define GOSSIP_TROLLBANE_ITEM1      "Tell me of the Sons of Lothar."
#define GOSSIP_TROLLBANE_ITEM2      "<more>"
#define GOSSIP_TROLLBANE_ITEM3      "Tell me of your homeland."

enum eTrollbane
{
    GOSSIP_TEXTID_TROLLBANE1        = 9932,
    GOSSIP_TEXTID_TROLLBANE2        = 9933,
    GOSSIP_TEXTID_TROLLBANE3        = 8772
};

bool GossipHello_npc_trollbane(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TROLLBANE_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TROLLBANE_ITEM3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_trollbane(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF+1:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TROLLBANE_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_TROLLBANE1, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_TROLLBANE2, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+3:
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_TROLLBANE3, pCreature->GetGUID());
        break;
    }

    return true;
}

/*######
## npc_fel_guard_hound
######*/

enum eFelGuard
{
    SPELL_SUMMON_POO                              = 37688,

    NPC_DERANGED_HELBOAR                          = 16863
};

struct npc_fel_guard_houndAI : public ScriptedAI
{
    npc_fel_guard_houndAI(Creature* pCreature) : ScriptedAI(pCreature) {}

    uint32 uiCheckTimer;
    uint64 uiHelboarGUID;

    void Reset()
    {
        uiCheckTimer = 5000; //check for creature every 5 sec
        uiHelboarGUID = 0;
    }

    void MovementInform(uint32 uiType, uint32 uiId)
    {
        if (uiType != POINT_MOTION_TYPE || uiId != 1)
            return;

        if (Creature* pHelboar = me->GetCreature(*me, uiHelboarGUID))
        {
            pHelboar->RemoveCorpse();
            DoCast(SPELL_SUMMON_POO);

            if (Player* pOwner = me->GetCharmerOrOwnerPlayerOrPlayerItself())
                me->GetMotionMaster()->MoveFollow(pOwner, 0.0f, 0.0f);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (uiCheckTimer <= uiDiff)
        {
            if (Creature* pHelboar = me->FindNearestCreature(NPC_DERANGED_HELBOAR, 10.0f, false))
            {
                if (pHelboar->GetGUID() != uiHelboarGUID && me->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE && !me->FindCurrentSpellBySpellId(SPELL_SUMMON_POO))
                {
                    uiHelboarGUID = pHelboar->GetGUID();
                    me->GetMotionMaster()->MovePoint(1, pHelboar->GetPositionX(), pHelboar->GetPositionY(), pHelboar->GetPositionZ());
                }
            }
            uiCheckTimer = 5000;
        }
        else uiCheckTimer -= uiDiff;

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_fel_guard_hound(Creature* pCreature)
{
    return new npc_fel_guard_houndAI(pCreature);
}

/*######
## npc_wing_commander_dabiree
######*/

#define GOSSIP_ITEM1_DAB "Fly me to Murketh and Shaadraz Gateways"
#define GOSSIP_ITEM2_DAB "Fly me to Shatter Point"

bool GossipHello_npc_wing_commander_dabiree(Player* player, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        player->PrepareQuestMenu(pCreature->GetGUID());

    //Mission: The Murketh and Shaadraz Gateways
    if (player->GetQuestStatus(10146) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(2, GOSSIP_ITEM1_DAB, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    //Shatter Point
    if (!player->GetQuestRewardStatus(10340))
        player->ADD_GOSSIP_ITEM(2, GOSSIP_ITEM2_DAB, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_wing_commander_dabiree(Player* player, Creature* /*pCreature*/, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        player->CLOSE_GOSSIP_MENU();
        player->CastSpell(player, 33768, true);             //TaxiPath 585 (Gateways Murket and Shaadraz)
    }
    if (action == GOSSIP_ACTION_INFO_DEF + 2)
    {
        player->CLOSE_GOSSIP_MENU();
        player->CastSpell(player, 35069, true);             //TaxiPath 612 (Taxi - Hellfire Peninsula - Expedition Point to Shatter Point)
    }
    return true;
}

/*######
## npc_gryphoneer_leafbeard
######*/

enum
{
    SPELL_TAXI_TO_SHATTERP      = 35066
};

#define GOSSIP_ITEM1_LEAF       "Fly me to Shatter Point"

bool GossipHello_npc_gryphoneer_leafbeard(Player* player, Creature* pCreature)
{
    //Go back to Shatter Point if player has completed the quest 10340 - Shatter Point
    if (player->GetQuestStatus(10340) == QUEST_STATUS_COMPLETE)
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM1_LEAF, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_gryphoneer_leafbeard(Player* player, Creature* /*pCreature*/, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        player->CLOSE_GOSSIP_MENU();
        //TaxiPath 609 (3.x.x)
        player->CastSpell(player, SPELL_TAXI_TO_SHATTERP, true);
    }
    return true;
}

/*######
## npc_wing_commander_brack
######*/

#define GOSSIP_ITEM1_BRA "Fly me to Murketh and Shaadraz Gateways"
#define GOSSIP_ITEM2_BRA "Fly me to The Abyssal Shelf"
#define GOSSIP_ITEM3_BRA "Fly me to Spinebreaker Post"

bool GossipHello_npc_wing_commander_brack(Player* player, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        player->PrepareQuestMenu(pCreature->GetGUID());

    //Mission: The Murketh and Shaadraz Gateways
    if (player->GetQuestStatus(10129) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(2, GOSSIP_ITEM1_BRA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    //Mission: The Abyssal Shelf || Return to the Abyssal Shelf
    if (player->GetQuestStatus(10162) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(10347) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(2, GOSSIP_ITEM2_BRA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

    //Spinebreaker Post
    if (player->GetQuestStatus(10242) == QUEST_STATUS_COMPLETE && !player->GetQuestRewardStatus(10242))
        player->ADD_GOSSIP_ITEM(2, GOSSIP_ITEM3_BRA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_wing_commander_brack(Player* player, Creature* /*pCreature*/, uint32 /*sender*/, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:
        player->CLOSE_GOSSIP_MENU();
        player->CastSpell(player, 33659, true);             //TaxiPath 584 (Gateways Murket and Shaadraz)
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:
        player->CLOSE_GOSSIP_MENU();
        player->CastSpell(player, 33825, true);             //TaxiPath 587 (Aerial Assault Flight (Horde))
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:
        player->CLOSE_GOSSIP_MENU();
        player->CastSpell(player, 34578, true);             //TaxiPath 604 (Taxi - Reaver's Fall to Spinebreaker Ridge)
        break;
    }
    return true;
}

/*######
## npc_wounded_blood_elf
######*/

enum eWoundedBloodElf
{
    SAY_ELF_START               = -1000117,
    SAY_ELF_SUMMON1             = -1000118,
    SAY_ELF_RESTING             = -1000119,
    SAY_ELF_SUMMON2             = -1000120,
    SAY_ELF_COMPLETE            = -1000121,
    SAY_ELF_AGGRO               = -1000122,

    QUEST_ROAD_TO_FALCON_WATCH  = 9375
};

struct npc_wounded_blood_elfAI : public npc_escortAI
{
    npc_wounded_blood_elfAI(Creature* c) : npc_escortAI(c) {}

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (i)
        {
        case 0:
            DoScriptText(SAY_ELF_START, me, pPlayer);
            break;
        case 9:
            DoScriptText(SAY_ELF_SUMMON1, me, pPlayer);
            // Spawn two Haal'eshi Talonguard
            DoSpawnCreature(16967, -15, -15, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
            DoSpawnCreature(16967, -17, -17, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
            break;
        case 13:
            DoScriptText(SAY_ELF_RESTING, me, pPlayer);
            break;
        case 14:
            DoScriptText(SAY_ELF_SUMMON2, me, pPlayer);
            // Spawn two Haal'eshi Windwalker
            DoSpawnCreature(16966, -15, -15, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
            DoSpawnCreature(16966, -17, -17, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
            break;
        case 27:
            DoScriptText(SAY_ELF_COMPLETE, me, pPlayer);
            // Award quest credit
            pPlayer->GroupEventHappens(QUEST_ROAD_TO_FALCON_WATCH, me);
            break;
        }
    }

    void Reset() { }

    void EnterCombat(Unit*)
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            DoScriptText(SAY_ELF_AGGRO, me);
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->AI()->AttackStart(me);
    }
};

CreatureAI* GetAI_npc_wounded_blood_elf(Creature* pCreature)
{
    return new npc_wounded_blood_elfAI(pCreature);
}

bool QuestAccept_npc_wounded_blood_elf(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_ROAD_TO_FALCON_WATCH)
    {
        if (npc_escortAI* pEscortAI = CAST_AI(npc_wounded_blood_elfAI, pCreature->AI()))
            pEscortAI->Start(true, false, pPlayer->GetGUID());

        // Change faction so mobs attack
        pCreature->setFaction(775);
    }

    return true;
}

/*######
## npc_anchorite_relic_bunny
######*/

enum
{
    NPC_HAND_BERSERKER      = 16878,
    NPC_FEL_SPIRIT          = 22454,
    SPELL_CHANNELS          = 39184,
    SPELL_SOUL_BURDEN       = 38879,

    GO_RELIC                = 185298,
    SAY_SP                  = -1900130
};

struct npc_anchorite_relic_bunnyAI : public ScriptedAI
{
    npc_anchorite_relic_bunnyAI(Creature* creature) : ScriptedAI(creature) {}

    uint32 ChTimer;
    uint32 EndTimer;

    void Reset()
    {
        ChTimer = 2000;
        EndTimer = 60000;
    }

    void AttackedBy(Unit* enemy) {}
    void AttackStart(Unit* enemy) {}

    void JustSummoned(Creature* summoned)
    {
        if (summoned->GetEntry() == NPC_FEL_SPIRIT)
        {
            DoScriptText(SAY_SP, summoned);
            summoned->AI()->AttackStart(summoned->getVictim());
        }
    }

    void DoSpellHit(Unit* caster)
    {
        float x,y,z;
        x = caster->GetPositionX();
        y = caster->GetPositionY();
        z = caster->GetPositionZ();

        me->InterruptNonMeleeSpells(false);
        me->SummonCreature(NPC_FEL_SPIRIT, x, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        ChTimer = 2000;
    }

    // @todo Fix SpellHit not being called when caster is dead
    // This may have other implications elsewhere in the core
    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if (spell->Id == SPELL_SOUL_BURDEN)
        {
            sLog.outDebug("SpellHit on dead caster has been resolved. Please remove me from hellfire_peninsula.cpp");
            me->InterruptNonMeleeSpells(false);
            me->SummonCreature(NPC_FEL_SPIRIT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            ChTimer = 2000;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (ChTimer <= diff)
        {
            if (Creature* Ber = GetClosestCreatureWithEntry(me, NPC_HAND_BERSERKER, 17.5f, true))
            {
                {
                    DoCast(Ber, SPELL_CHANNELS, false);
                    ChTimer = 95000;
                }
            }
            else me->InterruptNonMeleeSpells(false);
        }
        else ChTimer -= diff;

        if (EndTimer <= diff)
        {
            if (GameObject* relic = GetClosestGameObjectWithEntry(me, GO_RELIC, 5.0f))
            {
                relic->RemoveFromWorld();
                me->setDeathState(JUST_DIED);
                me->RemoveCorpse();
            }

            EndTimer = 60000;
        }
        else EndTimer -= diff;
    }
};

CreatureAI* GetAI_npc_anchorite_relic_bunny(Creature* creature)
{
    return new npc_anchorite_relic_bunnyAI(creature);
}

/*######
## npc_hand_berserker
######*/

enum
{
    SPELL_ENRAGE            = 8599,
    SPELL_CHARGE            = 35570,

    NPC_BUNNY               = 22444
};

struct npc_hand_berserkerAI : public ScriptedAI
{
    npc_hand_berserkerAI(Creature* creature) : ScriptedAI(creature) {}

    void Reset() {}

    void EnterCombat(Unit* who)
    {
        if (rand() % 60)
            DoCast(who, SPELL_CHARGE);
    }

    void DamageTaken(Unit* doneby, uint32& damage)
    {
        if (me->HasAura(SPELL_ENRAGE))
            return;

        if (doneby->GetTypeId() == TYPEID_PLAYER && (me->GetHealth() * 100 - damage) / me->GetMaxHealth() < 30)
            DoCast(me, SPELL_ENRAGE);
    }

    void JustDied(Unit* who)
    {
        if (Creature* Bunny = GetClosestCreatureWithEntry(me, NPC_BUNNY, 17.5f))
        {
            Bunny->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            DoCast(Bunny, SPELL_SOUL_BURDEN);
            CAST_AI(npc_anchorite_relic_bunnyAI, Bunny->AI())->DoSpellHit(me);
        }
    }
};

CreatureAI* GetAI_npc_hand_berserker(Creature* creature)
{
    return new npc_hand_berserkerAI(creature);
}

/*######
## npc_anchorite_barada
######*/

#define GOSSIP_ITEM_START      "I am ready Amchorite.Let us begin the exorcim."
#define SAY_BARADA1            -1900100
#define SAY_BARADA2            -1900101
#define SAY_BARADA3            -1900104
#define SAY_BARADA4            -1900105
#define SAY_BARADA5            -1900106
#define SAY_BARADA6            -1900107
#define SAY_BARADA7            -1900108
#define SAY_BARADA8            -1900109
#define SAY_COLONEL1           -1900110
#define SAY_COLONEL2           -1900111
#define SAY_COLONEL3           -1900112
#define SAY_COLONEL4           -1900113
#define SAY_COLONEL5           -1900114
#define SAY_COLONEL6           -1900115
#define SAY_COLONEL7           -1900116
#define SAY_COLONEL8           -1900117

enum
{
    QUEST_THE_EXORCIM          = 10935,
    NPC_COLONEL_JULES          = 22432,
    NPC_DARKNESS_RELEASED      = 22507,

    SPELL_EXORCIM              = 39277,
    SPELL_EXORCIM2             = 39278,
    SPELL_COLONEL1             = 39283,
    SPELL_COLONEL2             = 39294,
    SPELL_COLONEL3             = 39284,
    SPELL_COLONEL4             = 39294,
    SPELL_COLONEL5             = 39295,
    SPELL_COLONEL7             = 39381,
    SPELL_COLONEL8             = 39380
};

struct npc_anchorite_baradaAI : public ScriptedAI
{
    npc_anchorite_baradaAI(Creature* pCreature) : ScriptedAI(pCreature) {}

    bool Exorcim;

    uint64 uiPlayerGUID;
    uint32 uiStepsTimer;
    uint32 uiSteps;

    float colonel;

    void Reset()
    {
        Exorcim = false;
        uiStepsTimer = 0;
        uiSteps = 0;
        uiPlayerGUID = 0;
    }

    void AttackedBy(Unit*) {}
    void AttackStart(Unit*) {}

    void DoSpawnDarkness()
    {
        me->SummonCreature(NPC_DARKNESS_RELEASED, -710.924, 2754.683, 105.0, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER)
        {
            if (CAST_PLR(pWho)->GetQuestStatus(QUEST_THE_EXORCIM) == QUEST_STATUS_INCOMPLETE)
            {
                if (me->IsWithinDistInMap(((Player*)pWho), 5))
                    uiPlayerGUID = pWho->GetGUID();
            }
        }
    }

    uint32 NextStep(uint32 uiSteps)
    {
        Creature* pColonel = me->FindNearestCreature(NPC_COLONEL_JULES, 15);
        colonel = me->GetAngle(pColonel->GetPositionX(), pColonel->GetPositionY());

        switch (uiSteps)
        {
        case 1:
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            pColonel->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            me->SetStandState(UNIT_STAND_STATE_STAND);
            return 2000;
        case 2:
            DoScriptText(SAY_BARADA1, me, 0);
            return 5000;
        case 3:
            DoScriptText(SAY_BARADA2, me, 0);
            return 3000;
        case 4:
            DoScriptText(SAY_COLONEL1, pColonel, 0);
            return 3000;
        case 5:
            me->SetWalk(true);
            return 3000;
        case 6:
            me->GetMotionMaster()->MovePoint(0, -707.702f, 2749.038f, 101.590f);
            return 2000;
        case 7:
            me->GetMotionMaster()->MovePoint(0, -710.810f, 2748.376f, 101.590f);
            return 2100;
        case 8:
            me->SetOrientation(colonel);
            return 2000;
        case 9:
            me->CastSpell(me, SPELL_EXORCIM , false);
            return 10000;
        case 10:
            DoScriptText(SAY_BARADA3, me, 0);
            return 10000;
        case 11:
            DoScriptText(SAY_COLONEL2, pColonel, 0);
            return 8000;
        case 12:
            me->RemoveAllAuras();
        case 13:
            me->CastSpell(me, SPELL_EXORCIM2 , false);
        case 14:
            pColonel->CastSpell(pColonel, SPELL_COLONEL1, false);
        case 15:
            pColonel->SetLevitate(true);
            pColonel->SetSpeed(MOVE_RUN, 0.17f);
            pColonel->GetMotionMaster()->MovePoint(0, -710.611f, 2753.435f, 103.774f);
            pColonel->CastSpell(pColonel, SPELL_COLONEL3, false);
            return 14000;
        case 16:
            DoScriptText(SAY_COLONEL3, pColonel, 0);
            DoSpawnDarkness();
            DoSpawnDarkness();
            return 14000;
        case 17:
            DoScriptText(SAY_BARADA4, me, 0);
            DoSpawnDarkness();
            DoSpawnDarkness();
            return 14000;
        case 18:
            DoScriptText(SAY_COLONEL4, pColonel, 0);
            DoSpawnDarkness();
            return 14000;
        case 19:
            DoScriptText(SAY_BARADA5, me, 0);
            return 14000;
        case 20:
            pColonel->CastSpell(pColonel, SPELL_COLONEL4, false);
            pColonel->CastSpell(pColonel, SPELL_COLONEL2, false);
            DoSpawnDarkness();
            return 1500;
        case 21:
            pColonel->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 103.774f);
            return 5000;
        case 22:
            DoScriptText(SAY_COLONEL5, pColonel, 0);
            return 1000;
        case 23:
            pColonel->GetMotionMaster()->MovePoint(0, -707.784f, 2749.562f, 103.774f);
            DoSpawnDarkness();
            return 4000;
        case 24:
            pColonel->GetMotionMaster()->MovePoint(0, -708.558f, 2744.923f, 103.774f);
            pColonel->CastSpell(me, SPELL_COLONEL5, false);
            return 2500;
        case 25:
            DoScriptText(SAY_BARADA6, me, 0);
        case 26:
            pColonel->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 103.774f);
            DoSpawnDarkness();
            return 3500;
        case 27:
            pColonel->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 103.774f);
            return 4000;
        case 28:
            pColonel->GetMotionMaster()->MovePoint(0, -707.784f, 2749.562f, 103.774f);
            DoScriptText(SAY_COLONEL6, pColonel, 0);
            DoSpawnDarkness();
            return 4000;
        case 29:
            pColonel->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 103.774f);
            return 4000;
        case 30:
            pColonel->GetMotionMaster()->MovePoint(0, -707.784f, 2749.562f, 103.774f);
            return 4000;
        case 31:
            DoScriptText(SAY_BARADA7, me, 0);
            return 0;
        case 32:
            pColonel->GetMotionMaster()->MovePoint(0, -708.558f, 2744.923f, 103.774f);
            DoSpawnDarkness();
            return 4000;
        case 33:
            pColonel->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 103.774f);
            return 4000;
        case 34:
            pColonel->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 103.774f);
            DoScriptText(SAY_COLONEL7, pColonel, 0);
            DoSpawnDarkness();
            return 4000;
        case 35:
            pColonel->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 103.774f);
            return 4000;
        case 36:
            pColonel->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 103.774f);
            DoSpawnDarkness();
            return 4000;
        case 37:
            DoScriptText(SAY_BARADA6, me, 0);
        case 38:
            pColonel->GetMotionMaster()->MovePoint(0, -707.784f, 2749.562f, 103.774f);
            return 4000;
        case 39:
            pColonel->GetMotionMaster()->MovePoint(0, -708.558f, 2744.923f, 103.774f);
            return 4000;
        case 40:
            pColonel->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 103.774f);
            DoScriptText(SAY_COLONEL8, pColonel, 0);
            return 4000;
        case 41:
            pColonel->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 103.774f);
            return 4000;
        case 42:
            pColonel->GetMotionMaster()->MovePoint(0, -707.784f, 2749.562f, 103.774f);
            return 4000;
        case 43:
            DoScriptText(SAY_BARADA6, me, 0);
            return 1000;
        case 44:
            pColonel->GetMotionMaster()->MovePoint(0, -708.558f, 2744.923f, 103.774f);
            return 4000;
        case 45:
            pColonel->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 103.774f);
            pColonel->CastSpell(pColonel, SPELL_COLONEL8, false);
            return 4000;
        case 46:
            pColonel->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 103.774f);
            pColonel->CastSpell(pColonel, SPELL_COLONEL7, false);
            return 4000;
        case 47:
            pColonel->GetMotionMaster()->MovePoint(0, -710.792f, 2750.693f, 103.774f);
            return 5000;
        case 48:
            DoScriptText(SAY_BARADA8, me, 0);
            return 1000;
        case 49:
            pColonel->GetMotionMaster()->MovePoint(0, -710.111f, 2754.346f, 102.367f);
            return 3000;
        case 50:
            pColonel->RemoveAllAuras();
        case 51:
            me->RemoveAllAuras();
            return 2000;
        case 52:
            me->SetWalk(true);
            return 2000;
        case 53:
            me->GetMotionMaster()->MovePoint(0, -706.726f, 2751.632f, 101.591f);
            return 2200;
        case 54:
            me->GetMotionMaster()->MovePoint(0, -707.382f, 2753.994f, 101.591f);
            return 7000;
        case 55:
            me->SetStandState(UNIT_STAND_STATE_KNEEL);
            me->CombatStop();
            return 3000;
        case 56:
            pColonel->SetFlag(UNIT_NPC_FLAGS, 1);
            return 6000;
        case 57:
            me->SetFlag(UNIT_NPC_FLAGS, 1);
            if (Player* pPlayer = Unit::GetPlayer(*me, uiPlayerGUID))
                pPlayer->AreaExploredOrEventHappens(QUEST_THE_EXORCIM);
            return 1000;
        case 58:
            Reset();
        default:
            return 0;
        }
    }

    void JustDied(Unit*)
    {
        if (Creature* pColonel = me->FindNearestCreature(NPC_COLONEL_JULES, 15.0f, true))
        {
            pColonel->GetMotionMaster()->MovePoint(0, -710.111f, 2754.346f, 102.367f);
            pColonel->RemoveAllAuras();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (uiStepsTimer <= uiDiff)
        {
            if (Exorcim)
                uiStepsTimer = NextStep(++uiSteps);
        }
        else uiStepsTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_anchorite_barada(Creature* pCreature)
{
    return new npc_anchorite_baradaAI(pCreature);
}

bool GossipHello_npc_anchorite_barada(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_THE_EXORCIM) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_anchorite_barada(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        ((npc_anchorite_baradaAI*)pCreature->AI())->Exorcim = true;
        pPlayer->CLOSE_GOSSIP_MENU();
    }
    return true;
}

/*######
## npc_darkness_released
######*/

enum
{
    SPELL_AURA_ME     = 39303,
    SPELL_DARKNESS    = 39307,
    NPC_BARADA        = 22431
};

struct npc_darkness_releasedAI : public ScriptedAI
{
    npc_darkness_releasedAI(Creature* pCreature) : ScriptedAI(pCreature) {}

    uint32 uiAtTimer;
    uint32 uiChTimer;

    void Reset()
    {
        uiChTimer = 5000;
        uiAtTimer = 10000;
        DoCast(SPELL_AURA_ME);
        me->SetLevitate(true);
        me->SetSpeed(MOVE_RUN, 0.10f);
        switch (urand(0, 3))
        {
        case 0:
            me->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 105.0f);
            break;
        case 1:
            me->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 105.0f);
            break;
        case 2:
            me->GetMotionMaster()->MovePoint(0, -707.784f, 2749.562f, 105.0f);
            break;
        case 3:
            me->GetMotionMaster()->MovePoint(0, -708.558f, 2744.923f, 105.0f);
            break;
        }
    }

    void AttackedBy(Unit* /*pWho*/) {}
    void AttackStart(Unit* /*pWho*/) {}

    void JustDied(Unit* /*pWho*/)
    {
        me->RemoveCorpse();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (uiAtTimer <= uiDiff)
        {
            DoCast(SPELL_DARKNESS);
            switch (urand(0, 3))
            {
            case 0:
                me->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 105.0f);
                break;
            case 1:
                me->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 105.0f);
                break;
            case 2:
                me->GetMotionMaster()->MovePoint(0, -707.784f, 2749.562f, 105.0f);
                break;
            case 3:
                me->GetMotionMaster()->MovePoint(0, -708.558f, 2744.923f, 105.0f);
                break;
            }

            uiAtTimer = 10000;
        }
        else uiAtTimer -= uiDiff;

        if (uiChTimer <= uiDiff)
        {
            if (/*Creature* pBar = */me->FindNearestCreature(NPC_BARADA, 15.0f, false))
                me->setDeathState(CORPSE);

            if (Creature* pBara = me->FindNearestCreature(NPC_BARADA, 15.0f, true))
            {
                if (!pBara->HasAura(SPELL_EXORCIM2, 0))
                    me->setDeathState(CORPSE);
            }

            uiChTimer = 5000;
        }
        else uiChTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_darkness_released(Creature* pCreature)
{
    return new npc_darkness_releasedAI(pCreature);
}

/*######
## npc_foul_purge
######*/

struct npc_foul_purgeAI : public ScriptedAI
{
    npc_foul_purgeAI(Creature* pCreature) : ScriptedAI(pCreature) {}

    uint32 uiChTimer;

    void Reset()
    {
        if (Creature* pBara = me->FindNearestCreature(NPC_BARADA, 15.0f, true))
            AttackStart(pBara);
        uiChTimer = 4000;
    }

    void JustDied(Unit* /*pWho*/)
    {
        me->RemoveCorpse();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (uiChTimer <= uiDiff)
        {
            if (/*Creature* pBar = */me->FindNearestCreature(NPC_BARADA, 15.0f, false))
                me->setDeathState(CORPSE);

            if (Creature* pBara = me->FindNearestCreature(NPC_BARADA, 15.0f, true))
            {
                if (!pBara->HasAura(SPELL_EXORCIM2, 0))
                    me->setDeathState(CORPSE);
            }

            uiChTimer = 4000;
        }
        else uiChTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_foul_purge(Creature* pCreature)
{
    return new npc_foul_purgeAI(pCreature);
}

/*######
## npc_sedai_quest_credit_marker
######*/

enum
{
    NPC_ESCORT1    = 17417,
    NPC_SEDAI      = 17404
};

struct npc_sedai_quest_credit_markerAI : public ScriptedAI
{
    npc_sedai_quest_credit_markerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

    void Reset()
    {
        DoSpawn();
    }

    void DoSpawn()
    {
        me->SummonCreature(NPC_SEDAI, 225.908, 4124.034, 82.505, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 100000);
        me->SummonCreature(NPC_ESCORT1, 229.257, 4125.271, 83.388, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 40000);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_ESCORT1)
        {
            pSummoned->SetWalk(true);
            pSummoned->GetMotionMaster()->MovePoint(0, 208.029f, 4134.618f, 77.763f);
        }
    }
};

CreatureAI* GetAI_npc_sedai_quest_credit_marker(Creature* pCreature)
{
    return new npc_sedai_quest_credit_markerAI(pCreature);
}

/*######
## npc_vindicator_sedai
######*/

#define SAY_MAG_ESSCORT    -1900125
#define SAY_SEDAI1         -1900126
#define SAY_SEDAI2         -1900127
#define SAY_KRUN           -1900128

enum
{
    NPC_ESCORT        = 17417,
    NPC_AMBUSHER      = 17418,
    NPC_KRUN          = 17405,

    SPELL_STUN        = 13005,
    SPELL_HOLYFIRE    = 17141
};

struct npc_vindicator_sedaiAI : public ScriptedAI
{
    npc_vindicator_sedaiAI(Creature* pCreature) : ScriptedAI(pCreature) {}

    bool Vision;

    uint32 uiStepsTimer;
    uint32 uiSteps;

    void Reset()
    {
        Vision = true;
        uiStepsTimer = 0;
        uiSteps = 0;
    }

    void DoSpawnEscort()
    {
        me->SummonCreature(NPC_ESCORT, 227.188f, 4121.116f, 82.745f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 40000);
    }

    void DoSpawnAmbusher()
    {
        me->SummonCreature(NPC_AMBUSHER, 223.408f, 4120.086f, 81.843f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 30000);
    }

    void DoSpawnKrun()
    {
        me->SummonCreature(NPC_KRUN, 192.872f, 4129.417f, 73.655f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 6000);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_ESCORT)
        {
            pSummoned->SetWalk(true);
            pSummoned->GetMotionMaster()->MovePoint(0, 205.660f, 4130.663f, 77.175f);
        }

        if (pSummoned->GetEntry() == NPC_AMBUSHER)
        {
            Creature* pEscort = pSummoned->FindNearestCreature(NPC_ESCORT, 15);
            pSummoned->AI()->AttackStart(pEscort);
        }
        else
        {
            if (pSummoned->GetEntry() == NPC_KRUN)
            {
                pSummoned->SetWalk(true);
                pSummoned->GetMotionMaster()->MovePoint(0, 194.739868f, 4143.145996f, 73.798088f);
                DoScriptText(SAY_KRUN, pSummoned, 0);
                pSummoned->AI()->AttackStart(me);
            }
        }
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (who->GetTypeId() == TYPEID_PLAYER)
        {
            if (CAST_PLR(who)->GetQuestStatus(9545) == QUEST_STATUS_INCOMPLETE)
            {
                if (Creature* pCr = me->FindNearestCreature(17413, 6.0f))
                {
                    float Radius = 10.0;
                    if (me->IsWithinDistInMap(who, Radius))
                        CAST_PLR(who)->KilledMonsterCredit(17413, pCr->GetGUID());
                }
                else return;
            }
        }
    }

    uint32 NextStep(uint32 uiSteps)
    {
        Creature* pEsc = me->FindNearestCreature(NPC_ESCORT, 20);
        Creature* pAmb = me->FindNearestCreature(NPC_AMBUSHER, 35);
        Creature* pKrun = me->FindNearestCreature(NPC_KRUN, 20);

        switch (uiSteps)
        {
        case 1:
            DoSpawnEscort();
        case 2:
            me->SetWalk(true);
        case 3:
            me->GetMotionMaster()->MovePoint(0, 204.877f, 4133.172f, 76.897f);
            return 2900;
        case 4:
            DoScriptText(SAY_MAG_ESSCORT, pEsc, 0);
            return 1000;
        case 5:
            pEsc->GetMotionMaster()->MovePoint(0, 229.257f, 4125.271f, 83.388f);
            return 1500;
        case 6:
            pEsc->GetMotionMaster()->MovePoint(0, 227.188f, 4121.116f, 82.745f);
            return 1000;
        case 7:
            DoScriptText(SAY_SEDAI1, me, 0);
            return 1000;
        case 8:
            DoSpawnAmbusher();
            return 3000;
        case 9:
            DoSpawnAmbusher();
            return 1000;
        case 10:
            me->AI()->AttackStart(pAmb);
            return 2000;
        case 11:
            me->CastSpell(pAmb, SPELL_STUN , false);
            return 2000;
        case 12:
            pAmb->DealDamage(pAmb, pAmb->GetHealth(), 0, DIRECT_DAMAGE);
            return 1500;
        case 13:
            pEsc->DealDamage(pEsc, pEsc->GetHealth(), 0, DIRECT_DAMAGE);
        case 14:
            me->AI()->AttackStart(pAmb);
        case 15:
            pEsc->AI()->AttackStart(pAmb);
            return 1000;
        case 16:
            me->CastSpell(pAmb, SPELL_HOLYFIRE , false);
            return 6000;
        case 17:
            pAmb->DealDamage(pAmb, pAmb->GetHealth(), 0, DIRECT_DAMAGE);
            return 1000;
        case 18:
            pEsc->GetMotionMaster()->MovePoint(0, 235.063f, 4117.826f, 84.471f);
            return 1000;
        case 19:
            me->SetWalk(true);
            me->GetMotionMaster()->MovePoint(0, 199.706f, 4134.302f, 75.404f);
            return 6000;
        case 20:
            me->GetMotionMaster()->MovePoint(0, 193.524f, 4147.451f, 73.605f);
            return 7000;
        case 21:
            me->SetStandState(UNIT_STAND_STATE_KNEEL);
            DoScriptText(SAY_SEDAI2, me, 0);
            return 5000;
        case 22:
            DoSpawnKrun();
            return 1000;
        case 23:
            me->CastSpell(pKrun, SPELL_HOLYFIRE, false);
            return 3000;
        case 24:
            me->DealDamage(me, me->GetHealth(), 0, DIRECT_DAMAGE);
        default:
            return 0;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {

        if (uiStepsTimer <= uiDiff)
        {
            if (Vision)
                uiStepsTimer = NextStep(++uiSteps);
        }
        else uiStepsTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_vindicator_sedai(Creature* pCreature)
{
    return new npc_vindicator_sedaiAI(pCreature);
}

/*######
## npc_demoniac_scryer
######*/

#define GOSSIP_ITEM_ATTUNE          "Yes, Scryer. You may possess me."

enum
{
    GOSSIP_TEXTID_PROTECT           = 10659,
    GOSSIP_TEXTID_ATTUNED           = 10643,

    QUEST_DEMONIAC                  = 10838,
    NPC_HELLFIRE_WARDLING           = 22259,
    NPC_ORC_HA                      = 22273,
    NPC_BUTTRESS                    = 22267,
    NPC_BUTTRESS_SPAWNER            = 22260,

    MAX_BUTTRESS                    = 4,
    TIME_TOTAL                      = MINUTE * 10 * IN_MILLISECONDS,

    SPELL_SUMMONED                  = 7741,
    SPELL_DEMONIAC_VISITATION       = 38708,
    SPELL_BUTTRESS_APPERANCE        = 38719,
    SPELL_SUCKER_CHANNEL            = 38721,
    SPELL_SUCKER_DESPAWN_MOB        = 38691,
};

struct npc_demoniac_scryerAI : public ScriptedAI
{
    npc_demoniac_scryerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        IfIsComplete = false;
        uiSpawnDemonTimer = 15000;
        uiSpawnOrcTimer = 30000;
        uiSpawnButtressTimer = 45000;
        uiEndTimer = 262000;
        uiButtressCount = 0;
        Reset();
    }

    bool IfIsComplete;

    uint32 uiSpawnDemonTimer;
    uint32 uiSpawnOrcTimer;
    uint32 uiSpawnButtressTimer;
    uint32 uiEndTimer;
    uint32 uiButtressCount;

    void Reset() {}

    void AttackedBy(Unit* /*pEnemy*/) {}
    void AttackStart(Unit* /*pEnemy*/) {}

    void DoSpawnButtress()
    {
        ++uiButtressCount;

        float fAngle;
        switch (uiButtressCount)
        {
        case 1:
            fAngle = 0.0f;
            break;
        case 2:
            fAngle = 4.6f;
            break;
        case 3:
            fAngle = 1.5f;
            break;
        case 4:
            fAngle = 3.1f;
            break;
        }

        float fX, fY, fZ;
        me->GetNearPoint(me, fX, fY, fZ, 0.0f, 7.0f, fAngle);
        uint32 uiTime = TIME_TOTAL - (uiSpawnButtressTimer * uiButtressCount);
        me->SummonCreature(NPC_BUTTRESS, fX, fY, fZ, me->GetAngle(fX, fY), TEMPSUMMON_TIMED_DESPAWN, uiTime);
        me->SummonCreature(NPC_BUTTRESS_SPAWNER, fX, fY, fZ, me->GetAngle(fX, fY), TEMPSUMMON_TIMED_DESPAWN, uiTime);
    }

    void DoSpawnDemon()
    {
        float fX, fY, fZ;
        me->GetNearPoint(me, fX, fY, fZ, 5.0f, 5.0f, -13.0f);
        me->SummonCreature(NPC_HELLFIRE_WARDLING, fX, fY, fZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
    }

    void DospawnOrc()
    {
        float fX, fY, fZ;
        me->GetNearPoint(me, fX, fY, fZ, 5.0f, 5.0f, -13.0f);
        me->SummonCreature(NPC_ORC_HA, fX, fY, fZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_HELLFIRE_WARDLING)
        {
            pSummoned->CastSpell(pSummoned, SPELL_SUMMONED, false);
            pSummoned->AI()->AttackStart(me);
        }
        if (pSummoned->GetEntry() == NPC_ORC_HA)
        {
            pSummoned->CastSpell(pSummoned, SPELL_SUMMONED, false);
            pSummoned->AI()->AttackStart(me);
        }
        if (pSummoned->GetEntry() == NPC_BUTTRESS)
            pSummoned->CastSpell(pSummoned, SPELL_BUTTRESS_APPERANCE, false);
        else
        {
            if (pSummoned->GetEntry() == NPC_BUTTRESS_SPAWNER)
                pSummoned->CastSpell(me, SPELL_SUCKER_CHANNEL, true);
        }
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pTarget->GetEntry() == NPC_BUTTRESS && pSpell->Id == SPELL_SUCKER_DESPAWN_MOB)
            ((Creature*)pTarget)->setDeathState(CORPSE);
        if (pTarget->GetEntry() == NPC_BUTTRESS_SPAWNER && pSpell->Id == SPELL_SUCKER_DESPAWN_MOB)
            ((Creature*)pTarget)->setDeathState(CORPSE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (uiEndTimer <= uiDiff)
        {
            me->ForcedDespawn();
            uiEndTimer = 262000;
        }
        else uiEndTimer -= uiDiff;

        if (IfIsComplete || !me->IsAlive())
            return;

        if (uiSpawnButtressTimer <= uiDiff)
        {
            if (uiButtressCount >= MAX_BUTTRESS)
            {
                DoCast(SPELL_SUCKER_DESPAWN_MOB);
                IfIsComplete = true;
                return;
            }
            uiSpawnButtressTimer = 45000;
            DoSpawnButtress();
        }
        else uiSpawnButtressTimer -= uiDiff;

        if (uiSpawnDemonTimer <= uiDiff)
        {
            DoSpawnDemon();
            uiSpawnDemonTimer = 15000;
        }
        else uiSpawnDemonTimer -= uiDiff;

        if (uiSpawnOrcTimer <= uiDiff)
        {
            DospawnOrc();
            uiSpawnOrcTimer = 30000;
        }
        else uiSpawnOrcTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_demoniac_scryer(Creature* pCreature)
{
    return new npc_demoniac_scryerAI(pCreature);
}

bool GossipHello_npc_demoniac_scryer(Player* pPlayer, Creature* pCreature)
{
    if (npc_demoniac_scryerAI* pScryerAI = dynamic_cast<npc_demoniac_scryerAI*>(pCreature->AI()))
    {
        if (pScryerAI->IfIsComplete)
        {
            if (pPlayer->GetQuestStatus(QUEST_DEMONIAC) == QUEST_STATUS_INCOMPLETE)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ATTUNE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ATTUNED, pCreature->GetGUID());
            return true;
        }
    }
    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_PROTECT, pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_demoniac_scryer(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer, SPELL_DEMONIAC_VISITATION, false);
    }
    return true;
}

/*######
## npc_magic_sucker_device_spawner
######*/

enum
{
    SPELL_EFFECT    = 38724,
    NPC_SCRYER      = 22258,
    NPC_BUTTRES     = 22267
};

struct npc_magic_sucker_device_spawnerAI : public ScriptedAI
{
    npc_magic_sucker_device_spawnerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

    uint32 uiCastTimer;
    uint32 uiCheckTimer;

    void Reset()
    {
        uiCastTimer = 1800;
        uiCheckTimer = 5000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (uiCastTimer <= uiDiff)
        {
            DoCast(SPELL_EFFECT);
            uiCastTimer = 1800;
        }
        else uiCastTimer -= uiDiff;

        if (uiCheckTimer <= uiDiff)
        {
            if (/*Creature* pScr = */me->FindNearestCreature(NPC_SCRYER, 15.0f, false))
            {
                if (Creature* pBut = me->FindNearestCreature(NPC_BUTTRES, 5.0f))
                {
                    pBut->setDeathState(CORPSE);
                    me->setDeathState(CORPSE);
                }
            }
            else return;

            uiCheckTimer = 5000;
        }
        else uiCheckTimer -= uiDiff;
    }
};
CreatureAI* GetAI_npc_magic_sucker_device_spawner(Creature* pCreature)
{
    return new npc_magic_sucker_device_spawnerAI(pCreature);
}

/*######
## npc_living_flare
######*/

enum
{
    SPELL_COSMETIC             = 44880,
    SPELL_UNSTABLE_COSMETIC    = 46196,
    SPELL_ABSORBED             = 44944,

    NPC_FEL_SPARK              = 22323,
    NPC_TRIGGER                = 24959,

    GO_FIRE                    = 185319
};

struct npc_living_flareAI : public ScriptedAI
{
    npc_living_flareAI(Creature* pCreature) : ScriptedAI(pCreature) {}

    uint32 uiCheckTimer;
    uint64 uiSparkGUID;
    uint32 Count;

    void Reset()
    {
        DoCast(SPELL_COSMETIC);
        uiCheckTimer = 8000;
        uiSparkGUID = 0;
        Count = 0;
    }

    void AttackedBy(Unit* /*pWho*/) {}
    void AttackStart(Unit* /*pWho*/) {}

    void UpdateAI(const uint32 uiDiff)
    {
        if (uiCheckTimer <= uiDiff)
        {
            if (Creature* pSpark = me->FindNearestCreature(NPC_FEL_SPARK, 10.0f, false))
            {
                if (pSpark->GetGUID() != uiSparkGUID && CAST_PLR(me->GetOwner())->GetQuestStatus(11516) == QUEST_STATUS_INCOMPLETE)
                {
                    if (Count <= 7)
                    {
                        ++Count;
                        DoCast(SPELL_ABSORBED);
                        uiSparkGUID = pSpark->GetGUID();
                    }
                    else DoCast(SPELL_UNSTABLE_COSMETIC);
                }
            }

            if (Creature* pTrigger = me->FindNearestCreature(NPC_TRIGGER, 8.0f, true))
            {
                pTrigger->SummonGameObject(GO_FIRE, pTrigger->GetPositionX(), pTrigger->GetPositionY(), pTrigger->GetPositionZ(), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 60);
                CAST_PLR(me->GetOwner())->AreaExploredOrEventHappens(11516);
                me->setDeathState(CORPSE);
            }
        }
        else uiCheckTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_living_flare(Creature* pCreature)
{
    return new npc_living_flareAI(pCreature);
}

/*######
## npc_pathaleon_image
######*/

enum
{
    SAY_PATHALEON1         = -1900165,
    SAY_PATHALEON2         = -1900166,
    SAY_PATHALEON3         = -1900167,
    SAY_PATHALEON4         = -1900168,

    SPELL_ROOTS            = 35468,
    SPELL_INSECT           = 35471,
    SPELL_LIGHTING         = 35487,
    SPELL_TELE             = 7741,

    NPC_TARGET_TRIGGER     = 20781,
    NPC_CRYSTAL_TRIGGER    = 20617,
    NPC_GOLIATHON          = 19305,
};

struct npc_pathaleon_imageAI : public ScriptedAI
{
    npc_pathaleon_imageAI(Creature* pCreature) : ScriptedAI(pCreature) {}

    bool Event;
    bool SummonTrigger;

    uint32 uiSumTimer;
    uint32 uiStepsTimer;
    uint32 uiSteps;

    void Reset()
    {
        uiSumTimer = 5000;
        uiStepsTimer = 0;
        uiSteps = 0;
        Event = true;
        SummonTrigger = false;
    }

    void DoSpawnGoliathon()
    {
        me->SummonCreature(NPC_GOLIATHON, 113.29f, 4858.19f, 74.37f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
    }

    void DoSpawnTrigger()
    {
        me->SummonCreature(NPC_TARGET_TRIGGER, 81.20f, 4806.26f, 51.75f, 2.0f, TEMPSUMMON_TIMED_DESPAWN, 120000);
    }

    void DoSpawnCtrigger()
    {
        me->SummonCreature(NPC_CRYSTAL_TRIGGER, 106.21f, 4834.39f, 79.56f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 7000);
        me->SummonCreature(NPC_CRYSTAL_TRIGGER, 124.98f, 4813.17f, 79.66f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 7000);
        me->SummonCreature(NPC_CRYSTAL_TRIGGER, 124.01f, 4778.61f, 77.86f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 7000);
        me->SummonCreature(NPC_CRYSTAL_TRIGGER, 46.37f, 4795.72f, 66.73f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 7000);
        me->SummonCreature(NPC_CRYSTAL_TRIGGER, 60.14f, 4830.46f, 77.83f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 7000);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_GOLIATHON)
        {
            pSummoned->CastSpell(pSummoned, SPELL_TELE, false);
            pSummoned->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
        }
        if (pSummoned->GetEntry() == NPC_CRYSTAL_TRIGGER)
        {
            pSummoned->CastSpell(pSummoned, SPELL_INSECT, false);
            pSummoned->CastSpell(pSummoned, SPELL_LIGHTING, false);
        }
        else
        {
            if (pSummoned->GetEntry() == NPC_TARGET_TRIGGER)
                pSummoned->CastSpell(pSummoned, SPELL_ROOTS, false);
        }
    }

    int32 NextStep(uint32 uiSteps)
    {
        switch (uiSteps)
        {
        case 1:
            return 10000;
        case 2:
            DoSpawnTrigger();
            SummonTrigger = true;
            return 2000;
        case 3:
            DoScriptText(SAY_PATHALEON1, me, 0);
            return 15000;
        case 4:
            DoScriptText(SAY_PATHALEON2, me, 0);
            return 15000;
        case 5:
            DoScriptText(SAY_PATHALEON3, me, 0);
            return 15000;
        case 6:
            DoScriptText(SAY_PATHALEON4, me, 0);
            return 5000;
        case 7:
            DoSpawnGoliathon();
            return 1000;
        case 8:
            DoCast(SPELL_TELE);
            return 500;
        case 9:
            me->SetVisibility(VISIBILITY_OFF);
            return 60000;
        case 10:
            me->setDeathState(CORPSE);
        default:
            return 0;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (uiStepsTimer <= uiDiff)
        {
            if (Event)
                uiStepsTimer = NextStep(++uiSteps);
        }
        else uiStepsTimer -= uiDiff;

        if (SummonTrigger)
        {
            if (uiSumTimer <= uiDiff)
            {
                DoSpawnCtrigger();
                uiSumTimer = 5000;
            }
            else uiSumTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_pathaleon_image(Creature* pCreature)
{
    return new npc_pathaleon_imageAI(pCreature);
}

void AddSC_hellfire_peninsula()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_aeranas";
    newscript->GetAI = &GetAI_npc_aeranas;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_ancestral_wolf";
    newscript->GetAI = &GetAI_npc_ancestral_wolf;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_haaleshi_altar";
    newscript->pGOHello = &GOHello_go_haaleshi_altar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_naladu";
    newscript->pGossipHello = &GossipHello_npc_naladu;
    newscript->pGossipSelect = &GossipSelect_npc_naladu;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tracy_proudwell";
    newscript->pGossipHello = &GossipHello_npc_tracy_proudwell;
    newscript->pGossipSelect = &GossipSelect_npc_tracy_proudwell;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_trollbane";
    newscript->pGossipHello = &GossipHello_npc_trollbane;
    newscript->pGossipSelect = &GossipSelect_npc_trollbane;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_wing_commander_dabiree";
    newscript->pGossipHello =   &GossipHello_npc_wing_commander_dabiree;
    newscript->pGossipSelect =  &GossipSelect_npc_wing_commander_dabiree;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_gryphoneer_leafbeard";
    newscript->pGossipHello = &GossipHello_npc_gryphoneer_leafbeard;
    newscript->pGossipSelect = &GossipSelect_npc_gryphoneer_leafbeard;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_wing_commander_brack";
    newscript->pGossipHello =   &GossipHello_npc_wing_commander_brack;
    newscript->pGossipSelect =  &GossipSelect_npc_wing_commander_brack;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_wounded_blood_elf";
    newscript->GetAI = &GetAI_npc_wounded_blood_elf;
    newscript->pQuestAccept = &QuestAccept_npc_wounded_blood_elf;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_hand_berserker";
    newscript->GetAI = &GetAI_npc_hand_berserker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_anchorite_relic_bunny";
    newscript->GetAI = &GetAI_npc_anchorite_relic_bunny;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_anchorite_barada";
    newscript->pGossipHello =  &GossipHello_npc_anchorite_barada;
    newscript->pGossipSelect = &GossipSelect_npc_anchorite_barada;
    newscript->GetAI = &GetAI_npc_anchorite_barada;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_darkness_released";
    newscript->GetAI = &GetAI_npc_darkness_released;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_foul_purge";
    newscript->GetAI = &GetAI_npc_foul_purge;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_sedai_quest_credit_marker";
    newscript->GetAI = &GetAI_npc_sedai_quest_credit_marker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_vindicator_sedai";
    newscript->GetAI = &GetAI_npc_vindicator_sedai;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_demoniac_scryer";
    newscript->pGossipHello =  &GossipHello_npc_demoniac_scryer;
    newscript->pGossipSelect = &GossipSelect_npc_demoniac_scryer;
    newscript->GetAI = &GetAI_npc_demoniac_scryer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_magic_sucker_device_spawner";
    newscript->GetAI = &GetAI_npc_magic_sucker_device_spawner;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_living_flare";
    newscript->GetAI = &GetAI_npc_living_flare;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_pathaleon_image";
    newscript->GetAI = &GetAI_npc_pathaleon_image;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_fel_guard_hound";
    newscript->GetAI = &GetAI_npc_fel_guard_hound;
    newscript->RegisterSelf();
}
