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

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "World.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "AccountMgr.h"
#include "Opcodes.h"
#include "Chat.h"
#include "Log.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "Language.h"
#include "CellImpl.h"
#include "InstanceSaveMgr.h"
#include "Utilities/Util.h"
#include "TicketMgr.h"

#ifdef _DEBUG_VMAPS
#include "VMapFactory.h"
#endif

bool ChatHandler::HandleNpcSayCommand(const char* args)
{
    if (!*args)
        return false;

    Creature* pCreature = getSelectedCreature();
    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    pCreature->Say(args, LANG_UNIVERSAL, 0);

    // make some emotes
    char lastchar = args[strlen(args) - 1];
    switch (lastchar)
    {
    case '?':
        pCreature->HandleEmoteCommand(EMOTE_ONESHOT_QUESTION);
        break;
    case '!':
        pCreature->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
        break;
    default:
        pCreature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
        break;
    }

    return true;
}

bool ChatHandler::HandleNpcYellCommand(const char* args)
{
    if (!*args)
        return false;

    Creature* pCreature = getSelectedCreature();
    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    pCreature->Yell(args, LANG_UNIVERSAL, 0);

    // make an emote
    pCreature->HandleEmoteCommand(EMOTE_ONESHOT_SHOUT);

    return true;
}

//show text emote by creature in chat
bool ChatHandler::HandleNpcTextEmoteCommand(const char* args)
{
    if (!*args)
        return false;

    Creature* pCreature = getSelectedCreature();

    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    pCreature->TextEmote(args, 0);

    return true;
}

// make npc whisper to player
bool ChatHandler::HandleNpcWhisperCommand(const char* args)
{
    if (!*args)
        return false;

    char* receiver_str = strtok((char*)args, " ");
    char* text = strtok(NULL, "");

    uint64 guid = m_session->GetPlayer()->GetSelection();
    Creature* pCreature = m_session->GetPlayer()->GetMap()->GetCreature(guid);

    if (!pCreature || !receiver_str || !text)
        return false;

    uint64 receiver_guid = atol(receiver_str);

    pCreature->Whisper(text, receiver_guid);

    return true;
}

bool ChatHandler::HandleNameAnnounceCommand(const char* args)
{
    WorldPacket data;
    if (!*args)
        return false;

    sWorld.SendWorldText(LANG_ANNOUNCE_COLOR, m_session->GetPlayer()->GetName(), args);
    return true;
}

bool ChatHandler::HandleGMNameAnnounceCommand(const char* args)
{
    WorldPacket data;
    if (!*args)
        return false;

    sWorld.SendGMText(LANG_GM_ANNOUNCE_COLOR, m_session->GetPlayer()->GetName(), args);
    return true;
}

// global announce
bool ChatHandler::HandleAnnounceCommand(const char* args)
{
    if (!*args)
        return false;

    sWorld.SendWorldText(LANG_SYSTEMMESSAGE, args);
    return true;
}

// announce to logged in GMs
bool ChatHandler::HandleGMAnnounceCommand(const char* args)
{
    if (!*args)
        return false;

    sWorld.SendGMText(LANG_GM_BROADCAST, args);
    return true;
}

//notification player at the screen
bool ChatHandler::HandleNotifyCommand(const char* args)
{
    if (!*args)
        return false;

    std::string str = GetOregonString(LANG_GLOBAL_NOTIFY);
    str += args;

    WorldPacket data(SMSG_NOTIFICATION, (str.size() + 1));
    data << str;
    sWorld.SendGlobalMessage(&data);

    return true;
}

//notification GM at the screen
bool ChatHandler::HandleGMNotifyCommand(const char* args)
{
    if (!*args)
        return false;

    std::string str = GetOregonString(LANG_GM_NOTIFY);
    str += args;

    WorldPacket data(SMSG_NOTIFICATION, (str.size() + 1));
    data << str;
    sWorld.SendGlobalGMMessage(&data);

    return true;
}

//Enable\Dissable GM Mode
bool ChatHandler::HandleGMmodeCommand(const char* args)
{
    Player* _player = m_session->GetPlayer();

    if (!*args)
    {
        m_session->SendNotification(_player->IsGameMaster() ? LANG_GM_ON : LANG_GM_OFF);
        return true;
    }

    std::string argstr = (char*)args;

    if (argstr == "on")
    {
        _player->SetGameMaster(true);
        m_session->SendNotification(LANG_GM_ON);
        _player->UpdateTriggerVisibility();
        #ifdef _DEBUG_VMAPS
        VMAP::IVMapManager* vMapManager = VMAP::VMapFactory::createOrGetVMapManager();
        vMapManager->processCommand("stoplog");
        #endif
        return true;
    }

    if (argstr == "off")
    {
        _player->SetGameMaster(false);
        m_session->SendNotification(LANG_GM_OFF);
        _player->UpdateTriggerVisibility();
        #ifdef _DEBUG_VMAPS
        VMAP::IVMapManager* vMapManager = VMAP::VMapFactory::createOrGetVMapManager();
        vMapManager->processCommand("startlog");
        #endif
        return true;
    }

    SendSysMessage(LANG_USE_BOL);
    SetSentErrorMessage(true);
    return false;
}

// Enables or disables hiding of the staff badge
bool ChatHandler::HandleGMChatCommand(const char* args)
{
    if (!*args)
    {
        if (m_session->GetPlayer()->isGMChat())
            m_session->SendNotification(LANG_GM_CHAT_ON);
        else
            m_session->SendNotification(LANG_GM_CHAT_OFF);
        return true;
    }

    std::string argstr = (char*)args;

    if (argstr == "on")
    {
        m_session->GetPlayer()->SetGMChat(true);
        m_session->SendNotification(LANG_GM_CHAT_ON);
        return true;
    }

    if (argstr == "off")
    {
        m_session->GetPlayer()->SetGMChat(false);
        m_session->SendNotification(LANG_GM_CHAT_OFF);
        return true;
    }

    SendSysMessage(LANG_USE_BOL);
    SetSentErrorMessage(true);
    return false;
}

std::string ChatHandler::PGetParseString(int32 entry, ...)
{
    const char* format = GetOregonString(entry);
    va_list ap;
    char str [1024];
    va_start(ap, entry);
    vsnprintf(str, 1024, format, ap);
    va_end(ap);
    return (std::string)str;
}

bool ChatHandler::HandleGMTicketListCommand(const char* /*args*/)
{
    SendSysMessage(LANG_COMMAND_TICKETSHOWLIST);
    for (GmTicketList::iterator itr = ticketmgr.GM_TicketList.begin(); itr != ticketmgr.GM_TicketList.end(); ++itr)
    {
        if ((*itr)->closed != 0)
            continue;
        std::string gmname;
        std::stringstream ss;
        ss << PGetParseString(LANG_COMMAND_TICKETLISTGUID, (*itr)->guid);
        ss << PGetParseString(LANG_COMMAND_TICKETLISTNAME, (*itr)->name.c_str());
        ss << PGetParseString(LANG_COMMAND_TICKETLISTAGECREATE, (secsToTimeString(time(NULL) - (*itr)->createtime, true, false)).c_str());
        ss << PGetParseString(LANG_COMMAND_TICKETLISTAGE, (secsToTimeString(time(NULL) - (*itr)->timestamp, true, false)).c_str());
        if (sObjectMgr.GetPlayerNameByGUID((*itr)->assignedToGM, gmname))
            ss << PGetParseString(LANG_COMMAND_TICKETLISTASSIGNEDTO, gmname.c_str());
        SendSysMessage(ss.str().c_str());
    }
    return true;
}

bool ChatHandler::HandleGMTicketListOnlineCommand(const char* /*args*/)
{
    SendSysMessage(LANG_COMMAND_TICKETSHOWONLINELIST);
    for (GmTicketList::iterator itr = ticketmgr.GM_TicketList.begin(); itr != ticketmgr.GM_TicketList.end(); ++itr)
    {
        if ((*itr)->closed != 0 || !sObjectMgr.GetPlayer((*itr)->playerGuid))
            continue;

        std::string gmname;
        std::stringstream ss;
        ss << PGetParseString(LANG_COMMAND_TICKETLISTGUID, (*itr)->guid);
        ss << PGetParseString(LANG_COMMAND_TICKETLISTNAME, (*itr)->name.c_str());
        ss << PGetParseString(LANG_COMMAND_TICKETLISTAGECREATE, (secsToTimeString(time(NULL) - (*itr)->createtime, true, false)).c_str());
        ss << PGetParseString(LANG_COMMAND_TICKETLISTAGE, (secsToTimeString(time(NULL) - (*itr)->timestamp, true, false)).c_str());
        if (sObjectMgr.GetPlayerNameByGUID((*itr)->assignedToGM, gmname))
            ss << PGetParseString(LANG_COMMAND_TICKETLISTASSIGNEDTO, gmname.c_str());
        SendSysMessage(ss.str().c_str());
    }
    return true;
}

bool ChatHandler::HandleGMTicketListClosedCommand(const char* /*args*/)
{
    SendSysMessage(LANG_COMMAND_TICKETSHOWCLOSEDLIST);
    for (GmTicketList::iterator itr = ticketmgr.GM_TicketList.begin(); itr != ticketmgr.GM_TicketList.end(); ++itr)
    {
        if ((*itr)->closed == 0)
            continue;

        std::string gmname;
        std::stringstream ss;
        ss << PGetParseString(LANG_COMMAND_TICKETLISTGUID, (*itr)->guid);
        ss << PGetParseString(LANG_COMMAND_TICKETLISTNAME, (*itr)->name.c_str());
        ss << PGetParseString(LANG_COMMAND_TICKETLISTAGECREATE, (secsToTimeString(time(NULL) - (*itr)->createtime, true, false)).c_str());
        ss << PGetParseString(LANG_COMMAND_TICKETLISTAGE, (secsToTimeString(time(NULL) - (*itr)->timestamp, true, false)).c_str());
        if (sObjectMgr.GetPlayerNameByGUID((*itr)->assignedToGM, gmname))
            ss << PGetParseString(LANG_COMMAND_TICKETLISTASSIGNEDTO, gmname.c_str());
        SendSysMessage(ss.str().c_str());
    }
    return true;
}

