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
SDName: Azuremyst_Isle
SD%Complete: 75
SDComment: Quest support: 9283, 9537, 9582, 9554, 9531, 9303(special flight path, proper model for mount missing). Injured Draenei cosmetic only, 9582.
SDCategory: Azuremyst Isle
EndScriptData */

/* ContentData
npc_draenei_survivor
npc_tavara
npc_engineer_spark_overgrind
npc_injured_draenei
npc_magwin
npc_geezle
mob_nestlewood_owlkin
go_ravager_cage
npc_death_ravager
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"
#include "GridNotifiers.h"
#include "Cell.h"
#include "CellImpl.h"

/*######
## npc_draenei_survivor
######*/

enum eSurv
{
    HEAL1           = -1000176,
    HEAL2           = -1000177,
    HEAL3           = -1000178,
    HEAL4           = -1000179,

    HELP1           = -1000180,
    HELP2           = -1000181,
    HELP3           = -1000182,
    HELP4           = -1000183
};

struct npc_draenei_survivorAI : public ScriptedAI
{
    npc_draenei_survivorAI(Creature* c) : ScriptedAI(c) {}

    uint32 UnSpawnTimer;
    uint32 ResetlifeTimer;
    uint32 SayingTimer;
    uint32 HealSayTimer;
    bool UnSpawn;
    bool say;
    bool HealSay;
    bool isRun;
    bool isMove;

    void Reset()
    {
        UnSpawnTimer = 2500;
        ResetlifeTimer = 60000;
        SayingTimer    = 5000;
        HealSayTimer = 6000;
        say = false;
        isRun = false;
        isMove = false;
        UnSpawn    = false;
        HealSay = false;
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
        //cast red shining
        me->CastSpell(me, 29152, false, NULL);
        //set creature health
        me->SetHealth(int(me->GetMaxHealth()*.1));
        me->SetUInt32Value(UNIT_FIELD_BYTES_1, 3);
    }

    void EnterCombat(Unit*) {}

    void MoveInLineOfSight(Unit* who)                       //MoveInLineOfSight is called if creature could see you, updated all 100 ms
    {
        if (!who)
            return;

        if (who->GetTypeId() == TYPEID_PLAYER && me->IsFriendlyTo(who) && me->IsWithinDistInMap(who, 15) && say && !isRun)
        {
            switch (rand() % 4)                             //Random switch between 4 texts
            {
            case 0:
                DoScriptText(HELP1, me);
                SayingTimer = 15000;
                say = false;
                break;
            case 1:
                DoScriptText(HELP2, me);
                SayingTimer = 15000;
                say = false;
                break;
            case 2:
                DoScriptText(HELP3, me);
                SayingTimer = 15000;
                say = false;
                break;
            case 3:
                DoScriptText(HELP4, me);
                SayingTimer = 15000;
                say = false;
                break;
            }
        }
        else
            isRun = false;
    }

    void UpdateAI(const uint32 diff)                        //Is also called each ms for Creature AI Updates...
    {
        if (me->GetHealth() > 50)
        {
            if (ResetlifeTimer <= diff)
            {
                ResetlifeTimer = 60000;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
                //set creature health
                me->SetHealth(int(me->GetMaxHealth()*.1));
                // ley down
                me->SetUInt32Value(UNIT_FIELD_BYTES_1, 3);
            }
            else ResetlifeTimer -= diff;
        }

        if (HealSay)
        {
            if (HealSayTimer <= diff)
            {
                UnSpawn = true;
                isRun = true;
                isMove = true;
            }
            else HealSayTimer -= diff;
        }

        if (UnSpawn)
        {
            if (isMove)
            {
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MovePoint(0, -4115.053711f, -13754.831055f, 73.508949f);
                isMove = false;
            }

            if (UnSpawnTimer <= diff)
            {
                me->StopMoving();
                EnterEvadeMode();
                //set creature health
                me->SetHealth(int(me->GetMaxHealth()*.1));
                return;
            }
            else UnSpawnTimer -= diff;
        }

        if (SayingTimer <= diff)
            say = true;
        else SayingTimer -= diff;
    }

