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
SDName: Shattrath_City
SD%Complete: 100
SDComment: Quest support: 10004, 10009, 10211, 10231. Flask vendors, Teleport to Caverns of Time
SDCategory: Shattrath City
EndScriptData */

/* ContentData
npc_raliq_the_drunk
npc_salsalabim
npc_shattrathflaskvendors
npc_zephyr
npc_kservant
npc_dirty_larry
npc_ishanah
npc_khadgar
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"

/*######
## npc_raliq_the_drunk
######*/

#define GOSSIP_RALIQ            "I have been sent by Sal'salabim to collect a debt that you owe. Pay up or I'm going to hurt you."

#define SPELL_UPPERCUT          10966

#define SAY_NOWAY				-1910147

struct npc_raliq_the_drunkAI : public ScriptedAI
{
    npc_raliq_the_drunkAI(Creature* c) : ScriptedAI(c) {}

    uint32 Uppercut_Timer;

    void Reset()
    {
        Uppercut_Timer = 5000;
        me->setFaction(35);
    }

    void EnterCombat(Unit* /*who*/) {}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Uppercut_Timer <= diff)
        {
            DoCastVictim(SPELL_UPPERCUT);
            Uppercut_Timer = 15000;
        }
        else Uppercut_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_raliq_the_drunk(Creature* pCreature)
{
    return new npc_raliq_the_drunkAI (pCreature);
}

