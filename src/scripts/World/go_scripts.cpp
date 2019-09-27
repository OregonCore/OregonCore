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
SDName: GO_Scripts
SD%Complete: 100
SDComment: Quest support: 4285,4287,4288(crystal pylons), 4296, 6481, 10990, 10991, 10992. Field_Repair_Bot->Teaches spell 22704. Barov_journal->Teaches spell 26089, 2936. Soulwell
SDCategory: Game Objects
EndScriptData */

/* ContentData
go_cat_figurine (the "trap" version of GO, two different exist)
go_northern_crystal_pylon
go_eastern_crystal_pylon
go_western_crystal_pylon
go_barov_journal
go_ethereum_prison
go_ethereum_stasis
go_sacred_fire_of_life
go_iruxos
go_shrine_of_the_birds
go_southfury_moonstone
go_field_repair_bot_74A
go_orb_of_command
go_resonite_cask
go_tablet_of_madness
go_tablet_of_the_seven
go_soulwell
go_bashir_crystalforge
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"

/*######
## go_cat_figurine
######*/

enum eCatFigurine
{
    SPELL_SUMMON_GHOST_SABER    = 5968,
};

bool GOHello_go_cat_figurine(Player* pPlayer, GameObject* /*pGO*/)
{
    if (rand() % 100 < 25) // give 25% chance of summoning tiger
        pPlayer->CastSpell(pPlayer, SPELL_SUMMON_GHOST_SABER, true);
    return false;
}

/*######
## go_crystal_pylons (3x)
######*/

bool GOHello_go_northern_crystal_pylon(Player* pPlayer, GameObject* pGO)
{
    if (pGO->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER)
    {
        pPlayer->PrepareQuestMenu(pGO->GetGUID());
        pPlayer->SendPreparedQuest(pGO->GetGUID());
    }

    if (pPlayer->GetQuestStatus(4285) == QUEST_STATUS_INCOMPLETE)
        pPlayer->AreaExploredOrEventHappens(4285);

    return true;
}

bool GOHello_go_eastern_crystal_pylon(Player* pPlayer, GameObject* pGO)
{
    if (pGO->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER)
    {
        pPlayer->PrepareQuestMenu(pGO->GetGUID());
        pPlayer->SendPreparedQuest(pGO->GetGUID());
    }

    if (pPlayer->GetQuestStatus(4287) == QUEST_STATUS_INCOMPLETE)
        pPlayer->AreaExploredOrEventHappens(4287);

    return true;
}

bool GOHello_go_western_crystal_pylon(Player* pPlayer, GameObject* pGO)
{
    if (pGO->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER)
    {
        pPlayer->PrepareQuestMenu(pGO->GetGUID());
        pPlayer->SendPreparedQuest(pGO->GetGUID());
    }

    if (pPlayer->GetQuestStatus(4288) == QUEST_STATUS_INCOMPLETE)
        pPlayer->AreaExploredOrEventHappens(4288);

    return true;
}

/*######
## go_barov_journal
######*/

bool GOHello_go_barov_journal(Player* pPlayer, GameObject* /*pGO*/)
{
    if (pPlayer->HasSkill(SKILL_TAILORING) && pPlayer->GetBaseSkillValue(SKILL_TAILORING) >= 280 && !pPlayer->HasSpell(26086))
        pPlayer->CastSpell(pPlayer, 26095, false);
    return true;
}

/*######
## go_field_repair_bot_74A
######*/

bool GOHello_go_field_repair_bot_74A(Player* pPlayer, GameObject* /*pGO*/)
{
	if (pPlayer->HasSkill(SKILL_ENGINEERING) && pPlayer->GetBaseSkillValue(SKILL_ENGINEERING) >= 300 && !pPlayer->HasSpell(22704))
        pPlayer->CastSpell(pPlayer, 22864, false);
    return true;
}

/*######
## go_orb_of_command
######*/

#define GOSSIP_ORB_OF_COMMAND       "<Place my hand on the orb.>"
#define QUEST_BLACKHANDS_COMMAND    7761