bool ChatHandler::HandleGMTicketGetByIdCommand(const char* args)
{
    if (!*args)
        return false;

    uint64 tguid = atoi(args);
    GM_Ticket* ticket = ticketmgr.GetGMTicket(tguid);
    if (!ticket)
    {
        SendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
        return true;
    }

    ticket->viewed = true;
    Player* plr = sObjectMgr.GetPlayer(ticket->playerGuid);
    if (plr && plr->IsInWorld())
    {
        // tell client to update display of ticket status
        WorldPacket data(SMSG_GM_TICKET_STATUS_UPDATE, 4);
        data << uint32(1);
        plr->GetSession()->SendPacket(&data);
    }
    ticketmgr.SaveGMTicket(ticket); // update database

    std::string gmname;
    std::stringstream ss;
    ss << PGetParseString(LANG_COMMAND_TICKETLISTGUID, ticket->guid);
    ss << PGetParseString(LANG_COMMAND_TICKETLISTNAME, ticket->name.c_str());
    ss << PGetParseString(LANG_COMMAND_TICKETLISTAGECREATE, (secsToTimeString(time(NULL) - ticket->createtime, true, false)).c_str());
    ss << PGetParseString(LANG_COMMAND_TICKETLISTAGE, (secsToTimeString(time(NULL) - ticket->timestamp, true, false)).c_str());
    if (sObjectMgr.GetPlayerNameByGUID(ticket->assignedToGM, gmname))
        ss << PGetParseString(LANG_COMMAND_TICKETLISTASSIGNEDTO, gmname.c_str());
    ss <<  PGetParseString(LANG_COMMAND_TICKETLISTMESSAGE, ticket->message.c_str());
    if (ticket->comment != "")
        ss <<  PGetParseString(LANG_COMMAND_TICKETLISTCOMMENT, ticket->comment.c_str());
    SendSysMessage(ss.str().c_str());
    return true;
}

bool ChatHandler::HandleGMTicketGetByNameCommand(const char* args)
{
    if (!*args)
        return false;

    GM_Ticket* ticket = ticketmgr.GetGMTicketByName(args);
    if (!ticket)
    {
        SendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
        return true;
    }

    ticket->viewed = true;
    Player* plr = sObjectMgr.GetPlayer(ticket->playerGuid);
    if (plr && plr->IsInWorld())
    {
        // tell client to update display of ticket status
        WorldPacket data(SMSG_GM_TICKET_STATUS_UPDATE, 4);
        data << uint32(1);
        plr->GetSession()->SendPacket(&data);
    }
    ticketmgr.SaveGMTicket(ticket); // update database

    std::string gmname;
    std::stringstream ss;
    ss << PGetParseString(LANG_COMMAND_TICKETLISTGUID, ticket->guid);
    ss << PGetParseString(LANG_COMMAND_TICKETLISTNAME, ticket->name.c_str());
    ss << PGetParseString(LANG_COMMAND_TICKETLISTAGECREATE, (secsToTimeString(time(NULL) - ticket->createtime, true, false)).c_str());
    ss << PGetParseString(LANG_COMMAND_TICKETLISTAGE, (secsToTimeString(time(NULL) - ticket->timestamp, true, false)).c_str());
    if (sObjectMgr.GetPlayerNameByGUID(ticket->assignedToGM, gmname))
        ss << PGetParseString(LANG_COMMAND_TICKETLISTASSIGNEDTO, gmname.c_str());
    ss <<  PGetParseString(LANG_COMMAND_TICKETLISTMESSAGE, ticket->message.c_str());
    if (ticket->comment != "")
        ss <<  PGetParseString(LANG_COMMAND_TICKETLISTCOMMENT, ticket->comment.c_str());
    SendSysMessage(ss.str().c_str());
    return true;
}

bool ChatHandler::HandleGMTicketCloseByIdCommand(const char* args)
{
    if (!*args)
        return false;

    uint64 tguid = atoi(args);
    GM_Ticket* ticket = ticketmgr.GetGMTicket(tguid);
    if (!ticket || ticket->closed != 0)
    {
        SendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
        return true;
    }
    if (ticket->assignedToGM != 0 && ticket->assignedToGM != m_session->GetPlayer()->GetGUID())
    {
        PSendSysMessage(LANG_COMMAND_TICKETCANNOTCLOSE, ticket->guid);
        return true;
    }
    std::stringstream ss;
    ss << PGetParseString(LANG_COMMAND_TICKETLISTGUID, ticket->guid);
    ss << PGetParseString(LANG_COMMAND_TICKETLISTNAME, ticket->name.c_str());
    ss << PGetParseString(LANG_COMMAND_TICKETCLOSED, m_session->GetPlayer()->GetName());
    SendGlobalGMSysMessage(ss.str().c_str());
    Player* plr = sObjectMgr.GetPlayer(ticket->playerGuid);
    ticketmgr.RemoveGMTicket(ticket->guid, m_session->GetPlayer()->GetGUID());

    if (!plr || !plr->IsInWorld())
        return true;

    if ((float)rand_chance() < sWorld.getConfig(CONFIG_CHANCE_OF_GM_SURVEY))
    {
        // send survey
        WorldPacket data(SMSG_GM_TICKET_STATUS_UPDATE, 4);
        data << uint32(3); // 3 displays survey
        plr->GetSession()->SendPacket(&data);
    }
    else
    {
        // send abandon ticket
        WorldPacket data(SMSG_GMTICKET_DELETETICKET, 4);
        data << uint32(9);
        plr->GetSession()->SendPacket(&data);
    }

    return true;
}

bool ChatHandler::HandleGMTicketAssignToCommand(const char* args)
{
    if (!*args)
        return false;

    char* tguid = strtok((char*)args, " ");
    uint64 ticketGuid = atoi(tguid);
    char* targetgm = strtok(NULL, " ");

    if (!targetgm)
        return false;

    std::string targm = targetgm;
    if (!normalizePlayerName(targm))
        return false;

    Player* cplr = m_session->GetPlayer();
    GM_Ticket* ticket = ticketmgr.GetGMTicket(ticketGuid);

    if (!ticket || ticket->closed != 0)
    {
        SendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
        return true;
    }

    uint64 tarGUID = sObjectMgr.GetPlayerGUIDByName(targm.c_str());
    uint64 accid = sObjectMgr.GetPlayerAccountIdByGUID(tarGUID);
    uint32 gmlevel = sAccountMgr->GetSecurity(accid, realmID);

    if (!tarGUID || gmlevel == SEC_PLAYER)
    {
        SendSysMessage(LANG_COMMAND_TICKETASSIGNERROR_A);
        return true;
    }

    if (ticket->assignedToGM == tarGUID)
    {
        PSendSysMessage(LANG_COMMAND_TICKETASSIGNERROR_B, ticket->guid);
        return true;
    }

    std::string gmname;
    sObjectMgr.GetPlayerNameByGUID(tarGUID, gmname);
    if (ticket->assignedToGM != 0 && ticket->assignedToGM != cplr->GetGUID())
    {
        PSendSysMessage(LANG_COMMAND_TICKETALREADYASSIGNED, ticket->guid, gmname.c_str());
        return true;
    }

    ticket->escalated = true;
    Player* plr = sObjectMgr.GetPlayer(ticket->playerGuid);
    if (plr && plr->IsInWorld())
    {
        // tell client to update display of ticket status
        WorldPacket data(SMSG_GM_TICKET_STATUS_UPDATE, 4);
        data << uint32(1);
        plr->GetSession()->SendPacket(&data);
    }

    ticket->assignedToGM = tarGUID;
    ticketmgr.UpdateGMTicket(ticket);
    std::stringstream ss;
    ss << PGetParseString(LANG_COMMAND_TICKETLISTGUID, ticket->guid);
    ss << PGetParseString(LANG_COMMAND_TICKETLISTNAME, ticket->name.c_str());
    ss << PGetParseString(LANG_COMMAND_TICKETLISTASSIGNEDTO, gmname.c_str());
    SendGlobalGMSysMessage(ss.str().c_str());
    return true;
}

bool ChatHandler::HandleGMTicketUnAssignCommand(const char* args)
{
    if (!*args)
        return false;

    uint64 ticketGuid = atoi(args);
    Player* cplr = m_session->GetPlayer();
    GM_Ticket* ticket = ticketmgr.GetGMTicket(ticketGuid);

    if (!ticket || ticket->closed != 0)
    {
        SendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
        return true;
    }
    if (ticket->assignedToGM == 0)
    {
        PSendSysMessage(LANG_COMMAND_TICKETNOTASSIGNED, ticket->guid);
        return true;
    }

    std::string gmname;
    sObjectMgr.GetPlayerNameByGUID(ticket->assignedToGM, gmname);
    Player* plr = sObjectMgr.GetPlayer(ticket->assignedToGM);
    if (plr && plr->IsInWorld() && plr->GetSession()->GetSecurity() > cplr->GetSession()->GetSecurity())
    {
        SendSysMessage(LANG_COMMAND_TICKETUNASSIGNSECURITY);
        return true;
    }

    std::stringstream ss;
    ss << PGetParseString(LANG_COMMAND_TICKETLISTGUID, ticket->guid);
    ss << PGetParseString(LANG_COMMAND_TICKETLISTNAME, ticket->name.c_str());
    ss << PGetParseString(LANG_COMMAND_TICKETLISTASSIGNEDTO, gmname.c_str());
    ss << PGetParseString(LANG_COMMAND_TICKETLISTUNASSIGNED, cplr->GetName());
    SendGlobalGMSysMessage(ss.str().c_str());

    ticket->escalated = false;
    Player* player = sObjectMgr.GetPlayer(ticket->playerGuid);
    if (player && player->IsInWorld())
    {
        // tell client to update display of ticket status
        WorldPacket data(SMSG_GM_TICKET_STATUS_UPDATE, 4);
        data << uint32(1);
        player->GetSession()->SendPacket(&data);
    }

    ticket->assignedToGM = 0;
    ticketmgr.UpdateGMTicket(ticket);
    return true;
}

