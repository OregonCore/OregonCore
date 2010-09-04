/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Blades_Edge_Mountains
SD%Complete: 90
SDComment: Quest support: 10503, 10504, 10556, 10609, 10682, 10980. Ogri'la->Skettis Flight. (npc_daranelle needs bit more work before consider complete)
SDCategory: Blade's Edge Mountains
EndScriptData */

/* ContentData
mobs_bladespire_ogre
mobs_nether_drake
npc_daranelle
npc_overseer_nuaar
npc_saikkal_the_elder
EndContentData */

#include "precompiled.h"

/*######
## mobs_bladespire_ogre
######*/

//TODO: add support for quest 10512 + creature abilities
struct OREGON_DLL_DECL mobs_bladespire_ogreAI : public ScriptedAI
{
    mobs_bladespire_ogreAI(Creature *c) : ScriptedAI(c) {}

    void Reset()
    {
    }

    void EnterCombat(Unit* who)
    {
    }

    void JustDied(Unit* Killer)
    {
        if (Killer->GetTypeId() == TYPEID_PLAYER)
            CAST_PLR(Killer)->KilledMonster(19995, me->GetGUID());
    }
};
CreatureAI* GetAI_mobs_bladespire_ogre(Creature* pCreature)
{
    return new mobs_bladespire_ogreAI (pCreature);
}

/*######
## mobs_nether_drake
######*/

#define SAY_NIHIL_1         -1000396
#define SAY_NIHIL_2         -1000397
#define SAY_NIHIL_3         -1000398
#define SAY_NIHIL_4         -1000399
#define SAY_NIHIL_INTERRUPT -1000400

#define ENTRY_WHELP                 20021
#define ENTRY_PROTO                 21821
#define ENTRY_ADOLE                 21817
#define ENTRY_MATUR                 21820
#define ENTRY_NIHIL                 21823

#define SPELL_T_PHASE_MODULATOR     37573

#define SPELL_ARCANE_BLAST          38881
#define SPELL_MANA_BURN             38884
#define SPELL_INTANGIBLE_PRESENCE   36513

struct OREGON_DLL_DECL mobs_nether_drakeAI : public ScriptedAI
{
    mobs_nether_drakeAI(Creature *c) : ScriptedAI(c) {}

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

    void EnterCombat(Unit* who) { }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (spell->Id == SPELL_T_PHASE_MODULATOR && caster->GetTypeId() == TYPEID_PLAYER)
        {
            uint32 cEntry = 0;

            switch(me->GetEntry())
            {
                case ENTRY_WHELP:
                    switch(rand()%4)
                    {
                        case 0: cEntry = ENTRY_PROTO; break;
                        case 1: cEntry = ENTRY_ADOLE; break;
                        case 2: cEntry = ENTRY_MATUR; break;
                        case 3: cEntry = ENTRY_NIHIL; break;
                    }
                    break;
                case ENTRY_PROTO:
                    switch(rand()%3)
                    {
                        case 0: cEntry = ENTRY_ADOLE; break;
                        case 1: cEntry = ENTRY_MATUR; break;
                        case 2: cEntry = ENTRY_NIHIL; break;
                    }
                    break;
                case ENTRY_ADOLE:
                    switch(rand()%3)
                    {
                        case 0: cEntry = ENTRY_PROTO; break;
                        case 1: cEntry = ENTRY_MATUR; break;
                        case 2: cEntry = ENTRY_NIHIL; break;
                    }
                    break;
                case ENTRY_MATUR:
                    switch(rand()%3)
                    {
                        case 0: cEntry = ENTRY_PROTO; break;
                        case 1: cEntry = ENTRY_ADOLE; break;
                        case 2: cEntry = ENTRY_NIHIL; break;
                    }
                    break;
                case ENTRY_NIHIL:
                    if (NihilSpeech_Phase)
                    {
                        DoScriptText(SAY_NIHIL_INTERRUPT, me);
                        IsNihil = false;
                        switch(rand()%3)
                        {
                            case 0: cEntry = ENTRY_PROTO; break;
                            case 1: cEntry = ENTRY_ADOLE; break;
                            case 2: cEntry = ENTRY_MATUR; break;
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
                    switch(NihilSpeech_Phase)
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
                                                            // + MOVEMENTFLAG_LEVITATING
                            me->AddUnitMovementFlag(MOVEMENTFLAG_ONTRANSPORT);
                            //then take off to random location. creature is initially summoned, so don't bother do anything else.
                            me->GetMotionMaster()->MovePoint(0, me->GetPositionX()+100, me->GetPositionY(), me->GetPositionZ()+100);
                            NihilSpeech_Phase = 0;
                            break;
                    }
                    NihilSpeech_Timer = 5000;
                } else NihilSpeech_Timer -=diff;
            }
            return;                                         //anything below here is not interesting for Nihil, so skip it
        }

        if (!UpdateVictim())
            return;

        if (IntangiblePresence_Timer <= diff)
        {
            DoCast(me->getVictim(),SPELL_INTANGIBLE_PRESENCE);
            IntangiblePresence_Timer = 15000+rand()%15000;
        } else IntangiblePresence_Timer -= diff;

        if (ManaBurn_Timer <= diff)
        {
            Unit *pTarget = me->getVictim();
            if (pTarget && pTarget->getPowerType() == POWER_MANA)
                DoCast(pTarget,SPELL_MANA_BURN);
            ManaBurn_Timer = 8000+rand()%8000;
        } else ManaBurn_Timer -= diff;

        if (ArcaneBlast_Timer <= diff)
        {
            DoCast(me->getVictim(),SPELL_ARCANE_BLAST);
            ArcaneBlast_Timer = 2500+rand()%5000;
        } else ArcaneBlast_Timer -= diff;

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

#define SAY_DARANELLE -1000401

struct OREGON_DLL_DECL npc_daranelleAI : public ScriptedAI
{
    npc_daranelleAI(Creature *c) : ScriptedAI(c) {}

    void Reset()
    {
    }

    void EnterCombat(Unit* who)
    {
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (who->GetTypeId() == TYPEID_PLAYER)
        {
            if (who->HasAura(36904,0))
            {
                DoScriptText(SAY_DARANELLE, me, who);
                //TODO: Move the below to updateAI and run if this statement == true
                CAST_PLR(who)->KilledMonster(21511, me->GetGUID());
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

bool GossipHello_npc_overseer_nuaar(Player *player, Creature* pCreature)
{
    if (player->GetQuestStatus(10682) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    player->SEND_GOSSIP_MENU(10532, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_overseer_nuaar(Player *player, Creature* pCreature, uint32 sender, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF+1)
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

bool GossipHello_npc_saikkal_the_elder(Player *player, Creature* pCreature)
{
    if (player->GetQuestStatus(10980) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HSTE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    player->SEND_GOSSIP_MENU(10794, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_saikkal_the_elder(Player *player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_SSTE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
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
## AddSC
######*/

void AddSC_blades_edge_mountains()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "mobs_bladespire_ogre";
    newscript->GetAI = &GetAI_mobs_bladespire_ogre;
    newscript->RegisterSelf();

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
}