bool GOHello_go_orb_of_command(Player* pPlayer, GameObject* pGO)
{
    if (pPlayer->GetQuestRewardStatus(QUEST_BLACKHANDS_COMMAND))
    {
        pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_ORB_OF_COMMAND, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(7155, pGO->GetGUID());
    }
  
    return true;
}

bool GOSelect_go_orb_of_command(Player* pPlayer, GameObject* pGO, uint32 Sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            pPlayer->TeleportTo(469, -7672.46f, -1107.19f, 396.65f, 0.59f);
            break;
    }

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

/*######
## go_tablet_of_madness
######*/

bool GOHello_go_tablet_of_madness(Player* pPlayer, GameObject* /*pGO*/)
{
    if (pPlayer->HasSkill(SKILL_ALCHEMY) && pPlayer->GetSkillValue(SKILL_ALCHEMY) >= 300 && !pPlayer->HasSpell(24266))
        pPlayer->CastSpell(pPlayer, 24267, false);
    return true;
}

/*######
## go_tablet_of_the_seven
######*/

//@todo use gossip option ("Transcript the Tablet") instead, if Oregon adds support.
bool GOHello_go_tablet_of_the_seven(Player* pPlayer, GameObject* pGO)
{
    if (pGO->GetGoType() != GAMEOBJECT_TYPE_QUESTGIVER)
        return true;

    if (pPlayer->GetQuestStatus(4296) == QUEST_STATUS_INCOMPLETE)
        pPlayer->CastSpell(pPlayer, 15065, false);

    return true;
}

/*#####
## go_jump_a_tron
######*/

bool GOHello_go_jump_a_tron(Player* pPlayer, GameObject* /*pGO*/)
{
    if (pPlayer->GetQuestStatus(10111) == QUEST_STATUS_INCOMPLETE)
        pPlayer->CastSpell(pPlayer, 33382, true);

    return true;
}

/*######
## go_ethereum_prison
######*/

enum EthereumPrison
{
    SPELL_REP_LC        = 39456,
    SPELL_REP_SHAT      = 39457,
    SPELL_REP_CE        = 39460,
    SPELL_REP_CON       = 39474,
    SPELL_REP_KT        = 39475,
    SPELL_REP_SPOR      = 39476
};

const uint32 NpcPrisonEntry[] =
{
    22810, 22811, 22812, 22813, 22814, 22815,               //good guys
    20783, 20784, 20785, 20786, 20788, 20789, 20790         //bad guys
};

bool GOHello_go_ethereum_prison(Player* player, GameObject* go)
{
    go->UseDoorOrButton();
    int Random = rand() % (sizeof(NpcPrisonEntry) / sizeof(uint32));

    if (Creature* creature = player->SummonCreature(NpcPrisonEntry[Random], go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), go->GetAngle(player),
                             TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
    {
        if (!creature->IsHostileTo(player))
        {
            if (FactionTemplateEntry const* pFaction = creature->GetFactionTemplateEntry())
            {
                uint32 Spell = 0;

                switch (pFaction->faction)
                {
                case 1011:
                    Spell = SPELL_REP_LC;
                    break;
                case 935:
                    Spell = SPELL_REP_SHAT;
                    break;
                case 942:
                    Spell = SPELL_REP_CE;
                    break;
                case 933:
                    Spell = SPELL_REP_CON;
                    break;
                case 989:
                    Spell = SPELL_REP_KT;
                    break;
                case 970:
                    Spell = SPELL_REP_SPOR;
                    break;
                }

                if (Spell)
                    creature->CastSpell(player, Spell, false);
                else
                    sLog.outError("go_ethereum_prison summoned Creature (entry %u) but faction (%u) are not expected by script.", creature->GetEntry(), creature->GetFaction());
            }
        }
    }

    return false;
}

/*######
## go_ethereum_stasis
######*/

const uint32 NpcStasisEntry[] =
{
    22825, 20888, 22827, 22826, 22828
};

bool GOHello_go_ethereum_stasis(Player* player, GameObject* go)
{
    go->UseDoorOrButton();
    int Random = rand() % (sizeof(NpcStasisEntry) / sizeof(uint32));

    player->SummonCreature(NpcStasisEntry[Random],
                            go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), go->GetAngle(player),
                            TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);

    return false;
}

/*######
## go_resonite_cask
######*/

