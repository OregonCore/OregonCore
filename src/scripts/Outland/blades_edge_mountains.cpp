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
SDName: Blades_Edge_Mountains
SD%Complete: 90
SDComment: Quest support: 10556, 10609, 10682, 10980, 10512. Ogri'la->Skettis Flight. (npc_daranelle needs bit more work before consider complete)
SDCategory: Blade's Edge Mountains
EndScriptData */

/* ContentData
mobs_nether_drake
npc_daranelle
npc_overseer_nuaar
npc_saikkal_the_elder
npc_ogre_brute
npc_bloodmaul_brutebane
npc_light_orb_collector
EndContentData */

#include "ScriptPCH.h"

/*######
## mobs_nether_drake
######*/

enum eDrake
{
    SAY_NIHIL_1                 = -1000169,
    SAY_NIHIL_2                 = -1000170,
    SAY_NIHIL_3                 = -1000171,
    SAY_NIHIL_4                 = -1000172,
    SAY_NIHIL_INTERRUPT         = -1000173,

    ENTRY_WHELP                 = 20021,
    ENTRY_PROTO                 = 21821,
    ENTRY_ADOLE                 = 21817,
    ENTRY_MATUR                 = 21820,
    ENTRY_NIHIL                 = 21823,

    SPELL_T_PHASE_MODULATOR     = 37573,

    SPELL_ARCANE_BLAST          = 38881,
    SPELL_MANA_BURN             = 38884,
    SPELL_INTANGIBLE_PRESENCE   = 36513
};

struct mobs_nether_drakeAI : public ScriptedAI
{
    mobs_nether_drakeAI(Creature* c) : ScriptedAI(c) {}

    bool IsNihil;
    uint32 NihilSpeech_Timer;
    uint32 NihilSpeech_Phase;

    uint32 ArcaneBlast_Timer;
    uint32 ManaBurn_Timer;
    uint32 IntangiblePresence_Timer;

    void Reset()
    {
        NihilSpeech_Timer = 2000;
        IsNihil = false;
        if (me->GetEntry() == ENTRY_NIHIL)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            IsNihil = true;
        }
        NihilSpeech_Phase = 1;

