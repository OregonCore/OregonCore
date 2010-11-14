#ifndef DEF_DEADMINES_H
#define DEF_DEADMINES_H

#include "ScriptPCH.h"

enum CannonState
{
    CANNON_NOT_USED,
    CANNON_GUNPOWDER_USED,
    CANNON_BLAST_INITIATED,
    PIRATES_ATTACK,
    EVENT_DONE
};

enum Data
{
    EVENT_CANNON,
    EVENT_RHAHKZOR,
    EVENT_SNEED,
    EVENT_GILNID
};

enum GameObjects
{
    GO_FACTORY_DOOR                                        = 13965,
    GO_MAST_ROOM_DOOR                                      = 16400,
    GO_FOUNDRY_DOOR                                        = 16399,
    GO_IRONCLAD_DOOR                                       = 16397,
    GO_DEFIAS_CANNON                                       = 16398,
    GO_DOOR_LEVER                                          = 101833,
    GO_MR_SMITE_CHEST                                      = 144111
};
#endif

