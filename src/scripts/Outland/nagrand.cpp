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
SDName: Nagrand
SD%Complete: 99
SDComment: Quest support: 9849, 9918, 9874, 9991, 10107, 10108, 10044, 10172, 10646, 10085, 10987, 9868, 9948, 9923, 9924, 9955, 9879. TextId's unknown for altruis_the_sufferer and greatmother_geyah (npc_text)
SDCategory: Nagrand
EndScriptData */

/* ContentData
mob_shattered_rumbler
mob_lump
mob_sunspring_villager
npc_altruis_the_sufferer
npc_greatmother_geyah
npc_lantresor_of_the_blade
npc_maghar_captive
npc_creditmarker_visit_with_ancestors
mob_sparrowhawk
go_maghar_prison
npc_maghar_prisoner
go_corkis_prison1
npc_corki1
go_corkis_prison2
npc_corki2
go_corkis_prison3
npc_corki3
npc_kurenai_captive
npc_warmaul_pyre
npc_fel_cannon_haf
npc_living_cyclone
npc_enraged_crusher
npc_storm_rager
npc_lake_surger
npc_lake_spirit
npc_tortured_earth_spirit
npc_crashing_wave_spirit
npc_raging_fire_soul
npc_storming_wind_ripper
npc_rumbling_earth_heart
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"

/*######
## mob_shattered_rumbler - this should be done with ACID
######*/

#define SPELL_EARTH_RUMBLE  33840

struct mob_shattered_rumblerAI : public ScriptedAI
{
    bool Spawn;

    mob_shattered_rumblerAI(Creature* c) : ScriptedAI(c) {}

    uint32 EarthRumbleTimer;

