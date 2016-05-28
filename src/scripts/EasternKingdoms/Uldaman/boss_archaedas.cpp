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
SDName: boss_archaedas
SD%Complete: 100
SDComment: Archaedas is activated when 3 prople click on his altar.
Every 10 seconds he will awaken one of his minions along the wall.
At 66%, he will awaken the 6 Guardians.
At 33%, he will awaken the Vault Walkers
On his death the vault door opens.
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "uldaman.h"

#define SAY_AGGRO           "Who dares awaken Archaedas? Who dares the wrath of the makers!"
#define SOUND_AGGRO         5855

#define SAY_SUMMON          "Awake ye servants, defend the discs!"
#define SOUND_SUMMON        5856

#define SAY_SUMMON2         "To my side, brothers. For the makers!"
#define SOUND_SUMMON2       5857

#define SAY_KILL            "Reckless mortal."
#define SOUND_KILL            5858

#define SPELL_GROUND_TREMOR           6524
#define SPELL_ARCHAEDAS_AWAKEN        10347
#define SPELL_BOSS_OBJECT_VISUAL      11206
#define SPELL_BOSS_AGGRO              10340
#define SPELL_SUB_BOSS_AGGRO          11568
#define SPELL_AWAKEN_VAULT_WALKER     10258
#define SPELL_AWAKEN_EARTHEN_GUARDIAN 10252

struct boss_archaedasAI : public ScriptedAI
{
    boss_archaedasAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)me->GetInstanceData();
    }

    uint32 Tremor_Timer;
    int32  Awaken_Timer;
    uint32 WallMinionTimer;
    bool wakingUp;

    bool guardiansAwake;
    bool vaultWalkersAwake;
    ScriptedInstance* pInstance;

    void Reset()
    {
        Tremor_Timer = 60000;
        Awaken_Timer = 0;
        WallMinionTimer = 10000;

        wakingUp = false;
        guardiansAwake = false;
        vaultWalkersAwake = false;

        if (pInstance)
            pInstance->SetData (DATA_MINIONS, NOT_STARTED);    // respawn any dead minions
        me->setFaction(35);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		me->SetRooted(true);

    }

    void ActivateMinion (uint64 guid, bool flag)
    {
        Unit* minion = Unit::GetUnit(*me, guid);

        if (minion && minion->IsAlive())
        {
            DoCast (minion, SPELL_AWAKEN_VAULT_WALKER, flag);
            minion->CastSpell(minion, SPELL_ARCHAEDAS_AWAKEN, true);
        }
    }

    void EnterCombat(Unit* /*who*/)
    {
        me->setFaction(14);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		me->SetRooted(false);
    }

    void SpellHit (Unit* /*caster*/, const SpellEntry* spell)
    {
        // Being woken up from the altar, start the awaken sequence
        if (spell == GetSpellStore()->LookupEntry(SPELL_ARCHAEDAS_AWAKEN))
        {
            me->MonsterYell(SAY_AGGRO, LANG_UNIVERSAL, 0);
            DoPlaySoundToSet(me, SOUND_AGGRO);
            Awaken_Timer = 4000;
            wakingUp = true;
        }
    }

    void KilledUnit(Unit* /*victim*/)
    {
        me->MonsterYell(SAY_KILL, LANG_UNIVERSAL, 0);
        DoPlaySoundToSet(me, SOUND_KILL);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!pInstance)
            return;
        // we're still doing awaken animation
        if (wakingUp && Awaken_Timer >= 0)
        {
            Awaken_Timer -= diff;
            return;        // dont do anything until we are done
        }
        else if (wakingUp && Awaken_Timer <= 0)
        {
            wakingUp = false;
            AttackStart(Unit::GetUnit(*me, pInstance->GetData64(0)));
            return;     // dont want to continue until we finish the AttackStart method
        }

        //Return since we have no target
        if (!UpdateVictim())
            return;

        // wake a wall minion
        if (WallMinionTimer <= diff)
        {
            pInstance->SetData (DATA_MINIONS, IN_PROGRESS);

            WallMinionTimer = 10000;
        }
        else WallMinionTimer -= diff;

        //If we are <66 summon the guardians
        if (!guardiansAwake && me->GetHealth() * 100 / me->GetMaxHealth() <= 66)
        {
            ActivateMinion(pInstance->GetData64(5), true);  // EarthenGuardian1
            ActivateMinion(pInstance->GetData64(6), true);  // EarthenGuardian2
            ActivateMinion(pInstance->GetData64(7), true);  // EarthenGuardian3
            ActivateMinion(pInstance->GetData64(8), true);  // EarthenGuardian4
            ActivateMinion(pInstance->GetData64(9), true);  // EarthenGuardian5
            ActivateMinion(pInstance->GetData64(10), false); // EarthenGuardian6
            me->MonsterYell(SAY_SUMMON, LANG_UNIVERSAL, 0);
            DoPlaySoundToSet(me, SOUND_SUMMON);
            guardiansAwake = true;
        }

        //If we are <33 summon the vault walkers
        if (!vaultWalkersAwake && me->GetHealth() * 100 / me->GetMaxHealth() <= 33)
        {
            ActivateMinion(pInstance->GetData64(1), true);   // VaultWalker1
            ActivateMinion(pInstance->GetData64(2), true);   // VaultWalker2
            ActivateMinion(pInstance->GetData64(3), true);   // VaultWalker3
            ActivateMinion(pInstance->GetData64(4), false);   // VaultWalker4
            me->MonsterYell(SAY_SUMMON2, LANG_UNIVERSAL, 0);
            DoPlaySoundToSet(me, SOUND_SUMMON2);
            vaultWalkersAwake = true;
        }

        if (Tremor_Timer <= diff)
        {
            //Cast
            DoCastVictim( SPELL_GROUND_TREMOR);

            //45 seconds until we should cast this agian
            Tremor_Timer  = 45000;
        }
        else Tremor_Timer  -= diff;

        DoMeleeAttackIfReady();
    }

    void JustDied (Unit* /*killer*/)
    {
        if (pInstance)
        {
            pInstance->SetData(DATA_ANCIENT_DOOR, DONE);        // open the vault door
            pInstance->SetData(DATA_MINIONS, SPECIAL);        // deactivate his minions
        }
    }

};