enum eResoniteCask
{
    NPC_GOGGEROC    = 11920
};

bool GOHello_go_resonite_cask(Player* /*pPlayer*/, GameObject* pGO)
{
    if (pGO->GetGoType() == GAMEOBJECT_TYPE_GOOBER)
        pGO->SummonCreature(NPC_GOGGEROC, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 300000);

    return false;
}

/*######
## go_sacred_fire_of_life
######*/

#define NPC_ARIKARA  10882

bool GOHello_go_sacred_fire_of_life(Player* pPlayer, GameObject* pGO)
{
    if (pGO->GetGoType() == GAMEOBJECT_TYPE_GOOBER)
        pPlayer->SummonCreature(NPC_ARIKARA, -5008.338f, -2118.894f, 83.657f, 0.874f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);

    return true;
}

/*######
## go_iruxos. Quest 5381
######*/

bool GOHello_go_iruxos(Player* pPlayer, GameObject* /*pGO*/)
{
    if (pPlayer->GetQuestStatus(5381) == QUEST_STATUS_INCOMPLETE)
        pPlayer->SummonCreature(11876, pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);

    return true;
}

/*######
## go_shrine_of_the_birds
######*/

enum eShrineOfTheBirds
{
    NPC_HAWK_GUARD      = 22992,
    NPC_EAGLE_GUARD     = 22993,
    NPC_FALCON_GUARD    = 22994,
    GO_SHRINE_HAWK      = 185551,
    GO_SHRINE_EAGLE     = 185547,
    GO_SHRINE_FALCON    = 185553
};

bool GOHello_go_shrine_of_the_birds(Player* pPlayer, GameObject* pGO)
{
    uint32 BirdEntry = 0;

    float fX, fY, fZ;
    pGO->GetClosePoint(fX, fY, fZ, pGO->GetObjectSize(), INTERACTION_DISTANCE);

    switch (pGO->GetEntry())
    {
    case GO_SHRINE_HAWK:
        BirdEntry = NPC_HAWK_GUARD;
        break;
    case GO_SHRINE_EAGLE:
        BirdEntry = NPC_EAGLE_GUARD;
        break;
    case GO_SHRINE_FALCON:
        BirdEntry = NPC_FALCON_GUARD;
        break;
    }

    if (BirdEntry)
        pPlayer->SummonCreature(BirdEntry, fX, fY, fZ, pGO->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);

    return false;
}

/*######
## go_southfury_moonstone
######*/

enum eSouthfury
{
    NPC_RIZZLE                  = 23002,
    SPELL_BLACKJACK             = 39865, //stuns player
    SPELL_SUMMON_RIZZLE         = 39866
};

bool GOHello_go_southfury_moonstone(Player* pPlayer, GameObject* /*pGO*/)
{
    //implicitTarget=48 not implemented as of writing this code, and manual summon may be just ok for our purpose
    //pPlayer->CastSpell(pPlayer,SPELL_SUMMON_RIZZLE,false);

    if (Creature* pCreature = pPlayer->SummonCreature(NPC_RIZZLE, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0))
        pCreature->CastSpell(pPlayer, SPELL_BLACKJACK, false);

    return false;
}

/*######
## go_fel_crystalforge
######*/

#define GOSSIP_FEL_CRYSTALFORGE_TEXT 31000
#define GOSSIP_FEL_CRYSTALFORGE_ITEM_TEXT_RETURN 31001
#define GOSSIP_FEL_CRYSTALFORGE_ITEM_1 "Purchase 1 Unstable Flask of the Beast for the cost of 10 Apexis Shards"
#define GOSSIP_FEL_CRYSTALFORGE_ITEM_5 "Purchase 5 Unstable Flask of the Beast for the cost of 50 Apexis Shards"
#define GOSSIP_FEL_CRYSTALFORGE_ITEM_RETURN "Use the fel crystalforge to make another purchase."

enum eFelCrystalforge
{
    SPELL_CREATE_1_FLASK_OF_BEAST   = 40964,
    SPELL_CREATE_5_FLASK_OF_BEAST   = 40965,
};

