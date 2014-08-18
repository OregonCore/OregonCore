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
SDName: Zulaman
SD%Complete: 90
SDComment: Forest Frog will turn into different NPC's. Workaround to prevent new entry from running this script
SDCategory: Zul'Aman
EndScriptData */

/* ContentData
npc_forest_frog
EndContentData */

#include "ScriptPCH.h"
#include "zulaman.h"

/*######
## npc_forest_frog
######*/

enum //Misc
{
    SPELL_REMOVE_AMANI_CURSE   = 43732,
    SPELL_PUSH_MOJO            = 43923,
    ENTRY_FOREST_FROG          = 24396
};

struct npc_forest_frogAI : public ScriptedAI
{
    npc_forest_frogAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance *pInstance;

    void Reset() {}

    void EnterCombat(Unit * /*who*/) {}

    void DoSpawnRandom()
    {
        if (pInstance)
        {
            uint32 cEntry = 0;
            switch(rand()%10)
            {
                case 0: cEntry = 24397; break;          //Mannuth
                case 1: cEntry = 24403; break;          //Deez
                case 2: cEntry = 24404; break;          //Galathryn
                case 3: cEntry = 24405; break;          //Adarrah
                case 4: cEntry = 24406; break;          //Fudgerick
                case 5: cEntry = 24407; break;          //Darwen
                case 6: cEntry = 24445; break;          //Mitzi
                case 7: cEntry = 24448; break;          //Christian
                case 8: cEntry = 24453; break;          //Brennan
                case 9: cEntry = 24455; break;          //Hollee
            }

            if (!pInstance->GetData(ENCOUNTER_RAND_VENDOR_1))
                if (rand()%10 == 1) cEntry = 24408;      //Gunter
            if (!pInstance->GetData(ENCOUNTER_RAND_VENDOR_2))
                if (rand()%10 == 1) cEntry = 24409;      //Kyren

            if (cEntry) me->UpdateEntry(cEntry);

            if (cEntry == 24408) pInstance->SetData(ENCOUNTER_RAND_VENDOR_1,DONE);
            if (cEntry == 24409) pInstance->SetData(ENCOUNTER_RAND_VENDOR_2,DONE);
        }
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (spell->Id == SPELL_REMOVE_AMANI_CURSE && caster->GetTypeId() == TYPEID_PLAYER && me->GetEntry() == ENTRY_FOREST_FROG)
        {
            //increase or decrease chance of mojo?
            if (rand()%99 == 50) DoCast(caster, SPELL_PUSH_MOJO, true);
            else DoSpawnRandom();
        }
    }
};
CreatureAI* GetAI_npc_forest_frog(Creature* pCreature)
{
    return new npc_forest_frogAI (pCreature);
}

/*######
## npc_zulaman_hostage
######*/

static char GOSSIP_HOSTAGE1[] = "I am glad to have helped you.";

static uint32 HostageEntry[] = {23790, 23999, 24001, 24024};
static uint32 ChestEntry[] = {186648, 187021, 186672, 186667};

struct npc_zulaman_hostageAI : public ScriptedAI
{
    npc_zulaman_hostageAI(Creature *c) : ScriptedAI(c) {IsLoot = false;}
    bool IsLoot;
    uint64 PlayerGUID;
    void Reset() {}
    void EnterCombat(Unit * /*who*/) {}
    void JustDied(Unit* /*who*/)
    {
        Player* pPlayer = Unit::GetPlayer(*me, PlayerGUID);
        if (pPlayer) pPlayer->SendLoot(me->GetGUID(), LOOT_CORPSE);
    }
    void UpdateAI(const uint32 /*diff*/)
    {
        if (IsLoot)
            DoCast(me, 7, false);
    }
};