bool ChatHandler::HandleGMTicketCommentCommand(const char* args)
{
    if (!*args)
        return false;

    char* tguid = strtok((char*)args, " ");
    uint64 ticketGuid = atoi(tguid);
    char* comment = strtok(NULL, "\n");

    if (!comment)
        return false;

    Player* cplr = m_session->GetPlayer();
    GM_Ticket* ticket = ticketmgr.GetGMTicket(ticketGuid);

    if (!ticket || ticket->closed != 0)
    {
        PSendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
        return true;
    }
    if (ticket->assignedToGM != 0 && ticket->assignedToGM != cplr->GetGUID())
    {
        PSendSysMessage(LANG_COMMAND_TICKETALREADYASSIGNED, ticket->guid);
        return true;
    }

    std::string gmname;
    sObjectMgr.GetPlayerNameByGUID(ticket->assignedToGM, gmname);
    ticket->comment = comment;
    ticketmgr.UpdateGMTicket(ticket);
    std::stringstream ss;
    ss << PGetParseString(LANG_COMMAND_TICKETLISTGUID, ticket->guid);
    ss << PGetParseString(LANG_COMMAND_TICKETLISTNAME, ticket->name.c_str());
    if (sObjectMgr.GetPlayerNameByGUID(ticket->assignedToGM, gmname))
        ss << PGetParseString(LANG_COMMAND_TICKETLISTASSIGNEDTO, gmname.c_str());
    ss << PGetParseString(LANG_COMMAND_TICKETLISTADDCOMMENT, cplr->GetName(), ticket->comment.c_str());
    SendGlobalGMSysMessage(ss.str().c_str());
    return true;
}

bool ChatHandler::HandleGMTicketDeleteByIdCommand(const char* args)
{
    if (!*args)
        return false;
    uint64 ticketGuid = atoi(args);
    GM_Ticket* ticket = ticketmgr.GetGMTicket(ticketGuid);

    if (!ticket)
    {
        SendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
        return true;
    }
    if (ticket->closed == 0)
    {
        SendSysMessage(LANG_COMMAND_TICKETCLOSEFIRST);
        return true;
    }

    std::stringstream ss;
    ss << PGetParseString(LANG_COMMAND_TICKETLISTGUID, ticket->guid);
    ss << PGetParseString(LANG_COMMAND_TICKETLISTNAME, ticket->name.c_str());
    ss << PGetParseString(LANG_COMMAND_TICKETDELETED, m_session->GetPlayer()->GetName());
    SendGlobalGMSysMessage(ss.str().c_str());
    Player* plr = sObjectMgr.GetPlayer(ticket->playerGuid);
    ticketmgr.DeleteGMTicketPermanently(ticket->guid);
    if (plr && plr->IsInWorld())
    {
        // Force abandon ticket
        WorldPacket data(SMSG_GMTICKET_DELETETICKET, 4);
        data << uint32(9);
        plr->GetSession()->SendPacket(&data);
    }

    ticket = NULL;
    return true;
}

bool ChatHandler::HandleGMTicketReloadCommand(const char*)
{
    ticketmgr.LoadGMTickets();
    return true;
}

//Enable\Disable Invisible mode
bool ChatHandler::HandleVisibleCommand(const char* args)
{
    Player* _player = m_session->GetPlayer();

    if (!*args)
    {
        PSendSysMessage(LANG_YOU_ARE, _player->isGMVisible() ?  GetOregonString(LANG_VISIBLE) : GetOregonString(LANG_INVISIBLE));
        return true;
    }

    std::string argstr = (char*)args;

    if (argstr == "on")
    {
        if (_player->HasAura(SPELL_INVISIBILITY, 0))
            _player->RemoveAurasDueToSpell(SPELL_INVISIBILITY);

        _player->SetGMVisible(true);
        _player->UpdateObjectVisibility();
        m_session->SendNotification(LANG_INVISIBLE_VISIBLE);
        return true;
    }

    if (argstr == "off")
    {
        _player->AddAura(SPELL_INVISIBILITY, _player);
        _player->SetGMVisible(false);
        _player->UpdateObjectVisibility();
        m_session->SendNotification(LANG_INVISIBLE_INVISIBLE);
        return true;
    }

    SendSysMessage(LANG_USE_BOL);
    SetSentErrorMessage(true);
    return false;
}

bool ChatHandler::HandleGPSCommand(const char* args)
{
    WorldObject* obj = NULL;
    if (*args)
    {
        std::string name = args;
        if (normalizePlayerName(name))
            obj = sObjectMgr.GetPlayer(name.c_str());

        if (!obj)
        {
            SendSysMessage(LANG_PLAYER_NOT_FOUND);
            SetSentErrorMessage(true);
            return false;
        }
    }
    else
    {
        obj = getSelectedUnit();

        if (!obj)
        {
            SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            SetSentErrorMessage(true);
            return false;
        }
    }
    CellCoord cell_val = Oregon::ComputeCellCoord(obj->GetPositionX(), obj->GetPositionY());
    Cell cell(cell_val);

    uint32 zone_id = obj->GetZoneId();
    uint32 area_id = obj->GetAreaId();

    MapEntry const* mapEntry = sMapStore.LookupEntry(obj->GetMapId());
    AreaTableEntry const* zoneEntry = GetAreaEntryByAreaID(zone_id);
    AreaTableEntry const* areaEntry = GetAreaEntryByAreaID(area_id);

    float zone_x = obj->GetPositionX();
    float zone_y = obj->GetPositionY();

    Map2ZoneCoordinates(zone_x, zone_y, zone_id);

    Map const* map = obj->GetMap();
    float ground_z = map->GetHeight(obj->GetPositionX(), obj->GetPositionY(), MAX_HEIGHT);
    float floor_z = map->GetHeight(obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ());

    GridCoord p = Oregon::ComputeGridCoord(obj->GetPositionX(), obj->GetPositionY());

    int gx = 63 - p.x_coord;
    int gy = 63 - p.y_coord;

    uint32 have_map = Map::ExistMap(obj->GetMapId(), gx, gy) ? 1 : 0;
    uint32 have_vmap = Map::ExistVMap(obj->GetMapId(), gx, gy) ? 1 : 0;

    if (have_vmap)
    {
        if (map->IsOutdoors(obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ()))
            PSendSysMessage("You are outdoors");
        else
            PSendSysMessage("You are indoor");
    }
    else PSendSysMessage("no VMAP available for area info");

    PSendSysMessage(LANG_MAP_POSITION,
                    obj->GetMapId(), (mapEntry ? mapEntry->name[m_session->GetSessionDbcLocale()] : "<unknown>"),
                    zone_id, (zoneEntry ? zoneEntry->area_name[m_session->GetSessionDbcLocale()] : "<unknown>"),
                    area_id, (areaEntry ? areaEntry->area_name[m_session->GetSessionDbcLocale()] : "<unknown>"),
                    obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ(), obj->GetOrientation(),
                    cell.GridX(), cell.GridY(), cell.CellX(), cell.CellY(), obj->GetInstanceId(),
                    zone_x, zone_y, ground_z, floor_z, have_map, have_vmap);

    sLog.outDebug("Player %s GPS call for %s '%s' (%s: %u):",
                  GetName(),
                  (obj->GetTypeId() == TYPEID_PLAYER ? "player" : "creature"), obj->GetName(),
                  (obj->GetTypeId() == TYPEID_PLAYER ? "GUID" : "Entry"), (obj->GetTypeId() == TYPEID_PLAYER ? obj->GetGUIDLow() : obj->GetEntry()));
    sLog.outDebug(GetOregonString(LANG_MAP_POSITION),
                  obj->GetMapId(), (mapEntry ? mapEntry->name[sWorld.GetDefaultDbcLocale()] : "<unknown>"),
                  zone_id, (zoneEntry ? zoneEntry->area_name[sWorld.GetDefaultDbcLocale()] : "<unknown>"),
                  area_id, (areaEntry ? areaEntry->area_name[sWorld.GetDefaultDbcLocale()] : "<unknown>"),
                  obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ(), obj->GetOrientation(),
                  cell.GridX(), cell.GridY(), cell.CellX(), cell.CellY(), obj->GetInstanceId(),
                  zone_x, zone_y, ground_z, floor_z, have_map, have_vmap);

    LiquidData liquid_status;
    ZLiquidStatus res = map->getLiquidStatus(obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ(), MAP_ALL_LIQUIDS, &liquid_status);
    if (res)
        PSendSysMessage(LANG_LIQUID_STATUS, liquid_status.level, liquid_status.depth_level, liquid_status.entry, liquid_status.type_flags, res);
    return true;
}