bool GOHello_go_fel_crystalforge(Player* pPlayer, GameObject* pGO)
{
    if (pGO->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER) /* != GAMEOBJECT_TYPE_QUESTGIVER) */
        pPlayer->PrepareQuestMenu(pGO->GetGUID()); /* return true*/

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_FEL_CRYSTALFORGE_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_FEL_CRYSTALFORGE_ITEM_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_FEL_CRYSTALFORGE_TEXT, pGO->GetGUID());

    return true;
}

bool GOSelect_go_fel_crystalforge(Player* pPlayer, GameObject* pGO, uint32 /*uiSender*/, uint32 uiAction)
{
    pPlayer->PlayerTalkClass->ClearMenus();
    switch (uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF:
        pPlayer->CastSpell(pPlayer, SPELL_CREATE_1_FLASK_OF_BEAST, false);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_FEL_CRYSTALFORGE_ITEM_RETURN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_FEL_CRYSTALFORGE_ITEM_TEXT_RETURN, pGO->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 1:
        pPlayer->CastSpell(pPlayer, SPELL_CREATE_5_FLASK_OF_BEAST, false);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_FEL_CRYSTALFORGE_ITEM_RETURN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_FEL_CRYSTALFORGE_ITEM_TEXT_RETURN, pGO->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_FEL_CRYSTALFORGE_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_FEL_CRYSTALFORGE_ITEM_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_FEL_CRYSTALFORGE_TEXT, pGO->GetGUID());
        break;
    }
    return true;
}

/*######
## go_bashir_crystalforge
######*/

#define GOSSIP_BASHIR_CRYSTALFORGE_TEXT 31100
#define GOSSIP_BASHIR_CRYSTALFORGE_ITEM_TEXT_RETURN 31101
#define GOSSIP_BASHIR_CRYSTALFORGE_ITEM_1 "Purchase 1 Unstable Flask of the Sorcerer for the cost of 10 Apexis Shards"
#define GOSSIP_BASHIR_CRYSTALFORGE_ITEM_5 "Purchase 5 Unstable Flask of the Sorcerer for the cost of 50 Apexis Shards"
#define GOSSIP_BASHIR_CRYSTALFORGE_ITEM_RETURN "Use the bashir crystalforge to make another purchase."

enum eBashirCrystalforge
{
    SPELL_CREATE_1_FLASK_OF_SORCERER   = 40968,
    SPELL_CREATE_5_FLASK_OF_SORCERER   = 40970,
};

bool GOHello_go_bashir_crystalforge(Player* pPlayer, GameObject* pGO)
{
    if (pGO->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER) /* != GAMEOBJECT_TYPE_QUESTGIVER) */
        pPlayer->PrepareQuestMenu(pGO->GetGUID()); /* return true*/

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BASHIR_CRYSTALFORGE_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BASHIR_CRYSTALFORGE_ITEM_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_BASHIR_CRYSTALFORGE_TEXT, pGO->GetGUID());

    return true;
}

bool GOSelect_go_bashir_crystalforge(Player* pPlayer, GameObject* pGO, uint32 /*uiSender*/, uint32 uiAction)
{
    pPlayer->PlayerTalkClass->ClearMenus();
    switch (uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF:
        pPlayer->CastSpell(pPlayer, SPELL_CREATE_1_FLASK_OF_SORCERER, false);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BASHIR_CRYSTALFORGE_ITEM_RETURN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_BASHIR_CRYSTALFORGE_ITEM_TEXT_RETURN, pGO->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 1:
        pPlayer->CastSpell(pPlayer, SPELL_CREATE_5_FLASK_OF_SORCERER, false);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BASHIR_CRYSTALFORGE_ITEM_RETURN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_BASHIR_CRYSTALFORGE_ITEM_TEXT_RETURN, pGO->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BASHIR_CRYSTALFORGE_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BASHIR_CRYSTALFORGE_ITEM_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_BASHIR_CRYSTALFORGE_TEXT, pGO->GetGUID());
        break;
    }
    return true;
}

/*######
## matrix_punchograph
######*/

