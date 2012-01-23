/*
 * Copyright (C) 2010-2012 OregonCore <http://www.oregoncore.com/>
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
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

#ifndef _TICKETMGR_H
#define _TICKETMGR_H

#include "Policies/Singleton.h"
#include "Database/DatabaseEnv.h"
#include "Util.h"
#include "Config/Config.h"
#include <map>

enum GMTicketOpenedByGMStatus
{
    GMTICKET_OPENEDBYGM_STATUS_NOT_OPENED = 0,      // ticket has never been opened by a gm
    GMTICKET_OPENEDBYGM_STATUS_OPENED = 1,          // ticket has been opened by a gm
};

struct GM_Ticket
{
    uint64 guid;
    uint64 playerGuid;
    std::string name;
    float pos_x;
    float pos_y;
    float pos_z;
    uint32 map;
    std::string message;
    uint64 createtime;
    uint64 timestamp;
    uint64 closed;
    uint64 assignedToGM;
    std::string comment;
    bool escalated;
    bool viewed;
};

// Map Typedef
typedef std::list<GM_Ticket*>                                       GmTicketList;

class TicketMgr
{
    public:
        TicketMgr(){ InitTicketID();}    //constructor
        ~TicketMgr(){}  //destructor

        // Object Holder
        GmTicketList         GM_TicketList;

        void AddGMTicket(GM_Ticket *ticket, bool startup);
        void DeleteAllRemovedGMTickets();
        void DeleteGMTicketPermanently(uint64 ticketGuid);
        void LoadGMTickets();
        void LoadGMSurveys();
        void RemoveGMTicketByPlayer(uint64 playerGuid, uint64 GMguid);
        void RemoveGMTicket(uint64 ticketGuid, uint64 GMguid);
        void UpdateGMTicket(GM_Ticket *ticket);
        void SaveGMTicket(GM_Ticket* ticket);

        uint64 GenerateTicketID();
        void InitTicketID();
        GM_Ticket* GetGMTicket(uint64 ticketGuid);
        GM_Ticket* GetGMTicketByPlayer(uint64 playerGuid);
        GM_Ticket* GetGMTicketByName(const char *name);
        uint64 GetNextSurveyID()
        {
            return ++m_GMSurveyID;
        }


    protected:
        uint64 m_ticketid;
        uint64 m_GMSurveyID;
};

#endif
#define ticketmgr Oregon::Singleton<TicketMgr>::Instance()