    void SpellHit(Unit* Hitter, const SpellEntry* Spellkind)//Called if you cast a spell and do some things if Specified spell is true!
    {
        if (Hitter && Spellkind->Id == 28880)
        {
            me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
            me->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);
            me->HandleEmoteCommand(ANIM_RISE);
            switch (rand() % 4)                             //This switch doesn't work at all, creature say nothing!
            {
            case 0:
                DoScriptText(HEAL1, me, Hitter);
                break;
            case 1:
                DoScriptText(HEAL2, me, Hitter);
                break;
            case 2:
                DoScriptText(HEAL3, me, Hitter);
                break;
            case 3:
                DoScriptText(HEAL4, me, Hitter);
                break;
            }
            HealSay    = true;
        }
    }
};
CreatureAI* GetAI_npc_draenei_survivor(Creature* pCreature)
{
    return new npc_draenei_survivorAI (pCreature);
}

/*######
## npc_tavara
######*/

enum eTavara
{
    // NPCs
    NPC_TAVARA                  = 17551,

    // Spells
    SPELL_GIFT_OF_THE_NARUU     = 28880,
    SPELL_LESSER_HEAL_R1        = 2050,
    SPELL_LESSER_HEAL_R2        = 2052,
    SPELL_RENEW_R1              = 139
};

struct npc_tavaraAI : public ScriptedAI
{
    npc_tavaraAI(Creature* c) : ScriptedAI(c) {}

    void Reset() { }

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        Player* questTarget = caster->ToPlayer();
        if (questTarget && questTarget->getClass() == CLASS_PRIEST)
            switch (spell->Id)
            {
                case SPELL_GIFT_OF_THE_NARUU:
                case SPELL_LESSER_HEAL_R1:
                case SPELL_LESSER_HEAL_R2:
                case SPELL_RENEW_R1:
                    {
                        me->HandleEmoteCommand(ANIM_RISE);
                        questTarget->KilledMonsterCredit(NPC_TAVARA, me->GetGUID());
                    }
            }
    }
};

CreatureAI* GetAI_npc_tavara(Creature* creature)
{
    return new npc_tavaraAI (creature);
}

/*######
## npc_engineer_spark_overgrind
######*/

enum eSpark
{
    SAY_TEXT                = -1000184,
    SAY_EMOTE               = -1000185,
    ATTACK_YELL             = -1000186,

    SPELL_DYNAMITE          = 7978
};

#define GOSSIP_FIGHT    "Traitor! You will be brought to justice!"

struct npc_engineer_spark_overgrindAI : public ScriptedAI
{
    npc_engineer_spark_overgrindAI(Creature* c) : ScriptedAI(c) {}

    uint32 Dynamite_Timer;
    uint32 Emote_Timer;

    void Reset()
    {
        Dynamite_Timer = 8000;
        Emote_Timer = 120000 + rand() % 30000;
        me->SetFaction(875);
    }

    void EnterCombat(Unit*) { }

    void UpdateAI(const uint32 diff)
    {
        if (!me->IsInCombat())
        {
            if (Emote_Timer <= diff)
            {
                DoScriptText(SAY_TEXT, me);
                DoScriptText(SAY_EMOTE, me);
                Emote_Timer = 120000 + rand() % 30000;
            }
            else Emote_Timer -= diff;
        }

        if (!UpdateVictim())
            return;

        if (Dynamite_Timer <= diff)
        {
            DoCastVictim( SPELL_DYNAMITE);
            Dynamite_Timer = 8000;
        }
        else Dynamite_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_engineer_spark_overgrind(Creature* pCreature)
{
    return new npc_engineer_spark_overgrindAI (pCreature);
}

bool GossipHello_npc_engineer_spark_overgrind(Player* player, Creature* pCreature)
{
    if (player->GetQuestStatus(9537) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_FIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_engineer_spark_overgrind(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF)
    {
        player->CLOSE_GOSSIP_MENU();
        pCreature->SetFaction(14);
        DoScriptText(ATTACK_YELL, pCreature, player);
        ((npc_engineer_spark_overgrindAI*)pCreature->AI())->AttackStart(player);
    }
    return true;
}

/*######
## npc_injured_draenei
######*/

struct npc_injured_draeneiAI : public ScriptedAI
{
    npc_injured_draeneiAI(Creature* c) : ScriptedAI(c) {}

    void Reset()
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
        me->SetHealth(int(me->GetMaxHealth()*.15));
        switch (rand() % 2)
        {
        case 0:
            me->SetUInt32Value(UNIT_FIELD_BYTES_1, 1);
            break;
        case 1:
            me->SetUInt32Value(UNIT_FIELD_BYTES_1, 3);
            break;
        }
    }

    void EnterCombat(Unit* /*who*/) {}

    void MoveInLineOfSight(Unit* /*who*/)
    {
    }

    void UpdateAI(const uint32 /*diff*/)
    {
    }

};
CreatureAI* GetAI_npc_injured_draenei(Creature* pCreature)
{
    return new npc_injured_draeneiAI (pCreature);
}

/*######
## npc_magwin
######*/

enum eMagwin
{
    SAY_START                   = -1000111,
    SAY_AGGRO                   = -1000112,
    SAY_PROGRESS                = -1000113,
    SAY_END1                    = -1000114,
    SAY_END2                    = -1000115,
    EMOTE_HUG                   = -1000116,

    QUEST_A_CRY_FOR_SAY_HELP    = 9528
};

struct npc_magwinAI : public npc_escortAI
{
    npc_magwinAI(Creature* c) : npc_escortAI(c) {}

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (i)
        {
        case 0:
            DoScriptText(SAY_START, me, pPlayer);
            break;
        case 17:
            DoScriptText(SAY_PROGRESS, me, pPlayer);
            break;
        case 28:
            DoScriptText(SAY_END1, me, pPlayer);
            break;
        case 29:
            DoScriptText(EMOTE_HUG, me, pPlayer);
            DoScriptText(SAY_END2, me, pPlayer);
            pPlayer->GroupEventHappens(QUEST_A_CRY_FOR_SAY_HELP, me);
            break;
        }
    }

    void EnterCombat(Unit* who)
    {
        DoScriptText(SAY_AGGRO, me, who);
    }

    void Reset() { }
};

bool QuestAccept_npc_magwin(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_A_CRY_FOR_SAY_HELP)
    {
        pCreature->SetFaction(113);
        if (npc_escortAI* pEscortAI = CAST_AI(npc_escortAI, pCreature->AI()))
            pEscortAI->Start(true, false, pPlayer->GetGUID());
    }
    return true;
}

