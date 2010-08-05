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
SDName: Boss_Felblood_Kaelthas
SD%Complete: 80
SDComment: Normal and Heroic Support. Issues: Arcane Spheres do not initially follow targets. TODO: Convert Phoenix to ACID.
SDCategory: Magisters' Terrace
EndScriptData */

#include "precompiled.h"
#include "def_magisters_terrace.h"
#include "WorldPacket.h"

#define SAY_AGGRO                   -1585023                //This yell should be done when the room is cleared. For now, set it as aggro yell.
#define SAY_PHOENIX                 -1585024
#define SAY_FLAMESTRIKE             -1585025
#define SAY_GRAVITY_LAPSE           -1585026
#define SAY_TIRED                   -1585027
#define SAY_RECAST_GRAVITY          -1585028
#define SAY_DEATH                   -1585029

/*** Spells ***/

// Phase 1 spells

#define SPELL_FIREBALL_NORMAL         44189                 // Deals 2700-3300 damage at current target
#define SPELL_FIREBALL_HEROIC         46164                 //       4950-6050

#define SPELL_PHOENIX                 44194                 // Summons a phoenix (Doesn't work?)
#define SPELL_PHOENIX_BURN            44198                 // A spell Phoenix uses to damage everything around
#define SPELL_PHOENIX_FIREBALL        44202                 // Phoenix casts this in phase 2 and stops moving

#define SPELL_FLAMESTRIKE1_NORMAL     44190                 // Damage part
#define SPELL_FLAMESTRIKE1_HEROIC     46163                 // Heroic damage part
#define SPELL_FLAMESTRIKE2            44191                 // Flamestrike indicator before the damage
#define SPELL_FLAMESTRIKE3            44192                 // Summons the trigger + animation (projectile)

#define SPELL_SHOCK_BARRIER           46165                 // Heroic only; 10k damage shield, followed by Pyroblast
#define SPELL_PYROBLAST               36819                 // Heroic only; 45-55k fire damage

// Phase 2 spells

#define SPELL_GRAVITY_LAPSE_INITIAL   44224                 // Cast at the beginning of every Gravity Lapse
#define SPELL_GRAVITY_LAPSE_CHANNEL   44251                 // Channeled; blue beam animation to every enemy in range
#define SPELL_TELEPORT_CENTER         44218                 // Should teleport people to the center. Requires DB entry in spell_target_position.
#define SPELL_GRAVITY_LAPSE_FLY       44227                 // Hastens flyspeed and allows flying for 1 minute. For some reason removes 44226.
#define SPELL_GRAVITY_LAPSE_DOT       44226                 // Knocks up in the air and applies a 300 DPS DoT.
#define SPELL_ARCANE_SPHERE_PASSIVE   44263                 // Passive auras on Arcane Spheres
#define SPELL_POWER_FEEDBACK          44233                 // Stuns him, making him take 50% more damage for 10 seconds. Cast after Gravity Lapse

/*** Creatures ***/
#define CREATURE_PHOENIX              24674
#define CREATURE_PHOENIX_EGG          24675
#define CREATURE_ARCANE_SPHERE        24708

/** Locations **/
float KaelLocations[6][2]=
{
    {148.744659, 181.377426},//center
    {140.823883, 195.403046},//phoenixpos1
    {156.574188, 195.650482},//phoenixpos2
    {149.813, 160.917},//spherepos1
    {167.223, 173.594},//spherepos2
    {130.68, 173.007},//spherepos3
};
#define LOCATION_Z      -16.727455