enum eMatrixPunchograph
{
    ITEM_WHITE_PUNCH_CARD = 9279,
    ITEM_YELLOW_PUNCH_CARD = 9280,
    ITEM_BLUE_PUNCH_CARD = 9282,
    ITEM_RED_PUNCH_CARD = 9281,
    ITEM_PRISMATIC_PUNCH_CARD = 9316,
    SPELL_YELLOW_PUNCH_CARD = 11512,
    SPELL_BLUE_PUNCH_CARD = 11525,
    SPELL_RED_PUNCH_CARD = 11528,
    SPELL_PRISMATIC_PUNCH_CARD = 11545,
    MATRIX_PUNCHOGRAPH_3005_A = 142345,
    MATRIX_PUNCHOGRAPH_3005_B = 142475,
    MATRIX_PUNCHOGRAPH_3005_C = 142476,
    MATRIX_PUNCHOGRAPH_3005_D = 142696,
};

bool GOHello_go_matrix_punchograph(Player* pPlayer, GameObject* pGO)
{
    switch (pGO->GetEntry())
    {
    case MATRIX_PUNCHOGRAPH_3005_A:
        if (pPlayer->HasItemCount(ITEM_WHITE_PUNCH_CARD, 1))
        {
            pPlayer->DestroyItemCount(ITEM_WHITE_PUNCH_CARD, 1, true);
            pPlayer->CastSpell(pPlayer, SPELL_YELLOW_PUNCH_CARD, true);
        }
        break;
    case MATRIX_PUNCHOGRAPH_3005_B:
        if (pPlayer->HasItemCount(ITEM_YELLOW_PUNCH_CARD, 1))
        {
            pPlayer->DestroyItemCount(ITEM_YELLOW_PUNCH_CARD, 1, true);
            pPlayer->CastSpell(pPlayer, SPELL_BLUE_PUNCH_CARD, true);
        }
        break;
    case MATRIX_PUNCHOGRAPH_3005_C:
        if (pPlayer->HasItemCount(ITEM_BLUE_PUNCH_CARD, 1))
        {
            pPlayer->DestroyItemCount(ITEM_BLUE_PUNCH_CARD, 1, true);
            pPlayer->CastSpell(pPlayer, SPELL_RED_PUNCH_CARD, true);
        }
        break;
    case MATRIX_PUNCHOGRAPH_3005_D:
        if (pPlayer->HasItemCount(ITEM_RED_PUNCH_CARD, 1))
        {
            pPlayer->DestroyItemCount(ITEM_RED_PUNCH_CARD, 1, true);
            pPlayer->CastSpell(pPlayer, SPELL_PRISMATIC_PUNCH_CARD, true);
        }
        break;
    default:
        break;
    }
    return false;
};

/*######
## go_blood_filled_orb
######*/

#define NPC_ZELEMAR  17830

bool GOHello_go_blood_filled_orb(Player* pPlayer, GameObject* pGO)
{
    if (pGO->GetGoType() == GAMEOBJECT_TYPE_GOOBER)
        pPlayer->SummonCreature(NPC_ZELEMAR, -369.746f, 166.759f, -21.50f, 5.235f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);

    return true;
};

/*######
## go_soulwell
######*/

bool GOHello_go_soulwell(Player* pPlayer, GameObject* pGO)
{
    Unit* caster = pGO->GetOwner();
    if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
        return true;

    if (!pPlayer->IsInSameRaidWith(static_cast<Player* >(caster)))
        return true;

    // Repeating this at every use is ugly and inefficient. But as long as we don't have proper
    // GO scripting with at least On Create and On Update events, the other options are no less
    // ugly and hacky.
    uint32 newSpell = 0;
    if (pGO->GetEntry() == 193169)                                  // Soulwell for rank 2
    {
        if (caster->HasAura(18693, 0))     // Improved Healthstone rank 2
            newSpell = 58898;
        else if (caster->HasAura(18692, 0)) // Improved Healthstone rank 1
            newSpell = 58896;
        else newSpell = 58890;
    }
    else if (pGO->GetEntry() == 181621)                             // Soulwell for rank 1
    {
        if (caster->HasAura(18693, 0))     // Improved Healthstone rank 2
            newSpell = 34150;
        else if (caster->HasAura(18692, 0)) // Improved Healthstone rank 1
            newSpell = 34149;
        else newSpell = 34130;
    }

    pGO->AddUse();
    pPlayer->CastSpell(pPlayer, newSpell, true);
    return true;
};