//Summon Player
bool ChatHandler::HandleSummonCommand(const char* args)
{
    if (!*args)
        return false;

    std::string name = args;

    if (!normalizePlayerName(name))
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    Player* target = sObjectMgr.GetPlayer(name.c_str());
    if (target)
    {
        if (target->IsBeingTeleported())
        {
            PSendSysMessage(LANG_IS_TELEPORTED, target->GetName());
            SetSentErrorMessage(true);
            return false;
        }

        Map* pMap = m_session->GetPlayer()->GetMap();

        if (pMap->IsBattlegroundOrArena())
        {
            // only allow if gm mode is on
            if (!target->IsGameMaster())
            {
                PSendSysMessage(LANG_CANNOT_GO_TO_BG_GM, target->GetName());
                SetSentErrorMessage(true);
                return false;
            }
            // if both players are in different bgs
            else if (target->GetBattlegroundId() && m_session->GetPlayer()->GetBattlegroundId() != target->GetBattlegroundId())
            {
                PSendSysMessage(LANG_CANNOT_GO_TO_BG_FROM_BG, target->GetName());
                SetSentErrorMessage(true);
                return false;
            }
            // all's well, set bg id
            // when porting out from the bg, it will be reset to 0
            target->SetBattlegroundId(m_session->GetPlayer()->GetBattlegroundId());
            // remember current position as entry point for return at bg end teleportation
            if (!target->GetMap()->IsBattlegroundOrArena())
                target->SetBattlegroundEntryPoint();
        }
        else if (pMap->IsDungeon())
        {
            Map* cMap = target->GetMap();
            if (cMap->Instanceable() && cMap->GetInstanceId() != pMap->GetInstanceId())
            {
                // cannot summon from instance to instance
                PSendSysMessage(LANG_CANNOT_SUMMON_TO_INST, target->GetName());
                SetSentErrorMessage(true);
                return false;
            }

            // we are in instance, and can summon only player in our group with us as lead
            if (!m_session->GetPlayer()->GetGroup() || !target->GetGroup() ||
                (target->GetGroup()->GetLeaderGUID() != m_session->GetPlayer()->GetGUID()) ||
                (m_session->GetPlayer()->GetGroup()->GetLeaderGUID() != m_session->GetPlayer()->GetGUID()))
                // the last check is a bit excessive, but let it be, just in case
            {
                PSendSysMessage(LANG_CANNOT_SUMMON_TO_INST, target->GetName());
                SetSentErrorMessage(true);
                return false;
            }
        }

        PSendSysMessage(LANG_SUMMONING, target->GetName(), "");
        if (needReportToTarget(target))
            ChatHandler(target).PSendSysMessage(LANG_SUMMONED_BY, GetName());

        // stop flight if need
        if (target->IsInFlight())
        {
            target->GetMotionMaster()->MovementExpired();
            target->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            target->SaveRecallPosition();

        // before GM
        float x, y, z;
        m_session->GetPlayer()->GetClosePoint(x, y, z, target->GetObjectSize());
        target->TeleportTo(m_session->GetPlayer()->GetMapId(), x, y, z, target->GetOrientation());
    }
    else if (uint64 guid = sObjectMgr.GetPlayerGUIDByName(name))
    {
        PSendSysMessage(LANG_SUMMONING, name.c_str(), GetOregonString(LANG_OFFLINE));

        // in point where GM stay
        Player::SavePositionInDB(m_session->GetPlayer()->GetMapId(),
                                 m_session->GetPlayer()->GetPositionX(),
                                 m_session->GetPlayer()->GetPositionY(),
                                 m_session->GetPlayer()->GetPositionZ(),
                                 m_session->GetPlayer()->GetOrientation(),
                                 m_session->GetPlayer()->GetZoneId(),
                                 guid);
    }
    else
    {
        PSendSysMessage(LANG_NO_PLAYER, args);
        SetSentErrorMessage(true);
    }

    return true;
}

//Teleport to Player
bool ChatHandler::HandleAppearCommand(const char* args)
{
    if (!*args)
        return false;

    Player* _player = m_session->GetPlayer();

    std::string name = args;

    if (!normalizePlayerName(name))
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    Player* target = sObjectMgr.GetPlayer(name.c_str());
    if (target)
    {
        Map* cMap = target->GetMap();
        if (cMap->IsBattlegroundOrArena())
        {
            // only allow if gm mode is on
            if (!_player->IsGameMaster())
            {
                PSendSysMessage(LANG_CANNOT_GO_TO_BG_GM, target->GetName());
                SetSentErrorMessage(true);
                return false;
            }
            // if both players are in different bgs
            else if (_player->GetBattlegroundId() && _player->GetBattlegroundId() != target->GetBattlegroundId())
                _player->LeaveBattleground(false); // Note: should be changed so _player gets no Deserter debuff

            // all's well, set bg id
            // when porting out from the bg, it will be reset to 0
            _player->SetBattlegroundId(target->GetBattlegroundId());
            // remember current position as entry point for return at bg end teleportation
            if (!_player->GetMap()->IsBattlegroundOrArena())
                _player->SetBattlegroundEntryPoint();
        }
        else if (cMap->IsDungeon())
        {
            // we have to go to instance, and can go to player only if:
            //   1) we are in his group (either as leader or as member)
            //   2) we are not bound to any group and have GM mode on
            if (_player->GetGroup())
            {
                // we are in group, we can go only if we are in the player group
                if (_player->GetGroup() != target->GetGroup())
                {
                    PSendSysMessage(LANG_CANNOT_GO_TO_INST_PARTY, target->GetName());
                    SetSentErrorMessage(true);
                    return false;
                }
            }
            else
            {
                // we are not in group, let's verify our GM mode
                if (!_player->IsGameMaster())
                {
                    PSendSysMessage(LANG_CANNOT_GO_TO_INST_GM, target->GetName());
                    SetSentErrorMessage(true);
                    return false;
                }
            }

            // if the player or the player's group is bound to another instance
            // the player will not be bound to another one
            InstancePlayerBind *pBind = _player->GetBoundInstance(target->GetMapId(), target->GetDifficulty());
            if (!pBind)
            {
                Group *group = _player->GetGroup();
                // if no bind exists, create a solo bind
                InstanceGroupBind *gBind = group ? group->GetBoundInstance(target) : NULL;                // if no bind exists, create a solo bind
                if (!gBind)
                    if (InstanceSave *save = sInstanceSaveMgr.GetInstanceSave(target->GetInstanceId()))
                        _player->BindToInstance(save, !save->CanReset());
            }

            _player->SetDifficulty(target->GetDifficulty());
        }

        PSendSysMessage(LANG_APPEARING_AT, target->GetName());

        //if (_player->IsVisibleGloballyFor(target))
        //    ChatHandler(target).PSendSysMessage(LANG_APPEARING_TO, _player->GetName());

        // stop flight if need
        if (_player->IsInFlight())
        {
            _player->GetMotionMaster()->MovementExpired();
            _player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            _player->SaveRecallPosition();

        // to point to see at target with same orientation
        float x, y, z;
        target->GetPosition(x, y, z);

        if (_player->HasUnitMovementFlag(MOVEMENTFLAG_FLYING) ||
            target->HasUnitMovementFlag(MOVEMENTFLAG_FLYING))
        {
            WorldPacket data;
            data.SetOpcode(SMSG_MOVE_SET_CAN_FLY);
            data << _player->GetPackGUID();
            data << uint32(0);                                      // unknown
            _player->SendMessageToSet(&data, true);
        }

        _player->TeleportTo(target->GetMapId(), x, y, z, _player->GetAngle(target), TELE_TO_GM_MODE);

        return true;
    }

    if (uint64 guid = sObjectMgr.GetPlayerGUIDByName(name))
    {
        PSendSysMessage(LANG_APPEARING_AT, name.c_str());

        // to point where player stay (if loaded)
        float x, y, z, o;
        uint32 map;
        bool in_flight;
        if (!Player::LoadPositionFromDB(map, x, y, z, o, in_flight, guid))
            return false;

        // stop flight if need
        if (_player->IsInFlight())
        {
            _player->GetMotionMaster()->MovementExpired();
            _player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            _player->SaveRecallPosition();

        _player->TeleportTo(map, x, y, z, _player->GetOrientation());
        return true;
    }

    PSendSysMessage(LANG_NO_PLAYER, args);

    SetSentErrorMessage(true);
    return false;
}

// Teleport player to last position
bool ChatHandler::HandleRecallCommand(const char* args)
{
    Player* target = NULL;

    if (!*args)
    {
        target = getSelectedPlayerOrSelf();
    }
    else
    {
        std::string name = args;

        if (!normalizePlayerName(name))
        {
            SendSysMessage(LANG_PLAYER_NOT_FOUND);
            SetSentErrorMessage(true);
            return false;
        }

        target = sObjectMgr.GetPlayer(name.c_str());

        if (!target)
        {
            PSendSysMessage(LANG_NO_PLAYER, args);
            SetSentErrorMessage(true);
            return false;
        }
    }

    if (target->IsBeingTeleported())
    {
        PSendSysMessage(LANG_IS_TELEPORTED, target->GetName());
        SetSentErrorMessage(true);
        return false;
    }

    // stop flight if need
    if (target->IsInFlight())
    {
        target->GetMotionMaster()->MovementExpired();
        target->m_taxi.ClearTaxiDestinations();
    }

    target->TeleportTo(target->m_recallMap, target->m_recallX, target->m_recallY, target->m_recallZ, target->m_recallO);
    return true;
}

//Edit Player HP
bool ChatHandler::HandleModifyHPCommand(const char* args)
{
    if (!*args)
        return false;

    int32 hp = atoi((char*)args);
    int32 hpm = atoi((char*)args);

    if (hp < 1 || hpm < 1 || hpm < hp)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    Player* target = getSelectedPlayerOrSelf();
    if (target == NULL)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    if (!target->IsAlive())
    {
        SendSysMessage(LANG_NO_SELECTION);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_YOU_CHANGE_HP, target->GetName(), hp, hpm);
    if (needReportToTarget(target))
        ChatHandler(target).PSendSysMessage(LANG_YOURS_HP_CHANGED, GetName(), hp, hpm);

    target->SetMaxHealth(hpm);
    target->SetHealth(hp);

    return true;
}

//Edit Player Mana
bool ChatHandler::HandleModifyManaCommand(const char* args)
{
    if (!*args)
        return false;

    // char* pmana = strtok((char*)args, " ");
    // if (!pmana)
    //     return false;

    // char* pmanaMax = strtok(NULL, " ");
    // if (!pmanaMax)
    //     return false;

    // int32 manam = atoi(pmanaMax);
    // int32 mana = atoi(pmana);
    int32 mana = atoi((char*)args);
    int32 manam = atoi((char*)args);

    if (mana <= 0 || manam <= 0 || manam < mana)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    Player* target = getSelectedPlayerOrSelf();
    if (target == NULL)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_YOU_CHANGE_MANA, target->GetName(), mana, manam);
    if (needReportToTarget(target))
        ChatHandler(target).PSendSysMessage(LANG_YOURS_MANA_CHANGED, GetName(), mana, manam);

    target->SetMaxPower(POWER_MANA, manam);
    target->SetPower(POWER_MANA, mana);

    return true;
}

//Edit Player Energy
bool ChatHandler::HandleModifyEnergyCommand(const char* args)
{
    if (!*args)
        return false;

    // char* pmana = strtok((char*)args, " ");
    // if (!pmana)
    //     return false;

    // char* pmanaMax = strtok(NULL, " ");
    // if (!pmanaMax)
    //     return false;

    // int32 manam = atoi(pmanaMax);
    // int32 mana = atoi(pmana);

    int32 energy = atoi((char*)args) * 10;
    int32 energym = atoi((char*)args) * 10;

    if (energy <= 0 || energym <= 0 || energym < energy)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    Player* target = getSelectedPlayerOrSelf();
    if (!target)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_YOU_CHANGE_ENERGY, target->GetName(), energy / 10, energym / 10);
    if (needReportToTarget(target))
        ChatHandler(target).PSendSysMessage(LANG_YOURS_ENERGY_CHANGED, GetName(), energy / 10, energym / 10);

    target->SetMaxPower(POWER_ENERGY, energym);
    target->SetPower(POWER_ENERGY, energy);

    sLog.outDetail(GetOregonString(LANG_CURRENT_ENERGY), target->GetMaxPower(POWER_ENERGY));

    return true;
}

//Edit Player Rage
bool ChatHandler::HandleModifyRageCommand(const char* args)
{
    if (!*args)
        return false;

    // char* pmana = strtok((char*)args, " ");
    // if (!pmana)
    //     return false;

    // char* pmanaMax = strtok(NULL, " ");
    // if (!pmanaMax)
    //     return false;

    // int32 manam = atoi(pmanaMax);
    // int32 mana = atoi(pmana);

    int32 rage = atoi((char*)args) * 10;
    int32 ragem = atoi((char*)args) * 10;

    if (rage <= 0 || ragem <= 0 || ragem < rage)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    Player* target = getSelectedPlayerOrSelf();
    if (target == NULL)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_YOU_CHANGE_RAGE, target->GetName(), rage / 10, ragem / 10);
    if (needReportToTarget(target))
        ChatHandler(target).PSendSysMessage(LANG_YOURS_RAGE_CHANGED, GetName(), rage / 10, ragem / 10);

    target->SetMaxPower(POWER_RAGE, ragem);
    target->SetPower(POWER_RAGE, rage);

    return true;
}