CreatureAI* GetAI_boss_archaedas(Creature* pCreature)
{
    return GetInstanceAI<boss_archaedasAI>(pCreature);
}

/* ScriptData
SDName: mob_archaedas_minions
SD%Complete: 100
SDComment: These mobs are initially frozen until Archaedas awakens them
one at a time.
EndScriptData */

#define SPELL_ARCHAEDAS_AWAKEN        10347

struct mob_archaedas_minionsAI : public ScriptedAI
{
    mob_archaedas_minionsAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)me->GetInstanceData();
    }

    uint32 Arcing_Timer;
    int32 Awaken_Timer;
    bool wakingUp;

    bool amIAwake;
    ScriptedInstance* pInstance;

    void Reset()
    {
        Arcing_Timer = 3000;
        Awaken_Timer = 0;

        wakingUp = false;
        amIAwake = false;

        me->setFaction(35);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		me->SetRooted(true);
        me->RemoveAllAuras();
    }

    void EnterCombat(Unit* /*who*/)
    {
        me->setFaction(14);
        me->RemoveAllAuras();
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		me->SetRooted(false);
        amIAwake = true;
    }

    void SpellHit (Unit* /*caster*/, const SpellEntry* spell)
    {
        // time to wake up, start animation
        if (spell == GetSpellStore()->LookupEntry(SPELL_ARCHAEDAS_AWAKEN))
        {
            Awaken_Timer = 5000;
            wakingUp = true;
        }
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (amIAwake)
            ScriptedAI::MoveInLineOfSight(who);
    }

    void UpdateAI(const uint32 diff)
    {
        // we're still in the awaken animation
        if (wakingUp && Awaken_Timer >= 0)
        {
            Awaken_Timer -= diff;
            return;        // dont do anything until we are done
        }
        else if (wakingUp && Awaken_Timer <= 0)
        {
            wakingUp = false;
            amIAwake = true;
            AttackStart(Unit::GetUnit(*me, pInstance->GetData64(0))); // whoWokeArchaedasGUID
            return;     // dont want to continue until we finish the AttackStart method
        }

        //Return since we have no target
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_archaedas_minions(Creature* pCreature)
{
    return GetInstanceAI<mob_archaedas_minionsAI>(pCreature);
}

/* ScriptData
SDName: go_altar_archaedas
SD%Complete: 100
SDComment: Needs 3 people to activate the Archaedas script
SDCategory: Uldaman
EndScriptData */

#define OBJECT_ALTAR_OF_ARCHAEDAS   133234

#define NUMBER_NEEDED_TO_ACTIVATE 3

#define SPELL_BOSS_OBJECT_VISUAL    11206

uint64 altarOfArchaedasCount[5];
int32 altarOfArchaedasCounter = 0;


bool GOHello_go_altar_of_archaedas(Player* player, GameObject* go)
{
    bool alreadyUsed;
    go->AddUse ();

    alreadyUsed = false;
    for (uint32 loop = 0; loop < 5; loop++)
    {
        if (altarOfArchaedasCount[loop] == player->GetGUID()) alreadyUsed = true;
    }
    if (!alreadyUsed)
        altarOfArchaedasCount[altarOfArchaedasCounter++] = player->GetGUID();

    player->CastSpell (player, SPELL_BOSS_OBJECT_VISUAL, false);

    if (altarOfArchaedasCounter < NUMBER_NEEDED_TO_ACTIVATE)
    {
        return false;        // not enough people yet
    }

    // Check to make sure at least three people are still casting
    uint32 count = 0;
    Unit* pTarget;
    for (uint32 x = 0; x < 5; x++)
    {
        pTarget = Unit::GetUnit(*player, altarOfArchaedasCount[x]);
        if (!pTarget) continue;
        if (pTarget->IsNonMeleeSpellCast(true)) count++;
        if (count >= NUMBER_NEEDED_TO_ACTIVATE) break;
    }

    if (count < NUMBER_NEEDED_TO_ACTIVATE)
    {
        return false;            // not enough people
    }

    ScriptedInstance* pInstance = (ScriptedInstance*)player->GetInstanceData();
    if (!pInstance) return false;
    pInstance->SetData64(0, player->GetGUID());    // activate archaedas

    return false;
}

/* ScriptData
SDName: mob_stonekeepers
SD%Complete: 100
SDComment: After activating the altar of the keepers, the stone keepers will
wake up one by one.
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#define SPELL_SELF_DESTRUCT          9874

struct mob_stonekeepersAI : public ScriptedAI
{
    mob_stonekeepersAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)me->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    void Reset()
    {
        me->setFaction(35);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		me->SetRooted(true);
        me->RemoveAllAuras();
    }

    void EnterCombat(Unit* /*who*/)
    {
        me->setFaction(14);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		me->SetRooted(false);
    }

    void UpdateAI(const uint32 /*diff*/)
    {

        //Return since we have no target
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* /*attacker*/)
    {
        DoCast (me, SPELL_SELF_DESTRUCT, true);
        if (pInstance)
            pInstance->SetData(DATA_STONE_KEEPERS, IN_PROGRESS);    // activate next stonekeeper
    }

};