struct OREGON_DLL_DECL boss_felblood_kaelthasAI : public ScriptedAI
{
    boss_felblood_kaelthasAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();        Heroic = c->GetMap()->IsHeroic();
    }

    ScriptedInstance* pInstance;

    uint32 FireballTimer;
    uint32 PhoenixTimer;
    uint32 FlameStrikeTimer;
    uint32 CombatPulseTimer;

    //Heroic only
    uint32 PyroblastTimer;

    uint32 GravityLapseTimer;
    uint32 GravityLapsePhase;
    // 0 = No Gravity Lapse
    // 1 = Casting Gravity Lapse visual
    // 2 = Teleported people to self
    // 3 = Knocked people up in the air
    // 4 = Applied an aura that allows them to fly, channeling visual, relased Arcane Orbs.

    bool FirstGravityLapse;
    bool Heroic;

    uint8 Phase;
    // 0 = Not started
    // 1 = Fireball; Summon Phoenix; Flamestrike
    // 2 = Gravity Lapses

    void Reset()
    {
        // TODO: Timers
        FireballTimer = 0;
        PhoenixTimer = 30000;
        FlameStrikeTimer = 25000;
        CombatPulseTimer = 0;

        PyroblastTimer = 60000;

        GravityLapseTimer = 0;
        GravityLapsePhase = 0;

        FirstGravityLapse = true;

        Phase = 0;

        if (pInstance)
        {
            if (me->isDead())
                pInstance->SetData(DATA_KAELTHAS_EVENT, DONE);
            else
                pInstance->SetData(DATA_KAELTHAS_EVENT, NOT_STARTED);
        }
    }

    void KilledUnit(Unit* victim)
    {
        if (victim && (victim->GetTypeId() == TYPEID_PLAYER))
        {
            victim->RemoveAurasDueToSpell(SPELL_GRAVITY_LAPSE_FLY);
            victim->RemoveAurasDueToSpell(SPELL_GRAVITY_LAPSE_DOT);
        }
    }
    void JustDied(Unit *killer)
    {
        RemoveGravityLapse();
        DoScriptText(SAY_DEATH, me);
        if (pInstance)
            pInstance->SetData(DATA_KAELTHAS_EVENT, DONE);
    }

    void DamageTaken(Unit* done_by, uint32 &damage)
    {
        if (damage > me->GetHealth())
            RemoveGravityLapse();                           // Remove Gravity Lapse so that players fall to ground if they kill him when in air.
    }

    void EnterCombat(Unit *who)
    {
        DoScriptText(SAY_AGGRO, me);
        if (pInstance)
            pInstance->SetData(DATA_KAELTHAS_EVENT, IN_PROGRESS);
    }

    void SetThreatList(Creature* SummonedUnit)
    {
        if (!SummonedUnit)
            return;

        std::list<HostileReference*>& m_threatlist = me->getThreatManager().getThreatList();
        std::list<HostileReference*>::iterator i = m_threatlist.begin();
        for (i = m_threatlist.begin(); i != m_threatlist.end(); i++)
        {
            Unit* pUnit = Unit::GetUnit((*me), (*i)->getUnitGuid());
            if (pUnit && pUnit->isAlive())
            {
                float threat = DoGetThreat(pUnit);
                SummonedUnit->AddThreat(pUnit, 0.1f);
            }
        }
    }

    void EnterEvadeMode()
    {
        RemoveGravityLapse();
        me->InterruptNonMeleeSpells(true);
        me->RemoveAllAuras();
        me->DeleteThreatList();
        me->CombatStop(true);
        me->LoadCreaturesAddon();

        if (me->isAlive())
            me->GetMotionMaster()->MoveTargetedHome();

        me->SetLootRecipient(NULL);

        Reset();
    }

    void TeleportPlayersToSelf()
    {
        float x,y,z;
        me->Relocate(KaelLocations[0][0], KaelLocations[0][1], LOCATION_Z, 0);
        Map *map = me->GetMap();
        Map::PlayerList const &PlayerList = map->GetPlayers();
        Map::PlayerList::const_iterator i;
        for (i = PlayerList.begin(); i != PlayerList.end(); ++i)
        {
            if (Player* i_pl = i->getSource())
                if (i_pl->isAlive())
                {
                    i_pl->CastSpell(i_pl, SPELL_TELEPORT_CENTER, true);
                    me->GetNearPoint(me,x,y,z,5,5,0);
                    i_pl->TeleportTo(me->GetMapId(),x,y,LOCATION_Z,i_pl->GetOrientation());
                }
        }
        DoCast(me, SPELL_TELEPORT_CENTER, true);
    }

    void CastGravityLapseKnockUp()
    {
        Map *map = me->GetMap();
        Map::PlayerList const &PlayerList = map->GetPlayers();
        Map::PlayerList::const_iterator i;
        for (i = PlayerList.begin(); i != PlayerList.end(); ++i)
        {
            if (Player* i_pl = i->getSource())
                if (i_pl->isAlive())
                // Knockback into the air
                    i_pl->CastSpell(i_pl, SPELL_GRAVITY_LAPSE_DOT, true, 0, 0, me->GetGUID());
        }
    }

    // players can't cast "fly" spells unless in map 530. Has to be done a while after they get knocked into the air...
    void CastGravityLapseFly()
    {
        Map *map = me->GetMap();
        Map::PlayerList const &PlayerList = map->GetPlayers();
        Map::PlayerList::const_iterator i;
        for (i = PlayerList.begin(); i != PlayerList.end(); ++i)
        {
            if (Player* i_pl = i->getSource())
            {
                if (i_pl->isAlive())
                {
                    // Also needs an exception in spell system.
                    i_pl->CastSpell(i_pl, SPELL_GRAVITY_LAPSE_FLY, true, 0, 0, me->GetGUID());
                    i_pl->SetSpeed(MOVE_FLIGHT, 2.0f);
                }
            }
        }
    }

    void RemoveGravityLapse()
    {
        Map *map = me->GetMap();
        Map::PlayerList const &PlayerList = map->GetPlayers();
        Map::PlayerList::const_iterator i;
        for (i = PlayerList.begin(); i != PlayerList.end(); ++i)
        {
            if (Player* i_pl = i->getSource())
            {
                i_pl->RemoveAurasDueToSpell(SPELL_GRAVITY_LAPSE_FLY);
                i_pl->RemoveAurasDueToSpell(SPELL_GRAVITY_LAPSE_DOT);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        switch(Phase)
        {
            case 0:
            {
                // *Heroic mode only:
                if (Heroic)
                {
                    if (PyroblastTimer <= diff)
                    {
                        DoCast(me, SPELL_SHOCK_BARRIER, true);
                        DoCast(me->getVictim(), SPELL_PYROBLAST);
                        PyroblastTimer = 60000;
                    } else PyroblastTimer -= diff;
                }

                if (FireballTimer <= diff)
                {
                    DoCast(me->getVictim(), Heroic ? SPELL_FIREBALL_HEROIC : SPELL_FIREBALL_NORMAL);
                    FireballTimer = 2000 + rand()%4000;
                } else FireballTimer -= diff;

                if (PhoenixTimer <= diff)
                {
                    uint32 random = rand()%2 + 1;
                    float x = KaelLocations[random][0];
                    float y = KaelLocations[random][1];
                    Creature* Phoenix = me->SummonCreature(CREATURE_PHOENIX, x, y, LOCATION_Z, 0, TEMPSUMMON_CORPSE_DESPAWN, 60000);
                    if (Phoenix)
                    {
                        Phoenix->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE + UNIT_FLAG_NON_ATTACKABLE);
                        SetThreatList(Phoenix);
                        Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM,1);
                        if (pTarget)
                        {
                            Phoenix->AddThreat(pTarget,1000);
                            Phoenix->Attack(pTarget,true);
                            Phoenix->GetMotionMaster()->MoveChase(pTarget);
                        }
                        else
                        {
                            Phoenix->AddThreat(me->getVictim(),1000);
                            Phoenix->Attack(me->getVictim(),true);
                            Phoenix->GetMotionMaster()->MoveChase(me->getVictim());
                        }
                    }

                    DoScriptText(SAY_PHOENIX, me);

                    PhoenixTimer = 40000;
                } else PhoenixTimer -= diff;

                if (FlameStrikeTimer <= diff)
                {
                    if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    {
                        me->InterruptNonMeleeSpells(false);
                        DoCast(pTarget, SPELL_FLAMESTRIKE3, true);
                        DoScriptText(SAY_FLAMESTRIKE, me);

                        FlameStrikeTimer = 20000 + rand()%5000;
                    }
                } else FlameStrikeTimer -= diff;

                // Below 50%
                if (me->GetMaxHealth() * 0.5 > me->GetHealth())
                {
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveIdle();
                    GravityLapseTimer = 0;
                    GravityLapsePhase = 0;
                    Phase = 1;
                }
                DoMeleeAttackIfReady();
            }
            break;

            case 1:
            {
                me->StopMoving();
                if (GravityLapseTimer <= diff)
                {
                    switch(GravityLapsePhase)
                    {
                        case 0:
                            if (FirstGravityLapse)           // Different yells at 50%, and at every following Gravity Lapse
                            {
                                DoScriptText(SAY_GRAVITY_LAPSE, me);
                                FirstGravityLapse = false;
                                if (pInstance)
                                {
                                    GameObject* KaelLeft = GameObject::GetGameObject(*me, pInstance->GetData64(DATA_KAEL_STATUE_LEFT));
                                    if (KaelLeft) KaelLeft->SetGoState(GO_STATE_ACTIVE);
                                    GameObject* KaelRight = GameObject::GetGameObject(*me, pInstance->GetData64(DATA_KAEL_STATUE_RIGHT));
                                    if (KaelRight) KaelRight->SetGoState(GO_STATE_ACTIVE);
                                }
                            } else
                            {
                                DoScriptText(SAY_RECAST_GRAVITY, me);
                            }
                            me->StopMoving();
                            DoCast(me, SPELL_GRAVITY_LAPSE_INITIAL);
                            GravityLapseTimer = 2000 + diff;// Don't interrupt the visual spell
                            GravityLapsePhase = 1;
                            break;

                        case 1:
                            TeleportPlayersToSelf();
                            GravityLapseTimer = 1000;
                            GravityLapsePhase = 2;
                            break;

                        case 2:
                            CastGravityLapseKnockUp();
                            GravityLapseTimer = 1000;
                            GravityLapsePhase = 3;
                            break;

                        case 3:
                            CastGravityLapseFly();
                            GravityLapseTimer = 30000;
                            GravityLapsePhase = 4;
                            for (uint8 i = 0; i < 3; ++i)
                            {
                                Creature* Orb = me->SummonCreature(CREATURE_ARCANE_SPHERE,KaelLocations[3+i][0],KaelLocations[3+i][1],LOCATION_Z,0,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,30000);
                                if (Orb)
                                {
                                    SetThreatList(Orb);
                                    Unit *pTarget = SelectUnit(SELECT_TARGET_BOTTOMAGGRO,i);
                                    if (pTarget)
                                    {
                                        Orb->AddThreat(pTarget,1000);
                                        Orb->Attack(pTarget,true);
                                        Orb->GetMotionMaster()->MoveChase(pTarget);
                                    }
                                    else
                                    {
                                        Unit *ntarget = SelectUnit(SELECT_TARGET_RANDOM,0);
                                        if (ntarget)
                                        {
                                            Orb->AddThreat(ntarget,1000);
                                            Orb->Attack(ntarget,true);
                                            Orb->GetMotionMaster()->MoveChase(ntarget);
                                        }
                                    }
                                }
                            }
                            DoCast(me, SPELL_GRAVITY_LAPSE_CHANNEL);
                            break;

                        case 4:
                            me->InterruptNonMeleeSpells(false);
                            DoScriptText(SAY_TIRED, me);
                            DoCast(me, SPELL_POWER_FEEDBACK);
                            RemoveGravityLapse();
                            GravityLapseTimer = 10000;
                            GravityLapsePhase = 0;
                            break;
                    }
                } else GravityLapseTimer -= diff;
            }
            break;
        }
    }
};