/*######
## Quest 1126: Hive in the Tower
######*/

enum eHives
{
    QUEST_HIVE_IN_THE_TOWER                       = 9544,
    NPC_HIVE_AMBUSHER                             = 13301
};

bool GOHello_go_hive_pod(Player* pPlayer, GameObject* pGO)
{
    pPlayer->SendLoot(pGO->GetGUID(), LOOT_CORPSE);
    pGO->SummonCreature(NPC_HIVE_AMBUSHER, pGO->GetPositionX() + 1, pGO->GetPositionY(), pGO->GetPositionZ(), pGO->GetAngle(pPlayer), TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
    pGO->SummonCreature(NPC_HIVE_AMBUSHER, pGO->GetPositionX(), pGO->GetPositionY() + 1, pGO->GetPositionZ(), pGO->GetAngle(pPlayer), TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
    return true;
};

/*######
## Quest 11011: Eternal Vigilance
######*/

#define ITEM_ESSENCE_INFUSED_MOONSTONE 32449

bool GOHello_go_the_ravens_claw(Player* pPlayer, GameObject* /*pGO*/)
{
    if (pPlayer->HasItemCount(ITEM_ESSENCE_INFUSED_MOONSTONE, 1))
        pPlayer->DestroyItemCount(ITEM_ESSENCE_INFUSED_MOONSTONE, 1, true);
    return true;
}

void AddSC_go_scripts()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "go_cat_figurine";
    newscript->pGOHello = &GOHello_go_cat_figurine;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_northern_crystal_pylon";
    newscript->pGOHello = &GOHello_go_northern_crystal_pylon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_eastern_crystal_pylon";
    newscript->pGOHello = &GOHello_go_eastern_crystal_pylon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_western_crystal_pylon";
    newscript->pGOHello = &GOHello_go_western_crystal_pylon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_barov_journal";
    newscript->pGOHello = &GOHello_go_barov_journal;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_field_repair_bot_74A";
    newscript->pGOHello = &GOHello_go_field_repair_bot_74A;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_orb_of_command";
    newscript->pGOHello = &GOHello_go_orb_of_command;
    newscript->pGOSelect = &GOSelect_go_orb_of_command;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_shrine_of_the_birds";
    newscript->pGOHello = &GOHello_go_shrine_of_the_birds;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_southfury_moonstone";
    newscript->pGOHello = &GOHello_go_southfury_moonstone;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_tablet_of_madness";
    newscript->pGOHello = &GOHello_go_tablet_of_madness;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_tablet_of_the_seven";
    newscript->pGOHello = &GOHello_go_tablet_of_the_seven;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_jump_a_tron";
    newscript->pGOHello = &GOHello_go_jump_a_tron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_ethereum_prison";
    newscript->pGOHello = &GOHello_go_ethereum_prison;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_ethereum_stasis";
    newscript->pGOHello = &GOHello_go_ethereum_stasis;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_resonite_cask";
    newscript->pGOHello = &GOHello_go_resonite_cask;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_sacred_fire_of_life";
    newscript->pGOHello = &GOHello_go_sacred_fire_of_life;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_iruxos";
    newscript->pGOHello = &GOHello_go_iruxos;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_fel_crystalforge";
    newscript->pGOHello = &GOHello_go_fel_crystalforge;
    newscript->pGOSelect = &GOSelect_go_fel_crystalforge;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_bashir_crystalforge";
    newscript->pGOHello = &GOHello_go_bashir_crystalforge;
    newscript->pGOSelect = &GOSelect_go_bashir_crystalforge;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_matrix_punchograph";
    newscript->pGOHello = &GOHello_go_matrix_punchograph;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_blood_filled_orb";
    newscript->pGOHello = &GOHello_go_blood_filled_orb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_soulwell";
    newscript->pGOHello = &GOHello_go_soulwell;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_hive_pod";
    newscript->pGOHello = &GOHello_go_hive_pod;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_the_ravens_claw";
    newscript->pGOHello = &GOHello_go_the_ravens_claw;
    newscript->RegisterSelf();
}