//Edit Player Faction
bool ChatHandler::HandleModifyFactionCommand(const char* args)
{
    if (!*args)
        return false;

    char* pfactionid = extractKeyFromLink((char*)args, "Hfaction");

    Creature* target = getSelectedCreature();
    if (!target)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    if (!pfactionid)
    {
        if (target)
        {
            uint32 factionid = target->GetFaction();
            uint32 flag      = target->GetUInt32Value(UNIT_FIELD_FLAGS);
            uint32 npcflag   = target->GetUInt32Value(UNIT_NPC_FLAGS);
            uint32 dyflag    = target->GetUInt32Value(UNIT_DYNAMIC_FLAGS);
            PSendSysMessage(LANG_CURRENT_FACTION, target->GetGUIDLow(), factionid, flag, npcflag, dyflag);
        }
        return true;
    }

    if (!target)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 factionid = atoi(pfactionid);
    uint32 flag;

    char* pflag = strtok(NULL, " ");
    if (!pflag)
        flag = target->GetUInt32Value(UNIT_FIELD_FLAGS);
    else
        flag = atoi(pflag);

    char* pnpcflag = strtok(NULL, " ");

    uint32 npcflag;
    if (!pnpcflag)
        npcflag   = target->GetUInt32Value(UNIT_NPC_FLAGS);
    else
        npcflag = atoi(pnpcflag);

    char* pdyflag = strtok(NULL, " ");

    uint32  dyflag;
    if (!pdyflag)
        dyflag   = target->GetUInt32Value(UNIT_DYNAMIC_FLAGS);
    else
        dyflag = atoi(pdyflag);

    if (!sFactionTemplateStore.LookupEntry(factionid))
    {
        PSendSysMessage(LANG_WRONG_FACTION, factionid);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_YOU_CHANGE_FACTION, target->GetGUIDLow(), factionid, flag, npcflag, dyflag);

    target->SetFaction(factionid);
    target->SetUInt32Value(UNIT_FIELD_FLAGS, flag);
    target->SetUInt32Value(UNIT_NPC_FLAGS, npcflag);
    target->SetUInt32Value(UNIT_DYNAMIC_FLAGS, dyflag);

    return true;
}

//Edit Player Spell
bool ChatHandler::HandleModifySpellCommand(const char* args)
{
    if (!*args) return false;
    char* pspellflatid = strtok((char*)args, " ");
    if (!pspellflatid)
        return false;

    char* pop = strtok(NULL, " ");
    if (!pop)
        return false;

    char* pval = strtok(NULL, " ");
    if (!pval)
        return false;

    uint16 mark;

    char* pmark = strtok(NULL, " ");

    uint8 spellflatid = atoi(pspellflatid);
    uint8 op   = atoi(pop);
    uint16 val = atoi(pval);
    if (!pmark)
        mark = 65535;
    else
        mark = atoi(pmark);

    Player* target = getSelectedPlayerOrSelf();
    if (target == NULL)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_YOU_CHANGE_SPELLFLATID, spellflatid, val, mark, target->GetName());
    if (needReportToTarget(target))
        ChatHandler(target).PSendSysMessage(LANG_YOURS_SPELLFLATID_CHANGED, GetName(), spellflatid, val, mark);

    WorldPacket data(SMSG_SET_FLAT_SPELL_MODIFIER, (1 + 1 + 2 + 2));
    data << uint8(spellflatid);
    data << uint8(op);
    data << uint16(val);
    data << uint16(mark);
    target->GetSession()->SendPacket(&data);

    return true;
}

//Edit Player TP
bool ChatHandler::HandleModifyTalentCommand (const char* args)
{
    if (!*args)
        return false;

    int tp = atoi((char*)args);
    if (tp < 0)
        return false;

    Player* target = getSelectedPlayerOrSelf();
    if (!target)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    target->SetFreeTalentPoints(tp);
    return true;
}

//Enable On\OFF all taxi paths
bool ChatHandler::HandleTaxiCheatCommand(const char* args)
{
    if (!*args)
    {
        SendSysMessage(LANG_USE_BOL);
        SetSentErrorMessage(true);
        return false;
    }

    std::string argstr = (char*)args;

    Player* target = getSelectedPlayerOrSelf();

    if (argstr == "on")
    {
        target->SetTaxiCheater(true);
        PSendSysMessage(LANG_YOU_GIVE_TAXIS, target->GetName());
        if (needReportToTarget(target))
            ChatHandler(target).PSendSysMessage(LANG_YOURS_TAXIS_ADDED, GetName());
        return true;
    }

    if (argstr == "off")
    {
        target->SetTaxiCheater(false);
        PSendSysMessage(LANG_YOU_REMOVE_TAXIS, target->GetName());
        if (needReportToTarget(target))
            ChatHandler(target).PSendSysMessage(LANG_YOURS_TAXIS_REMOVED, GetName());

        return true;
    }

    SendSysMessage(LANG_USE_BOL);
    SetSentErrorMessage(true);
    return false;
}

//Edit Player Aspeed
bool ChatHandler::HandleModifyASpeedCommand(const char* args)
{
    if (!*args)
        return false;

    float modSpeed = (float)atof((char*)args);


    if (modSpeed > 10 || modSpeed < 0.1)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    Player* target = getSelectedPlayerOrSelf();
    if (target == NULL)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    if (target->IsInFlight())
    {
        PSendSysMessage(LANG_CHAR_IN_FLIGHT, target->GetName());
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_YOU_CHANGE_ASPEED, modSpeed, target->GetName());
    if (needReportToTarget(target))
        ChatHandler(target).PSendSysMessage(LANG_YOURS_ASPEED_CHANGED, GetName(), modSpeed);

    target->SetSpeed(MOVE_WALK,    modSpeed, true);
    target->SetSpeed(MOVE_RUN,     modSpeed, true);
    target->SetSpeed(MOVE_SWIM,    modSpeed, true);
    //target->SetSpeed(MOVE_TURN,    modSpeed, true);
    target->SetSpeed(MOVE_FLIGHT,     modSpeed, true);
    return true;
}

//Edit Player Speed
bool ChatHandler::HandleModifySpeedCommand(const char* args)
{
    if (!*args)
        return false;

    float modSpeed = (float)atof((char*)args);

    if (modSpeed > 10 || modSpeed < 0.1)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    Player* target = getSelectedPlayerOrSelf();
    if (target == NULL)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    if (target->IsInFlight())
    {
        PSendSysMessage(LANG_CHAR_IN_FLIGHT, target->GetName());
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_YOU_CHANGE_SPEED, modSpeed, target->GetName());
    if (needReportToTarget(target))
        ChatHandler(target).PSendSysMessage(LANG_YOURS_SPEED_CHANGED, GetName(), modSpeed);

    target->SetSpeed(MOVE_RUN, modSpeed, true);

    return true;
}

//Edit Player Swim Speed
bool ChatHandler::HandleModifySwimCommand(const char* args)
{
    if (!*args)
        return false;

    float modSpeed = (float)atof((char*)args);

    if (modSpeed > 10.0f || modSpeed < 0.1f)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    Player* target = getSelectedPlayerOrSelf();
    if (target == NULL)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    if (target->IsInFlight())
    {
        PSendSysMessage(LANG_CHAR_IN_FLIGHT, target->GetName());
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_YOU_CHANGE_SWIM_SPEED, modSpeed, target->GetName());
    if (needReportToTarget(target))
        ChatHandler(target).PSendSysMessage(LANG_YOURS_SWIM_SPEED_CHANGED, GetName(), modSpeed);

    target->SetSpeed(MOVE_SWIM, modSpeed, true);

    return true;
}

//Edit Player Walk Speed
bool ChatHandler::HandleModifyBWalkCommand(const char* args)
{
    if (!*args)
        return false;

    float modSpeed = (float)atof((char*)args);

    if (modSpeed > 10.0f || modSpeed < 0.1f)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    Player* target = getSelectedPlayerOrSelf();
    if (target == NULL)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    if (target->IsInFlight())
    {
        PSendSysMessage(LANG_CHAR_IN_FLIGHT, target->GetName());
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_YOU_CHANGE_BACK_SPEED, modSpeed, target->GetName());
    if (needReportToTarget(target))
        ChatHandler(target).PSendSysMessage(LANG_YOURS_BACK_SPEED_CHANGED, GetName(), modSpeed);

    target->SetSpeed(MOVE_RUN_BACK, modSpeed, true);

    return true;
}