bool GossipHello_npc_raliq_the_drunk(Player* player, Creature* pCreature)
{
    if (player->GetQuestStatus(10009) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_RALIQ, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    player->SEND_GOSSIP_MENU(9440, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_raliq_the_drunk(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        player->CLOSE_GOSSIP_MENU();
		DoScriptText(SAY_NOWAY, pCreature);
        pCreature->setFaction(45);
        ((npc_raliq_the_drunkAI*)pCreature->AI())->AttackStart(player);
    }
    return true;
}

/*######
# npc_salsalabim
######*/

#define FACTION_HOSTILE_SA              90
#define FACTION_FRIENDLY_SA             35
#define QUEST_10004                     10004

#define SPELL_MAGNETIC_PULL             31705
#define SAY_DEMONIC						-1910146

#define GOSSIP_SALADIM "Altruis sent me. He said that you could help me."

struct npc_salsalabimAI : public ScriptedAI
{
    npc_salsalabimAI(Creature* c) : ScriptedAI(c) {}

    uint32 MagneticPull_Timer;

    void Reset()
    {
        MagneticPull_Timer = 15000;
        me->setFaction(FACTION_FRIENDLY_SA);
    }

    void EnterCombat(Unit* /*who*/) {}

    void DamageTaken(Unit* done_by, uint32& damage)
    {
        if (done_by->GetTypeId() == TYPEID_PLAYER)
            if ((me->GetHealth() - damage) * 100 / me->GetMaxHealth() < 20)
            {
                CAST_PLR(done_by)->GroupEventHappens(QUEST_10004, me);
                damage = 0;
                EnterEvadeMode();
            }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (MagneticPull_Timer <= diff)
        {
            DoCastVictim(SPELL_MAGNETIC_PULL);
            MagneticPull_Timer = 15000;
        }
        else MagneticPull_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_npc_salsalabim(Creature* pCreature)
{
    return new npc_salsalabimAI (pCreature);
}

bool GossipHello_npc_salsalabim(Player* player, Creature* pCreature)
{
	if (pCreature->isQuestGiver())
		player->PrepareQuestMenu(pCreature->GetGUID());

	if (player->GetQuestStatus(QUEST_10004) == QUEST_STATUS_INCOMPLETE)	
		player->ADD_GOSSIP_ITEM(0, GOSSIP_SALADIM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
	
		player->SEND_GOSSIP_MENU(9435, pCreature->GetGUID());
	
	return true;
}

bool GossipSelect_npc_salsalabim(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
	if (action == GOSSIP_ACTION_INFO_DEF + 1)
	{
		player->CLOSE_GOSSIP_MENU();
		DoScriptText(SAY_DEMONIC, pCreature);
		pCreature->setFaction(FACTION_HOSTILE_SA);
		((npc_salsalabimAI*)pCreature->AI())->AttackStart(player);
	}
	return true;
}

/*
##################################################
Shattrath City Flask Vendors provides flasks to people exalted with 3 factions:
Haldor the Compulsive
Arcanist Xorith
Both sell special flasks for use in Outlands 25man raids only,
purchasable for one Mark of Illidari each
Purchase requires exalted reputation with Scryers/Aldor, Cenarion Expedition and The Sha'tar
##################################################
*/

bool GossipHello_npc_shattrathflaskvendors(Player* player, Creature* pCreature)
{
    if (pCreature->GetEntry() == 23484)
    {
        // Aldor vendor
        if (pCreature->isVendor() && (player->GetReputationRank(932) == REP_EXALTED) && (player->GetReputationRank(935) == REP_EXALTED) && (player->GetReputationRank(942) == REP_EXALTED))
        {
            player->ADD_GOSSIP_ITEM(1, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
            player->SEND_GOSSIP_MENU(11085, pCreature->GetGUID());
        }
        else
            player->SEND_GOSSIP_MENU(11083, pCreature->GetGUID());
    }

    if (pCreature->GetEntry() == 23483)
    {
        // Scryers vendor
        if (pCreature->isVendor() && (player->GetReputationRank(934) == REP_EXALTED) && (player->GetReputationRank(935) == REP_EXALTED) && (player->GetReputationRank(942) == REP_EXALTED))
        {
            player->ADD_GOSSIP_ITEM(1, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
            player->SEND_GOSSIP_MENU(11085, pCreature->GetGUID());
        }
        else
            player->SEND_GOSSIP_MENU(11084, pCreature->GetGUID());
    }

    return true;
}

bool GossipSelect_npc_shattrathflaskvendors(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_TRADE)
        player->SEND_VENDORLIST(pCreature->GetGUID());

    return true;
}

/*######
# npc_zephyr
######*/

#define GOSSIP_HZ "Take me to the Caverns of Time."

bool GossipHello_npc_zephyr(Player* player, Creature* pCreature)
{
    if (player->GetReputationRank(989) >= REP_REVERED)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HZ, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_zephyr(Player* player, Creature* /*pCreature*/, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
        player->CastSpell(player, 37778, false);

    return true;
}


/*######
## npc_khadgars_servant
######*/

enum eServant
{
    SAY_KHAD_START          = -1000489,
    SAY_KHAD_SERV_0         = -1000234,
    SAY_KHAD_SERV_1         = -1000235,
    SAY_KHAD_SERV_2         = -1000236,
    SAY_KHAD_SERV_3         = -1000237,
    SAY_KHAD_SERV_4         = -1000238,
    SAY_KHAD_SERV_5         = -1000239,
    SAY_KHAD_SERV_6         = -1000240,
    SAY_KHAD_SERV_7         = -1000241,
    SAY_KHAD_SERV_8         = -1000242,
    SAY_KHAD_SERV_9         = -1000243,
    SAY_KHAD_SERV_10        = -1000244,
    SAY_KHAD_SERV_11        = -1000245,
    SAY_KHAD_SERV_12        = -1000246,
    SAY_KHAD_SERV_13        = -1000247,
    SAY_KHAD_SERV_14        = -1000248,
    SAY_KHAD_SERV_15        = -1000249,
    SAY_KHAD_SERV_16        = -1000250,
    SAY_KHAD_SERV_17        = -1000251,
    SAY_KHAD_SERV_18        = -1000252,
    SAY_KHAD_SERV_19        = -1000253,
    SAY_KHAD_SERV_20        = -1000254,
    SAY_KHAD_SERV_21        = -1000255,
    SAY_KHAD_INJURED        = -1000490,
    SAY_KHAD_MIND_YOU       = -1000491,
    SAY_KHAD_MIND_ALWAYS    = -1000492,
    SAY_KHAD_ALDOR_GREET    = -1000493,
    SAY_KHAD_SCRYER_GREET   = -1000494,
    SAY_KHAD_HAGGARD        = -1000495,
    NPC_KHADGAR             = 18166,
    NPC_SHANIR              = 18597,
    NPC_IZZARD              = 18622,
    NPC_ADYRIA              = 18596,
    NPC_ANCHORITE           = 19142,
    NPC_ARCANIST            = 18547,
    NPC_HAGGARD             = 19684,
    QUEST_CITY_LIGHT        = 10211
};

struct npc_khadgars_servantAI : public npc_escortAI
{
    npc_khadgars_servantAI(Creature* pCreature) : npc_escortAI(pCreature) {}

    uint32 m_uiPointId;
    uint32 m_uiTalkTimer;
    uint32 m_uiTalkCount;
    uint32 m_uiRandomTalkCooldown;

    void Reset()
    {
        me->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
        m_uiTalkTimer = 2500;
        m_uiTalkCount = 0;
        m_uiPointId = 0;
        m_uiRandomTalkCooldown = 0;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_uiRandomTalkCooldown && pWho->GetTypeId() == TYPEID_UNIT && me->IsWithinDistInMap(pWho, 10.0f))
        {
            switch (pWho->GetEntry())
            {
            case NPC_HAGGARD:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_KHAD_HAGGARD, pWho, pPlayer);
                m_uiRandomTalkCooldown = 7500;
                break;
            case NPC_ANCHORITE:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_KHAD_ALDOR_GREET, pWho, pPlayer);
                m_uiRandomTalkCooldown = 7500;
                break;
            case NPC_ARCANIST:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_KHAD_SCRYER_GREET, pWho, pPlayer);
                m_uiRandomTalkCooldown = 7500;
                break;
            }
        }

        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        if (pWho->GetTypeId() == TYPEID_PLAYER)
        {
            if (CAST_PLR(pWho)->GetQuestStatus(10211) == QUEST_STATUS_INCOMPLETE)
            {
                float Radius = 10.0f;
                if (me->IsWithinDistInMap(pWho, Radius))
                    Start(false, false, pWho->GetGUID());
            }
        }
    }

    void WaypointStart(uint32 uiPointId)
    {
        if (uiPointId == 2)
            DoScriptText(SAY_KHAD_SERV_0, me);
    }

    void WaypointReached(uint32 uiPointId)
    {
        m_uiPointId = uiPointId;

        switch (uiPointId)
        {
        case 0:
            if (Creature* pKhadgar = me->FindNearestCreature(NPC_KHADGAR, 10.0f))
                DoScriptText(SAY_KHAD_START, pKhadgar);
            break;
        case 5:
        case 24:
        case 50:
        case 63:
        case 74:
        case 75:
            SetEscortPaused(true);
            break;
        case 34:
            if (Creature* pIzzard = me->FindNearestCreature(NPC_IZZARD, 10.0f))
                DoScriptText(SAY_KHAD_MIND_YOU, pIzzard);
            break;
        case 35:
            if (Creature* pAdyria = me->FindNearestCreature(NPC_ADYRIA, 10.0f))
                DoScriptText(SAY_KHAD_MIND_ALWAYS, pAdyria);
            break;
        }
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (m_uiRandomTalkCooldown)
        {
            if (m_uiRandomTalkCooldown <= uiDiff)
                m_uiRandomTalkCooldown = 0;
            else
                m_uiRandomTalkCooldown -= uiDiff;
        }

        if (HasEscortState(STATE_ESCORT_PAUSED))
        {
            Player* pPlayer = GetPlayerForEscort();
            if (!pPlayer)
                return;

            if (m_uiTalkTimer <= uiDiff)
            {
                m_uiTalkTimer = 7500;

                switch (m_uiPointId)
                {
                case 5:                                 //to lower city
                    {
                        switch (m_uiTalkCount)
                        {
                        case 1:
                            DoScriptText(SAY_KHAD_SERV_1, me, pPlayer);
                            break;
                        case 2:
                            DoScriptText(SAY_KHAD_SERV_2, me, pPlayer);
                            break;
                        case 3:
                            DoScriptText(SAY_KHAD_SERV_3, me, pPlayer);
                            break;
                        case 4:
                            DoScriptText(SAY_KHAD_SERV_4, me, pPlayer);
                            SetEscortPaused(false);
                            break;
                        }
                        break;
                    }
                case 24:                                //in lower city
                    {
                        switch (m_uiTalkCount)
                        {
                        case 5:
                            if (Creature* pShanir = me->FindNearestCreature(NPC_SHANIR, 10.0f))
                                DoScriptText(SAY_KHAD_INJURED, pShanir, pPlayer);

                            DoScriptText(SAY_KHAD_SERV_5, me, pPlayer);
                            break;
                        case 6:
                            DoScriptText(SAY_KHAD_SERV_6, me, pPlayer);
                            break;
                        case 7:
                            DoScriptText(SAY_KHAD_SERV_7, me, pPlayer);
                            SetEscortPaused(false);
                            break;
                        }
                        break;
                    }
                case 50:                                //outside
                    {
                        switch (m_uiTalkCount)
                        {
                        case 8:
                            DoScriptText(SAY_KHAD_SERV_8, me, pPlayer);
                            break;
                        case 9:
                            DoScriptText(SAY_KHAD_SERV_9, me, pPlayer);
                            break;
                        case 10:
                            DoScriptText(SAY_KHAD_SERV_10, me, pPlayer);
                            break;
                        case 11:
                            DoScriptText(SAY_KHAD_SERV_11, me, pPlayer);
                            SetEscortPaused(false);
                            break;
                        }
                        break;
                    }
                case 63:                                //scryer
                    {
                        switch (m_uiTalkCount)
                        {
                        case 12:
                            DoScriptText(SAY_KHAD_SERV_12, me, pPlayer);
                            break;
                        case 13:
                            DoScriptText(SAY_KHAD_SERV_13, me, pPlayer);
                            SetEscortPaused(false);
                            break;
                        }
                        break;
                    }
                case 74:                                //aldor
                    {
                        switch (m_uiTalkCount)
                        {
                        case 14:
                            DoScriptText(SAY_KHAD_SERV_14, me, pPlayer);
                            break;
                        case 15:
                            DoScriptText(SAY_KHAD_SERV_15, me, pPlayer);
                            break;
                        case 16:
                            DoScriptText(SAY_KHAD_SERV_16, me, pPlayer);
                            break;
                        case 17:
                            DoScriptText(SAY_KHAD_SERV_17, me, pPlayer);
                            SetEscortPaused(false);
                            break;
                        }
                        break;
                    }
                case 75:                                //a'dal
                    {
                        switch (m_uiTalkCount)
                        {
                        case 18:
                            DoScriptText(SAY_KHAD_SERV_18, me, pPlayer);
                            break;
                        case 19:
                            DoScriptText(SAY_KHAD_SERV_19, me, pPlayer);
                            break;
                        case 20:
                            DoScriptText(SAY_KHAD_SERV_20, me, pPlayer);
                            break;
                        case 21:
                            DoScriptText(SAY_KHAD_SERV_21, me, pPlayer);
                            pPlayer->AreaExploredOrEventHappens(QUEST_CITY_LIGHT);
                            SetEscortPaused(false);
                            break;
                        }
                        break;
                    }
                }
                ++m_uiTalkCount;
            }
            else
                m_uiTalkTimer -= uiDiff;
        }
        return;
    }
};

CreatureAI* GetAI_npc_khadgars_servant(Creature* pCreature)
{
    return new npc_khadgars_servantAI(pCreature);
}

/*######
# npc_dirty_larry
######*/

#define GOSSIP_BOOK "Ezekiel said that you might have a certain book..."

enum eLarry
{
    SAY_1       = -1000274,
    SAY_2       = -1000275,
    SAY_3       = -1000276,
    SAY_4       = -1000277,
    SAY_5       = -1000278,
    SAY_GIVEUP  = -1000279,

    QUEST_WBI       = 10231,
    NPC_CREEPJACK   = 19726,
    NPC_MALONE      = 19725
};

struct npc_dirty_larryAI : public ScriptedAI
{
    npc_dirty_larryAI(Creature* c) : ScriptedAI(c) {}

    bool Event;
    bool Attack;
    bool Done;

    uint64 PlayerGUID;

    uint32 SayTimer;
    uint32 Step;

    void Reset()
    {
        Event = false;
        Attack = false;
        Done = false;

        PlayerGUID = 0;
        SayTimer = 0;
        Step = 0;

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->setFaction(1194);
        Unit* Creepjack = me->FindNearestCreature(NPC_CREEPJACK, 20);
        if (Creepjack)
        {
            CAST_CRE(Creepjack)->AI()->EnterEvadeMode();
            Creepjack->setFaction(1194);
            Creepjack->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }
        Unit* Malone = me->FindNearestCreature(NPC_MALONE, 20);
        if (Malone)
        {
            CAST_CRE(Malone)->AI()->EnterEvadeMode();
            Malone->setFaction(1194);
            Malone->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }
    }

    uint32 NextStep(uint32 Step)
    {
        Player* pPlayer = Unit::GetPlayer(*me, PlayerGUID);

        switch (Step)
        {
        case 0:
            {
                me->SetInFront(pPlayer);
                Unit* Creepjack = me->FindNearestCreature(NPC_CREEPJACK, 20);
                if (Creepjack)
                    Creepjack->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                Unit* Malone = me->FindNearestCreature(NPC_MALONE, 20);
                if (Malone)
                    Malone->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
            return 2000;
        case 1:
            DoScriptText(SAY_1, me, pPlayer);
            return 3000;
        case 2:
            DoScriptText(SAY_2, me, pPlayer);
            return 5000;
        case 3:
            DoScriptText(SAY_3, me, pPlayer);
            return 2000;
        case 4:
            DoScriptText(SAY_4, me, pPlayer);
            return 2000;
        case 5:
            DoScriptText(SAY_5, me, pPlayer);
            return 2000;
        case 6:
            Attack = true;
            return 2000;
        default:
            return 0;
        }
    }

    void EnterCombat(Unit* /*who*/) {}

    void UpdateAI(const uint32 diff)
    {
        if (SayTimer <= diff)
        {
            if (Event)
                SayTimer = NextStep(++Step);
        }
        else SayTimer -= diff;

        if (Attack)
        {
            Player* pPlayer = Unit::GetPlayer(*me, PlayerGUID);
            me->setFaction(14);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            if (pPlayer)
            {
                Unit* Creepjack = me->FindNearestCreature(NPC_CREEPJACK, 20);
                if (Creepjack)
                {
                    Creepjack->Attack(pPlayer, true);
                    Creepjack->setFaction(14);
                    Creepjack->GetMotionMaster()->MoveChase(pPlayer);
                    Creepjack->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                }
                Unit* Malone = me->FindNearestCreature(NPC_MALONE, 20);
                if (Malone)
                {
                    Malone->Attack(pPlayer, true);
                    Malone->setFaction(14);
                    Malone->GetMotionMaster()->MoveChase(pPlayer);
                    Malone->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                }
                DoStartMovement(pPlayer);
                AttackStart(pPlayer);
            }
            Attack = false;
        }

        if ((me->GetHealth() * 100) / me->GetMaxHealth() < 15 && !Done)
        {
            Unit* Creepjack = me->FindNearestCreature(NPC_CREEPJACK, 20);
            if (Creepjack)
            {
                CAST_CRE(Creepjack)->AI()->EnterEvadeMode();
                Creepjack->setFaction(1194);
                Creepjack->GetMotionMaster()->MoveTargetedHome();
                Creepjack->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }
            Unit* Malone = me->FindNearestCreature(NPC_MALONE, 20);
            if (Malone)
            {
                CAST_CRE(Malone)->AI()->EnterEvadeMode();
                Malone->setFaction(1194);
                Malone->GetMotionMaster()->MoveTargetedHome();
                Malone->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->setFaction(1194);
            Done = true;
            DoScriptText(SAY_GIVEUP, me, NULL);
            me->DeleteThreatList();
            me->CombatStop();
            me->GetMotionMaster()->MoveTargetedHome();
            Player* player = Unit::GetPlayer(*me, PlayerGUID);
            if (player)
                player->GroupEventHappens(QUEST_WBI, me);
        }
        DoMeleeAttackIfReady();
    }
};

bool GossipHello_npc_dirty_larry(Player* player, Creature* creature)
{
    if (creature->isQuestGiver())
        player->PrepareQuestMenu(creature->GetGUID());

    if (player->GetQuestStatus(QUEST_WBI) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_BOOK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
    return true;
}

bool GossipSelect_npc_dirty_larry(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        ((npc_dirty_larryAI*)creature->AI())->Event = true;
        ((npc_dirty_larryAI*)creature->AI())->PlayerGUID = player->GetGUID();
        player->CLOSE_GOSSIP_MENU();
    }

    return true;
}

CreatureAI* GetAI_npc_dirty_larryAI(Creature* pCreature)
{
    return new npc_dirty_larryAI (pCreature);
}

/*######
# npc_ishanah
######*/

#define ISANAH_GOSSIP_1 "Who are the Sha'tar?"
#define ISANAH_GOSSIP_2 "Isn't Shattrath a draenei city? Why do you allow others here?"

#define KAYLAAN_SAY_1 "Teacher..."
#define ISHANAH_SAY_1 "The Light wants its son back, Socrethar."
#define ISHANAH_SAY_2 "I offer myself in exchange. You will have Ishanah the High Priestess of the Aldor, as your prisoner if you release Kaylaan from your dark grasp."
#define SOCRETHAR_YELL "You foolish old hag... Why would I do that when I can have you both?"
#define KAYLAAN_SAY_2 "No! What have I done?"
#define KAYLAAN_SAY_3 "Light grant me strenght!"
#define SOCRETHAR_YELL_1 "Oh, please! This is sickening I'm going to have to kill you all myself!"

#define SPELL_HOLY_SMITE 15238
#define SPELL_PWS 22187

struct npc_ishanahAI : public ScriptedAI
{
	npc_ishanahAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->SetReactState(REACT_AGGRESSIVE);

		// Event
		spellHit = false;
		spellHit1 = false;
		EventBegun = false;
		speak1 = false;
		speak2 = false;
		speak3 = false;
		speak4 = false;
		speak5 = false;
		speak6 = false;
		speak7 = false;
		combatStart = false;

		// Skills
		smite_timer = 6600;
		pws_timer = 15000;
	}

	// Event
	bool spellHit;
	bool spellHit1;
	bool EventBegun;
	bool speak1;
	bool speak2;
	bool speak3;
	bool speak4;
	bool speak5;
	bool speak6;
	bool speak7;
	bool combatStart;

	uint32 continue_timer;
	uint32 continue_timer1;
	uint32 continue_timer2;
	uint32 continue_timer3;
	uint32 continue_timer4;
	uint32 continue_timer5;
	uint32 continue_timer6;
	uint32 continue_timer7;

	// Skills
	uint32 smite_timer;
	uint32 pws_timer;

	void SpellHit(Unit* Hitter, const SpellEntry* Spellkind)
	{
		if (Spellkind->Id == 35600 && !spellHit)
		{
			me->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
			me->SetStandState(UNIT_STAND_STATE_DEAD);
			spellHit = true;
			continue_timer3 = 6500;
		}

		if (Spellkind->Id == 35599 && !spellHit1)
		{
			me->SetStandState(UNIT_STAND_STATE_STAND);
			me->SetUInt32Value(UNIT_DYNAMIC_FLAGS, 0);
			spellHit1 = true;
			continue_timer3 = 6500;
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (!UpdateVictim())
		{
			if (!EventBegun)
			{
				if (Creature* socrethar = me->FindNearestCreature(20132, 45.0f, true))
				{
					if (Creature* kaylaan = me->FindNearestCreature(20794, 20.0f, true))
					{				
						kaylaan->SetStandState(UNIT_STAND_STATE_KNEEL);
						kaylaan->MonsterSay(KAYLAAN_SAY_1, LANG_UNIVERSAL, 0);

						continue_timer = 6000;
						EventBegun = true;
					}				
				}				
			}

			if (EventBegun == true && !speak1)
			{
				if (continue_timer <= diff)
				{
					me->MonsterSay(ISHANAH_SAY_1, LANG_UNIVERSAL, 0);
					speak1 = true;
					continue_timer1 = 7000;
				}
				else continue_timer -= diff;
			}

			if (speak1 == true && !speak2)
			{
				if (continue_timer1 <= diff)
				{
					me->MonsterSay(ISHANAH_SAY_2, LANG_UNIVERSAL, 0);
					speak2 = true;
					continue_timer2 = 7000;
				}
				else continue_timer1 -= diff;
			}

			if (speak2 == true && !speak3)
			{
				if (continue_timer2 <= diff)
				{
					if (Creature* socrethar = me->FindNearestCreature(20132, 15.0f, true))						
					{
						socrethar->MonsterYell(SOCRETHAR_YELL, LANG_UNIVERSAL, 0);
						socrethar->CastSpell(me, 35600, true);
					}

					speak3 = true;					
				}
				else continue_timer2 -= diff;
			}

			if (spellHit == true && !speak4)
			{
				if (continue_timer3 <= diff)
				{
					if (Creature* kaylaan = me->FindNearestCreature(20794, 15.0f, true))
					{			
						kaylaan->RemoveAllAuras();
						kaylaan->setFaction(35);
						kaylaan->SetStandState(UNIT_STAND_STATE_STAND);
						kaylaan->MonsterYell(KAYLAAN_SAY_2, LANG_UNIVERSAL, 0);		
						continue_timer4 = 7000;
					}

					speak4 = true;
				}
				else continue_timer3 -= diff;
			}

			if (speak4 == true && !speak5)
			{
				if (continue_timer4 <= diff)
				{
					if (Creature* kaylaan = me->FindNearestCreature(20794, 15.0f, true))
					{					
						kaylaan->MonsterYell(KAYLAAN_SAY_3, LANG_UNIVERSAL, 0);
						kaylaan->CastSpell(kaylaan, 13874, true);
						continue_timer5 = 1500;
					}

					speak5 = true;
				}
				else continue_timer4 -= diff;
			}

			if (speak5 == true && !speak6)
			{
				if (continue_timer5 <= diff)
				{
					if (Creature* kaylaan = me->FindNearestCreature(20794, 15.0f, true))
					{					
						kaylaan->CastSpell(me, 35599, true);
						continue_timer6 = 8000;
					}

					speak6 = true;
				}
				else continue_timer5 -= diff;
			}

			if (speak6 == true && !speak7)
			{
				if (continue_timer6 <= diff)
				{
					if (Creature* socrethar = me->FindNearestCreature(20132, 25.0f, true))
					{
						socrethar->MonsterYell(SOCRETHAR_YELL_1, LANG_UNIVERSAL, 0);

						if (Creature* kaylaan = me->FindNearestCreature(20794, 15.0f, true))
						{
							socrethar->CastSpell(kaylaan, 35598, true);
						}

						continue_timer7 = 5500;
					}

					speak7 = true;
				}
				else continue_timer6 -= diff;
			}

			if (speak7 == true && !combatStart)
			{
				if (continue_timer7 <= diff)
				{
					if (Creature* socrethar = me->FindNearestCreature(20132, 25.0f, true))
					{
						socrethar->setFaction(1786);
						socrethar->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
						socrethar->AI()->AttackStart(me);
						me->AI()->AttackStart(socrethar);

						if (Creature* karja = me->FindNearestCreature(61022, 50.0f, true))
							karja->AI()->AttackStart(socrethar);

						if (Creature* orelis = me->FindNearestCreature(19466, 50.0f, true))
							orelis->AI()->AttackStart(socrethar);

						if (Creature * adyen = me->FindNearestCreature(61021, 50.0f, true))
							adyen->AI()->AttackStart(socrethar);

						combatStart = true;
					}				
				}
				else continue_timer7 -= diff;
			}
		}

		if (UpdateVictim())
		{
			if (smite_timer <= diff)
			{
				DoCastVictim(SPELL_HOLY_SMITE);
				smite_timer = urand(8000, 12000);
			}
			else smite_timer -= diff;

			if (pws_timer <= diff)
			{
				DoCast(me, SPELL_PWS);
				pws_timer = urand(20000, 28000);
			}
			else pws_timer -= diff;
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_ishanah(Creature* pCreature)
{
	return new npc_ishanahAI(pCreature);
}

bool GossipHello_npc_ishanah(Player* player, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        player->PrepareQuestMenu(pCreature->GetGUID());

    player->ADD_GOSSIP_ITEM(0, ISANAH_GOSSIP_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(0, ISANAH_GOSSIP_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_ishanah(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
        player->SEND_GOSSIP_MENU(9458, pCreature->GetGUID());
    else if (action == GOSSIP_ACTION_INFO_DEF + 2)
        player->SEND_GOSSIP_MENU(9459, pCreature->GetGUID());

    return true;
}

/*######
# npc_khadgar
######*/

#define KHADGAR_GOSSIP_1    "I've heard your name spoken only in whispers, mage. Who are you?"
#define KHADGAR_GOSSIP_2    "Go on, please."
#define KHADGAR_GOSSIP_3    "I see." //6th too this
#define KHADGAR_GOSSIP_4    "What did you do then?"
#define KHADGAR_GOSSIP_5    "What happened next?"
#define KHADGAR_GOSSIP_7    "There was something else I wanted to ask you."

bool GossipHello_npc_khadgar(Player* player, Creature* creature)
{
    if (creature->isQuestGiver())
        player->PrepareQuestMenu(creature->GetGUID());

    if (!player->hasQuest(10211))
        player->ADD_GOSSIP_ITEM(0, KHADGAR_GOSSIP_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    player->SEND_GOSSIP_MENU(9243, creature->GetGUID());

    return true;
}

bool GossipSelect_npc_khadgar(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF+1:
        player->ADD_GOSSIP_ITEM(0, KHADGAR_GOSSIP_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(9876, creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        player->ADD_GOSSIP_ITEM(0, KHADGAR_GOSSIP_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->SEND_GOSSIP_MENU(9877, creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+3:
        player->ADD_GOSSIP_ITEM(0, KHADGAR_GOSSIP_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->SEND_GOSSIP_MENU(9878, creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+4:
        player->ADD_GOSSIP_ITEM(0, KHADGAR_GOSSIP_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->SEND_GOSSIP_MENU(9879, creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+5:
        player->ADD_GOSSIP_ITEM(0, KHADGAR_GOSSIP_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->SEND_GOSSIP_MENU(9880, creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+6:
        player->ADD_GOSSIP_ITEM(0, KHADGAR_GOSSIP_7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->SEND_GOSSIP_MENU(9881, creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+7:
        player->ADD_GOSSIP_ITEM(0, KHADGAR_GOSSIP_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(9243, creature->GetGUID());
        break;
    }
    return true;
}

void AddSC_shattrath_city()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_raliq_the_drunk";
    newscript->pGossipHello =  &GossipHello_npc_raliq_the_drunk;
    newscript->pGossipSelect = &GossipSelect_npc_raliq_the_drunk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_salsalabim";
    newscript->GetAI = &GetAI_npc_salsalabim;
    newscript->pGossipHello =  &GossipHello_npc_salsalabim;
	newscript->pGossipSelect = &GossipSelect_npc_salsalabim;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_shattrathflaskvendors";
    newscript->pGossipHello =  &GossipHello_npc_shattrathflaskvendors;
    newscript->pGossipSelect = &GossipSelect_npc_shattrathflaskvendors;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_zephyr";
    newscript->pGossipHello =  &GossipHello_npc_zephyr;
    newscript->pGossipSelect = &GossipSelect_npc_zephyr;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_khadgars_servant";
    newscript->GetAI = &GetAI_npc_khadgars_servant;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_dirty_larry";
    newscript->GetAI = &GetAI_npc_dirty_larryAI;
    newscript->pGossipHello =   &GossipHello_npc_dirty_larry;
    newscript->pGossipSelect = &GossipSelect_npc_dirty_larry;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_ishanah";
	newscript->GetAI = &GetAI_npc_ishanah;
    newscript->pGossipHello =  &GossipHello_npc_ishanah;
    newscript->pGossipSelect = &GossipSelect_npc_ishanah;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_khadgar";
    newscript->pGossipHello =  &GossipHello_npc_khadgar;
    newscript->pGossipSelect = &GossipSelect_npc_khadgar;
    newscript->RegisterSelf();
}