        ArcaneBlast_Timer = 7500;
        ManaBurn_Timer = 10000;
        IntangiblePresence_Timer = 15000;
    }

    void EnterCombat(Unit* /*who*/) { }

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if (spell->Id == SPELL_T_PHASE_MODULATOR && caster->GetTypeId() == TYPEID_PLAYER)
        {
            uint32 cEntry = 0;

            switch (me->GetEntry())
            {
            case ENTRY_WHELP:
                switch (rand() % 4)
                {
                case 0:
                    cEntry = ENTRY_PROTO;
                    break;
                case 1:
                    cEntry = ENTRY_ADOLE;
                    break;
                case 2:
                    cEntry = ENTRY_MATUR;
                    break;
                case 3:
                    cEntry = ENTRY_NIHIL;
                    break;
                }
                break;
            case ENTRY_PROTO:
                switch (rand() % 3)
                {
                case 0:
                    cEntry = ENTRY_ADOLE;
                    break;
                case 1:
                    cEntry = ENTRY_MATUR;
                    break;
                case 2:
                    cEntry = ENTRY_NIHIL;
                    break;
                }
                break;
            case ENTRY_ADOLE:
                switch (rand() % 3)
                {
                case 0:
                    cEntry = ENTRY_PROTO;
                    break;
                case 1:
                    cEntry = ENTRY_MATUR;
                    break;
                case 2:
                    cEntry = ENTRY_NIHIL;
                    break;
                }
                break;
            case ENTRY_MATUR:
                switch (rand() % 3)
                {
                case 0:
                    cEntry = ENTRY_PROTO;
                    break;
                case 1:
                    cEntry = ENTRY_ADOLE;
                    break;
                case 2:
                    cEntry = ENTRY_NIHIL;
                    break;
                }
                break;
            case ENTRY_NIHIL:
                if (NihilSpeech_Phase)
                {
                    DoScriptText(SAY_NIHIL_INTERRUPT, me);
                    IsNihil = false;
                    switch (rand() % 3)
                    {
                    case 0:
                        cEntry = ENTRY_PROTO;
                        break;
                    case 1:
                        cEntry = ENTRY_ADOLE;
                        break;
                    case 2:
                        cEntry = ENTRY_MATUR;
                        break;
                    }
                }
                break;
            }

            if (cEntry)
            {
                me->UpdateEntry(cEntry);

                if (cEntry == ENTRY_NIHIL)
                {
                    me->InterruptNonMeleeSpells(true);
                    me->RemoveAllAuras();
                    me->DeleteThreatList();
                    me->CombatStop(true);
                    Reset();
                }
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (IsNihil)
        {
            if (NihilSpeech_Phase)
            {
                if (NihilSpeech_Timer <= diff)
                {
                    switch (NihilSpeech_Phase)
                    {
                    case 1:
                        DoScriptText(SAY_NIHIL_1, me);
                        ++NihilSpeech_Phase;
                        break;
                    case 2:
                        DoScriptText(SAY_NIHIL_2, me);
                        ++NihilSpeech_Phase;
                        break;
                    case 3:
                        DoScriptText(SAY_NIHIL_3, me);
                        ++NihilSpeech_Phase;
                        break;
                    case 4:
                        DoScriptText(SAY_NIHIL_4, me);
                        ++NihilSpeech_Phase;
                        break;
                    case 5:
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        // + MOVEFLAG_LEVITATING
                        me->AddUnitMovementFlag(MOVEFLAG_ONTRANSPORT);
                        //then take off to random location. creature is initially summoned, so don't bother do anything else.
                        me->GetMotionMaster()->MovePoint(0, me->GetPositionX() + 100, me->GetPositionY(), me->GetPositionZ() + 100);
                        NihilSpeech_Phase = 0;
                        break;
                    }
                    NihilSpeech_Timer = 5000;
                }
                else NihilSpeech_Timer -= diff;
            }
            return;                                         //anything below here is not interesting for Nihil, so skip it
        }

        if (!UpdateVictim())
            return;

        if (IntangiblePresence_Timer <= diff)
        {
            DoCastVictim(SPELL_INTANGIBLE_PRESENCE);
            IntangiblePresence_Timer = 15000 + rand() % 15000;
        }
        else IntangiblePresence_Timer -= diff;

        if (ManaBurn_Timer <= diff)
        {
            Unit* pTarget = me->getVictim();
            if (pTarget && pTarget->getPowerType() == POWER_MANA)
                DoCast(pTarget, SPELL_MANA_BURN);
            ManaBurn_Timer = 8000 + rand() % 8000;
        }
        else ManaBurn_Timer -= diff;

        if (ArcaneBlast_Timer <= diff)
        {
            DoCastVictim(SPELL_ARCANE_BLAST);
            ArcaneBlast_Timer = 2500 + rand() % 5000;
        }
        else ArcaneBlast_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_mobs_nether_drake(Creature* pCreature)
{
    return new mobs_nether_drakeAI (pCreature);
}

/*######
## npc_daranelle
######*/

#define SAY_DARANELLE -1000174

struct npc_daranelleAI : public ScriptedAI
{
    npc_daranelleAI(Creature* c) : ScriptedAI(c) {}

    void Reset()
    {
    }

    void EnterCombat(Unit* /*who*/)
    {
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (who->GetTypeId() == TYPEID_PLAYER)
        {
            if (who->HasAura(36904, 0))
            {
                DoScriptText(SAY_DARANELLE, me, who);
                //@todo Move the below to updateAI and run if this statement == true
                CAST_PLR(who)->KilledMonsterCredit(21511, me->GetGUID());
                CAST_PLR(who)->RemoveAurasDueToSpell(36904);
            }
        }

        ScriptedAI::MoveInLineOfSight(who);
    }
};

CreatureAI* GetAI_npc_daranelle(Creature* pCreature)
{
    return new npc_daranelleAI (pCreature);
}

/*######
## npc_overseer_nuaar
######*/

#define GOSSIP_HON "Overseer, I am here to negotiate on behalf of the Cenarion Expedition."

bool GossipHello_npc_overseer_nuaar(Player* player, Creature* pCreature)
{
    if (player->GetQuestStatus(10682) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    player->SEND_GOSSIP_MENU(10532, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_overseer_nuaar(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        player->SEND_GOSSIP_MENU(10533, pCreature->GetGUID());
        player->AreaExploredOrEventHappens(10682);
    }
    return true;
}

/*######
## npc_saikkal_the_elder
######*/

#define GOSSIP_HSTE "Yes... yes, it's me."
#define GOSSIP_SSTE "Yes elder. Tell me more of the book."

bool GossipHello_npc_saikkal_the_elder(Player* player, Creature* pCreature)
{
    if (player->GetQuestStatus(10980) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HSTE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    player->SEND_GOSSIP_MENU(10794, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_saikkal_the_elder(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF+1:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SSTE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(10795, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        player->TalkedToCreature(pCreature->GetEntry(), pCreature->GetGUID());
        player->SEND_GOSSIP_MENU(10796, pCreature->GetGUID());
        break;
    }
    return true;
}

/*######
## npc_bloodmaul_brutebane
######*/

enum eBloodmaul
{
    NPC_OGRE_BRUTE        = 19995,
    NPC_QUEST_CREDIT      = 21241,
    GO_KEG                = 184315
};

struct npc_bloodmaul_brutebaneAI : public ScriptedAI
{
    npc_bloodmaul_brutebaneAI(Creature* c) : ScriptedAI(c)
    {
        if (Creature* pOgre = me->FindNearestCreature(NPC_OGRE_BRUTE, 50, true))
        {
            pOgre->SetReactState(REACT_DEFENSIVE);
            pOgre->GetMotionMaster()->MovePoint(1, me->GetPositionX() - 1, me->GetPositionY() + 1, me->GetPositionZ());
        }
    }

    uint64 OgreGUID;

    void Reset()
    {
        OgreGUID = 0;
    }

    void UpdateAI(const uint32 /*uiDiff*/) {}
};

CreatureAI* GetAI_npc_bloodmaul_brutebane(Creature* pCreature)
{
    return new npc_bloodmaul_brutebaneAI (pCreature);
}

/*######
## npc_ogre_brute
######*/

struct npc_ogre_bruteAI : public ScriptedAI
{
    npc_ogre_bruteAI(Creature* c) : ScriptedAI(c) {}

    uint64 PlayerGUID;

    void Reset()
    {
        PlayerGUID = 0;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho || (!pWho->IsAlive()))
            return;

        if (me->IsWithinDistInMap(pWho, 50.0f) && (pWho->GetTypeId() == TYPEID_PLAYER) && pWho->ToPlayer()->GetQuestStatus(10512) == QUEST_STATUS_INCOMPLETE)
            PlayerGUID = pWho->GetGUID();
    }

    void MovementInform(uint32 /*uiMotionType*/, uint32 uiPointId)
    {
        if (Player* pPlayer = Unit::GetPlayer(*me, PlayerGUID))
        {
            if (uiPointId == 1)
            {
                if (GameObject* pKeg = me->FindNearestGameObject(GO_KEG, 20))
                    pKeg->Delete();
                me->HandleEmoteCommand(7);
                me->SetReactState(REACT_AGGRESSIVE);
                me->GetMotionMaster()->MoveTargetedHome();
                if (Creature* pCredit = me->FindNearestCreature(NPC_QUEST_CREDIT, 50, true))
                    pPlayer->KilledMonsterCredit(NPC_QUEST_CREDIT, pCredit->GetGUID());
            }
        }
    }

    void UpdateAI(const uint32 /*diff*/)
    {
        if (!UpdateVictim())
            return;
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_ogre_brute(Creature* pCreature)
{
    return new npc_ogre_bruteAI(pCreature);
}

/*#########
# go_thunderspike
# UPDATE `gameobject_template` SET `ScriptName` = "go_thunderspike" WHERE `entry` = 184729;
#########*/

#define Q_THE_THUNDERSPIKE 10526
#define GOR_GRIMGUT_ENTRY  21319

bool GOUse_go_thunderspike(Player* player, GameObject* /*_GO*/)
{
    if (player->GetQuestStatus(Q_THE_THUNDERSPIKE) == QUEST_STATUS_INCOMPLETE)
    {
        // to prevent spawn spam :)
        if (Creature* pGor = GetClosestCreatureWithEntry(player, GOR_GRIMGUT_ENTRY, 50.0f, true))
        {
            if (!pGor->getVictim())
                pGor->AI()->AttackStart(player);

            return false;
        }

        Position dest;
        //player->GetValidPointInAngle(dest, 5.0f, frand(0.0f, 2*M_PI), true);
        player->GetPosition(&dest);
        if (Creature* pGor = player->SummonCreature(GOR_GRIMGUT_ENTRY, dest.m_positionX, dest.m_positionY, dest.m_positionZ, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000))
            pGor->AI()->AttackStart(player);
    }

    return false;
}

/*######
## npc_light_orb_collector
######*/

enum RazaniLightOrb
{
    // Creatures
    NPC_LIGHT_ORB_MINI          = 20771,
    NPC_KILL_CREDIT_TRIGGER     = 21929,

    // Quests
    QUEST_LIGHT_FANTASTIC       = 10674,
    QUEST_GATHER_THE_ORBS       = 10859,

    // Misc
    MAX_PULL_DISTANCE           = 20
};

struct npc_light_orb_collectorAI : public ScriptedAI
{
    npc_light_orb_collectorAI(Creature* c) : ScriptedAI(c)
    {
        Reset();
    }

    uint64 selectedOrbGUID;
    bool orbPulled;
    uint32 startTimer;

    void Reset()
    {
        orbPulled    = false;
        startTimer  = 0;
    }

    void RewardCredit(uint64 playerGUID)
    { 
        Map* map = me->GetMap();
        Map::PlayerList const &PlayerList = map->GetPlayers();

        for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
            if (Player* player = itr->getSource())
                if (player->GetGUID() == playerGUID)
                    if (me->IsWithinDistInMap(player, 15.0f))
                        if (player->GetQuestStatus(QUEST_LIGHT_FANTASTIC) || player->GetQuestStatus(QUEST_GATHER_THE_ORBS) == QUEST_STATUS_INCOMPLETE)
                            player->KilledMonsterCredit(NPC_KILL_CREDIT_TRIGGER, me->GetGUID());
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (who->GetTypeId() != TYPEID_UNIT || who->GetEntry() != NPC_LIGHT_ORB_MINI)
            return;

        // Select an nearby orb to collect
        if (!startTimer && !orbPulled)
        {
            if (me->GetDistance(who) <= MAX_PULL_DISTANCE)
            {
                selectedOrbGUID = who->GetGUID();
                startTimer = 2000;
            }
        }
        else if (orbPulled && who->GetGUID() == selectedOrbGUID && me->IsWithinDistInMap(who, 3.5f))
        {
            // Despawn the collected orb if close enough
            ((Creature*)who)->ForcedDespawn();

            // Give kill credit to the player
            RewardCredit(who->GetOwnerGUID());

            // Despawn collector
            me->ForcedDespawn();
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (startTimer)
        {
            // Start collecting after some delay
            if (startTimer <= diff)
            {
                Creature* selectedOrb = me->GetMap()->GetCreature(selectedOrbGUID);
                if (!selectedOrb)
                    return;

                // Orb is pulled fast
                selectedOrb->SetWalk(false);

                // Move orb to the collector
                float fX, fY, fZ;;
                selectedOrb->GetMotionMaster()->MoveIdle();
                me->GetContactPoint(selectedOrb, fX, fY, fZ);
                selectedOrb->GetMotionMaster()->MovePoint(0, fX, fY, fZ);

                orbPulled = true;
                startTimer = 0;
            }
            else
                startTimer -= diff;
        }
    }
};

CreatureAI* GetAI_npc_light_orb_collector(Creature* creature)
{
    return new npc_light_orb_collectorAI(creature);
}


void AddSC_blades_edge_mountains()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "mobs_nether_drake";
    newscript->GetAI = &GetAI_mobs_nether_drake;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_daranelle";
    newscript->GetAI = &GetAI_npc_daranelle;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_overseer_nuaar";
    newscript->pGossipHello = &GossipHello_npc_overseer_nuaar;
    newscript->pGossipSelect = &GossipSelect_npc_overseer_nuaar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_saikkal_the_elder";
    newscript->pGossipHello = &GossipHello_npc_saikkal_the_elder;
    newscript->pGossipSelect = &GossipSelect_npc_saikkal_the_elder;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_bloodmaul_brutebane";
    newscript->GetAI = &GetAI_npc_bloodmaul_brutebane;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_ogre_brute";
    newscript->GetAI = &GetAI_npc_ogre_brute;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_thunderspike";
    newscript->pGOHello = &GOUse_go_thunderspike;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_light_orb_collector";
    newscript->GetAI = &GetAI_npc_light_orb_collector;
    newscript->RegisterSelf();
}