//Edit Player Fly
bool ChatHandler::HandleModifyFlyCommand(const char* args)
{
    if (!*args)
        return false;

    float modSpeed = (float)atof((char*)args);

    if (modSpeed > 50.0f || modSpeed < 0.1f)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    Player* target = getSelectedPlayerOrSelf();
    if (target == NULL)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_YOU_CHANGE_FLY_SPEED, modSpeed, target->GetName());
    if (needReportToTarget(target))
        ChatHandler(target).PSendSysMessage(LANG_YOURS_FLY_SPEED_CHANGED, GetName(), modSpeed);

    target->SetSpeed(MOVE_FLIGHT, modSpeed, true);

    return true;
}

//Edit Player Scale
bool ChatHandler::HandleModifyScaleCommand(const char* args)
{
    if (!*args)
        return false;

    float Scale = (float)atof((char*)args);
    if (Scale > 10.0f || Scale < 0.1f)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    Unit* target = getSelectedUnit();
    if (target == NULL)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    if (target->GetTypeId() == TYPEID_PLAYER)
    {
        PSendSysMessage(LANG_YOU_CHANGE_SIZE, Scale, target->GetName());
        if (needReportToTarget(target->ToPlayer()))
            ChatHandler(target->ToPlayer()).PSendSysMessage(LANG_YOURS_SIZE_CHANGED, GetName(), Scale);
    }

    target->SetObjectScale(Scale);

    return true;
}

//Enable Player mount
bool ChatHandler::HandleModifyMountCommand(const char* args)
{
    if (!*args)
        return false;

    uint16 mId = 1147;
    float speed = (float)15;
    uint32 num = 0;

    num = atoi((char*)args);
    switch (num)
    {
    case 1:
        mId = 14340;
        break;
    case 2:
        mId = 4806;
        break;
    case 3:
        mId = 6471;
        break;
    case 4:
        mId = 12345;
        break;
    case 5:
        mId = 6472;
        break;
    case 6:
        mId = 6473;
        break;
    case 7:
        mId = 10670;
        break;
    case 8:
        mId = 10719;
        break;
    case 9:
        mId = 10671;
        break;
    case 10:
        mId = 10672;
        break;
    case 11:
        mId = 10720;
        break;
    case 12:
        mId = 14349;
        break;
    case 13:
        mId = 11641;
        break;
    case 14:
        mId = 12244;
        break;
    case 15:
        mId = 12242;
        break;
    case 16:
        mId = 14578;
        break;
    case 17:
        mId = 14579;
        break;
    case 18:
        mId = 14349;
        break;
    case 19:
        mId = 12245;
        break;
    case 20:
        mId = 14335;
        break;
    case 21:
        mId = 207;
        break;
    case 22:
        mId = 2328;
        break;
    case 23:
        mId = 2327;
        break;
    case 24:
        mId = 2326;
        break;
    case 25:
        mId = 14573;
        break;
    case 26:
        mId = 14574;
        break;
    case 27:
        mId = 14575;
        break;
    case 28:
        mId = 604;
        break;
    case 29:
        mId = 1166;
        break;
    case 30:
        mId = 2402;
        break;
    case 31:
        mId = 2410;
        break;
    case 32:
        mId = 2409;
        break;
    case 33:
        mId = 2408;
        break;
    case 34:
        mId = 2405;
        break;
    case 35:
        mId = 14337;
        break;
    case 36:
        mId = 6569;
        break;
    case 37:
        mId = 10661;
        break;
    case 38:
        mId = 10666;
        break;
    case 39:
        mId = 9473;
        break;
    case 40:
        mId = 9476;
        break;
    case 41:
        mId = 9474;
        break;
    case 42:
        mId = 14374;
        break;
    case 43:
        mId = 14376;
        break;
    case 44:
        mId = 14377;
        break;
    case 45:
        mId = 2404;
        break;
    case 46:
        mId = 2784;
        break;
    case 47:
        mId = 2787;
        break;
    case 48:
        mId = 2785;
        break;
    case 49:
        mId = 2736;
        break;
    case 50:
        mId = 2786;
        break;
    case 51:
        mId = 14347;
        break;
    case 52:
        mId = 14346;
        break;
    case 53:
        mId = 14576;
        break;
    case 54:
        mId = 9695;
        break;
    case 55:
        mId = 9991;
        break;
    case 56:
        mId = 6448;
        break;
    case 57:
        mId = 6444;
        break;
    case 58:
        mId = 6080;
        break;
    case 59:
        mId = 6447;
        break;
    case 60:
        mId = 4805;
        break;
    case 61:
        mId = 9714;
        break;
    case 62:
        mId = 6448;
        break;
    case 63:
        mId = 6442;
        break;
    case 64:
        mId = 14632;
        break;
    case 65:
        mId = 14332;
        break;
    case 66:
        mId = 14331;
        break;
    case 67:
        mId = 8469;
        break;
    case 68:
        mId = 2830;
        break;
    case 69:
        mId = 2346;
        break;
    default:
        SendSysMessage(LANG_NO_MOUNT);
        SetSentErrorMessage(true);
        return false;
    }

    Player* target = getSelectedPlayerOrSelf();
    if (target == NULL)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_YOU_GIVE_MOUNT, target->GetName());
    if (needReportToTarget(target))
        ChatHandler(target).PSendSysMessage(LANG_MOUNT_GIVED, GetName());

    target->SetUInt32Value(UNIT_FIELD_FLAGS, 0x001000);
    target->Mount(mId);

    WorldPacket data(SMSG_FORCE_RUN_SPEED_CHANGE, (8 + 4 + 1 + 4));
    data << target->GetPackGUID();
    data << (uint32)0;
    data << (uint8)0;                                       //new 2.1.0
    data << float(speed);
    target->SendMessageToSet(&data, true);

    data.Initialize(SMSG_FORCE_SWIM_SPEED_CHANGE, (8 + 4 + 4));
    data << target->GetPackGUID();
    data << (uint32)0;
    data << float(speed);
    target->SendMessageToSet(&data, true);

    return true;
}

//Edit Player money
bool ChatHandler::HandleModifyMoneyCommand(const char* args)
{
    if (!*args)
        return false;

    Player* target = getSelectedPlayerOrSelf();
    if (target == NULL)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    int32 addmoney = atoi((char*)args);

    uint32 moneyuser = target->GetMoney();

    if (addmoney < 0)
    {
        int32 newmoney = moneyuser + addmoney;

        sLog.outDetail(GetOregonString(LANG_CURRENT_MONEY), moneyuser, addmoney, newmoney);
        if (newmoney <= 0)
        {
            PSendSysMessage(LANG_YOU_TAKE_ALL_MONEY, target->GetName());
            if (needReportToTarget(target))
                ChatHandler(target).PSendSysMessage(LANG_YOURS_ALL_MONEY_GONE, GetName());

            target->SetMoney(0);
        }
        else
        {
            PSendSysMessage(LANG_YOU_TAKE_MONEY, abs(addmoney), target->GetName());
            if (needReportToTarget(target))
                ChatHandler(target).PSendSysMessage(LANG_YOURS_MONEY_TAKEN, GetName(), abs(addmoney));
            target->SetMoney(newmoney);
        }
    }
    else
    {
        PSendSysMessage(LANG_YOU_GIVE_MONEY, addmoney, target->GetName());
        if (needReportToTarget(target))
            ChatHandler(target).PSendSysMessage(LANG_YOURS_MONEY_GIVEN, GetName(), addmoney);
        target->ModifyMoney(addmoney);
    }

    sLog.outDetail(GetOregonString(LANG_NEW_MONEY), moneyuser, addmoney, target->GetMoney());

    return true;
}

//Edit Player field
bool ChatHandler::HandleModifyBitCommand(const char* args)
{
    if (!*args)
        return false;

    Player* target = getSelectedPlayerOrSelf();
    if (!target)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    char* pField = strtok((char*)args, " ");
    if (!pField)
        return false;

    char* pBit = strtok(NULL, " ");
    if (!pBit)
        return false;

    uint16 field = atoi(pField);
    uint32 bit   = atoi(pBit);

    if (field < 1 || field >= PLAYER_END)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }
    if (bit < 1 || bit > 32)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    if (target->HasFlag(field, (1 << (bit - 1))))
    {
        target->RemoveFlag(field, (1 << (bit - 1)));
        PSendSysMessage(LANG_REMOVE_BIT, bit, field);
    }
    else
    {
        target->SetFlag(field, (1 << (bit - 1)));
        PSendSysMessage(LANG_SET_BIT, bit, field);
    }
    return true;
}

bool ChatHandler::HandleModifyHonorCommand (const char* args)
{
    if (!*args)
        return false;

    Player* target = getSelectedPlayerOrSelf();
    if (!target)
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    int32 amount = (uint32)atoi(args);

    target->ModifyHonorPoints(amount);

    PSendSysMessage(LANG_COMMAND_MODIFY_HONOR, target->GetName(), target->GetHonorPoints());

    return true;
}

bool ChatHandler::HandleTeleCommand(const char* args)
{
    if (!*args)
        return false;

    Player* _player = m_session->GetPlayer();

    // id, or string, or [name] Shift-click form |color|Htele:id|h[name]|h|r
    GameTele const* tele = extractGameTeleFromLink((char*)args);

    if (!tele)
    {
        SendSysMessage(LANG_COMMAND_TELE_NOTFOUND);
        SetSentErrorMessage(true);
        return false;
    }

    MapEntry const* me = sMapStore.LookupEntry(tele->mapId);
    if (!me || me->IsBattlegroundOrArena())
    {
        SendSysMessage(LANG_CANNOT_TELE_TO_BG);
        SetSentErrorMessage(true);
        return false;
    }

    // stop flight if need
    if (_player->IsInFlight())
    {
        _player->GetMotionMaster()->MovementExpired();
        _player->CleanupAfterTaxiFlight();
    }
    // save only in non-flight case
    else
        _player->SaveRecallPosition();

    // let gm fly remain
    if (_player->HasUnitMovementFlag(MOVEMENTFLAG_FLYING))
    {
        WorldPacket data;
        data.SetOpcode(SMSG_MOVE_SET_CAN_FLY);
        data << _player->GetPackGUID();
        data << uint32(0);                                      // unknown
        _player->SendMessageToSet(&data, true);
    }

    _player->TeleportTo(tele->mapId, tele->position_x, tele->position_y, tele->position_z, tele->orientation);
    return true;
}