    void Reset()
    {
        Spawn = false;
        EarthRumbleTimer = urand(12000, 17000);

        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);
    }

    void UpdateAI(const uint32 diff)
    {
        if (EarthRumbleTimer < diff)
        {
            DoCast(me, SPELL_EARTH_RUMBLE);
            EarthRumbleTimer = urand(14000, 21000);
        }
        else
            EarthRumbleTimer -= diff;
    }            

    void EnterCombat(Unit* /*who*/) {}

    void SpellHit(Unit* Hitter, const SpellEntry* Spellkind)
    {
        if (Spellkind->Id == 32001 && !Spawn)
        {
            float x = me->GetPositionX();
            float y = me->GetPositionY();
            float z = me->GetPositionZ();

            Hitter->SummonCreature(18181, x + (0.7f * (rand() % 30)), y + (rand() % 5), z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
            Hitter->SummonCreature(18181, x + (rand() % 5), y - (rand() % 5), z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
            Hitter->SummonCreature(18181, x - (rand() % 5), y + (0.5f * (rand() % 60)), z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
            me->DisappearAndDie(false);
            me->setDeathState(CORPSE);
            Spawn = true;
        }
        return;
    }
};
CreatureAI* GetAI_mob_shattered_rumbler(Creature* pCreature)
{
    return new mob_shattered_rumblerAI (pCreature);
}

/*######
## mob_lump
######*/

enum eLump
{
    SPELL_VISUAL_SLEEP  = 16093,
    SPELL_SPEAR_THROW   = 32248,

    LUMP_SAY0           = -1000190,
    LUMP_SAY1           = -1000191,
    LUMP_DEFEAT         = -1000192
};

#define GOSSIP_HL "I need answers, ogre!"
#define GOSSIP_SL1 "Why are Boulderfist out this far? You know that this is Kurenai territory."
#define GOSSIP_SL2 "And you think you can just eat anything you want? You're obviously trying to eat the Broken of Telaar."
#define GOSSIP_SL3 "This means war, Lump! War I say!"

struct mob_lumpAI : public ScriptedAI
{
    mob_lumpAI(Creature* c) : ScriptedAI(c)
    {
        bReset = false;
    }

    uint32 Reset_Timer;
    uint32 Spear_Throw_Timer;
    bool bReset;

    void Reset()
    {
        Reset_Timer = 60000;
        Spear_Throw_Timer = 2000;

        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void DamageTaken(Unit* done_by, uint32& damage)
    {
        if (done_by->GetTypeId() == TYPEID_PLAYER && me->HealthBelowPctDamaged(30, damage))
        {
            if (!bReset && CAST_PLR(done_by)->GetQuestStatus(9918) == QUEST_STATUS_INCOMPLETE)
            {
                //Take 0 damage
                damage = 0;

                CAST_PLR(done_by)->AttackStop();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->RemoveAllAuras();
                me->DeleteThreatList();
                me->CombatStop();
                me->SetFaction(1080);               //friendly
                me->SetStandState(UNIT_STAND_STATE_SIT);
                DoScriptText(LUMP_DEFEAT, me);

                bReset = true;
            }
        }
    }

    void EnterCombat(Unit* /*who*/)
    {
        if (me->HasAura(SPELL_VISUAL_SLEEP, 0))
            me->RemoveAura(SPELL_VISUAL_SLEEP, 0);

        if (!me->IsStandState())
            me->SetStandState(UNIT_STAND_STATE_STAND);

        switch (rand() % 2)
        {
        case 0:
            DoScriptText(LUMP_SAY0, me);
            break;
        case 1:
            DoScriptText(LUMP_SAY1, me);
            break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //check if we waiting for a reset
        if (bReset)
        {
            if (Reset_Timer <= diff)
            {
                EnterEvadeMode();
                bReset = false;
                me->SetFaction(1711);               //hostile
                return;
            }
            else Reset_Timer -= diff;
        }

        //Return since we have no target
        if (!UpdateVictim())
            return;

        //Spear_Throw_Timer
        if (Spear_Throw_Timer <= diff)
        {
            DoCastVictim( SPELL_SPEAR_THROW);
            Spear_Throw_Timer = 20000;
        }
        else Spear_Throw_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_lump(Creature* pCreature)
{
    return new mob_lumpAI(pCreature);
}

bool GossipHello_mob_lump(Player* player, Creature* pCreature)
{
    if (player->GetQuestStatus(9918) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    player->SEND_GOSSIP_MENU(9352, pCreature->GetGUID());

    return true;
}

bool GossipSelect_mob_lump(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SL1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(9353, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+1:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SL2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(9354, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SL3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->SEND_GOSSIP_MENU(9355, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+3:
        player->SEND_GOSSIP_MENU(9356, pCreature->GetGUID());
        player->TalkedToCreature(18354, pCreature->GetGUID());
        break;
    }
    return true;
}

/*####
# mob_sunspring_villager - should be done with ACID
####*/

enum eSunspringVillager
{
	SPELL_LIQUID_FIRE_QUEST_ITEM = 32146,
	SPELL_SUMMON_LIQUID_FIRE = 31706,
	NPC_SUNSPRING_VILLAGER = 18240
};

struct mob_sunspring_villagerAI : public ScriptedAI
{
    mob_sunspring_villagerAI(Creature* c) : ScriptedAI(c) {}

    void EnterCombat(Unit* /*who*/) {}

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if (spell->Id == SPELL_LIQUID_FIRE_QUEST_ITEM)
        {
            if (Player* player = caster->ToPlayer())
                player->KilledMonsterCredit(NPC_SUNSPRING_VILLAGER, me->GetGUID());
			me->CastSpell(me, SPELL_SUMMON_LIQUID_FIRE, true);
			me->DespawnOrUnsummon(5000);
        }
    }
};
CreatureAI* GetAI_mob_sunspring_villager(Creature* pCreature)
{
    return new mob_sunspring_villagerAI (pCreature);
}

/*######
## npc_altruis_the_sufferer
######*/

#define GOSSIP_HATS1 "I see twisted steel and smell sundered earth."
#define GOSSIP_HATS2 "Well...?"
#define GOSSIP_HATS3 "[PH] Story about Illidan's Pupil"

#define GOSSIP_SATS1 "Legion?"
#define GOSSIP_SATS2 "And now?"
#define GOSSIP_SATS3 "How do you see them now?"
#define GOSSIP_SATS4 "Forge camps?"
#define GOSSIP_SATS5 "Ok."
#define GOSSIP_SATS6 "[PH] Story done"

bool GossipHello_npc_altruis_the_sufferer(Player* player, Creature* pCreature)
{
    if (pCreature->IsQuestGiver())
        player->PrepareQuestMenu(pCreature->GetGUID());

    //gossip before obtaining Survey the Land
    if (player->GetQuestStatus(9991) == QUEST_STATUS_NONE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HATS1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);

    //gossip when Survey the Land is incomplete (technically, after the flight)
    if (player->GetQuestStatus(9991) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HATS2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 20);

    //wowwiki.com/Varedis
    if (player->GetQuestStatus(10646) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HATS3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 30);

    player->SEND_GOSSIP_MENU(9419, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_altruis_the_sufferer(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF+10:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SATS1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
        player->SEND_GOSSIP_MENU(9420, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+11:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SATS2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
        player->SEND_GOSSIP_MENU(9421, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+12:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SATS3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
        player->SEND_GOSSIP_MENU(9422, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+13:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SATS4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
        player->SEND_GOSSIP_MENU(9423, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+14:
        player->SEND_GOSSIP_MENU(9424, pCreature->GetGUID());
        break;

    case GOSSIP_ACTION_INFO_DEF+20:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SATS5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 21);
        player->SEND_GOSSIP_MENU(9427, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+21:
        player->CLOSE_GOSSIP_MENU();
        player->AreaExploredOrEventHappens(9991);
        break;

    case GOSSIP_ACTION_INFO_DEF+30:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SATS6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 31);
        player->SEND_GOSSIP_MENU(384, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+31:
        player->CLOSE_GOSSIP_MENU();
        player->AreaExploredOrEventHappens(10646);
        break;
    }
    return true;
}

bool QuestAccept_npc_altruis_the_sufferer(Player* player, Creature* /*creature*/, Quest const* /*quest*/)
{
    if (!player->GetQuestRewardStatus(9991))              //Survey the Land, q-id 9991
    {
        player->CLOSE_GOSSIP_MENU();

        std::vector<uint32> nodes;

        nodes.resize(2);
        nodes[0] = 113;                                     //from
        nodes[1] = 114;                                     //end at
        player->ActivateTaxiPathTo(nodes);                  //TaxiPath 532
    }
    return true;
}

/*######
## npc_greatmother_geyah
######*/

#define GOSSIP_HGG1 "Hello, Greatmother. Garrosh told me that you wanted to speak with me."
#define GOSSIP_HGG2 "Garrosh is beyond redemption, Greatmother. I fear that in helping the Mag'har, I have convinced Garrosh that he is unfit to lead."

#define GOSSIP_SGG1 "You raised all of the orcs here, Greatmother?"
#define GOSSIP_SGG2 "Do you believe that?"
#define GOSSIP_SGG3 "What can be done? I have tried many different things. I have done my best to help the people of Nagrand. Each time I have approached Garrosh, he has dismissed me."
#define GOSSIP_SGG4 "Left? How can you choose to leave?"
#define GOSSIP_SGG5 "What is this duty?"
#define GOSSIP_SGG6 "Is there anything I can do for you, Greatmother?"
#define GOSSIP_SGG7 "I have done all that I could, Greatmother. I thank you for your kind words."
#define GOSSIP_SGG8 "Greatmother, you are the mother of Durotan?"
#define GOSSIP_SGG9 "Greatmother, I never had the honor. Durotan died long before my time, but his heroics are known to all on my world. The orcs of Azeroth reside in a place known as Durotar, named after your son. And ... (You take a moment to breathe and think through what you are about to tell the Greatmother.)"
#define GOSSIP_SGG10 "It is my Warchief, Greatmother. The leader of my people. From my world. He ... He is the son of Durotan. He is your grandchild."
#define GOSSIP_SGG11 "I will return to Azeroth at once, Greatmother."

//all the textId's for the below is unknown, but i do believe the gossip item texts are proper.
bool GossipHello_npc_greatmother_geyah(Player* player, Creature* pCreature)
{
    if (pCreature->IsQuestGiver())
        player->PrepareQuestMenu(pCreature->GetGUID());

    if (player->GetQuestStatus(10044) == QUEST_STATUS_INCOMPLETE)
    {
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HGG1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		player->SEND_GOSSIP_MENU(9507, pCreature->GetGUID());
    }
    else if (player->GetQuestStatus(10172) == QUEST_STATUS_INCOMPLETE)
    {
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HGG2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
		player->SEND_GOSSIP_MENU(23013, pCreature->GetGUID());
    }
    else

		player->SEND_GOSSIP_MENU(9507, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_greatmother_geyah(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SGG1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
		player->SEND_GOSSIP_MENU(9508, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SGG2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
		player->SEND_GOSSIP_MENU(9509, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SGG3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
		player->SEND_GOSSIP_MENU(9510, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SGG4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
		player->SEND_GOSSIP_MENU(9511, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SGG5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
		player->SEND_GOSSIP_MENU(9512, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SGG6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
		player->SEND_GOSSIP_MENU(9513, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:
        player->AreaExploredOrEventHappens(10044);
        player->CLOSE_GOSSIP_MENU();
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SGG7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
		player->SEND_GOSSIP_MENU(23014, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SGG8, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
		player->SEND_GOSSIP_MENU(23015, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 12:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SGG9, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
		player->SEND_GOSSIP_MENU(23016, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 13:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SGG10, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
		player->SEND_GOSSIP_MENU(23017, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 14:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SGG11, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15);
		player->SEND_GOSSIP_MENU(23018, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 15:
        player->AreaExploredOrEventHappens(10172);
        player->CLOSE_GOSSIP_MENU();
        break;
    }
    return true;
}

/*######
## npc_lantresor_of_the_blade
######*/

#define GOSSIP_HLB "I have killed many of your ogres, Lantresor. I have no fear."
#define GOSSIP_SLB1 "Should I know? You look like an orc to me."
#define GOSSIP_SLB2 "And the other half?"
#define GOSSIP_SLB3 "I have heard of your kind, but I never thought to see the day when I would meet a half-breed."
#define GOSSIP_SLB4 "My apologies. I did not mean to offend. I am here on behalf of my people."
#define GOSSIP_SLB5 "My people ask that you pull back your Boulderfist ogres and cease all attacks on our territories. In return, we will also pull back our forces."
#define GOSSIP_SLB6 "We will fight you until the end, then, Lantresor. We will not stand idly by as you pillage our towns and kill our people."
#define GOSSIP_SLB7 "What do I need to do?"

bool GossipHello_npc_lantresor_of_the_blade(Player* player, Creature* pCreature)
{
    if (pCreature->IsQuestGiver())
        player->PrepareQuestMenu(pCreature->GetGUID());

    if (player->GetQuestStatus(10107) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(10108) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HLB, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    player->SEND_GOSSIP_MENU(9361, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_lantresor_of_the_blade(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SLB1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(9362, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+1:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SLB2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(9363, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SLB3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->SEND_GOSSIP_MENU(9364, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+3:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SLB4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->SEND_GOSSIP_MENU(9365, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+4:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SLB5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->SEND_GOSSIP_MENU(9366, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+5:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SLB6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->SEND_GOSSIP_MENU(9367, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+6:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SLB7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->SEND_GOSSIP_MENU(9368, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+7:
        player->SEND_GOSSIP_MENU(9369, pCreature->GetGUID());
        if (player->GetQuestStatus(10107) == QUEST_STATUS_INCOMPLETE)
            player->AreaExploredOrEventHappens(10107);
        if (player->GetQuestStatus(10108) == QUEST_STATUS_INCOMPLETE)
            player->AreaExploredOrEventHappens(10108);
        break;
    }
    return true;
}

/*#####
## npc_maghar_captive
#####*/

enum eMagharCaptive
{
    SAY_MAG_START               = -1000482,
    SAY_MAG_NO_ESCAPE           = -1000483,
    SAY_MAG_MORE                = -1000484,
    SAY_MAG_MORE_REPLY          = -1000485,
    SAY_MAG_LIGHTNING           = -1000486,
    SAY_MAG_SHOCK               = -1000487,
    SAY_MAG_COMPLETE            = -1000488,

    SPELL_CHAIN_LIGHTNING       = 16006,
    SPELL_EARTHBIND_TOTEM       = 15786,
    SPELL_FROST_SHOCK           = 12548,
    SPELL_HEALING_WAVE          = 12491,

    QUEST_TOTEM_KARDASH_H       = 9868,

    NPC_MURK_RAIDER             = 18203,
    NPC_MURK_BRUTE              = 18211,
    NPC_MURK_SCAVENGER          = 18207,
    NPC_MURK_PUTRIFIER          = 18202
};

static float m_afAmbushA[] = { -1568.805786f, 8533.873047f, 1.958f};
static float m_afAmbushB[] = { -1491.554321f, 8506.483398f, 1.248f};

struct npc_maghar_captiveAI : public npc_escortAI
{
    npc_maghar_captiveAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiChainLightningTimer;
    uint32 m_uiHealTimer;
    uint32 m_uiFrostShockTimer;

    void Reset()
    {
        m_uiChainLightningTimer = 1000;
        m_uiHealTimer = 0;
        m_uiFrostShockTimer = 6000;
    }

    void EnterCombat(Unit* /*pWho*/)
    {
        DoCast(me, SPELL_EARTHBIND_TOTEM, false);
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch (uiPointId)
        {
        case 7:
            DoScriptText(SAY_MAG_MORE, me);

            if (Creature* pTemp = me->SummonCreature(NPC_MURK_PUTRIFIER, m_afAmbushB[0], m_afAmbushB[1], m_afAmbushB[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000))
                DoScriptText(SAY_MAG_MORE_REPLY, pTemp);
            me->SummonCreature(NPC_MURK_PUTRIFIER, m_afAmbushB[0] - 2.5f, m_afAmbushB[1] - 2.5f, m_afAmbushB[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            me->SummonCreature(NPC_MURK_SCAVENGER, m_afAmbushB[0] + 2.5f, m_afAmbushB[1] + 2.5f, m_afAmbushB[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            me->SummonCreature(NPC_MURK_SCAVENGER, m_afAmbushB[0] + 2.5f, m_afAmbushB[1] - 2.5f, m_afAmbushB[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            break;
        case 16:
            DoScriptText(SAY_MAG_COMPLETE, me);
            if (Player* pPlayer = GetPlayerForEscort())
                pPlayer->GroupEventHappens(QUEST_TOTEM_KARDASH_H, me);
            SetRun();
            break;
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_MURK_BRUTE)
            DoScriptText(SAY_MAG_NO_ESCAPE, pSummoned);

        if (pSummoned->IsTotem())
            return;

        pSummoned->SetWalk(false);
        pSummoned->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
        pSummoned->AI()->AttackStart(me);

    }

    void SpellHitTarget(Unit* /*pTarget*/, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_CHAIN_LIGHTNING)
        {
            if (rand() % 10)
                return;

            DoScriptText(SAY_MAG_LIGHTNING, me);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        npc_escortAI::UpdateAI(uiDiff);
        if (!me->GetVictim())
            return;

        if (m_uiChainLightningTimer <= uiDiff)
        {
            DoCastVictim( SPELL_CHAIN_LIGHTNING);
            m_uiChainLightningTimer = urand(7000, 14000);
        }
        else
            m_uiChainLightningTimer -= uiDiff;

        if (me->GetHealth() * 100 < me->GetMaxHealth() * 30)
        {
            if (m_uiHealTimer <= uiDiff)
            {
                DoCast(me, SPELL_HEALING_WAVE);
                m_uiHealTimer = 5000;
            }
            else
                m_uiHealTimer -= uiDiff;
        }

        if (m_uiFrostShockTimer <= uiDiff)
        {
            DoCastVictim( SPELL_FROST_SHOCK);
            m_uiFrostShockTimer = urand(7500, 15000);
        }
        else
            m_uiFrostShockTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_npc_maghar_captive(Creature* pCreature)
{
    return new npc_maghar_captiveAI(pCreature);
}

bool QuestAccept_npc_maghar_captive(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_TOTEM_KARDASH_H)
    {
        if (npc_maghar_captiveAI* pEscortAI = dynamic_cast<npc_maghar_captiveAI*>(pCreature->AI()))
        {
            pCreature->SetStandState(UNIT_STAND_STATE_STAND);
            pCreature->SetFaction(232);

            pEscortAI->Start(true, false, pPlayer->GetGUID(), pQuest);

            DoScriptText(SAY_MAG_START, pCreature);

            pCreature->SummonCreature(NPC_MURK_RAIDER, m_afAmbushA[0] + 2.5f, m_afAmbushA[1] - 2.5f, m_afAmbushA[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            pCreature->SummonCreature(NPC_MURK_PUTRIFIER, m_afAmbushA[0] - 2.5f, m_afAmbushA[1] + 2.5f, m_afAmbushA[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            pCreature->SummonCreature(NPC_MURK_BRUTE, m_afAmbushA[0], m_afAmbushA[1], m_afAmbushA[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
        }
    }
    return true;
}


/*######
## npc_creditmarker_visist_with_ancestors
######*/

struct npc_creditmarker_visit_with_ancestorsAI : public ScriptedAI
{
    npc_creditmarker_visit_with_ancestorsAI(Creature* c) : ScriptedAI(c) {}

    void Reset() {}

    void EnterCombat(Unit* /*who*/) {}

    void MoveInLineOfSight(Unit* who)
    {
        if (!who)
            return;

        if (who->GetTypeId() == TYPEID_PLAYER)
        {
            if (CAST_PLR(who)->GetQuestStatus(10085) == QUEST_STATUS_INCOMPLETE)
            {
                uint32 creditMarkerId = me->GetEntry();
                if ((creditMarkerId >= 18840) && (creditMarkerId <= 18843))
                {
                    // 18840: Sunspring, 18841: Laughing, 18842: Garadar, 18843: Bleeding
                    if (!CAST_PLR(who)->GetReqKillOrCastCurrentCount(10085, creditMarkerId))
                        CAST_PLR(who)->KilledMonsterCredit(creditMarkerId, me->GetGUID());
                }
            }
        }
    }
};

CreatureAI* GetAI_npc_creditmarker_visit_with_ancestors(Creature* pCreature)
{
    return new npc_creditmarker_visit_with_ancestorsAI (pCreature);
}

/*######
## mob_sparrowhawk
######*/

#define SPELL_SPARROWHAWK_NET 39810
#define SPELL_ITEM_CAPTIVE_SPARROWHAWK 39812

struct mob_sparrowhawkAI : public ScriptedAI
{

    mob_sparrowhawkAI(Creature* c) : ScriptedAI(c) {}

    uint32 Check_Timer;
    uint64 PlayerGUID;
    bool fleeing;

    void Reset()
    {
        me->RemoveAurasDueToSpell(SPELL_SPARROWHAWK_NET);
        Check_Timer = 1000;
        PlayerGUID = 0;
        fleeing = false;
    }
    void AttackStart(Unit* who)
    {
        if (PlayerGUID)
            return;

        ScriptedAI::AttackStart(who);
    }

    void EnterCombat(Unit* /*who*/) {}

    void MoveInLineOfSight(Unit* who)
    {
        if (!who || PlayerGUID)
            return;

        if (!PlayerGUID && who->GetTypeId() == TYPEID_PLAYER && me->IsWithinDistInMap(((Player*)who), 30) && ((Player*)who)->GetQuestStatus(10987) == QUEST_STATUS_INCOMPLETE)
        {
            PlayerGUID = who->GetGUID();
            return;
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void UpdateAI(const uint32 diff)
    {
        if (Check_Timer <= diff)
        {
            if (PlayerGUID)
            {
                if (fleeing && me->GetMotionMaster()->GetCurrentMovementGeneratorType() != FLEEING_MOTION_TYPE)
                    fleeing = false;

                Player* player = Unit::GetPlayer(*me, PlayerGUID);
                if (player && me->IsWithinDistInMap(player, 30))
                {
                    if (!fleeing)
                    {
                        me->DeleteThreatList();
                        me->GetMotionMaster()->MoveFleeing(player);
                        fleeing = true;
                    }
                }
                else if (fleeing)
                {
                    me->GetMotionMaster()->MovementExpired(false);
                    PlayerGUID = 0;
                    fleeing = false;
                }
            }
            Check_Timer = 1000;
        }
        else Check_Timer -= diff;

        if (PlayerGUID)
            return;

        ScriptedAI::UpdateAI(diff);
    }

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if (caster->GetTypeId() == TYPEID_PLAYER)
        {
            if (spell->Id == SPELL_SPARROWHAWK_NET && CAST_PLR(caster)->GetQuestStatus(10987) == QUEST_STATUS_INCOMPLETE)
            {
                me->CastSpell(caster, SPELL_ITEM_CAPTIVE_SPARROWHAWK, true);
                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            }
        }
        return;
    }
};

CreatureAI* GetAI_mob_sparrowhawk(Creature* pCreature)
{
    return new mob_sparrowhawkAI (pCreature);
}

/*#####
## go_maghar_prison & npc_maghar_prisoner
#####*/

enum
{
    QUEST_SURVIVORS       = 9948,
    NPC_MPRISONER         = 18428,

    SAY_MAG_PRISONER1     = -1900148,
    SAY_MAG_PRISONER2     = -1900149,
    SAY_MAG_PRISONER3     = -1900150,
    SAY_MAG_PRISONER4     = -1900151,
    SAYT_MAG_PRISONER1    = -1900152,
    SAYT_MAG_PRISONER2    = -1900153,
    SAYT_MAG_PRISONER3    = -1900154,
    SAYT_MAG_PRISONER4    = -1900155
};

struct npc_maghar_prisonerAI : public npc_escortAI
{
    npc_maghar_prisonerAI(Creature* pCreature) : npc_escortAI(pCreature) {}

    uint64 uiPlayerGUID;

    void Reset()
    {
        uiPlayerGUID = 0;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER && ((Player*)pWho)->GetReputationRank(941) >= REP_FRIENDLY && me->IsWithinDistInMap(((Player*)pWho), 20))
        {
            if (uiPlayerGUID == pWho->GetGUID())
                return;
            else uiPlayerGUID = 0;

            switch (urand(0, 3))
            {
            case 0:
                DoScriptText(SAY_MAG_PRISONER1, me);
                break;
            case 1:
                DoScriptText(SAY_MAG_PRISONER2, me);
                break;
            case 2:
                DoScriptText(SAY_MAG_PRISONER3, me);
                break;
            case 3:
                DoScriptText(SAY_MAG_PRISONER4, me);
                break;
            }
            uiPlayerGUID = pWho->GetGUID();
        }
    }

    uint32 WaypointID()
    {
        switch (me->GetGUIDLow())
        {
        case 65828:
            return 1;
            break;
        case 65826:
            return 1;
            break;
        case 65827:
            return 1;
            break;
        case 65825:
            return 1;
            break;
        case 65829:
            return 1;
            break;
        case 65823:
            return 2;
            break;
        case 65824:
            return 2;
            break;
        case 65821:
            return 2;
            break;
        case 65815:
            return 2;
            break;
        case 65814:
            return 3;
            break;
        case 65813:
            return 4;
            break;
        case 65819:
            return 5;
            break;
        case 65820:
            return 5;
            break;
        case 65817:
            return 6;
            break;
        case 65822:
            return 6;
            break;
        case 65816:
            return 6;
            break;
        case 65831:
            return 7;
            break;
        case 65832:
            return 7;
            break;
        case 65830:
            return 7;
            break;
        case 65818:
            return 8;
            break;
        default:
            return 1;
            break;
        }
    }

    void StartRun(Player* pPlayer)
    {
        switch (WaypointID())
        {
        case 1:
            AddWaypoint(0, -1076.000f, 8945.270f, 101.891f);
            AddWaypoint(1, -1087.469f, 8894.919f, 102.183f);
            Start(false, false, pPlayer->GetGUID());
            break;
        case 2:
            AddWaypoint(0, -782.796f, 8875.171f, 181.745f);
            AddWaypoint(1, -821.331f, 8913.110f, 171.417f);
            Start(false, false, pPlayer->GetGUID());
            break;
        case 3:
            AddWaypoint(0, -670.298f, 8810.587f, 196.057f);
            AddWaypoint(1, -717.270f, 8806.274f, 184.591f);
            Start(false, false, pPlayer->GetGUID());
            break;
        case 4:
            AddWaypoint(0, -710.969f, 8763.471f, 186.513f);
            AddWaypoint(1, -782.685f, 8874.755f, 181.740f);
            Start(false, false, pPlayer->GetGUID());
            break;
        case 5:
            AddWaypoint(0, -865.144f, 8713.610f, 248.041f);
            AddWaypoint(1, -880.415f, 8743.203f, 233.202f);
            Start(false, false, pPlayer->GetGUID());
            break;
        case 6:
            AddWaypoint(0, -847.285f, 8722.406f, 177.255f);
            AddWaypoint(1, -810.138f, 8731.109f, 178.226f);
            Start(false, false, pPlayer->GetGUID());
            break;
        case 7:
            AddWaypoint(0, -897.005f, 8689.280f, 170.527f);
            AddWaypoint(1, -844.252f, 8721.320f, 177.257f);
            Start(false, false, pPlayer->GetGUID());
            break;
        case 8:
            AddWaypoint(0, -838.047f, 8691.124f, 180.549f);
            AddWaypoint(1, -821.200f, 8712.569f, 182.702f);
            Start(false, false, pPlayer->GetGUID());
            break;
        }
        return;
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch (uiPointId)
        {
        case 0:
            SetRun();
            break;
        case 1:
            me->ForcedDespawn();
            break;
        }
    }
};

CreatureAI* GetAI_npc_maghar_prisoner(Creature* pCreature)
{
    return new npc_maghar_prisonerAI(pCreature);
}

bool GOHello_maghar_prison(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_SURVIVORS) == QUEST_STATUS_INCOMPLETE)
    {
        if (Creature* pPrisoner = pGo->FindNearestCreature(NPC_MPRISONER, 5, true))
        {
            pPlayer->KilledMonsterCredit(NPC_MPRISONER, pPrisoner->GetGUID());

            switch (urand(0, 3))
            {
            case 0:
                DoScriptText(SAYT_MAG_PRISONER1, pPrisoner, pPlayer);
                break;
            case 1:
                DoScriptText(SAYT_MAG_PRISONER2, pPrisoner, pPlayer);
                break;
            case 2:
                DoScriptText(SAYT_MAG_PRISONER3, pPrisoner, pPlayer);
                break;
            case 3:
                DoScriptText(SAYT_MAG_PRISONER4, pPrisoner, pPlayer);
                break;
            }

            if (npc_maghar_prisonerAI* pEscortAI = CAST_AI(npc_maghar_prisonerAI, pPrisoner->AI()))
                pEscortAI->StartRun(pPlayer);
        }
    }
    return false;
};

/*#####
## go_corkis_prison1 & npc_corki1
#####*/

enum
{
    QUEST_HELP              = 9923,
    NPC_CORKI1              = 18445,
    NPC_CORKI_CREDIT_1      = 18369,
    SAY_THANKS              = -1900133,
    SAY_KORKI2              = -1900134,
    SAY_KORKI3              = -1900135,
    SAY_KORKI4              = -1900136,
    SAY_KORKI5              = -1900137,
    SAY_KORKI6              = -1900138
};

struct npc_corki1AI : public npc_escortAI
{
    npc_corki1AI(Creature* pCreature) : npc_escortAI(pCreature) {}

    uint64 uiPlayerGUID;

    void Reset()
    {
        uiPlayerGUID = 0;
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch (uiPointId)
        {
        case 0:
            SetRun();
            break;
        case 3:
            me->ForcedDespawn();
        }
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER && ((Player*)pWho)->GetReputationRank(978) >= REP_FRIENDLY && me->IsWithinDistInMap(((Player*)pWho), 20))
        {
            if (uiPlayerGUID == pWho->GetGUID())
                return;
            else uiPlayerGUID = 0;

            switch (urand(0, 4))
            {
            case 0:
                DoScriptText(SAY_KORKI2, me);
                break;
            case 1:
                DoScriptText(SAY_KORKI3, me);
                break;
            case 2:
                DoScriptText(SAY_KORKI4, me);
                break;
            case 3:
                DoScriptText(SAY_KORKI5, me);
                break;
            case 4:
                DoScriptText(SAY_KORKI6, me);
                break;
            }

            uiPlayerGUID = pWho->GetGUID();
        }
    }
};

CreatureAI* GetAI_npc_corki1(Creature* pCreature)
{
    return new npc_corki1AI(pCreature);
}

bool GOHello_corkis_prison1(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_HELP) == QUEST_STATUS_INCOMPLETE)
    {
        if (Creature* pCor1 = pGo->FindNearestCreature( NPC_CORKI1, 5, true))
        {
			pPlayer->KilledMonsterCredit(NPC_CORKI1, pCor1->GetGUID());
            DoScriptText(SAY_THANKS, pCor1, pPlayer);
            ((npc_corki1AI*)pCor1->AI())->Start(false, false, pPlayer->GetGUID());
        }
		
    }
    return false;
};

/*#####
## go_corkis_prison2 & npc_corki2
#####*/

enum
{
    QUEST_HELP1    = 9924,
    NPC_CORKI2     = 20812,

    SAY_THANKS1    = -1900139
};

struct npc_corki2AI : public npc_escortAI
{
    npc_corki2AI(Creature* pCreature) : npc_escortAI(pCreature) {}

    uint64 uiPlayerGUID;

    void Reset()
    {
        uiPlayerGUID = 0;
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch (uiPointId)
        {
        case 0:
            SetRun();
            break;
        case 2:
            me->ForcedDespawn();
        }
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER && ((Player*)pWho)->GetReputationRank(978) >= REP_FRIENDLY && me->IsWithinDistInMap(((Player*)pWho), 20))
        {
            if (uiPlayerGUID == pWho->GetGUID())
                return;
            else uiPlayerGUID = 0;

            switch (urand(0, 4))
            {
            case 0:
                DoScriptText(SAY_KORKI2, me);
                break;
            case 1:
                DoScriptText(SAY_KORKI3, me);
                break;
            case 2:
                DoScriptText(SAY_KORKI4, me);
                break;
            case 3:
                DoScriptText(SAY_KORKI5, me);
                break;
            case 4:
                DoScriptText(SAY_KORKI6, me);
                break;
            }

            uiPlayerGUID = pWho->GetGUID();
        }
    }
};

CreatureAI* GetAI_npc_corki2(Creature* pCreature)
{
    return new npc_corki2AI(pCreature);
}

bool GOHello_corkis_prison2(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_HELP1) == QUEST_STATUS_INCOMPLETE)
    {
        if (Creature* pCor2 = pGo->FindNearestCreature( NPC_CORKI2, 5, true))
        {
            pPlayer->KilledMonsterCredit(NPC_CORKI2, pCor2->GetGUID());
            DoScriptText(SAY_THANKS1, pCor2, pPlayer);
            ((npc_corki2AI*)pCor2->AI())->Start(false, false, pPlayer->GetGUID());
        }
    }
    return false;
};

/*#####
## go_corkis_prison3 & npc_corki3
#####*/

enum
{
    QUEST_HELP2         = 9955,
    NPC_CORKI3          = 18369,
    NPC_CORKI_CREDIT_3  = 18444,
};

struct npc_corki3AI : public npc_escortAI
{
    npc_corki3AI(Creature* pCreature) : npc_escortAI(pCreature) {}

    uint64 uiPlayerGUID;

    void Reset()
    {
        uiPlayerGUID = 0;
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch (uiPointId)
        {
        case 0:
            SetRun();
            break;
        case 2:
            me->ForcedDespawn();
        }
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER && ((Player*)pWho)->GetReputationRank(978) >= REP_FRIENDLY && me->IsWithinDistInMap(((Player*)pWho), 20))
        {
            if (uiPlayerGUID == pWho->GetGUID())
                return;
            else uiPlayerGUID = 0;

            switch (urand(0, 4))
            {
            case 0:
                DoScriptText(SAY_KORKI2, me);
                break;
            case 1:
                DoScriptText(SAY_KORKI3, me);
                break;
            case 2:
                DoScriptText(SAY_KORKI4, me);
                break;
            case 3:
                DoScriptText(SAY_KORKI5, me);
                break;
            case 4:
                DoScriptText(SAY_KORKI6, me);
                break;
            }

            uiPlayerGUID = pWho->GetGUID();
        }
    }
};

CreatureAI* GetAI_npc_corki3(Creature* pCreature)
{
    return new npc_corki3AI(pCreature);
}

bool GOHello_corkis_prison3(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_HELP2) == QUEST_STATUS_INCOMPLETE)
    {
        if (Creature* pCor3 = pGo->FindNearestCreature( NPC_CORKI3, 5, true))
        {
			pPlayer->KilledMonsterCredit(NPC_CORKI3, pCor3->GetGUID());
            DoScriptText(SAY_THANKS, pCor3, pPlayer);
            ((npc_corki3AI*)pCor3->AI())->Start(false, false, pPlayer->GetGUID());
        }
    }
    return false;
};

/*#####
## npc_kurenai_captive
#####*/

enum
{
    SAY_KUR_START            = -1000482,
    SAY_KUR_NO_ESCAPE        = -1000483,
    SAY_KUR_MORE             = -1000484,
    SAY_KUR_MORE_REPLY       = -1000485,
    SAY_KUR_LIGHTNING        = -1000486,
    SAY_KUR_SHOCK            = -1000487,
	SAY_KUR_MIDDLE			 = -1910145,
    SAY_KUR_COMPLETE         = -1900132,

    MAGIC_CHAIN_LIGHTNING    = 16006,
    MAGIC_EARTHBIND_TOTEM    = 15786,
    MAGIC_FROST_SHOCK        = 12548,
    MAGIC_HEALING_WAVE       = 12491,

    QUEST_TOTEM_KARDASH_A    = 9879,
    MURK_RAIDER              = 18203,
    MURK_BRUTE               = 18211,
    MURK_SCAVENGER           = 18207,
    MURK_PUTRIFIER           = 18202
};

static float m_afAmbushC[] = { -1531.204712f, 8456.174805f, -4.102f};
static float m_afAmbushD[] = { -1442.524780f, 8500.364258f, 6.381f};

struct npc_kurenai_captiveAI : public npc_escortAI
{
    npc_kurenai_captiveAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiChainLightningTimer;
    uint32 m_uiHealTimer;
    uint32 m_uiFrostShockTimer;

    void Reset()
    {
        m_uiChainLightningTimer = 1000;
        m_uiHealTimer = 0;
        m_uiFrostShockTimer = 6000;
    }

    void EnterCombat(Unit* /*pWho*/)
    {
        DoCast(me, MAGIC_EARTHBIND_TOTEM, false);
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch (uiPointId)
        {
        case 7:
            DoScriptText(SAY_KUR_MORE, me);
            if (Creature* pTemp = me->SummonCreature(MURK_PUTRIFIER, m_afAmbushD[0], m_afAmbushD[1], m_afAmbushD[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000))
                DoScriptText(SAY_KUR_MORE_REPLY, pTemp);
            me->SummonCreature(MURK_PUTRIFIER, m_afAmbushD[0] - 2.5f, m_afAmbushD[1] - 2.5f, m_afAmbushD[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            me->SummonCreature(MURK_BRUTE, m_afAmbushD[0] + 2.5f, m_afAmbushD[1] + 2.5f, m_afAmbushD[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            me->SummonCreature(MURK_RAIDER, m_afAmbushD[0] + 2.5f, m_afAmbushD[1] - 2.5f, m_afAmbushD[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            break;
		case 8:
			DoScriptText(SAY_KUR_MIDDLE, me);
			break;
        case 16:
            DoScriptText(SAY_KUR_COMPLETE, me);
            if (Player* pPlayer = GetPlayerForEscort())
                pPlayer->GroupEventHappens(QUEST_TOTEM_KARDASH_A, me);
            SetRun();
            break;
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == MURK_BRUTE)
            DoScriptText(SAY_KUR_NO_ESCAPE, pSummoned);

		if (pSummoned->GetEntry() == MURK_RAIDER)
			DoScriptText(SAY_KUR_NO_ESCAPE, pSummoned);

        if (pSummoned->IsTotem())
            return;

        pSummoned->SetWalk(false);
        pSummoned->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
        pSummoned->AI()->AttackStart(me);
    }

    void SpellHitTarget(Unit* /*pTarget*/, const SpellEntry* pSpell)
    {
        if (pSpell->Id == MAGIC_CHAIN_LIGHTNING)
        {
            if (rand() % 10)
                return;

            DoScriptText(SAY_KUR_LIGHTNING, me);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        npc_escortAI::UpdateAI(uiDiff);

        if (!me->GetVictim())
            return;

        if (m_uiChainLightningTimer <= uiDiff)
        {
            DoCastVictim( MAGIC_CHAIN_LIGHTNING);
            m_uiChainLightningTimer = urand(7000, 14000);
        }
        else m_uiChainLightningTimer -= uiDiff;

        if (me->GetHealth() * 100 < me->GetMaxHealth() * 30)
        {
            if (m_uiHealTimer <= uiDiff)
            {
                DoCast(me, MAGIC_HEALING_WAVE);
                m_uiHealTimer = 5000;
            }
            else m_uiHealTimer -= uiDiff;
        }

        if (m_uiFrostShockTimer <= uiDiff)
        {
            DoCastVictim( MAGIC_FROST_SHOCK);
            m_uiFrostShockTimer = urand(7500, 15000);
        }
        else m_uiFrostShockTimer -= uiDiff;
    }
};


CreatureAI* GetAI_npc_kurenai_captive(Creature* pCreature)
{
    return new npc_kurenai_captiveAI(pCreature);
}

bool QuestAccept_npc_kurenai_captive(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_TOTEM_KARDASH_A)
    {
        if (npc_kurenai_captiveAI* pEscortAI = dynamic_cast<npc_kurenai_captiveAI*>(pCreature->AI()))
        {
            pCreature->SetStandState(UNIT_STAND_STATE_STAND);
            pCreature->SetFaction(231);

            pEscortAI->Start(true, false, pPlayer->GetGUID(), pQuest);
            DoScriptText(SAY_KUR_START, pCreature);
			pCreature->HandleEmoteCommand(EMOTE_ONESHOT_SHOUT);

            pCreature->SummonCreature(MURK_RAIDER, m_afAmbushC[0] + 2.5f, m_afAmbushC[1] - 2.5f, m_afAmbushC[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            pCreature->SummonCreature(MURK_PUTRIFIER, m_afAmbushC[0] - 2.5f, m_afAmbushC[1] + 2.5f, m_afAmbushC[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            pCreature->SummonCreature(MURK_BRUTE, m_afAmbushC[0], m_afAmbushC[1], m_afAmbushC[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
        }
    }
    return true;
}

/*#####
## npc_warmaul_pyre Q 9932
#####*/

enum
{
	NPC_SABOTEUR = 18396,
	NPC_CORPSE = 18397,

	// Opening chat
	SAY_SABOTEUR1 = -1900192,
	SAY_SABOTEUR2 = -1900193,
	SAY_SABOTEUR3 = -1900194,
	SAY_SABOTEUR4 = -1900195,

	// Random phrases to say
	SAY_SABOTEUR5 = -1900196,
	SAY_SABOTEUR6 = -1900197,
	SAY_SABOTEUR7 = -1900198,
	SAY_SABOTEUR8 = -1900199,
	SAY_SABOTEUR9 = -1900200,
	SAY_SABOTEUR10 = -1900201,
	SAY_SABOTEUR11 = -1900202,

	// Quest complete phrase
	SAY_SABOTEUR_COMPLETE = -1900203
};

struct Move
{
	float x, y, z;
};

static Move Z[] =
{
	{ -885.76f, 7717.75f, 35.24f },
	{ -882.96f, 7723.00f, 34.78f },
	{ -871.40f, 7724.87f, 33.36f },
	{ -873.16F, 7727.59f, 33.35f },
	{ -855.66f, 7732.36f, 33.42f },
	{ -855.44f, 7735.44f, 33.44f },
	{ -843.39f, 7726.59f, 34.50f },
	{ -840.20f, 7728.34f, 34.39f },
	{ -848.31f, 7714.37f, 34.42f },
	{ -845.44f, 7710.70f, 35.05f },
	{ -859.99f, 7713.96f, 35.94f },
	{ -859.70f, 7710.61f, 36.68f },
	{ -873.74f, 7720.35f, 33.98f },
	{ -875.16f, 7717.15f, 34.39f }
};

struct npc_warmaul_pyreAI : public ScriptedAI
{
	npc_warmaul_pyreAI(Creature* creature) : ScriptedAI(creature) {}

	bool Event;

	std::list<Creature*> SaboteurList;
	uint64 PlayerGUID;
	uint32 StepsTimer;
	uint32 Steps;
	uint32 CorpseCount;
	uint32 MoveCount;

	void Reset()
	{
		Event = false;
		PlayerGUID = 0;
		StepsTimer = 0;
		Steps = 0;
		CorpseCount = 0;
		MoveCount = 1;
		me->SetVisible(false);
	}

	void EnterCombat(Unit *who){}

	void DoSpawn()
	{
		me->SummonCreature(NPC_SABOTEUR, Z[0].x, Z[0].y, Z[0].z, 0.6f, TEMPSUMMON_CORPSE_DESPAWN, 60000);
		me->SummonCreature(NPC_SABOTEUR, Z[1].x, Z[1].y, Z[1].z, 3.8f, TEMPSUMMON_CORPSE_DESPAWN, 60000);
	}

	void DoSummon()
	{
		++CorpseCount;

		uint32 Time = 100000 - (10000 * CorpseCount);

		if (Creature* Saboteur = GetSaboteur(2))
		{
			Saboteur->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
			me->SummonCreature(NPC_CORPSE, Saboteur->GetPositionX(), Saboteur->GetPositionY(), Saboteur->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, Time);
		}

		if (Creature* Saboteur = GetSaboteur(1))
		{
			Saboteur->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
			me->SummonCreature(NPC_CORPSE, Saboteur->GetPositionX(), Saboteur->GetPositionY(), Saboteur->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, Time);
		}
	}

	void Move()
	{
		++MoveCount;
		if (Creature* Saboteur = GetSaboteur(2))
			Saboteur->GetMotionMaster()->MovePoint(0, Z[MoveCount].x, Z[MoveCount].y, Z[MoveCount].z);

		++MoveCount;
		if (Creature* Saboteur = GetSaboteur(1))
			Saboteur->GetMotionMaster()->MovePoint(0, Z[MoveCount].x, Z[MoveCount].y, Z[MoveCount].z);
	}

	void JustSummoned(Creature* summoned)
	{
		if (summoned->GetEntry() == NPC_SABOTEUR)
			summoned->SetWalk(true);
	}

	void MoveInLineOfSight(Unit *who)
	{
		if (who->GetTypeId() == TYPEID_PLAYER)
		{
			Player* player = (Player*) who;
			if (player->GetQuestStatus(9932) == QUEST_STATUS_INCOMPLETE)
			{
				if (me->IsWithinDistInMap(player, 3.0f))
				{
					PlayerGUID = player->GetGUID();
					Event = true;                     // this is not the best way to start the event :)
				}
			}
		}
	}

	void Started()
	{
		SaboteurList.clear();

		me->GetCreatureListWithEntryInGrid(SaboteurList, NPC_SABOTEUR, 25.0f);
	}

	Creature* GetSaboteur(uint8 ListNum)
	{
		if (!SaboteurList.empty())
		{
			uint8 Num = 1;

			for (std::list<Creature*>::iterator itr = SaboteurList.begin(); itr != SaboteurList.end(); ++itr)
			{
				if (ListNum && ListNum != Num)
				{
					++Num;
					continue;
				}

				if ((*itr)->IsAlive() && (*itr)->IsWithinDistInMap(me, 25.0f))
					return (*itr);
			}
		}

		return NULL;
	}

	uint32 NextStep(uint32 Steps)
	{
		switch (Steps)
		{
		case 1:
			DoSpawn();
			return 4000;
		case 2:
			Started();
			return 3000;
		case 3:
			if (Creature* Saboteur = GetSaboteur(2))
				DoScriptText(SAY_SABOTEUR1, Saboteur);
			return 5000;
		case 4:
			if (Creature* Saboteur = GetSaboteur(1))
				DoScriptText(SAY_SABOTEUR2, Saboteur);
			return 5000;
		case 5:
			if (Creature* Saboteur = GetSaboteur(2))
				DoScriptText(SAY_SABOTEUR3, Saboteur);
			return 5000;
		case 6:
			if (Creature* Saboteur = GetSaboteur(1))
				DoScriptText(SAY_SABOTEUR4, Saboteur);
			return 4000;
		case 7:
			Move();
			return 6000;
		case 8:
			DoSummon();
			return 2000;
		case 9:
			if (Creature* Saboteur = GetSaboteur(1))
				DoScriptText(RAND(SAY_SABOTEUR5, SAY_SABOTEUR6, SAY_SABOTEUR7, SAY_SABOTEUR8, SAY_SABOTEUR9, SAY_SABOTEUR10, SAY_SABOTEUR11), Saboteur);
			if (Creature* Saboteur = GetSaboteur(2))
				DoScriptText(RAND(SAY_SABOTEUR5, SAY_SABOTEUR6, SAY_SABOTEUR7, SAY_SABOTEUR8, SAY_SABOTEUR9, SAY_SABOTEUR10, SAY_SABOTEUR11), Saboteur);
			return 2000;
		case 10:
			Move();
			return 7000;
		case 11:
			DoSummon();
			return 2000;
		case 12:
			if (Creature* Saboteur = GetSaboteur(2))
				DoScriptText(RAND(SAY_SABOTEUR5, SAY_SABOTEUR6, SAY_SABOTEUR7, SAY_SABOTEUR8, SAY_SABOTEUR9, SAY_SABOTEUR10, SAY_SABOTEUR11), Saboteur);
			if (Creature* Saboteur = GetSaboteur(1))
				DoScriptText(RAND(SAY_SABOTEUR5, SAY_SABOTEUR6, SAY_SABOTEUR7, SAY_SABOTEUR8, SAY_SABOTEUR9, SAY_SABOTEUR10, SAY_SABOTEUR11), Saboteur);
			return 2000;
		case 13:
			Move();
			return 7000;
		case 14:
			DoSummon();
			return 2000;
		case 15:
			if (Creature* Saboteur = GetSaboteur(2))
				DoScriptText(RAND(SAY_SABOTEUR5, SAY_SABOTEUR6, SAY_SABOTEUR7, SAY_SABOTEUR8, SAY_SABOTEUR9, SAY_SABOTEUR10, SAY_SABOTEUR11), Saboteur);
			if (Creature* Saboteur = GetSaboteur(1))
				DoScriptText(RAND(SAY_SABOTEUR5, SAY_SABOTEUR6, SAY_SABOTEUR7, SAY_SABOTEUR8, SAY_SABOTEUR9, SAY_SABOTEUR10, SAY_SABOTEUR11), Saboteur);
			return 2000;
		case 16:
			Move();
			return 7000;
		case 17:
			DoSummon();
			return 2000;
		case 18:
			if (Creature* Saboteur = GetSaboteur(2))
				DoScriptText(RAND(SAY_SABOTEUR5, SAY_SABOTEUR6, SAY_SABOTEUR7, SAY_SABOTEUR8, SAY_SABOTEUR9, SAY_SABOTEUR10, SAY_SABOTEUR11), Saboteur);
			if (Creature* Saboteur = GetSaboteur(1))
				DoScriptText(RAND(SAY_SABOTEUR5, SAY_SABOTEUR6, SAY_SABOTEUR7, SAY_SABOTEUR8, SAY_SABOTEUR9, SAY_SABOTEUR10, SAY_SABOTEUR11), Saboteur);
			return 2000;
		case 19:
			Move();
			return 7000;
		case 20:
			DoSummon();
			return 2000;
		case 21:
			if (Creature* Saboteur = GetSaboteur(2))
				DoScriptText(SAY_SABOTEUR_COMPLETE, Saboteur);
			return 2000;
		case 22:
			Move();
			return 7000;
		case 23:
			if (Player* player = Unit::GetPlayer(*me, PlayerGUID))
			{
				float Radius = 15.0f;
				if (me->IsWithinDistInMap(player, Radius))
					((Player*)player)->KilledMonsterCredit(18395, me->GetGUID());
			}
			return 2000;
        case 24:
            for (std::list<Creature*>::iterator itr = SaboteurList.begin(); itr != SaboteurList.end(); ++itr)
                (*itr)->DespawnOrUnsummon();
            Reset();
            break;
		default:
			return 0;
		}

        return 0;
	}

	void UpdateAI(const uint32 diff)
	{

		if (StepsTimer <= diff)
		{
			if (Event)
				StepsTimer = NextStep(++Steps);
		}
		else StepsTimer -= diff;
	}
};

CreatureAI* GetAI_npc_warmaul_pyre(Creature *creature)
{
	return new npc_warmaul_pyreAI(creature);
}

/*######
## npc_fel_cannon
######*/

enum
{
	NPC_CANNON_FEAR = 19210,
	NPC_FEAR_TARGET = 19211,
	NPC_CANNON_HATE = 19067,
	NPC_HATE_TARGET = 19212,

	SPELL_BOLT = 40109,
	SPELL_HATE = 33531,
	SPELL_FEAR = 33532,

	OBJECT_LARGE_FIRE = 187084,
};

struct npc_fel_cannon_hafAI : public ScriptedAI
{
	npc_fel_cannon_hafAI(Creature *creature) : ScriptedAI(creature) {}

	void Reset(){}

	void EnterCombat(Unit *who){}

	void SpellHit(Unit *caster, const SpellEntry *spell)
	{
		if (spell->Id == SPELL_HATE && me->GetEntry() == NPC_CANNON_HATE)
		{
			if (Creature* Target = GetClosestCreatureWithEntry(me, NPC_HATE_TARGET, 85.5f))
			{
				me->SetFacingToObject(Target);
				DoCast(Target, SPELL_BOLT);
				me->SummonGameObject(OBJECT_LARGE_FIRE, Target->GetPositionX(), Target->GetPositionY(), Target->GetPositionZ(), Target->GetOrientation(), 0, 0, 0, 0, 30);
			}
		}

		if (spell->Id == SPELL_FEAR && me->GetEntry() == NPC_CANNON_FEAR)
		{
			if (Creature* Target = GetClosestCreatureWithEntry(me, NPC_FEAR_TARGET, 85.5f))
			{
				me->SetFacingToObject(Target);
				DoCast(Target, SPELL_BOLT);
				me->SummonGameObject(OBJECT_LARGE_FIRE, Target->GetPositionX(), Target->GetPositionY(), Target->GetPositionZ(), Target->GetOrientation(), 0, 0, 0, 0, 30);
			}
		}
		return;
	}
};
CreatureAI* GetAI_npc_fel_cannon_haf(Creature *creature)
{
	return new npc_fel_cannon_hafAI(creature);
}

#define SPELL_LIGHTSHIELD	12550
#define SPELL_MAGNETIC_PULL 31705

struct npc_living_cycloneAI : public ScriptedAI
{
	npc_living_cycloneAI(Creature *creature) : ScriptedAI(creature) {}

	void Reset()
	{
		lightshield_timer = 20000;
		magpull_timer = 15000;

		DoCast(me, SPELL_LIGHTSHIELD);

		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);
	}

	uint32 lightshield_timer;
	uint32 magpull_timer;

	void EnterCombat(Unit* /*who*/) { }

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (lightshield_timer <= diff)
		{
			DoCast(me, SPELL_LIGHTSHIELD);
			lightshield_timer = 20000;
		}
		else lightshield_timer -= diff;

		if (magpull_timer <= diff)
		{
			DoCastVictim(SPELL_MAGNETIC_PULL);
			magpull_timer = 30000;
		}
		else magpull_timer -= diff;

		DoMeleeAttackIfReady();
	}
};
CreatureAI* GetAI_npc_living_cyclone(Creature *creature)
{
	return new npc_living_cycloneAI(creature);
}

#define SPELL_KNOCKDOWN 32015
#define SPELL_ENRAGED 8599

struct npc_enraged_crusherAI : public ScriptedAI
{
	npc_enraged_crusherAI(Creature *creature) : ScriptedAI(creature) {}

	void Reset()
	{
		knockdown_timer = 15000;

		enraged = false;

		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);
	}

	uint32 knockdown_timer;

	bool enraged;

	void EnterCombat(Unit* /*who*/) { }

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (knockdown_timer <= diff)
		{
			DoCastVictim(SPELL_KNOCKDOWN);
			knockdown_timer = 15000;
		}
		else knockdown_timer -= diff;

		if (!enraged)
		{
			if (HealthBelowPct(30))
			{
				DoCast(me, SPELL_ENRAGED);
				enraged = true;
			}
		}

		DoMeleeAttackIfReady();
	}
};
CreatureAI* GetAI_npc_enraged_crusher(Creature *creature)
{
	return new npc_enraged_crusherAI(creature);
}

#define SPELL_CALLLIGHTNING 32018

struct npc_storm_ragerAI : public ScriptedAI
{
	npc_storm_ragerAI(Creature *creature) : ScriptedAI(creature) {}

	void Reset()
	{		
		calllightning_timer = 10000;

		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);
	}

	uint32 calllightning_timer;

	void EnterCombat(Unit* /*who*/) { }

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (calllightning_timer <= diff)
		{
			DoCastVictim(SPELL_CALLLIGHTNING);
			calllightning_timer = 10000;
		}
		else calllightning_timer -= diff;

		DoMeleeAttackIfReady();
	}
};
CreatureAI* GetAI_npc_storm_rager(Creature *creature)
{
	return new npc_storm_ragerAI(creature);
}

struct npc_lake_surgerAI : public ScriptedAI
{
	npc_lake_surgerAI(Creature *creature) : ScriptedAI(creature) {}

	void Reset()
	{		
		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
	}

	void EnterCombat(Unit* /*who*/) { }

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		DoMeleeAttackIfReady();
	}
};
CreatureAI* GetAI_npc_lake_surger(Creature *creature)
{
	return new npc_lake_surgerAI(creature);
}

#define SPELL_WATERBOLT 34425

struct npc_lake_spiritAI : public ScriptedAI
{
	npc_lake_spiritAI(Creature *creature) : ScriptedAI(creature) {}

	void Reset()
	{
		waterbolt_timer = 6000;

		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
	}

	uint32 waterbolt_timer;

	void EnterCombat(Unit* /*who*/) { }

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (waterbolt_timer <= diff)
		{
			DoCastVictim(SPELL_WATERBOLT);
			waterbolt_timer = 7000;
		}
		else waterbolt_timer -= diff;

		DoMeleeAttackIfReady();
	}
};
CreatureAI* GetAI_npc_lake_spirit(Creature *creature)
{
	return new npc_lake_spiritAI(creature);
}

#define SPELL_EARTH_SCHOCKED 13728
#define SPELL_EARTH_ENRAGE 3019

struct npc_tortured_earth_spiritAI : public ScriptedAI
{
	npc_tortured_earth_spiritAI(Creature *creature) : ScriptedAI(creature) {}

	void Reset()
	{
		earthschock_timer = 15000;

		enrage = false;

		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);
	}

	uint32 earthschock_timer;

	bool enrage;

	void EnterCombat(Unit* /*who*/) { }

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (earthschock_timer <= diff)
		{
			DoCastVictim(SPELL_EARTH_SCHOCKED);
			earthschock_timer = 15000;
		}
		else earthschock_timer -= diff;

		if (!enrage)
		{
			if (HealthBelowPct(30))
			{
				DoCast(me, SPELL_EARTH_ENRAGE);
				enrage = true;
			}
		}

		DoMeleeAttackIfReady();
	}
};
CreatureAI* GetAI_npc_tortured_earth_spirit(Creature *creature)
{
	return new npc_tortured_earth_spiritAI(creature);
}

struct npc_crashing_wave_spiritAI : public ScriptedAI
{
	npc_crashing_wave_spiritAI(Creature *creature) : ScriptedAI(creature) {}

	void Reset()
	{
		waterbolt_timer = 6000;

		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
	}

	uint32 waterbolt_timer;

	void EnterCombat(Unit* /*who*/) { }

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (waterbolt_timer <= diff)
		{
			DoCastVictim(SPELL_WATERBOLT);
			waterbolt_timer = 7000;
		}
		else waterbolt_timer -= diff;

		DoMeleeAttackIfReady();
	}
};
CreatureAI* GetAI_npc_crashing_wave_spirit(Creature *creature)
{
	return new npc_crashing_wave_spiritAI(creature);
}

struct npc_raging_fire_soulAI : public ScriptedAI
{
	npc_raging_fire_soulAI(Creature *creature) : ScriptedAI(creature) {}

	void Reset()
	{	
		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);

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
	}

	void EnterCombat(Unit* /*who*/) { }

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		DoMeleeAttackIfReady();
	}
};
CreatureAI* GetAI_npc_raging_fire_soul(Creature *creature)
{
	return new npc_raging_fire_soulAI(creature);
}

#define SPELL_LIGHTBOLT 20295

struct npc_storming_wind_ripperAI : public ScriptedAI
{
	npc_storming_wind_ripperAI(Creature *creature) : ScriptedAI(creature) {}

	void Reset()
	{
		lightning_Shield_timer = 20000;
		light_bolt_timer = 6000;

		DoCast(me, SPELL_LIGHTSHIELD);

		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);
		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);

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
	}

	uint32 light_bolt_timer;
	uint32 lightning_Shield_timer;

	void EnterCombat(Unit* /*who*/) { }

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (lightning_Shield_timer <= diff)
		{
			DoCast(me, SPELL_LIGHTSHIELD);
			lightning_Shield_timer = 20000;
		}
		else lightning_Shield_timer -= diff;

		if (light_bolt_timer <= diff)
		{
			DoCastVictim(SPELL_LIGHTBOLT);
			light_bolt_timer = 6500;
		}
		else light_bolt_timer -= diff;

		DoMeleeAttackIfReady();
	}
};
CreatureAI* GetAI_npc_storming_wind_ripper(Creature *creature)
{
	return new npc_storming_wind_ripperAI(creature);
}

#define SPELL_EARTH_RUMBLE 33840
#define SPELL_HEART_ENRAGE 8599

struct npc_rumbling_earth_heartAI : public ScriptedAI
{
	npc_rumbling_earth_heartAI(Creature *creature) : ScriptedAI(creature) {}

	void Reset()
	{
		rumble_timer = 6000;

		enrage = false;

		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);
	}

	uint32 rumble_timer;

	bool enrage;

	void EnterCombat(Unit* /*who*/) { }

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (rumble_timer <= diff)
		{
			DoCast(SPELL_EARTH_RUMBLE);
			rumble_timer = 20000;
		}
		else rumble_timer -= diff;

		if (!enrage)
		{
			if (HealthBelowPct(40))
			{
				DoCast(me, SPELL_HEART_ENRAGE);
				enrage = true;
			}
		}

		DoMeleeAttackIfReady();
	}
};
CreatureAI* GetAI_npc_rumbling_earth_heart(Creature *creature)
{
	return new npc_rumbling_earth_heartAI(creature);
}

void AddSC_nagrand()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "mob_shattered_rumbler";
    newscript->GetAI = &GetAI_mob_shattered_rumbler;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_lump";
    newscript->GetAI = &GetAI_mob_lump;
    newscript->pGossipHello =  &GossipHello_mob_lump;
    newscript->pGossipSelect = &GossipSelect_mob_lump;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_sunspring_villager";
    newscript->GetAI = &GetAI_mob_sunspring_villager;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_altruis_the_sufferer";
    newscript->pGossipHello =  &GossipHello_npc_altruis_the_sufferer;
    newscript->pGossipSelect = &GossipSelect_npc_altruis_the_sufferer;
    newscript->pQuestAccept =  &QuestAccept_npc_altruis_the_sufferer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_greatmother_geyah";
    newscript->pGossipHello =  &GossipHello_npc_greatmother_geyah;
    newscript->pGossipSelect = &GossipSelect_npc_greatmother_geyah;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_lantresor_of_the_blade";
    newscript->pGossipHello =  &GossipHello_npc_lantresor_of_the_blade;
    newscript->pGossipSelect = &GossipSelect_npc_lantresor_of_the_blade;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_maghar_captive";
    newscript->GetAI = &GetAI_npc_maghar_captive;
    newscript->pQuestAccept = &QuestAccept_npc_maghar_captive;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_creditmarker_visit_with_ancestors";
    newscript->GetAI = &GetAI_npc_creditmarker_visit_with_ancestors;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_sparrowhawk";
    newscript->GetAI = &GetAI_mob_sparrowhawk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_maghar_prison";
    newscript->pGOHello =  &GOHello_maghar_prison;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_maghar_prisoner";
    newscript->GetAI = &GetAI_npc_maghar_prisoner;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_corkis_prison1";
    newscript->pGOHello =  &GOHello_corkis_prison1;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_corki1";
    newscript->GetAI = &GetAI_npc_corki1;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_corkis_prison2";
    newscript->pGOHello =  &GOHello_corkis_prison2;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_corki2";
    newscript->GetAI = &GetAI_npc_corki2;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_corkis_prison3";
    newscript->pGOHello =  &GOHello_corkis_prison3;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_corki3";
    newscript->GetAI = &GetAI_npc_corki3;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_kurenai_captive";
    newscript->GetAI = &GetAI_npc_kurenai_captive;
    newscript->pQuestAccept = &QuestAccept_npc_kurenai_captive;
    newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_warmaul_pyre";
	newscript->GetAI = &GetAI_npc_warmaul_pyre;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_fel_cannon_haf";
	newscript->GetAI = &GetAI_npc_fel_cannon_haf;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_living_cyclone";
	newscript->GetAI = &GetAI_npc_living_cyclone;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_enraged_crusher";
	newscript->GetAI = &GetAI_npc_enraged_crusher;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_storm_rager";
	newscript->GetAI = &GetAI_npc_storm_rager;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_lake_surger";
	newscript->GetAI = &GetAI_npc_lake_surger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_lake_spirit";
	newscript->GetAI = &GetAI_npc_lake_spirit;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_tortured_earth_spirit";
	newscript->GetAI = &GetAI_npc_tortured_earth_spirit;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_crashing_wave_spirit";
	newscript->GetAI = &GetAI_npc_crashing_wave_spirit;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_raging_fire_soul";
	newscript->GetAI = &GetAI_npc_raging_fire_soul;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_storming_wind_ripper";
	newscript->GetAI = &GetAI_npc_storming_wind_ripper;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_rumbling_earth_heart";
	newscript->GetAI = &GetAI_npc_rumbling_earth_heart;
	newscript->RegisterSelf();
}