CreatureAI* GetAI_mob_stonekeepers(Creature* pCreature)
{
    return GetInstanceAI<mob_stonekeepersAI>(pCreature);
}

/* ScriptData
SDName: go_altar_of_the_keepers
SD%Complete: 100
SDComment: Need 3 people to activate to open the altar.  One by one the StoneKeepers will activate.  After all four are dead than the door will open.
SDCategory: Uldaman
EndScriptData */

#define SPELL_BOSS_OBJECT_VISUAL    11206

#define NUMBER_NEEDED_TO_ACTIVATE 3

static uint64 altarOfTheKeeperCount[5];
static uint32 altarOfTheKeeperCounter = 0;

bool GOHello_go_altar_of_the_keepers(Player* pPlayer, GameObject* pGo)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pPlayer->GetInstanceData();
    if (!pInstance)
        return true;

    bool alreadyUsed;

    pGo->AddUse();

    alreadyUsed = false;
    for (uint32 loop = 0; loop < 5; ++loop)
    {
        if (altarOfTheKeeperCount[loop] == pPlayer->GetGUID())
            alreadyUsed = true;
    }
    if (!alreadyUsed && altarOfTheKeeperCounter < 5)
        altarOfTheKeeperCount[altarOfTheKeeperCounter++] = pPlayer->GetGUID();
    pPlayer->CastSpell (pPlayer, SPELL_BOSS_OBJECT_VISUAL, false);

    if (altarOfTheKeeperCounter < NUMBER_NEEDED_TO_ACTIVATE)
    {
        //error_log("not enough people yet, altarOfTheKeeperCounter = %d", altarOfTheKeeperCounter);
        return false; // not enough people yet
    }

    // Check to make sure at least three people are still casting
    uint8 count = 0;
    Unit* pTarget;
    for (uint8 x = 0; x < 5; ++x)
    {
        pTarget = Unit::GetUnit(*pPlayer, altarOfTheKeeperCount[x]);
        //error_log("number of people currently activating it: %d", x+1);
        if (!pTarget)
            continue;
        if (pTarget->IsNonMeleeSpellCast(true))
            ++count;
        if (count >= NUMBER_NEEDED_TO_ACTIVATE)
            break;
    }

    if (count < NUMBER_NEEDED_TO_ACTIVATE)
    {
        //error_log("still not enough people");
        return true; // not enough people
    }

    //error_log ("activating stone keepers");
    pInstance->SetData(DATA_STONE_KEEPERS, IN_PROGRESS);        // activate the Stone Keepers
    return true;
}

void AddSC_boss_archaedas()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_archaedas";
    newscript->GetAI = &GetAI_boss_archaedas;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_altar_of_archaedas";
    newscript->pGOHello = &GOHello_go_altar_of_archaedas;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_archaedas_minions";
    newscript->GetAI = &GetAI_mob_archaedas_minions;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_altar_of_the_keepers";
    newscript->pGOHello = &GOHello_go_altar_of_the_keepers;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_stonekeepers";
    newscript->GetAI = &GetAI_mob_stonekeepers;
    newscript->RegisterSelf();
}