bool ChatHandler::HandleLookupAreaCommand(const char* args)
{
    if (!*args)
        return false;

    std::string namepart = args;
    std::wstring wnamepart;

    if (!Utf8toWStr (namepart, wnamepart))
        return false;

    bool found = false;
    uint32 count = 0;
    uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

    // converting string that we try to find to lower case
    wstrToLower (wnamepart);

    // Search in AreaTable.dbc
    for (uint32 areaflag = 0; areaflag < sAreaStore.GetNumRows (); ++areaflag)
    {
        AreaTableEntry const* areaEntry = sAreaStore.LookupEntry (areaflag);
        if (areaEntry)
        {
            int loc = m_session ? int(m_session->GetSessionDbcLocale()) : sWorld.GetDefaultDbcLocale();
            std::string name = areaEntry->area_name[loc];
            if (name.empty())
                continue;

            if (!Utf8FitTo (name, wnamepart))
            {
                loc = 0;
                for (; loc < MAX_LOCALE; ++loc)
                {
                    if (m_session && loc == m_session->GetSessionDbcLocale ())
                        continue;

                    name = areaEntry->area_name[loc];
                    if (name.empty ())
                        continue;

                    if (Utf8FitTo (name, wnamepart))
                        break;
                }
            }

            if (loc < MAX_LOCALE)
            {
                if (maxResults && count++ == maxResults)
                {
                    PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                    return true;
                }

                // send area in "id - [name]" format
                std::ostringstream ss;
                if (m_session)
                    ss << areaEntry->ID << " - |cffffffff|Harea:" << areaEntry->ID << "|h[" << name << " " << localeNames[loc] << "]|h|r";
                else
                    ss << areaEntry->ID << " - " << name << " " << localeNames[loc];

                SendSysMessage (ss.str ().c_str());

                if (!found)
                    found = true;
            }
        }
    }

    if (!found)
        SendSysMessage (LANG_COMMAND_NOAREAFOUND);

    return true;
}

//Find tele in game_tele order by name
bool ChatHandler::HandleLookupTeleCommand(const char* args)
{
    if (!*args)
    {
        SendSysMessage(LANG_COMMAND_TELE_PARAMETER);
        SetSentErrorMessage(true);
        return false;
    }

    char const* str = strtok((char*)args, " ");
    if (!str)
        return false;

    std::string namepart = str;
    std::wstring wnamepart;

    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    // converting string that we try to find to lower case
    wstrToLower(wnamepart);

    std::ostringstream reply;
    uint32 count = 0;
    uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);
    bool limitReached = false;

    GameTeleMap const& teleMap = sObjectMgr.GetGameTeleMap();
    for (GameTeleMap::const_iterator itr = teleMap.begin(); itr != teleMap.end(); ++itr)
    {
        GameTele const* tele = &itr->second;

        if (tele->wnameLow.find(wnamepart) == std::wstring::npos)
            continue;

        if (maxResults && count++ == maxResults)
        {
            limitReached = true;
            break;
        }

        if (m_session)
            reply << "  |cffffffff|Htele:" << itr->first << "|h[" << tele->name << "]|h|r\n";
        else
            reply << "  " << itr->first << " " << tele->name << "\n";
    }

    if (reply.str().empty())
        SendSysMessage(LANG_COMMAND_TELE_NOLOCATION);
    else
        PSendSysMessage(LANG_COMMAND_TELE_LOCATION, reply.str().c_str());

    if (limitReached)
        PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);

    return true;
}

//Enable\Dissable accept whispers (for GM)
bool ChatHandler::HandleWhispersCommand(const char* args)
{
    if (!*args)
    {
        PSendSysMessage(LANG_COMMAND_WHISPERACCEPTING, m_session->GetPlayer()->isAcceptWhispers() ?  GetOregonString(LANG_ON) : GetOregonString(LANG_OFF));
        return true;
    }

    std::string argstr = (char*)args;
    // whisper on
    if (argstr == "on")
    {
        m_session->GetPlayer()->SetAcceptWhispers(true);
        SendSysMessage(LANG_COMMAND_WHISPERON);
        return true;
    }

    // whisper off
    if (argstr == "off")
    {
        m_session->GetPlayer()->SetAcceptWhispers(false);
        SendSysMessage(LANG_COMMAND_WHISPEROFF);
        return true;
    }

    SendSysMessage(LANG_USE_BOL);
    SetSentErrorMessage(true);
    return false;
}

//Save all players in the world
bool ChatHandler::HandleSaveAllCommand(const char* /*args*/)
{
    ObjectAccessor::Instance().SaveAllPlayers();
    SendSysMessage(LANG_PLAYERS_SAVED);
    return true;
}

//Send mail by command
bool ChatHandler::HandleSendMailCommand(const char* args)
{
    if (!*args)
        return false;

    // format: name "subject text" "mail text"

    char* pName = strtok((char*)args, " ");
    if (!pName)
        return false;

    char* tail1 = strtok(NULL, "");
    if (!tail1)
        return false;

    char* msgSubject;
    if (*tail1 == '"')
        msgSubject = strtok(tail1 + 1, "\"");
    else
    {
        char* space = strtok(tail1, "\"");
        if (!space)
            return false;
        msgSubject = strtok(NULL, "\"");
    }

    if (!msgSubject)
        return false;

    char* tail2 = strtok(NULL, "");
    if (!tail2)
        return false;

    char* msgText;
    if (*tail2 == '"')
        msgText = strtok(tail2 + 1, "\"");
    else
    {
        char* space = strtok(tail2, "\"");
        if (!space)
            return false;
        msgText = strtok(NULL, "\"");
    }

    if (!msgText)
        return false;

    // pName, msgSubject, msgText isn't NUL after prev. check
    std::string name    = pName;
    std::string subject = msgSubject;
    std::string text    = msgText;

    if (!normalizePlayerName(name))
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    uint64 receiver_guid = sObjectMgr.GetPlayerGUIDByName(name);
    if (!receiver_guid)
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    sObjectMgr.GenerateMailID();

    // from console show not existed sender
    MailSender sender(MAIL_NORMAL, m_session ? m_session->GetPlayer()->GetGUIDLow() : 0, MAIL_STATIONERY_GM);

    uint32 itemTextId = !text.empty() ? sObjectMgr.CreateItemText(text) : 0;

    Player* receiver = sObjectMgr.GetPlayer(receiver_guid);

    MailDraft(subject, itemTextId)
    .SendMailTo(MailReceiver(receiver, GUID_LOPART(receiver_guid)), sender);

    PSendSysMessage(LANG_MAIL_SENT, name.c_str());
    return true;
}

// teleport player to given game_tele.entry
bool ChatHandler::HandleNameTeleCommand(const char* args)
{
    if (!*args)
        return false;

    char* pName = strtok((char*)args, " ");

    if (!pName)
        return false;

    std::string name = pName;

    if (!normalizePlayerName(name))
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    char* tail = strtok(NULL, "");
    if (!tail)
        return false;

    // id, or string, or [name] Shift-click form |color|Htele:id|h[name]|h|r
    GameTele const* tele = extractGameTeleFromLink(tail);
    if (!tele)
    {
        SendSysMessage(LANG_COMMAND_TELE_NOTFOUND);
        SetSentErrorMessage(true);
        return false;
    }

    MapEntry const* me = sMapStore.LookupEntry(tele->mapId);
    if (!me || me->IsBattlegroundOrArena())
    {
        SendSysMessage(LANG_CANNOT_TELE_TO_BG);
        SetSentErrorMessage(true);
        return false;
    }

    Player* target = sObjectMgr.GetPlayer(name.c_str());
    if (target)
    {
        if (target->IsBeingTeleported())
        {
            PSendSysMessage(LANG_IS_TELEPORTED, target->GetName());
            SetSentErrorMessage(true);
            return false;
        }

        PSendSysMessage(LANG_TELEPORTING_TO, target->GetName(), "", tele->name.c_str());
        if (needReportToTarget(target))
            ChatHandler(target).PSendSysMessage(LANG_TELEPORTED_TO_BY, GetName());

        // stop flight if need
        if (target->IsInFlight())
        {
            target->GetMotionMaster()->MovementExpired();
            target->m_taxi.ClearTaxiDestinations();
        }
        // save only in non-flight case
        else
            target->SaveRecallPosition();

        target->TeleportTo(tele->mapId, tele->position_x, tele->position_y, tele->position_z, tele->orientation);
    }
    else if (uint64 guid = sObjectMgr.GetPlayerGUIDByName(name.c_str()))
    {
        PSendSysMessage(LANG_TELEPORTING_TO, name.c_str(), GetOregonString(LANG_OFFLINE), tele->name.c_str());
        Player::SavePositionInDB(tele->mapId, tele->position_x, tele->position_y, tele->position_z, tele->orientation,
                                 MapManager::Instance().GetZoneId(tele->mapId, tele->position_x, tele->position_y, tele->position_z), guid);
    }
    else
        PSendSysMessage(LANG_NO_PLAYER, name.c_str());

    return true;
}

//Teleport group to given game_tele.entry
bool ChatHandler::HandleGroupTeleCommand(const char* args)
{
    if (!*args)
        return false;

    Player* player = getSelectedPlayer();
    if (!player)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    // id, or string, or [name] Shift-click form |color|Htele:id|h[name]|h|r
    GameTele const* tele = extractGameTeleFromLink((char*)args);
    if (!tele)
    {
        SendSysMessage(LANG_COMMAND_TELE_NOTFOUND);
        SetSentErrorMessage(true);
        return false;
    }

    MapEntry const* me = sMapStore.LookupEntry(tele->mapId);
    if (!me || me->IsBattlegroundOrArena())
    {
        SendSysMessage(LANG_CANNOT_TELE_TO_BG);
        SetSentErrorMessage(true);
        return false;
    }
    Group* grp = player->GetGroup();
    if (!grp)
    {
        PSendSysMessage(LANG_NOT_IN_GROUP, player->GetName());
        SetSentErrorMessage(true);
        return false;
    }

    for (GroupReference* itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* pl = itr->GetSource();

        if (!pl || !pl->GetSession())
            continue;

        if (pl->IsBeingTeleported())
        {
            PSendSysMessage(LANG_IS_TELEPORTED, pl->GetName());
            continue;
        }

        PSendSysMessage(LANG_TELEPORTING_TO, pl->GetName(), "", tele->name.c_str());
        if (needReportToTarget(pl))
            ChatHandler(pl).PSendSysMessage(LANG_TELEPORTED_TO_BY, GetName());

        // stop flight if need
        if (pl->IsInFlight())
        {
            pl->GetMotionMaster()->MovementExpired();
            pl->m_taxi.ClearTaxiDestinations();
        }
        // save only in non-flight case
        else
            pl->SaveRecallPosition();

        pl->TeleportTo(tele->mapId, tele->position_x, tele->position_y, tele->position_z, tele->orientation);
    }

    return true;
}