struct OREGON_DLL_DECL mob_felkael_flamestrikeAI : public ScriptedAI
{
    mob_felkael_flamestrikeAI(Creature *c) : ScriptedAI(c)
    {
        Heroic = c->GetMap()->IsHeroic();
    }

    uint32 FlameStrikeTimer;
    bool Heroic;

    void Reset()
    {
        FlameStrikeTimer = 5000;

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->setFaction(14);

        DoCast(me, SPELL_FLAMESTRIKE2, true);
    }

    void EnterCombat(Unit *who) {}
    void MoveInLineOfSight(Unit *who) {}
    void UpdateAI(const uint32 diff)
    {
        if (FlameStrikeTimer <= diff)
        {
            DoCast(me, Heroic ? SPELL_FLAMESTRIKE1_HEROIC : SPELL_FLAMESTRIKE1_NORMAL, true);
            me->DealDamage(me, me->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        } else FlameStrikeTimer -= diff;
    }
};

struct OREGON_DLL_DECL mob_felkael_phoenixAI : public ScriptedAI
{
    mob_felkael_phoenixAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }
    uint32 BurnTimer;
    uint32 CheckTimer;
    uint8 phase;
    ScriptedInstance* pInstance;
    bool end;

    void Reset()
    {
        me->SetSpeed(MOVE_RUN, 0.5f);
        me->SetSpeed(MOVE_WALK, 0.5f);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE + UNIT_FLAG_NON_ATTACKABLE);
        BurnTimer = 2000;
        CheckTimer = 1000;
        phase = 0;
        end = false;
    }

    void EnterCombat(Unit* who) {}

    void JustDied(Unit* slayer)
    {
        if (end)
            return;
        DoSpawnCreature(CREATURE_PHOENIX_EGG, 0, 0, 0, 0, TEMPSUMMON_CORPSE_DESPAWN, 45000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (CheckTimer <= diff)
        {
            if (pInstance)
            {
                Creature *boss = (Unit::GetCreature((*me),pInstance->GetData64(DATA_KAEL)));
                if (boss)
                {
                    phase = ((boss_felblood_kaelthasAI*)boss->AI())->Phase;
                    if (boss->isDead() || !boss->isInCombat())
                    {
                        end = true;
                        me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_FIRE, NULL, false);//temphack, hellfire is not damaging self
                    }
                }
            }
            CheckTimer = 1000;
        } else CheckTimer -= diff;

        if (!UpdateVictim())
            return;

        if (BurnTimer <= diff)
        {
            if (!phase)
            {
                DoCast(me, SPELL_PHOENIX_BURN);
                me->DealDamage(me, 1500, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_FIRE, NULL, false);//temphack, hellfire is not damaging self
            }
            else
            {
                me->StopMoving();
                DoCast(me->getVictim(), SPELL_PHOENIX_FIREBALL);
            }
            BurnTimer = 2000;
        } else BurnTimer -= diff;

        //DoMeleeAttackIfReady();
    }
};