CreatureAI* GetAI_npc_magwinAI(Creature* pCreature)
{
    return new npc_magwinAI(pCreature);
}

/*######
## npc_geezle
######*/

enum Geezle
{
    // Quests
    QUEST_TREES_COMPANY = 9531,

    // Spells
    SPELL_TREE_DISGUISE = 30298,

    // Texts
    GEEZLE_SAY_1   = -1000728,
    SPARK_SAY_2    = -1000729,
    SPARK_SAY_3    = -1000730,
    GEEZLE_SAY_4   = -1000731,
    SPARK_SAY_5    = -1000732,
    SPARK_SAY_6    = -1000733,
    GEEZLE_SAY_7   = -1000734,

    // Emotes
    EMOTE_SPARK     = -1000735,

    // NPCs
    NPC_SPARK       = 17243,

    // Gameobjects
    GO_NAGA_FLAG    = 181694
};

static float SparkPos[3] = { -5030.95f, -11291.99f, 7.97f};

struct npc_geezleAI : public ScriptedAI
{
    npc_geezleAI(Creature* c) : ScriptedAI(c) {}

    uint64 SparkGUID;

    uint32 Step;
    uint32 SayTimer;

    bool EventStarted;

    void Reset()
    {
        SparkGUID = 0;
        Step = 0;
        EventStarted = false;
        StartEvent();
    }

    void EnterCombat(Unit* /*who*/) {}

    void StartEvent()
    {
        Step = 0;
        EventStarted = true;
        if (Creature* Spark = me->SummonCreature(NPC_SPARK, SparkPos[0], SparkPos[1], SparkPos[2], 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000))
        {
            SparkGUID = Spark->GetGUID();
            Spark->setActive(true);
            Spark->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            Spark->GetMotionMaster()->MovePoint(0, -5080.70f, -11253.61f, 0.56f);
        }
        me->GetMotionMaster()->MovePoint(0, -5092.26f, -11252, 0.71f);
        SayTimer = 23000;
    }