//Summon group of player
bool ChatHandler::HandleGroupSummonCommand(const char* args)
{
    if (!*args)
        return false;

    std::string name = args;

    if (!normalizePlayerName(name))
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    Player* player = sObjectMgr.GetPlayer(name.c_str());
    if (!player)
    {
        PSendSysMessage(LANG_NO_PLAYER, args);
        SetSentErrorMessage(true);
        return false;
    }

    Group* grp = player->GetGroup();

    if (!grp)
    {
        PSendSysMessage(LANG_NOT_IN_GROUP, player->GetName());
        SetSentErrorMessage(true);
        return false;
    }

    Map* gmMap = m_session->GetPlayer()->GetMap();
    bool to_instance =  gmMap->Instanceable();

    // we are in instance, and can summon only player in our group with us as lead
    if (to_instance && (
            !m_session->GetPlayer()->GetGroup() || (grp->GetLeaderGUID() != m_session->GetPlayer()->GetGUID()) ||
            (m_session->GetPlayer()->GetGroup()->GetLeaderGUID() != m_session->GetPlayer()->GetGUID())))
        // the last check is a bit excessive, but let it be, just in case
    {
        SendSysMessage(LANG_CANNOT_SUMMON_TO_INST);
        SetSentErrorMessage(true);
        return false;
    }

    for (GroupReference* itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* pl = itr->GetSource();

        if (!pl || pl == m_session->GetPlayer() || !pl->GetSession())
            continue;

        if (pl->IsBeingTeleported())
        {
            PSendSysMessage(LANG_IS_TELEPORTED, pl->GetName());
            SetSentErrorMessage(true);
            return false;
        }

        if (to_instance)
        {
            Map* plMap = pl->GetMap();

            if (plMap->Instanceable() && plMap->GetInstanceId() != gmMap->GetInstanceId())
            {
                // cannot summon from instance to instance
                PSendSysMessage(LANG_CANNOT_SUMMON_TO_INST, pl->GetName());
                SetSentErrorMessage(true);
                return false;
            }
        }

        PSendSysMessage(LANG_SUMMONING, pl->GetName(), "");
        if (needReportToTarget(pl))
            ChatHandler(pl).PSendSysMessage(LANG_SUMMONED_BY, GetName());

        // stop flight if need
        if (pl->IsInFlight())
        {
            pl->GetMotionMaster()->MovementExpired();
            pl->m_taxi.ClearTaxiDestinations();
        }
        // save only in non-flight case
        else
            pl->SaveRecallPosition();

        // before GM
        float x, y, z;
        m_session->GetPlayer()->GetClosePoint(x, y, z, pl->GetObjectSize());
        pl->TeleportTo(m_session->GetPlayer()->GetMapId(), x, y, z, pl->GetOrientation());
    }

    return true;
}

//teleport at coordinates
bool ChatHandler::HandleGoXYCommand(const char* args)
{
    if (!*args)
        return false;

    Player* _player = m_session->GetPlayer();

    char* px = strtok((char*)args, " ");
    char* py = strtok(NULL, " ");
    char* pmapid = strtok(NULL, " ");

    if (!px || !py)
        return false;

    float x = (float)atof(px);
    float y = (float)atof(py);
    uint32 mapid;
    if (pmapid)
        mapid = (uint32)atoi(pmapid);
    else mapid = _player->GetMapId();

    if (!MapManager::IsValidMapCoord(mapid, x, y))
    {
        PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapid);
        SetSentErrorMessage(true);
        return false;
    }

    // stop flight if need
    if (_player->IsInFlight())
    {
        _player->GetMotionMaster()->MovementExpired();
        _player->CleanupAfterTaxiFlight();
    }
    // save only in non-flight case
    else
        _player->SaveRecallPosition();

    Map const* map = MapManager::Instance().CreateBaseMap(mapid);
    float z = std::max(map->GetHeight(x, y, MAX_HEIGHT), map->GetWaterLevel(x, y));

    _player->TeleportTo(mapid, x, y, z, _player->GetOrientation());

    return true;
}

//teleport at coordinates, including Z
bool ChatHandler::HandleGoXYZCommand(const char* args)
{
    if (!*args)
        return false;

    Player* _player = m_session->GetPlayer();

    char* px = strtok((char*)args, " ");
    char* py = strtok(NULL, " ");
    char* pz = strtok(NULL, " ");
    char* pmapid = strtok(NULL, " ");

    if (!px || !py || !pz)
        return false;

    float x = (float)atof(px);
    float y = (float)atof(py);
    float z = (float)atof(pz);
    uint32 mapid;
    if (pmapid)
        mapid = (uint32)atoi(pmapid);
    else
        mapid = _player->GetMapId();

    if (!MapManager::IsValidMapCoord(mapid, x, y, z))
    {
        PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapid);
        SetSentErrorMessage(true);
        return false;
    }

    // stop flight if need
    if (_player->IsInFlight())
    {
        _player->GetMotionMaster()->MovementExpired();
        _player->CleanupAfterTaxiFlight();
    }
    // save only in non-flight case
    else
        _player->SaveRecallPosition();

    _player->TeleportTo(mapid, x, y, z, _player->GetOrientation());

    return true;
}

//teleport at coordinates
bool ChatHandler::HandleGoZoneXYCommand(const char* args)
{
    if (!*args)
        return false;

    Player* _player = m_session->GetPlayer();

    char* px = strtok((char*)args, " ");
    char* py = strtok(NULL, " ");
    char* tail = strtok(NULL, "");

    char* cAreaId = extractKeyFromLink(tail, "Harea");      // string or [name] Shift-click form |color|Harea:area_id|h[name]|h|r

    if (!px || !py)
        return false;

    float x = (float)atof(px);
    float y = (float)atof(py);
    uint32 areaid = cAreaId ? (uint32)atoi(cAreaId) : _player->GetZoneId();

    AreaTableEntry const* areaEntry = GetAreaEntryByAreaID(areaid);

    if (x < 0 || x > 100 || y < 0 || y > 100 || !areaEntry)
    {
        PSendSysMessage(LANG_INVALID_ZONE_COORD, x, y, areaid);
        SetSentErrorMessage(true);
        return false;
    }

    // update to parent zone if exist (client map show only zones without parents)
    AreaTableEntry const* zoneEntry = areaEntry->zone ? GetAreaEntryByAreaID(areaEntry->zone) : areaEntry;

    Map const* map = MapManager::Instance().CreateBaseMap(zoneEntry->mapid);

    if (map->Instanceable())
    {
        PSendSysMessage(LANG_INVALID_ZONE_MAP, areaEntry->ID, areaEntry->area_name[m_session->GetSessionDbcLocale()], map->GetId(), map->GetMapName());
        SetSentErrorMessage(true);
        return false;
    }

    Zone2MapCoordinates(x, y, zoneEntry->ID);

    if (!MapManager::IsValidMapCoord(zoneEntry->mapid, x, y))
    {
        PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, zoneEntry->mapid);
        SetSentErrorMessage(true);
        return false;
    }

    // stop flight if need
    if (_player->IsInFlight())
    {
        _player->GetMotionMaster()->MovementExpired();
        _player->CleanupAfterTaxiFlight();
    }
    // save only in non-flight case
    else
        _player->SaveRecallPosition();

    float z = std::max(map->GetHeight(x, y, MAX_HEIGHT), map->GetWaterLevel(x, y));
    _player->TeleportTo(zoneEntry->mapid, x, y, z, _player->GetOrientation());

    return true;
}

//teleport to grid
bool ChatHandler::HandleGoGridCommand(const char* args)
{
    if (!*args)    return false;
    Player* _player = m_session->GetPlayer();

    char* px = strtok((char*)args, " ");
    char* py = strtok(NULL, " ");
    char* pmapid = strtok(NULL, " ");

    if (!px || !py)
        return false;

    float grid_x = (float)atof(px);
    float grid_y = (float)atof(py);
    uint32 mapid;
    if (pmapid)
        mapid = (uint32)atoi(pmapid);
    else mapid = _player->GetMapId();

    // center of grid
    float x = (grid_x - CENTER_GRID_ID + 0.5f) * SIZE_OF_GRIDS;
    float y = (grid_y - CENTER_GRID_ID + 0.5f) * SIZE_OF_GRIDS;

    if (!MapManager::IsValidMapCoord(mapid, x, y))
    {
        PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapid);
        SetSentErrorMessage(true);
        return false;
    }

    // stop flight if need
    if (_player->IsInFlight())
    {
        _player->GetMotionMaster()->MovementExpired();
        _player->CleanupAfterTaxiFlight();
    }
    // save only in non-flight case
    else
        _player->SaveRecallPosition();

    Map const* map = MapManager::Instance().CreateBaseMap(mapid);
    float z = std::max(map->GetHeight(x, y, MAX_HEIGHT), map->GetWaterLevel(x, y));
    _player->TeleportTo(mapid, x, y, z, _player->GetOrientation());

    return true;
}

bool ChatHandler::HandleDrunkCommand(const char* args)
{
    if (!*args)    return false;

    uint32 drunklevel = (uint32)atoi(args);
    if (drunklevel > 100)
        drunklevel = 100;

    uint16 drunkMod = drunklevel * 0xFFFF / 100;

    Player* target = getSelectedPlayer();
    if (!target)
        target = m_session->GetPlayer();

    if (target)
        target->SetDrunkValue(drunkMod);

    return true;
}