struct OREGON_DLL_DECL mob_felkael_phoenix_eggAI : public Scripted_NoMovementAI
{
    mob_felkael_phoenix_eggAI(Creature *c) : Scripted_NoMovementAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    uint32 HatchTimer;
    ScriptedInstance* pInstance;
    void Reset() {   HatchTimer = 15000;   }

    void EnterCombat(Unit* who) {}
    void MoveInLineOfSight(Unit* who) {}
    void UpdateAI(const uint32 diff)
    {

        if (HatchTimer <= diff)
        {
            Creature *bird = DoSpawnCreature(CREATURE_PHOENIX, 0, 0, 0, 0, TEMPSUMMON_CORPSE_DESPAWN, 60000);
            if (bird)
            {
                Unit *boss = Unit::GetUnit((*me),pInstance->GetData64(DATA_KAEL));
                if (boss && boss->getVictim())
                {
                    bird->AddThreat(boss->getVictim(),100);
                    bird->Attack(boss->getVictim(),true);
                    bird->GetMotionMaster()->MoveChase(boss->getVictim());
                }
            }
            me->DealDamage(me, me->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        } else HatchTimer -= diff;
    }
};

struct OREGON_DLL_DECL mob_arcane_sphereAI : public ScriptedAI
{
    mob_arcane_sphereAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }
    uint32 DespawnTimer;
    uint32 ChangeTargetTimer;
    uint32 CheckTimer;