    uint32 NextStep(uint32 Step)
    {
        Creature* Spark = Unit::GetCreature((*me), SparkGUID);
        if (!Spark)
            return 99999999;

        switch (Step)
        {
        case 0:
            return 99999;
        case 1:
            DespawnNagaFlag(true);
            // @todo: this emote doesnt seem to include Spark's name
            DoScriptText(EMOTE_SPARK, Spark);
            return 1000;
        case 2:
            DoScriptText(GEEZLE_SAY_1, me, Spark);
            Spark->SetInFront(me);
            me->SetInFront(Spark);
            return 5000;
        case 3:
            DoScriptText(SPARK_SAY_2, Spark);
            return 7000;
        case 4:
            DoScriptText(SPARK_SAY_3, Spark);
            return 8000;
        case 5:
            DoScriptText(GEEZLE_SAY_4, me, Spark);
            return 8000;
        case 6:
            DoScriptText(SPARK_SAY_5, Spark);
            return 9000;
        case 7:
            DoScriptText(SPARK_SAY_6, Spark);
            return 8000;
        case 8:
            DoScriptText(GEEZLE_SAY_7, me, Spark);
            return 2000;
        case 9:
            me->GetMotionMaster()->MoveTargetedHome();
            Spark->GetMotionMaster()->MovePoint(0, -5030.95f, -11291.99f, 7.97f);
            CompleteQuest();
            return 9000;
        case 10:
            Spark->DisappearAndDie();
            DespawnNagaFlag(false);
            me->DisappearAndDie();
        default:
            return 99999999;
        }
    }

    // will complete Tree's company quest for all nearby players that are disguised as trees
    void CompleteQuest()
    {
        float radius = 50.0f;
        std::list<Player*> players;
        Oregon::AnyPlayerInObjectRangeCheck checker(me, radius);
        Oregon::PlayerListSearcher<Oregon::AnyPlayerInObjectRangeCheck> searcher(players, checker);
        me->VisitNearbyWorldObject(radius, searcher);

        for (std::list<Player*>::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            if ((*itr)->GetQuestStatus(QUEST_TREES_COMPANY) == QUEST_STATUS_INCOMPLETE && (*itr)->HasAura(SPELL_TREE_DISGUISE))
                (*itr)->KilledMonsterCredit(NPC_SPARK);
    }

    void DespawnNagaFlag(bool despawn)
    {
        std::list<GameObject*> FlagList;
        me->GetGameObjectListWithEntryInGrid(FlagList, GO_NAGA_FLAG, 100.0f);

        if (!FlagList.empty())
        {
            for (std::list<GameObject*>::iterator itr = FlagList.begin(); itr != FlagList.end(); ++itr)
            {
                if (despawn)
                    (*itr)->SetLootState(GO_JUST_DEACTIVATED);
                else
                    (*itr)->Respawn();
            }
        }
        else error_log("SD2 ERROR: FlagList is empty!");
    }

    void UpdateAI(const uint32 diff)
    {
        if (SayTimer <= diff)
        {
            if (EventStarted)
                SayTimer = NextStep(++Step);
        }
        else SayTimer -= diff;
    }
};

CreatureAI* GetAI_npc_geezleAI(Creature* pCreature)
{
    return new npc_geezleAI(pCreature);
}

/*######
## mob_nestlewood_owlkin
######*/

#define INOCULATION_CHANNEL 29528
#define INOCULATED_OWLKIN   16534

struct mob_nestlewood_owlkinAI : public ScriptedAI
{
    mob_nestlewood_owlkinAI(Creature* c) : ScriptedAI(c) {}

    uint32 ChannelTimer;
    bool Channeled;
    bool Hitted;

    void Reset()
    {
        ChannelTimer = 0;
        Channeled = false;
        Hitted = false;
    }

    void EnterCombat(Unit* /*who*/) {}

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if (!caster)
            return;

