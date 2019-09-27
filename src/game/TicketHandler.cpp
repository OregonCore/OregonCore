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

#include "Language.h"
#include "ObjectAccessor.h"
#include "WorldPacket.h"
#include "Common.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Chat.h"
#include "TicketMgr.h"
#include "World.h"

void WorldSession::HandleGMTicketCreateOpcode(WorldPacket& recv_data)
{
    // Let's see if we have a ticket already, if so don't create a new one
    if (ticketmgr.GetGMTicketByPlayer(GetPlayer()->GetGUID()))
    {
        WorldPacket data(SMSG_GMTICKET_CREATE, 4);
        data << uint32(1); // You already have an open ticket.
        SendPacket (&data);
        return;
    }

    uint32 map;
    float x, y, z;
    std::string ticketText = "";
    std::string ticketText2 = "";
    GM_Ticket* ticket = new GM_Ticket;

    WorldPacket data(SMSG_GMTICKET_CREATE, 4);

    // recv Data
    //@todo Add map coordinates to tickets.
    recv_data >> map;
    recv_data >> x;
    recv_data >> y;
    recv_data >> z;
    recv_data >> ticketText;

    // get additional data, rarely used
    recv_data >> ticketText2;

    // assign values
    ticket->name = GetPlayer()->GetName();
    ticket->guid = ticketmgr.GenerateTicketID();
    ticket->playerGuid = GetPlayer()->GetGUID();
    ticket->message = ticketText;
    ticket->createtime = time(NULL);
    ticket->map = map;
    ticket->pos_x = x;
    ticket->pos_y = y;
    ticket->pos_z = z;
    ticket->timestamp = time(NULL);
    ticket->closed = 0;
    ticket->assignedToGM = 0;
    ticket->comment = "";
    ticket->escalated = false;
    ticket->viewed = false;

    // remove ticket by player, shouldn't happen
    ticketmgr.RemoveGMTicketByPlayer(GetPlayer()->GetGUID(), GetPlayer()->GetGUID());

    // add ticket
    ticketmgr.AddGMTicket(ticket, false);

    // Response - no errors
    data << uint32(2);

    // Send ticket creation
    SendPacket(&data);

    sWorld.SendGMText(LANG_COMMAND_TICKETNEW, ticket->name.c_str(), ticket->guid);

}

void WorldSession::HandleGMTicketUpdateOpcode(WorldPacket& recv_data)
{
    std::string message = "";
    time_t t = time(NULL);

    WorldPacket data(SMSG_GMTICKET_UPDATETEXT, 4);

    // recv Data
    recv_data >> message;

    // Update Ticket
    GM_Ticket* ticket = ticketmgr.GetGMTicketByPlayer(GetPlayer()->GetGUID());

    // Check if player has a GM Ticket yet
    if (!ticket)
    {
        // Response - error couldnt find existing Ticket
        data << uint32(1);

        // Send packet
        SendPacket(&data);
        return;
    }

    ticket->message = message;
    ticket->timestamp = (uint32)t;

    ticketmgr.UpdateGMTicket(ticket);

    // Response - no errors
    data << uint32(2);

    // Send packet
    SendPacket(&data);

    sWorld.SendGMText(LANG_COMMAND_TICKETUPDATED, GetPlayer()->GetName(), ticket->guid);

}

void WorldSession::HandleGMTicketDeleteOpcode(WorldPacket& /*recv_data*/)
{
    // NO recv_data, NO packet check size

    GM_Ticket* ticket = ticketmgr.GetGMTicketByPlayer(GetPlayer()->GetGUID());

    // CHeck for Ticket
    if (ticket)
    {
        // Remove Tickets from Player

        // Response - no errors
        WorldPacket data(SMSG_GMTICKET_DELETETICKET, 4);
        data << uint32(9);
        // Send Packet
        SendPacket(&data);

        sWorld.SendGMText(LANG_COMMAND_TICKETPLAYERABANDON, GetPlayer()->GetName(), ticket->guid);
        ticketmgr.RemoveGMTicketByPlayer(GetPlayer()->GetGUID(), GetPlayer()->GetGUID());
    }
}