    ScriptedInstance* pInstance;

    void Reset()
    {
        me->SetUnitMovementFlags(MOVEMENTFLAG_LEVITATING);
        DespawnTimer = 30000;
        ChangeTargetTimer = 5000;
        CheckTimer = 1000;
        me->SetSpeed(MOVE_RUN, 0.5f);
        me->SetSpeed(MOVE_WALK, 0.5f);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->setFaction(14);
        DoCast(me, SPELL_ARCANE_SPHERE_PASSIVE, true);
    }

    void EnterCombat(Unit* who) {}

    void StalkTarget(Unit *pTarget)
    {
        if (!pTarget)
            return;
        me->AddThreat(pTarget,100000);
        me->GetMotionMaster()->MoveChase(pTarget);
        me->Attack(pTarget,true);
    }

    void UpdateAI(const uint32 diff)
    {
        if (DespawnTimer <= diff)
            me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        else DespawnTimer -= diff;

        if (!UpdateVictim())
            ChangeTargetTimer = 0;

        if (ChangeTargetTimer <= diff)
        {
            DoResetThreat();
            Unit *ntarget = SelectUnit(SELECT_TARGET_RANDOM,0);
            if (ntarget)
                StalkTarget(ntarget);
            ChangeTargetTimer = 10000;
        } else ChangeTargetTimer -= diff;

        if (CheckTimer <= diff)
        {
            if (pInstance)
            {
                Creature *boss = Unit::GetCreature((*me),pInstance->GetData64(DATA_KAEL));
                if (boss)
                {
                    if (!((boss_felblood_kaelthasAI*)boss->AI())->Phase || boss->isDead())
                        DespawnTimer = 0;
                } else DespawnTimer = 0;
            }
            CheckTimer = 1000;
        } else CheckTimer -= diff;
    }
};