        if (caster->GetTypeId() == TYPEID_PLAYER && spell->Id == INOCULATION_CHANNEL)
        {
            ChannelTimer = 3000;
            Hitted = true;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (ChannelTimer <= diff && !Channeled && Hitted)
        {
            me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            me->RemoveCorpse();
            me->SummonCreature(INOCULATED_OWLKIN, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 180000);
            Channeled = true;
        }
        else ChannelTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_nestlewood_owlkinAI(Creature* pCreature)
{
    return new mob_nestlewood_owlkinAI (pCreature);
}

enum eRavegerCage
{
    NPC_DEATH_RAVAGER       = 17556,

    SPELL_REND              = 13443,
    SPELL_ENRAGING_BITE     = 30736,

    QUEST_STRENGTH_ONE      = 9582
};

bool go_ravager_cage(Player* player, GameObject* go)
{
    go->UseDoorOrButton();
    if (player->GetQuestStatus(QUEST_STRENGTH_ONE) == QUEST_STATUS_INCOMPLETE)
    {
        if (Creature* ravager = go->FindNearestCreature(NPC_DEATH_RAVAGER, 5.0f, true))
        {
            ravager->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            ravager->SetReactState(REACT_AGGRESSIVE);
            ravager->AI()->AttackStart(player);
        }
    }
    return true ;
}

struct npc_death_ravagerAI : public ScriptedAI
{
    npc_death_ravagerAI(Creature* c) : ScriptedAI(c) {}

    uint32 RendTimer;
    uint32 EnragingBiteTimer;

    void Reset()
    {
        RendTimer = 30000;
        EnragingBiteTimer = 20000;

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetReactState(REACT_PASSIVE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (RendTimer <= diff)
        {
            DoCastVictim( SPELL_REND);
            RendTimer = 30000;
        }
        else RendTimer -= diff;

        if (EnragingBiteTimer <= diff)
        {
            DoCastVictim( SPELL_ENRAGING_BITE);
            EnragingBiteTimer = 15000;
        }
        else EnragingBiteTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_death_ravagerAI(Creature* pCreature)
{
    return new npc_death_ravagerAI(pCreature);
}

/*########
## Quest: The Prophecy of Akida
########*/

enum BristlelimbCage
{
    CAPITIVE_SAY_1                      = -1000602,
    CAPITIVE_SAY_2                      = -1000603,
    CAPITIVE_SAY_3                      = -1000604,

    QUEST_THE_PROPHECY_OF_AKIDA         = 9544,
    NPC_STILLPINE_CAPITIVE              = 17375,
    GO_BRISTELIMB_CAGE                  = 181714

};


struct npc_stillpine_capitiveAI : public ScriptedAI
{
    npc_stillpine_capitiveAI(Creature* c) : ScriptedAI(c) {}

    uint32 FleeTimer;

    void Reset()
    {
        FleeTimer = 0;
        if (GameObject* cage = me->FindNearestGameObject(GO_BRISTELIMB_CAGE, 5.0f))
        {
            cage->SetLootState(GO_JUST_DEACTIVATED);
            cage->SetGoState(GO_STATE_READY);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (FleeTimer)
        {
            if (FleeTimer <= diff)
                me->ForcedDespawn();
            else FleeTimer -= diff;
        }
    }
};

CreatureAI* GetAI_npc_stillpine_capitiveAI(Creature* pCreature)
{
    return new npc_stillpine_capitiveAI(pCreature);
}

bool go_bristlelimb_cage(Player* player, GameObject* go)
{
    go->SetGoState(GO_STATE_READY);
    if (player->GetQuestStatus(QUEST_THE_PROPHECY_OF_AKIDA) == QUEST_STATUS_INCOMPLETE)
    {
        if (Creature* creature = go->FindNearestCreature(NPC_STILLPINE_CAPITIVE, 5.0f, true))
        {
            DoScriptText(RAND(CAPITIVE_SAY_1, CAPITIVE_SAY_2, CAPITIVE_SAY_3), creature, player);
            creature->GetMotionMaster()->MoveFleeing(player, 3500);
            player->KilledMonsterCredit(creature->GetEntry(), creature->GetGUID());
            CAST_AI(npc_stillpine_capitiveAI, creature->AI())->FleeTimer = 3500;
            return false;
        }
    }
    return true;
}

void AddSC_azuremyst_isle()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_draenei_survivor";
    newscript->GetAI = &GetAI_npc_draenei_survivor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tavara";
    newscript->GetAI = &GetAI_npc_tavara;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_engineer_spark_overgrind";
    newscript->GetAI = &GetAI_npc_engineer_spark_overgrind;
    newscript->pGossipHello =  &GossipHello_npc_engineer_spark_overgrind;
    newscript->pGossipSelect = &GossipSelect_npc_engineer_spark_overgrind;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_injured_draenei";
    newscript->GetAI = &GetAI_npc_injured_draenei;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_magwin";
    newscript->GetAI = &GetAI_npc_magwinAI;
    newscript->pQuestAccept = &QuestAccept_npc_magwin;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_geezle";
    newscript->GetAI = &GetAI_npc_geezleAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_nestlewood_owlkin";
    newscript->GetAI = &GetAI_mob_nestlewood_owlkinAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_death_ravager";
    newscript->GetAI = &GetAI_npc_death_ravagerAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_ravager_cage";
    newscript->pGOHello = &go_ravager_cage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_stillpine_capitive";
    newscript->GetAI = &GetAI_npc_stillpine_capitiveAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_bristlelimb_cage";
    newscript->pGOHello = &go_bristlelimb_cage;
    newscript->RegisterSelf();

}