void WorldSession::HandleGMTicketGetTicketOpcode(WorldPacket& /*recv_data*/)
{
    // NO recv_data NO packet size check

    WorldPacket data(SMSG_GMTICKET_GETTICKET, 400);

    // get Current Ticket
    GM_Ticket* ticket = ticketmgr.GetGMTicketByPlayer(GetPlayer()->GetGUID());

    // check for existing ticket
    if (!ticket)
    {
        data << uint32(10);
        // send packet
        SendPacket(&data);
        return;
    }

    // Send current Ticket
    data << uint32(6); // unk ?
    data << ticket->message.c_str();
    data << uint8(0x7); // ticket category
    data << float(0); // tickets in queue?
    data << float(0); // if > "tickets in queue" then "We are currently experiencing a high volume of petitions."
    data << float(ticket->viewed ? GMTICKET_OPENEDBYGM_STATUS_NOT_OPENED : GMTICKET_OPENEDBYGM_STATUS_OPENED); // 0 - "Your ticket will be serviced soon", 1 - "Wait time currently unavailable"
    if (ticket->escalated)
    {
        data << uint8(2); // if == 2 and next field == 1 then "Your ticket has been escalated"
        data << uint8(1); // const
    }
    else
    {
        data << uint8(0); // ticket is not assigned
        data << uint8(0);
    }

    SendPacket(&data);

}

void WorldSession::HandleGMSurveySubmit(WorldPacket& recv_data)
{
    uint64 nextSurveyID = ticketmgr.GetNextSurveyID();
    uint32 x;
    recv_data >> x; // answer range? (6 = 0-5?)

    // @todo columns for "playerguid" "playername" and possibly "gm" after `surveyid` but how do we retrieve after ticket deletion?
    // first we must get in basic template so each answer can be inserted to the same field since they are not handled all at once
    CharacterDatabase.PExecute("INSERT INTO gm_surveys (surveyid) VALUES ('" UI64FMTD "')", nextSurveyID);

    uint8 result[10];
    memset(result, 0, sizeof(result));
    for (int i = 0; i < 10; ++i)
    {
        uint32 questionID;
        recv_data >> questionID; // GMSurveyQuestions.dbc
        if (!questionID)
            break;

        uint8 value;
        std::string unk_text;
        recv_data >> value; // answer
        recv_data >> unk_text; // always empty

        result[i] = value;
        // if anyone has a better programming method be my guest. this is the only way I see to prevent multiple rows
        if (questionID == 28)
            CharacterDatabase.PExecute("UPDATE gm_surveys SET AppropriateAnswer = '%u' WHERE surveyid = " UI64FMTD ";", value, nextSurveyID);
        if (questionID == 29)
            CharacterDatabase.PExecute("UPDATE gm_surveys SET Understandability = '%u' WHERE surveyid = " UI64FMTD ";", value, nextSurveyID);
        if (questionID == 30)
            CharacterDatabase.PExecute("UPDATE gm_surveys SET GMRating = '%u' WHERE surveyid = " UI64FMTD ";", value, nextSurveyID);
        if (questionID == 31)
            CharacterDatabase.PExecute("UPDATE gm_surveys SET ResponseTime = '%u' WHERE surveyid = " UI64FMTD ";", value, nextSurveyID);
        if (questionID == 32)
            CharacterDatabase.PExecute("UPDATE gm_surveys SET OverallGMExperience = '%u' WHERE surveyid = " UI64FMTD ";", value, nextSurveyID);
    }

    std::string comment;
    recv_data >> comment; // additional comment
    CharacterDatabase.escape_string(comment);
    CharacterDatabase.PExecute("UPDATE gm_surveys SET comment = '%s' WHERE surveyid = " UI64FMTD ";", comment.c_str(), nextSurveyID);
}

void WorldSession::HandleGMTicketSystemStatusOpcode(WorldPacket& /*recv_data*/)
{
    // NO recv_data NO packet size check

    WorldPacket data(SMSG_GMTICKET_SYSTEMSTATUS, 4);

    // Response - System is working Fine

    // No need for checks, ticket system is active
    // in case of disactivity, this should be set to (0)

    data << uint32(1);


    // Send Packet
    SendPacket(&data);
}