bool GOHello_go_kael_orb(Player *player, GameObject* _GO)
{
    ScriptedInstance* pInst = _GO->GetInstanceData();
    if (pInst && player)
    {
        Unit *kael = Unit::GetUnit((*_GO),pInst->GetData64(DATA_KAEL));
        if (kael && kael->isDead())
            player->TeleportTo(530, 12888, -6876, 9, 0.3);
    }
    return true;
}

bool GOHello_go_movie_orb(Player *player, GameObject* _GO)
{
    if (player)
    {
        WorldPacket data(SMSG_TRIGGER_CINEMATIC, 4);
        data << (uint32)164;
        player->GetSession()->SendPacket(&data);

        if (player->GetQuestStatus(11490) == QUEST_STATUS_INCOMPLETE)
        {
            Unit *qUnit = player->SummonCreature(25042,player->GetPositionX(),player->GetPositionY(),player->GetPositionZ()-10,0,TEMPSUMMON_CORPSE_DESPAWN,0);
            if (qUnit)
                player->DealDamage(qUnit, qUnit->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }
    return true;
}

CreatureAI* GetAI_boss_felblood_kaelthas(Creature* c)
{
    return new boss_felblood_kaelthasAI(c);
}

CreatureAI* GetAI_mob_arcane_sphere(Creature* c)
{
    return new mob_arcane_sphereAI(c);
}

CreatureAI* GetAI_mob_felkael_phoenix(Creature* c)
{
    return new mob_felkael_phoenixAI(c);
}

CreatureAI* GetAI_mob_felkael_phoenix_egg(Creature* c)
{
    return new mob_felkael_phoenix_eggAI(c);
}

CreatureAI* GetAI_mob_felkael_flamestrike(Creature* c)
{
    return new mob_felkael_flamestrikeAI(c);
}

void AddSC_boss_felblood_kaelthas()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_felblood_kaelthas";
    newscript->GetAI = &GetAI_boss_felblood_kaelthas;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_arcane_sphere";
    newscript->GetAI = &GetAI_mob_arcane_sphere;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_felkael_phoenix";
    newscript->GetAI = &GetAI_mob_felkael_phoenix;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_felkael_phoenix_egg";
    newscript->GetAI = &GetAI_mob_felkael_phoenix_egg;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_felkael_flamestrike";
    newscript->GetAI = &GetAI_mob_felkael_flamestrike;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_kael_orb";
    newscript->pGOHello = &GOHello_go_kael_orb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_movie_orb";
    newscript->pGOHello = &GOHello_go_movie_orb;
    newscript->RegisterSelf();
}

