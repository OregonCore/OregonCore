#include "ScriptMgr.h"
#include "ScriptedCreature.h"

bool QuestComplete_Helculars_Grave(Player *player, GameObject *pGo, const Quest *_Quest)
{
    if (_Quest->GetQuestId() == 553)
    {
        Position pos;
        pos = pGo->GetPosition();
        Creature* Hellcular = pGo->SummonCreature(2433, pos, TEMPSUMMON_CORPSE_DESPAWN, 0);

        Hellcular->SetWalk(true);
        Hellcular->GetMotionMaster()->MovePoint(0, -811.053, -527.344, 483561, true);
        
        return true;
    }


    return true;
}


void AddSC_hillsbrad_foothills()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "go_helculars_grave";
    newscript->GoQuestComplete = &QuestComplete_Helculars_Grave;
    newscript->RegisterSelf();
}