bool GossipHello_npc_zulaman_hostage(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HOSTAGE1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_zulaman_hostage(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
        pPlayer->CLOSE_GOSSIP_MENU();

    if (!pCreature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
        return true;
    pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

    ScriptedInstance* pInstance = pCreature->GetInstanceData();
    if (pInstance)
    {
        uint8 progress = pInstance->GetData(ENCOUNTER_CHESTLOOTED);
        pInstance->SetData(ENCOUNTER_CHESTLOOTED, progress + 1);
        float x, y, z;
        pCreature->GetPosition(x, y, z);
        uint32 entry = pCreature->GetEntry();
        for (uint8 i = 0; i < 4; ++i)
        {
            if (HostageEntry[i] == entry)
            {
                pCreature->SummonGameObject(ChestEntry[i], x-2, y, z, 0, 0, 0, 0, 0, 0);
                break;
            }
        }
        /*Creature* summon = pCreature->SummonCreature(HostageInfo[progress], x-2, y, z, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
        if (summon)
        {
            CAST_AI(npc_zulaman_hostageAI, summon->AI())->PlayerGUID = pPlayer->GetGUID();
            CAST_AI(npc_zulaman_hostageAI, summon->AI())->IsLoot = true;
            summon->SetDisplayId(10056);
            summon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            summon->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }*/
    }
    return true;
}

CreatureAI* GetAI_npc_zulaman_hostage(Creature* pCreature)
{
    return new npc_zulaman_hostageAI(pCreature);
}

/* npc_harrison_jones */

static char GOSSIP_HARRISON[] = "Tooltip Missing";
static char SAY_HARRISON_UNK0[] = "Puzzling. They're clearly harnessing great power from their sacrifices. But for what?";
static char YELL_HARRISON_FOLLOWME[] = "Suit yourself. At least five of you must assist me if we're to get inside. Follow me..."; 
static char YELL_HARRISON_UNK1[] = "According to my calculations, if enough of us bang the gong at once the seal on these doors will break and we can enter.";
static char YELL_HARRISON_UNK2[] = "I've researched this site extensively and I won't allow any dim-witted treasure hunters to swoop in and steal what belongs in a museum. I'll lead this charge.";
static char YELL_HARRISON_UNK3[] = "In fact, it would be best if you just stay here. You'd only get in my way....";
static float HarrisonsWay[3] = { 131.81f, 1642.91f, 42.021595f };

struct npc_harrison_jonesAI : public ScriptedAI
{
    npc_harrison_jonesAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        instance = pCreature->GetInstanceData();
        handleGossips = !!instance;

        if (handleGossips)
        {
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            me->SetSpeed(MOVE_RUN, 1);
            //me->RemoveUnitMovementFlag(MOVEFLAG_WALK_MODE);
        }
    }

    bool GossipHello(Player* player)
    {
        if (!handleGossips)
            return false;

        Group* group = player->GetGroup();

        if (!group || group->IsLeader(player->GetGUID()) || player->isGameMaster())
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HARRISON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(me), me->GetGUID());
        }
        return true;
    }

    bool GossipSelect()
    {
        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        me->GetMotionMaster()->MovePoint(0, HarrisonsWay[0], HarrisonsWay[1], HarrisonsWay[2]);
        me->MonsterYell(YELL_HARRISON_FOLLOWME, LANG_UNIVERSAL, 0);
        return true;
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE || id != 0)
            return;

        me->Say("Gong Event is not implemented, yet. Skipping the event...", LANG_UNIVERSAL, 0);
        instance->SetData(ENCOUNTER_GONG, DONE);
    }

    void Update(uint32 /*diff*/)
    {
    }

    ScriptedInstance* instance;
    bool handleGossips;
};

static bool GossipHello_npc_harrison_jones(Player* pPlayer, Creature* pCreature)
{
    if (npc_harrison_jonesAI* ai = dynamic_cast <npc_harrison_jonesAI*> (pCreature->AI()))
        return ai->GossipHello(pPlayer);

    return false;
}

static bool GossipSelect_npc_harrison_jones(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (npc_harrison_jonesAI* ai = dynamic_cast <npc_harrison_jonesAI*> (pCreature->AI()))
        return ai->GossipSelect();

    return false;
}

CreatureAI* GetAI_npc_harrison_jones(Creature* pCreature)
{
    return new npc_harrison_jonesAI(pCreature);
}

void AddSC_zulaman()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_forest_frog";
    newscript->GetAI = &GetAI_npc_forest_frog;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_zulaman_hostage";
    newscript->GetAI = &GetAI_npc_zulaman_hostage;
    newscript->pGossipHello = &GossipHello_npc_zulaman_hostage;
    newscript->pGossipSelect = &GossipSelect_npc_zulaman_hostage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_harrison_jones";
    newscript->GetAI = &GetAI_npc_harrison_jones;
    newscript->pGossipHello = &GossipHello_npc_harrison_jones;
    newscript->pGossipSelect = &GossipSelect_npc_harrison_jones;
    newscript->RegisterSelf();
}

