/*
 * Copyright (C) 2008 - 2010 Trinity <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "OutdoorPvPWG.h"
#include "SpellAuras.h"
#include "Vehicle.h"
#include "ObjectMgr.h"
#include "World.h"
#include "Group.h"
#include "Chat.h"
#include "MapManager.h"
#include "ScriptedCreature.h"
#include "ScriptPCH.h"
#include "OutdoorPvPMgr.h"

/* Vehicle teleport system*/
Map* pMap;

bool Captured = true;
bool Changing = false;

OutdoorPvPWG::OutdoorPvPWG()
{
    m_TypeId = OUTDOOR_PVP_WG;
    m_BattleId = OUTDOOR_PVP_WG;
    m_ZoneId = 4197;
    m_maxplayer = 120; // Maximum player in wintergrasp...
    m_TimeForResponseOfQueue = 500;
    m_LastResurrectTime = 0; // Temporal copy of BG system till 3.2
}

void _LoadTeamPair(TeamPairMap &pairMap, const TeamPair *pair)
{
    while((*pair)[0])
    {
        pairMap[(*pair)[0]] = (*pair)[1];
        pairMap[(*pair)[1]] = (*pair)[0];
        ++pair;
    }
}

void _RespawnCreatureIfNeeded(Creature *cr, uint32 entry)
{
    if (!cr)
        return;

        cr->UpdateEntry(entry); // SetOriginalEntry as used before may lead to crash
        if (cr->GetAreaId() == 4575)
           cr->AI()->EnterEvadeMode();
        if (entry != cr->GetEntry() || !cr->isAlive())
            cr->Respawn(true);
        cr->SetVisible(true);
        if (cr->GetEntry() == CRE_ENG_A || cr->GetEntry() == CRE_ENG_H)
            cr->SetReactState(REACT_PASSIVE);
}

void CapturedWorkshop(Creature* cr, uint32 entry)
{
    if (!cr)
        return;

    bool Continue = false;
    std::string name;
    if (OutdoorPvPWG *pvpWG = (OutdoorPvPWG*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(4197))
    {
        if (cr->GetEntry() == CRE_ENG_A || cr->GetEntry() == CRE_ENG_H)
            switch (cr->GetAreaId())
            {
                case 4538: // Sunken Ring
                    name = "The Sunken Ring siege workshop";
                    Continue = true;
                    break;
                case 4539: // Broken Temple
                    name = "The Broken Temple siege workshop";
                    Continue = true;
                    break;
                case 4611: // Westspark Workshop
                    name = "The Westspark siege workshop";
                    Continue = true;
                    break;
                case 4612: // Eastspark Workshop
                    name = "The Eastspark siege workshop";
                    Continue = true;
                    break;
            }

        if (Continue == true)
        {
            if (cr->GetEntry() == CRE_ENG_A && entry == CRE_ENG_A)
                pvpWG->SendWarningToAllInZone(WG_TEXT_ALLIANCE_CAPTURED, name.c_str());
            if (cr->GetEntry() == CRE_ENG_H && entry == CRE_ENG_H)
                pvpWG->SendWarningToAllInZone(WG_TEXT_HORDE_CAPTURED, name.c_str());
        }
    }
}

bool OutdoorPvPWG::SetupOutdoorPvP()
{
    if (!sWorld->getBoolConfig(CONFIG_OUTDOORPVP_WINTERGRASP_ENABLED))
    {
        sWorld->setWorldState(WORLDSTATE_WINTERGRASP_CONTROLING_FACTION, TEAM_NEUTRAL);
        return false;
    }

    //load worlstates
    m_wartime  = sWorld->getWorldState(WORLDSTATE_WINTERGRASP_WARTIME) ? true : false;
    m_timer    = uint32(sWorld->getWorldState(WORLDSTATE_WINTERGRASP_TIMER));
    m_defender = TeamId(sWorld->getWorldState(WORLDSTATE_WINTERGRASP_DEFENDERS) ? true : false);
    m_WSSaveTimer = sWorld->getIntConfig(CONFIG_OUTDOORPVP_WINTERGRASP_SAVESTATE_PERIOD);

    if (m_timer == 0)
       m_timer = sWorld->getIntConfig(CONFIG_OUTDOORPVP_WINTERGRASP_START_TIME) * MINUTE * IN_MILLISECONDS;

    sWorld->setWorldState(WORLDSTATE_WINTERGRASP_CONTROLING_FACTION, getDefenderTeam());
    m_changeDefender = false;
    m_workshopCount[TEAM_ALLIANCE] = 0;
    m_workshopCount[TEAM_HORDE] = 0;
    m_tenacityStack = 0;
    m_gate = NULL;

    std::list<uint32> engGuids;
    std::list<uint32> spiritGuids;

    // Store Eng, spirit guide guids and questgiver for later use
    QueryResult result = WorldDatabase.PQuery("SELECT guid, id FROM creature WHERE creature.map=571"
        " AND creature.id IN (%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u);",
        CRE_ENG_A, CRE_ENG_H, CRE_SPI_A, CRE_SPI_H, 31101, 31051, 31102, 31052,
        31107, 31109, 31151, 31153, 31106, 31108, 31053, 31054, 31091, 31036);
    if (!result)
        sLog->outError("Cannot find siege workshop master or spirit guides in creature!");
    else
    {
        do
        {
            Position posHorde, posAlli;
            Field *fields = result->Fetch();
            switch(fields[1].GetUInt32())
            {
                case CRE_ENG_A:
                case CRE_ENG_H:
                    engGuids.push_back(fields[0].GetUInt32());
                    break;
                case CRE_SPI_A:
                case CRE_SPI_H:
                    spiritGuids.push_back(fields[0].GetUInt32());
                    break;
                case 31051:
                    posHorde.Relocate(5081.7f, 2173.73f, 365.878f, 0.855211f);
                    posAlli.Relocate(5296.56f, 2789.87f, 409.275f, 0.733038f);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31101:
                    posHorde.Relocate(5296.56f, 2789.87f, 409.275f, 0.733038f);
                    posAlli.Relocate(5016.57f, 3677.53f, 362.982f, 5.7525f);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31052:
                    posHorde.Relocate(5100.07f, 2168.89f, 365.779f, 1.97222f);
                    posAlli.Relocate(5298.43f, 2738.76f, 409.316f, 3.97174f);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31102:
                    posHorde.Relocate(5298.43f, 2738.76f, 409.316f, 3.97174f);
                    posAlli.Relocate(5030.44f, 3659.82f, 363.194f, 1.83336f);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31109:
                    posHorde.Relocate(5080.4f, 2199.0f, 359.489f, 2.96706f);
                    posAlli.Relocate(5234.97f, 2883.4f, 409.275f, 4.29351f);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31107:
                    posHorde.Relocate(5234.97f, 2883.4f, 409.275f, 4.29351f);
                    posAlli.Relocate(5008.64f, 3659.91f, 361.07f, 4.0796f);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31153:
                    posHorde.Relocate(5088.49f, 2188.18f, 365.647f, 5.25344f);
                    posAlli.Relocate(5366.13f, 2833.4f, 409.323f, 3.14159f);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31151:
                    posHorde.Relocate(5366.13f, 2833.4f, 409.323f, 3.14159f);
                    posAlli.Relocate(5032.33f, 3680.7f, 363.018f, 3.43167f);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31108:
                    posHorde.Relocate(5095.67f, 2193.28f, 365.924f, 4.93928f);
                    posAlli.Relocate(5295.56f, 2926.67f, 409.275f, 0.872665f);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31106:
                    posHorde.Relocate(5295.56f, 2926.67f, 409.275f, 0.872665f);
                    posAlli.Relocate(5032.66f, 3674.28f, 363.053f, 2.9447f);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31054:
                    posHorde.Relocate(5088.61f, 2167.66f, 365.689f, 0.680678f);
                    posAlli.Relocate(5371.4f, 3026.51f, 409.206f, 3.25003f);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31053:
                    posHorde.Relocate(5371.4f, 3026.51f, 409.206f, 3.25003f);
                    posAlli.Relocate(5032.44f, 3668.66f, 363.11f, 2.87402f);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31036:
                    posHorde.Relocate(5078.28f, 2183.7f, 365.029f, 1.46608f);
                    posAlli.Relocate(5359.13f, 2837.99f, 409.364f, 4.69893f);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31091:
                    posHorde.Relocate(5359.13f, 2837.99f, 409.364f, 4.69893f);
                    posAlli.Relocate(5022.43f, 3659.91f, 361.61f, 1.35426f);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                default:
                    break;
            }
        }while(result->NextRow());
    }

    // Select POI
    AreaPOIList areaPOIs;
    float minX = 9999, minY = 9999, maxX = -9999, maxY = -9999;
    for (uint32 i = 0; i < sAreaPOIStore.GetNumRows(); ++i)
    {
        const AreaPOIEntry * poiInfo = sAreaPOIStore.LookupEntry(i);
        if (poiInfo && poiInfo->zoneId == ZONE_WINTERGRASP)
        {
            areaPOIs.push_back(poiInfo);
            if (minX > poiInfo->x) minX = poiInfo->x;
            if (minY > poiInfo->y) minY = poiInfo->y;
            if (maxX < poiInfo->x) maxX = poiInfo->x;
            if (maxY < poiInfo->y) maxY = poiInfo->y;
        }
    }
    minX -= 20; minY -= 20; maxX += 20; maxY += 20;

    // Coords: 4290.330078, 1790.359985 - 5558.379883, 4048.889893
    result = WorldDatabase.PQuery("SELECT guid FROM gameobject,gameobject_template"
        " WHERE gameobject.map=571"
        " AND gameobject.position_x>%f AND gameobject.position_y>%f"
        " AND gameobject.position_x<%f AND gameobject.position_y<%f"
        " AND gameobject_template.type=33"
        " AND gameobject.id=gameobject_template.entry",
        minX, minY, maxX, maxY);
    if (!result)
        return false;

    do
    {
        Field *fields = result->Fetch();

        uint32 guid = fields[0].GetUInt32();
        GameObjectData const * goData = sObjectMgr->GetGOData(guid);
        if (!goData) // this should not happen
            continue;

        float x = goData->posX, y = goData->posY;
        float minDist = 100;
        AreaPOIList::iterator poi = areaPOIs.end();
        for (AreaPOIList::iterator itr = areaPOIs.begin(); itr != areaPOIs.end(); ++itr)
        {
            if (!(*itr)->icon[1]) // note: may for other use
                continue;

            float dist = (abs((*itr)->x - x) + abs((*itr)->y - y));
            if (minDist > dist)
            {
                minDist = dist;
                poi = itr;
            }
        }

        if (poi == areaPOIs.end())
            continue;

        // add building to the list
        TeamId teamId = x > POS_X_CENTER-300 ? getDefenderTeam() : getAttackerTeam();
        m_buildingStates[guid] = new BuildingState((*poi)->worldState, teamId, getDefenderTeam() != TEAM_ALLIANCE);
        if ((*poi)->id == 2246)
            m_gate = m_buildingStates[guid];
        areaPOIs.erase(poi);

        // add capture point
        uint32 capturePointEntry = 0;

        switch(goData->id)
        {
            //West Goblin Workshops
            case 192028: // NW
            case 192030: // W
            case 192032: // SW
                capturePointEntry = 190475;
                break;
            //East Goblin Workshops
            case 192029: // NE
            case 192031: // E
            case 192033: // SE
                capturePointEntry = 190487;
                break;
        }

        if (capturePointEntry)
        {
            uint32 engGuid = 0;
            uint32 spiritGuid = 0;
            // Find closest Eng to Workshop
            float minDist = 100;
            for (std::list<uint32>::iterator itr = engGuids.begin(); itr != engGuids.end(); ++itr)
            {
                const CreatureData *creData = sObjectMgr->GetCreatureData(*itr);
                if (!creData)
                    continue;

                float dist = (abs(creData->posX - x) + abs(creData->posY - y));
                if (minDist > dist)
                {
                    minDist = dist;
                    engGuid = *itr;
                }
            }

            if (!engGuid)
            {
                sLog->outError("Cannot find nearby siege workshop master!");
                continue;
            }
            else
                engGuids.remove(engGuid);
            // Find closest Spirit Guide to Workshop
            minDist = 255;
            for (std::list<uint32>::iterator itr = spiritGuids.begin(); itr != spiritGuids.end(); ++itr)
            {
                const CreatureData *creData = sObjectMgr->GetCreatureData(*itr);
                if (!creData)
                    continue;

                float dist = (abs(creData->posX - x) + abs(creData->posY - y));
                if (minDist > dist)
                {
                    minDist = dist;
                    spiritGuid = *itr;
                }
            }

            // Inside fortress won't be capturable
            OPvPCapturePointWG *workshop = new OPvPCapturePointWG(this, m_buildingStates[guid]);
            if (goData->posX < POS_X_CENTER && !workshop->SetCapturePointData(capturePointEntry, goData->mapid, goData->posX + 40 * cos(goData->orientation + M_PI / 2), goData->posY + 40 * sin(goData->orientation + M_PI / 2), goData->posZ))
            {
                delete workshop;
                sLog->outError("Cannot add capture point!");
                continue;
            }
            // West fortress Workshop fix
            if(goData->id==192028 || goData->id==192029)
                workshop->m_capturePointGUID = goData->id;

            const CreatureData *creData = sObjectMgr->GetCreatureData(engGuid);
            if (!creData)
                continue;

            workshop->m_engEntry = const_cast<uint32*>(&creData->id);
            const_cast<CreatureData*>(creData)->displayid = 0;
            workshop->m_engGuid = engGuid;

            // Back spirit is linked to one of the inside workshops, 1 workshop wont have spirit
            if (spiritGuid)
            {
                spiritGuids.remove(spiritGuid);

                const CreatureData *spiritData = sObjectMgr->GetCreatureData(spiritGuid);
                if (!spiritData)
                    continue;

                workshop->m_spiEntry = const_cast<uint32*>(&spiritData->id);
                const_cast<CreatureData*>(spiritData)->displayid = 0;
                workshop->m_spiGuid = spiritGuid;
            }
            else
                workshop->m_spiGuid = 0;
            workshop->m_workshopGuid = guid;
            AddCapturePoint(workshop);
            m_buildingStates[guid]->type = BUILDING_WORKSHOP;
            workshop->SetTeamByBuildingState();
        }
    }while(result->NextRow());

    engGuids.clear();
    spiritGuids.clear();

    if (!m_gate)
    {
        sLog->outError("Cannot find wintergrasp fortress gate!");
        return false;
    }

    // Load Graveyard
    GraveYardMap::const_iterator graveLow  = sObjectMgr->mGraveYardMap.lower_bound(ZONE_WINTERGRASP);
    GraveYardMap::const_iterator graveUp   = sObjectMgr->mGraveYardMap.upper_bound(ZONE_WINTERGRASP);
    for (AreaPOIList::iterator itr = areaPOIs.begin(); itr != areaPOIs.end();)
    {
        if ((*itr)->icon[1] == 8)
        {
            // find or create grave yard
            const WorldSafeLocsEntry *loc = sObjectMgr->GetClosestGraveYard((*itr)->x, (*itr)->y, 0, (*itr)->mapId, 0); // (*itr)->z
            if (!loc)
            {
                ++itr;
                continue;
            }

            GraveYardMap::const_iterator graveItr;
            for (graveItr = graveLow; graveItr != graveUp; ++graveItr)
                if (graveItr->second.safeLocId == loc->ID)
                    break;
            if (graveItr == graveUp)
            {
                GraveYardData graveData;
                graveData.safeLocId = loc->ID;
                graveData.team = 0;
                graveItr = sObjectMgr->mGraveYardMap.insert(std::make_pair(ZONE_WINTERGRASP, graveData));
            }

            for (BuildingStateMap::iterator stateItr = m_buildingStates.begin(); stateItr != m_buildingStates.end(); ++stateItr)
                if (stateItr->second->worldState == (*itr)->worldState)
                {
                    stateItr->second->graveTeam = const_cast<uint32*>(&graveItr->second.team);
                    break;
                }
            areaPOIs.erase(itr++);
        }
        else
            ++itr;
    }

    //Titan Relic
    sObjectMgr->AddGOData(192829, 571, 5440.0f, 2840.8f, 420.43f + 10.0f, 0);

    _LoadTeamPair(m_goDisplayPair, OutdoorPvPWGGODisplayPair);
    _LoadTeamPair(m_creEntryPair, OutdoorPvPWGCreEntryPair);

    sWorld->SendWintergraspState();
    m_towerDamagedCount[TEAM_ALLIANCE] = 0;
    m_towerDestroyedCount[TEAM_ALLIANCE] = 0;
    m_towerDamagedCount[TEAM_HORDE] = 0;
    m_towerDestroyedCount[TEAM_HORDE] = 0;

    RemoveOfflinePlayerWGAuras();

    RegisterZone(ZONE_WINTERGRASP);

    if (m_wartime)
    {
        uint32 m_WSTimer = m_timer;
        StartBattle();
        m_timer = m_WSTimer;
    }

    return true;
}

void OutdoorPvPWG::ProcessEvent(GameObject *obj, uint32 eventId)
{
    if (obj->GetEntry() == 192829) // Titan Relic
    {
        if (obj->GetGOInfo()->goober.eventId == eventId && isWarTime() && /*MaingateDestroyed==true &&*/ m_gate &&  m_gate->damageState == DAMAGE_DESTROYED)
        {
            m_changeDefender = true;
            m_timer = 0;
        }
    }
    else if (obj->GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
    {
        BuildingStateMap::const_iterator itr = m_buildingStates.find(obj->GetDBTableGUIDLow());
        if (itr == m_buildingStates.end())
            return;

        switch(eventId)
        { // TODO - Localized msgs of GO names
            case 19672: case 19675: // Winter's Edge Tower
                msgStr = "The southern tower";
                break;
            case 18553: case 19677: // Shadowsight Tower
                msgStr = "The western tower";
                break;
            case 19673: case 19676: // Flamewatch Tower
                msgStr = "The eastern tower";
                break;
            case 19782: case 19786: // NW Workshop damaged
                msgStr = "The north-western factory";
                break;
            case 19783: case 19787: // NE Workshop damaged
                msgStr = "The north-eastern factory";
                break;
            case 19784: case 19788: // SW Workshop damaged
                msgStr = "The Westspark siege workshop";
                break;
            case 19785: case 19789: // SE Workshop damaged
                msgStr = "The Eastspark siege workshop";
                break;
            case 19776: case 19778: // E Workshop damaged
                msgStr = "The Sunken Ring siege workshop";
                break;
            case 19777: case 19779: // W Workshop damaged
                msgStr = "The Broken Temple siege workshop";
                break;
            case 19657: case 19661: // NW Wintergrasp Keep Tower damaged
                msgStr = "The north-western keep tower";
                break;
            case 19658: case 19663: // NE Wintergrasp Keep Tower damaged
                msgStr = "The north-eastern keep tower";
                break;
            case 19659: case 19662: // SW Wintergrasp Keep Tower damaged
                msgStr = "The south-western keep tower";
                break;
            case 19660: case 19664: // SE Wintergrasp Keep Tower damaged
                msgStr = "The south-eastern keep tower";
                break;
            default:
                msgStr = "";
        }

        BuildingState *state = itr->second;
        if (eventId == obj->GetGOInfo()->building.damagedEvent)
        {
           if (obj->GetEntry()==191810)
            {
               //obj->TakenDamage(30000);
               MaingateDestroyed=true;
            }

            state->damageState = DAMAGE_DAMAGED;
            switch(state->type)
            {
                case BUILDING_WORKSHOP:
                    break;
                case BUILDING_WALL:
                    for (PlayerSet::iterator itr = m_players[getDefenderTeam()].begin(); itr != m_players[getDefenderTeam()].end(); ++itr)
                    {
                        if (getDefenderTeam()==TEAM_ALLIANCE)
                            TeamIDsound=OutdoorPvP_WG_SOUND_KEEP_ASSAULTED_HORDE; //Allience Worn Sound
                        else
                            TeamIDsound=OutdoorPvP_WG_SOUND_KEEP_ASSAULTED_ALLIANCE;  //Horde Worn Sound
                        (*itr)->PlayDirectSound(TeamIDsound) ; // Wintergrasp Fortress under Siege
                    }
                    for (PlayerSet::iterator itr = m_players[getAttackerTeam()].begin(); itr != m_players[getAttackerTeam()].end(); ++itr)
                    {
                        if (getDefenderTeam()==TEAM_ALLIANCE)
                            TeamIDsound=OutdoorPvP_WG_SOUND_KEEP_ASSAULTED_HORDE; //Allience Worn Sound
                        else
                            TeamIDsound=OutdoorPvP_WG_SOUND_KEEP_ASSAULTED_ALLIANCE;  //Horde Worn Sound
                        (*itr)->PlayDirectSound(TeamIDsound) ; // Wintergrasp Fortress under Siege
                    }
                    break;
                case BUILDING_TOWER:
                    ++m_towerDamagedCount[state->GetTeam()];
                    if (msgStr.c_str())
                    SendWarningToAllInZone(WG_TEXT_DAMAGED, msgStr.c_str());
                    for (PlayerSet::iterator itr = m_players[getDefenderTeam()].begin(); itr != m_players[getDefenderTeam()].end(); ++itr)
                    {
                        if (getDefenderTeam()==TEAM_ALLIANCE)
                            TeamIDsound=OutdoorPvP_WG_SOUND_KEEP_CAPTURED_HORDE; //Allience Worn Sound
                        else
                            TeamIDsound=OutdoorPvP_WG_SOUND_KEEP_CAPTURED_ALLIANCE;  //Horde Worn Sound
                        (*itr)->PlayDirectSound(TeamIDsound) ; // Wintergrasp Fortress under Siege
                    }
                    for (PlayerSet::iterator itr = m_players[getAttackerTeam()].begin(); itr != m_players[getAttackerTeam()].end(); ++itr)
                    {
                        if (getDefenderTeam()==TEAM_ALLIANCE)
                            TeamIDsound=OutdoorPvP_WG_SOUND_KEEP_CAPTURED_HORDE; //Allience Worn Sound
                        else
                            TeamIDsound=OutdoorPvP_WG_SOUND_KEEP_CAPTURED_ALLIANCE;  //Horde Worn Sound
                        (*itr)->PlayDirectSound(TeamIDsound) ; // Wintergrasp Fortress under Siege
                    }
                    break;
                case BUILDING_KEEP_TOWER:
                    if (msgStr.c_str())
                    SendWarningToAllInZone(WG_TEXT_DAMAGED, msgStr.c_str());
                    break;
            }
        }
        else if (eventId == obj->GetGOInfo()->building.destroyedEvent)
        {
            state->damageState = DAMAGE_DESTROYED;

            switch(state->type)
            {
                case BUILDING_WORKSHOP:
                    ModifyWorkshopCount(state->GetTeam(), false);
                    break;
                case BUILDING_WALL:
                    for (PlayerSet::iterator itr = m_players[getDefenderTeam()].begin(); itr != m_players[getDefenderTeam()].end(); ++itr)
                    {
                        if (getDefenderTeam()==TEAM_ALLIANCE)
                            TeamIDsound=OutdoorPvP_WG_SOUND_KEEP_CAPTURED_HORDE; //Allience Worn Sound
                        else
                            TeamIDsound=OutdoorPvP_WG_SOUND_KEEP_CAPTURED_ALLIANCE;  //Horde Worn Sound
                        (*itr)->PlayDirectSound(TeamIDsound) ; // Wintergrasp Fortress under Siege
                    }
                    for (PlayerSet::iterator itr = m_players[getAttackerTeam()].begin(); itr != m_players[getAttackerTeam()].end(); ++itr)
                    {
                        if (getDefenderTeam()==TEAM_ALLIANCE)
                            TeamIDsound=OutdoorPvP_WG_SOUND_KEEP_CAPTURED_HORDE; //Allience Worn Sound
                        else
                            TeamIDsound=OutdoorPvP_WG_SOUND_KEEP_CAPTURED_ALLIANCE;  //Horde Worn Sound
                        (*itr)->PlayDirectSound(TeamIDsound) ; // Wintergrasp Fortress under Siege
                    }
                    break;
                case BUILDING_TOWER:
                     --m_towerDamagedCount[state->GetTeam()];
                     ++m_towerDestroyedCount[state->GetTeam()];
                     if (msgStr.c_str())
                     SendWarningToAllInZone(WG_TEXT_DESTROYED, msgStr.c_str());
                     if (state->GetTeam() == getAttackerTeam())
                     {
                        TeamCastSpell(getAttackerTeam(), -SPELL_TOWER_CONTROL);
                        TeamCastSpell(getDefenderTeam(), -SPELL_TOWER_CONTROL);
                        uint32 attStack = 3 - m_towerDestroyedCount[getAttackerTeam()];
                        if (m_towerDestroyedCount[getAttackerTeam()])
                            for (PlayerSet::iterator itr = m_players[getDefenderTeam()].begin(); itr != m_players[getDefenderTeam()].end(); ++itr)
                                if ((*itr)->getLevel() > 74)
                                    (*itr)->SetAuraStack(SPELL_TOWER_CONTROL, (*itr), m_towerDestroyedCount[getAttackerTeam()]);

                        if (attStack!=0)
                        {
                            for (PlayerSet::iterator itr = m_players[getAttackerTeam()].begin(); itr != m_players[getAttackerTeam()].end(); ++itr)
                                if ((*itr)->getLevel() > 74)
                                    (*itr)->SetAuraStack(SPELL_TOWER_CONTROL, (*itr), attStack);
                        }
                        else
                        {
                            if (m_timer < 600000)
                                m_timer = 0;
                            else
                                m_timer = m_timer - 600000; // - 10 mins
                        }
                    }
                    for (PlayerSet::iterator itr = m_players[getDefenderTeam()].begin(); itr != m_players[getDefenderTeam()].end(); ++itr)
                    {
                        if (getDefenderTeam()==TEAM_ALLIANCE)
                            TeamIDsound=OutdoorPvP_WG_HORDE_CAPTAIN; //Allience Worn Sound
                        else
                            TeamIDsound=OutdoorPvP_WG_ALLIANCE_CAPTAIN;  //Horde Worn Sound
                        (*itr)->PlayDirectSound(TeamIDsound) ; // Wintergrasp Fortress under Siege

                     // Add Support of Quests Toppling the Towers & Southern Sabotage
                        if (obj->GetEntry()==190356 || obj->GetEntry()==190357 || obj->GetEntry()==190358)
                        (*itr)->RewardPlayerAndGroupAtEvent(TOWER_PVP_DESTROYED, obj);
                    }
                    for (PlayerSet::iterator itr = m_players[getAttackerTeam()].begin(); itr != m_players[getAttackerTeam()].end(); ++itr)
                    {
                        if (getDefenderTeam()==TEAM_ALLIANCE)
                            TeamIDsound=OutdoorPvP_WG_HORDE_CAPTAIN; //Allience Worn Sound
                        else
                            TeamIDsound=OutdoorPvP_WG_ALLIANCE_CAPTAIN;  //Horde Worn Sound
                        (*itr)->PlayDirectSound(TeamIDsound) ; // Wintergrasp Fortress destroyed Siege
                    }
                    break;
                case BUILDING_KEEP_TOWER:
                    if (msgStr.c_str())
                    SendWarningToAllInZone(WG_TEXT_DESTROYED, msgStr.c_str());
                    break;
            }
            BroadcastStateChange(state);
        }
    }
}

void OutdoorPvPWG::RemoveOfflinePlayerWGAuras()
{
    // if server crashed while in battle there could be players with rank or tenacity
    CharacterDatabase.PExecute("DELETE FROM character_aura WHERE spell IN (%u, %u, %u, %u, %u)",
         SPELL_RECRUIT, SPELL_CORPORAL, SPELL_LIEUTENANT, SPELL_TENACITY, SPELL_TOWER_CONTROL);
}

void OutdoorPvPWG::ModifyWorkshopCount(TeamId team, bool add)
{
    if (team == TEAM_NEUTRAL)
        return;

    if (add)
        ++m_workshopCount[team];
    else if (m_workshopCount[team])
        --m_workshopCount[team];
    else
        sLog->outError("OutdoorPvPWG::ModifyWorkshopCount: negative workshop count!");

    SendUpdateWorldState(MaxVehNumWorldState[team], m_workshopCount[team] * MAX_VEHICLE_PER_WORKSHOP);
}

uint32 OutdoorPvPWG::GetCreatureEntry(uint32 /*guidlow*/, const CreatureData *data)
{
    if (getDefenderTeam() == TEAM_ALLIANCE)
    {
        TeamPairMap::const_iterator itr = m_creEntryPair.find(data->id);
        if (itr != m_creEntryPair.end())
        {
            const_cast<CreatureData*>(data)->displayid = 0;
            return itr->second;
        }
    }
    return data->id;
}

OutdoorPvPWGCreType OutdoorPvPWG::GetCreatureType(uint32 entry) const
{
    // VEHICLES, GUARDS and TURRETS gives kill credit
    // OTHER Not in wartime
    // TURRET Only during wartime
    // SPECIAL like "OTHER" but no despawn conditions
    // Entries like Case A: Case: B have their own despawn function
    switch(entry)
    {
        case 27881: // Catapult
        case 28094: // Demolisher
        case 28312: // Alliance Siege Engine
        case 32627: // Horde Siege Engine
        case 28319: // Siege turret
        case 32629: // Siege turret
            return CREATURE_SIEGE_VEHICLE;
        case 28366: // Wintergrasp Tower cannon
            return CREATURE_TURRET;
        case CRE_ENG_A: // Alliance Engineer
        case CRE_ENG_H: // Horde Engineer
            return CREATURE_ENGINEER;
        case 30739:case 30740: // Champions
        case 32307:case 32308: // Guards
            return CREATURE_GUARD;
        case CRE_SPI_A: // Dwarven Spirit Guide
        case CRE_SPI_H: // Taunka Spirit Guide
            return CREATURE_SPIRIT_GUIDE;
        case 6491: // Spirit Healers
            return CREATURE_SPIRIT_HEALER;
        case 31101:case 31051: // Hoodoo Master & Sorceress
        case 31102:case 31052: // Vieron Blazefeather & Bowyer
        case 31107:case 31109: // Lieutenant & Senior Demolitionist
        case 31151:case 31153: // Tactical Officer
        case 31106:case 31108: // Siegesmith & Siege Master
        case 31053:case 31054: // Primalist & Anchorite
        case 31091:case 31036: // Commander
            return CREATURE_QUESTGIVER;
        case 32615:case 32626: // Warbringer && Brigadier General
        case 32296:case 32294: // Quartermaster
        case 39173:case 39172: // Ros'slai && Marshal Magruder
            return CREATURE_VENDOR;
        case 30870:case 30869: // Flight Masters
            return CREATURE_SPECIAL;
        default:
            return CREATURE_OTHER; // Revenants, Elementals, etc
    }
}

void OutdoorPvPWG::OnCreatureCreate(Creature *creature)
{
    uint32 entry = creature->GetEntry();
    switch(GetCreatureType(entry))
    {
        case CREATURE_SIEGE_VEHICLE:
        {
            if (!creature->isSummon())
                return;

            TeamId team;
            if (creature->getFaction() == WintergraspFaction[TEAM_ALLIANCE])
                team = TEAM_ALLIANCE;
            else if (creature->getFaction() == WintergraspFaction[TEAM_HORDE])
                team = TEAM_HORDE;
            else
                return;

            if (uint32 engLowguid = GUID_LOPART(((TempSummon*)creature)->GetSummonerGUID()))
            {
                if (OPvPCapturePointWG *workshop = GetWorkshopByEngGuid(engLowguid))
                {
                    if (CanBuildVehicle(workshop))
                        m_vehicles[team].insert(creature);
                    else
                    {
                        creature->setDeathState(DEAD);
                        creature->SetRespawnTime(DAY);
                        return;
                    }
                }

                if (m_tenacityStack > 0 && team == TEAM_ALLIANCE)
                    creature->SetAuraStack(SPELL_TENACITY_VEHICLE, creature, m_tenacityStack);
                else if (m_tenacityStack < 0 && team == TEAM_HORDE)
                    creature->SetAuraStack(SPELL_TENACITY_VEHICLE, creature, -m_tenacityStack);
            }
            SendUpdateWorldState(VehNumWorldState[team], m_vehicles[team].size());
            break;
        }
        case CREATURE_QUESTGIVER:
            m_questgivers[creature->GetDBTableGUIDLow()] = creature;
            creature->SetReactState(REACT_PASSIVE);
            break;
        case CREATURE_ENGINEER:
            for (OutdoorPvP::OPvPCapturePointMap::iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
                if (OPvPCapturePointWG *workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
                    if (workshop->m_engGuid == creature->GetDBTableGUIDLow())
                    {
                        workshop->m_engineer = creature;
                        break;
                    }
            break;
        case CREATURE_SPIRIT_GUIDE:
            for (OutdoorPvP::OPvPCapturePointMap::iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
                if (OPvPCapturePointWG *workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
                    if (workshop->m_spiGuid == creature->GetDBTableGUIDLow())
                    {
                        workshop->m_spiritguide = creature;
                        if (OutdoorPvPWG *pvpWG = (OutdoorPvPWG*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(4197))
                            if (creature->IsVisible())
                            {
                                if (creature->GetEntry() == CRE_SPI_A)
                                {
                                    TeamPairMap::const_iterator itr = m_creEntryPair.find(creature->GetCreatureData()->id);
                                    if (itr != m_creEntryPair.end())
                                        pvpWG->RelocateAllianceDeadPlayers(creature); // Alliance
                                }
                                else
                                {
                                    TeamPairMap::const_iterator itr = m_creEntryPair.find(creature->GetCreatureData()->id);
                                    if (itr != m_creEntryPair.end())
                                        pvpWG->RelocateHordeDeadPlayers(creature); // Horde
                                }
                            }
                        break;
                    }
            creature->CastSpell(creature, SPELL_SPIRITUAL_IMMUNITY, true);
        case CREATURE_SPIRIT_HEALER:
        case CREATURE_TURRET:
        case CREATURE_OTHER:
            UpdateCreatureInfo(creature);
        default:
            m_creatures.insert(creature);
            break;
    }
}

void OutdoorPvPWG::OnCreatureRemove(Creature *creature)
{
    uint32 entry = creature->GetEntry();
    switch(GetCreatureType(entry))
    {
        case CREATURE_SIEGE_VEHICLE:
        {
            if (!creature->isSummon())
                return;

            TeamId team;
            // the faction may be changed in uncharm
            // TODO: now you have to wait until the corpse of vehicle disappear to build a new one
            if (m_vehicles[TEAM_ALLIANCE].erase(creature))
                team = TEAM_ALLIANCE;
            else if (m_vehicles[TEAM_HORDE].erase(creature))
                team = TEAM_HORDE;
            else
                return;

            SendUpdateWorldState(VehNumWorldState[team], m_vehicles[team].size());
            break;
        }
        case CREATURE_QUESTGIVER:
            m_questgivers.erase(creature->GetDBTableGUIDLow());
            break;
        case CREATURE_ENGINEER:
            for (OutdoorPvP::OPvPCapturePointMap::iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
                if (OPvPCapturePointWG *workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
                    if (workshop->m_engGuid == creature->GetDBTableGUIDLow())
                    {
                        workshop->m_engineer = NULL;
                        break;
                    }
            m_creatures.erase(creature); // prevents crash, but i think it's wrong
            break;
        case CREATURE_SPIRIT_GUIDE:
            for (OutdoorPvP::OPvPCapturePointMap::iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
                if (OPvPCapturePointWG *workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
                    if (workshop->m_spiGuid == creature->GetDBTableGUIDLow())
                    {
                        workshop->m_spiritguide = NULL;
                        break;
                    }
            m_creatures.erase(creature); // prevents crash, but i think it's wrong
            break;
        default:
            m_creatures.erase(creature);
            break;
    }
}

void OutdoorPvPWG::OnGameObjectCreate(GameObject *go)
{
    OutdoorPvP::OnGameObjectCreate(go);

    if (go || go->GetGOInfo())
        if (go->GetGOInfo()->displayId == 5651 || go->GetGOInfo()->displayId == 5652 || go->GetGOInfo()->displayId == 8256 || go->GetGOInfo()->displayId == 8257)
            m_flags.insert(go);
    
    if (UpdateGameObjectInfo(go))
        m_gobjects.insert(go);

    else if (go->GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
    {
        BuildingStateMap::const_iterator itr = m_buildingStates.find(go->GetDBTableGUIDLow());
        if (itr != m_buildingStates.end())
        {
            itr->second->building = go;
            if (go->GetGOInfo()->displayId == 7878 || go->GetGOInfo()->displayId == 7900)
                itr->second->type = BUILDING_TOWER;
            if (go->GetEntry() == 190221 || go->GetEntry() == 190373 || go->GetEntry() == 190377 || go->GetEntry() == 190378)
                itr->second->type = BUILDING_KEEP_TOWER;
            if (itr->second->damageState == DAMAGE_INTACT && !itr->second->health)
            {
                itr->second->health = go->GetGOValue()->Building.Health;
                go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);
            } else {
                go->GetGOValue()->Building.Health = itr->second->health;
                if (itr->second->damageState == DAMAGE_DAMAGED)
                    go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED);
                else if (itr->second->damageState == DAMAGE_DESTROYED)
                    go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);
            }
        }
    }
}

void OutdoorPvPWG::OnGameObjectRemove(GameObject *go)
{
    OutdoorPvP::OnGameObjectRemove(go);

    if (go || go->GetGOInfo())
        if (go->GetGOInfo()->displayId == 5651 || go->GetGOInfo()->displayId == 5652 || go->GetGOInfo()->displayId == 8256 || go->GetGOInfo()->displayId == 8257)
            m_flags.erase(go);
    
    if (UpdateGameObjectInfo(go))
        m_gobjects.erase(go);

    else if (go->GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
    {
        BuildingStateMap::const_iterator itr = m_buildingStates.find(go->GetDBTableGUIDLow());
        if (itr != m_buildingStates.end())
        {
            itr->second->building = NULL;
        }
    }
}

void OutdoorPvPWG::UpdateAllWorldObject()
{
    // update cre and go factions
    for (GameObjectSet::iterator itr = m_gobjects.begin(); itr != m_gobjects.end(); ++itr)
        UpdateGameObjectInfo(*itr);
    for (CreatureSet::iterator itr = m_creatures.begin(); itr != m_creatures.end(); ++itr)
        UpdateCreatureInfo(*itr);
    for (QuestGiverMap::iterator itr = m_questgivers.begin(); itr != m_questgivers.end(); ++itr)
        UpdateQuestGiverPosition((*itr).first, (*itr).second);

    // rebuild and update building states
    RebuildAllBuildings();

    // update capture points
    for (OPvPCapturePointMap::iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
        if (OPvPCapturePointWG *workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
            workshop->SetTeamByBuildingState();
}

void OutdoorPvPWG::RebuildAllBuildings()
{
    for (BuildingStateMap::const_iterator itr = m_buildingStates.begin(); itr != m_buildingStates.end(); ++itr)
    {
        if (itr->second->building && itr->second->building->GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
        {
            UpdateGameObjectInfo(itr->second->building);
            itr->second->building->Rebuild();
            itr->second->health = itr->second->building->GetGOValue()->Building.Health;
            itr->second->damageState = DAMAGE_INTACT;
        }
        else
            itr->second->health = 0;
        itr->second->SetTeam(getDefenderTeam() == TEAM_ALLIANCE ? OTHER_TEAM(itr->second->defaultTeam) : itr->second->defaultTeam);
    }
    m_towerDamagedCount[TEAM_ALLIANCE] = 0;
    m_towerDestroyedCount[TEAM_ALLIANCE] = 0;
    m_towerDamagedCount[TEAM_HORDE] = 0;
    m_towerDestroyedCount[TEAM_HORDE] = 0;
}

void OutdoorPvPWG::SendInitWorldStatesTo(Player *player) const
{
    WorldPacket data(SMSG_INIT_WORLD_STATES, (4+4+4+2+(m_buildingStates.size()*8)));
    data << uint32(571);
    data << uint32(ZONE_WINTERGRASP);
    data << uint32(0);
    data << uint16(4+2+4+m_buildingStates.size());

    data << uint32(3803) << uint32(getDefenderTeam() == TEAM_ALLIANCE ? 1 : 0);
    data << uint32(3802) << uint32(getDefenderTeam() != TEAM_ALLIANCE ? 1 : 0);
    data << uint32(3801) << uint32(isWarTime() ? 0 : 1);
    data << uint32(3710) << uint32(isWarTime() ? 1 : 0);

    for (uint32 i = 0; i < 2; ++i)
        data << ClockWorldState[i] << m_clock[i];

    data << uint32(3490) << uint32(m_vehicles[TEAM_HORDE].size());
    data << uint32(3491) << m_workshopCount[TEAM_HORDE] * MAX_VEHICLE_PER_WORKSHOP;
    data << uint32(3680) << uint32(m_vehicles[TEAM_ALLIANCE].size());
    data << uint32(3681) << m_workshopCount[TEAM_ALLIANCE] * MAX_VEHICLE_PER_WORKSHOP;

    for (BuildingStateMap::const_iterator itr = m_buildingStates.begin(); itr != m_buildingStates.end(); ++itr)
        itr->second->FillData(data);

    if (player)
        player->GetSession()->SendPacket(&data);
    else
        BroadcastPacket(data);
}

void OutdoorPvPWG::BroadcastStateChange(BuildingState *state) const
{
    if (m_sendUpdate)
        for (uint32 team = 0; team < 2; ++team)
            for (PlayerSet::const_iterator p_itr = m_players[team].begin(); p_itr != m_players[team].end(); ++p_itr)
                state->SendUpdate(*p_itr);
}

// Called at Start and Battle End
bool OutdoorPvPWG::UpdateCreatureInfo(Creature *creature)
{
    if (!creature)
        return false;
    uint32 entry = creature->GetEntry();

    switch(GetCreatureType(entry))
    {
        case CREATURE_TURRET:
            if (isWarTime())
            {
                if (!creature->isAlive())
                    creature->Respawn(true);
                creature->setFaction(WintergraspFaction[getDefenderTeam()]);
                creature->SetVisible(true);
            } else {
                if (creature->IsVehicle() && creature->GetVehicleKit())
                    creature->GetVehicleKit()->RemoveAllPassengers();
                creature->SetVisible(false);
                creature->setFaction(35);
            }
            return false;
        case CREATURE_OTHER:
            if (isWarTime())
            {
                creature->SetVisible(false);
                creature->setFaction(35);
            }
            else
            {
                creature->RestoreFaction();
                creature->SetVisible(true);
            }
            return false;
        case CREATURE_SPIRIT_GUIDE:
            /*Vehicle teleport system*/
            pMap = creature->GetMap();
            if (isWarTime())
            {
                if (creature->GetAreaId() == 4575) // Select Fortress Spirit
                {
                    if (getDefenderTeam() == TEAM_ALLIANCE) // Fortress Spirit Alliance
                        creature->UpdateEntry(CRE_SPI_A);
                    if (getDefenderTeam() == TEAM_HORDE) // Fortress Spirit Horde
                        creature->UpdateEntry(CRE_SPI_H);
                }
                creature->SetVisible(true);
            }
            else
            {
                creature->SetVisible(false);
                //creature->setDeathState(DEAD);
            }
            return false;
        case CREATURE_SPIRIT_HEALER:
            creature->SetVisible(isWarTime() ? false : true);
            return false;
        case CREATURE_ENGINEER:
           return false;
        case CREATURE_SIEGE_VEHICLE:
            if (!isWarTime())
            {
                if (creature->IsVehicle() && creature->GetVehicleKit())
                    creature->GetVehicleKit()->RemoveAllPassengers();
                creature->DisappearAndDie();
            }
            return false;
        case CREATURE_QUESTGIVER:
           creature->AI()->EnterEvadeMode();
           //creature->SetVisible(isWarTime() ? false : true);
           return false;
        case CREATURE_GUARD:
            creature->AI()->EnterEvadeMode();
            /*if ((creature->GetEntry()==30740 || creature->GetEntry()==30739))
            {
                if (creature->GetAreaId()!=4575 && creature->GetAreaId()!=4611 && creature->GetAreaId()!=4583 && creature->GetAreaId()!=4539 && 
                    creature->GetAreaId()!=4538 && creature->GetAreaId()!=4582 && creature->GetAreaId()!=4612 && creature->GetAreaId()!=4581)
                {
                    switch (entry)
                    {
                        case 30740://Alliance champion
                        case 32308://Alliance guard
                        {
                            if (getDefenderTeam() == TEAM_ALLIANCE)
                                creature->SetPhaseMask(1, true);
                            else 
                                creature->SetPhaseMask(2, true);
                            break;
                        }
                        case 30739://Horde champion
                        case 32307://Horde guard
                        {
                            if (getDefenderTeam() == TEAM_ALLIANCE)
                                creature->SetPhaseMask(2, true);
                            else 
                                creature->SetPhaseMask(1, true);
                            break;
                        }
                    }
                }
                _RespawnCreatureIfNeeded(creature, entry);
                creature->AI()->EnterEvadeMode();
            }*/
        case CREATURE_VENDOR:
            /*if (creature->GetCreatureType() == CREATURE_VENDOR)
                creature->SetVisible(isWarTime() ? false : true);*/
        case CREATURE_SPECIAL:
        {
            TeamPairMap::const_iterator itr = m_creEntryPair.find(creature->GetCreatureData()->id);
            if (itr != m_creEntryPair.end())
            {
                entry = getDefenderTeam() == TEAM_ALLIANCE ? itr->second : itr->first;
                _RespawnCreatureIfNeeded(creature, entry);
            }
            return false;
        }
        default:
            return false;
    }
}

bool OutdoorPvPWG::UpdateQuestGiverPosition(uint32 guid, Creature *creature)
{
    assert(guid);
    Position pos = m_qgPosMap[std::pair<uint32, bool>(guid, getDefenderTeam() == TEAM_HORDE)];

    if (creature && creature->IsInWorld())
    {
        // if not questgiver or position is the same, do nothing
        if (creature->GetPositionX() == pos.GetPositionX() &&
            creature->GetPositionY() == pos.GetPositionY() &&
            creature->GetPositionZ() == pos.GetPositionZ())
            return false;

        if (creature->isAlive() && creature->isInCombat())
        {
            creature->CombatStop(true);
            creature->getHostileRefManager().deleteReferences();
        }
        creature->SetHomePosition(pos);
        if (creature->GetEntry() != 30400 || creature->GetEntry() != 30499)
            creature->SetReactState(REACT_AGGRESSIVE);
        creature->DestroyForNearbyPlayers();
        if (!creature->GetMap()->IsLoaded(pos.GetPositionX(), pos.GetPositionY()))
            creature->GetMap()->LoadGrid(pos.GetPositionX(), pos.GetPositionY());
        creature->GetMap()->CreatureRelocation(creature, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
        if (!creature->isAlive())
            creature->Respawn(true);
    }
    else
        sObjectMgr->MoveCreData(guid, 571, pos);

    return true;
}

void OutdoorPvPWG::ChangeFlags(Creature* creature)
{
    uint32 _displayid;
    if (!creature)
        return;

        bool cArea = creature->GetAreaId();
        for (GameObjectSet::iterator itr = m_flags.begin(); itr != m_flags.end(); ++itr)
        {
            if (!m_flags.empty())
                if (cArea)
                    if (creature->GetAreaId() == (*itr)->GetAreaId())
                        if (!creature->IsVisible())
                            (*itr)->SetPhaseMask(2, true);
                        else
                        {
                            if ((*itr)->GetGOInfo())
                                switch((*itr)->GetGOInfo()->displayId)
                                {
                                    case 5651: // Small Alliance Flag
                                    case 5652: // Small Horde Flag
                                    case 8256: // Big Alliance Flag
                                    case 8257: // Big Horde Flag
                                    {
                                        if (creature->GetEntry() == CRE_ENG_H)
                                        {
                                            TeamPairMap::const_iterator itr2 = m_goDisplayPair.find((*itr)->GetGOInfo()->displayId);
                                            if (!m_goDisplayPair.empty())
                                                if (getDefenderTeam() == TEAM_ALLIANCE)
                                                {
                                                    if ((*itr)->GetAreaId() == 4538 || (*itr)->GetAreaId() == 4539)
                                                        _displayid = itr2->first;
                                                    else
                                                        _displayid = itr2->second;
                                                }
                                                else
                                                {
                                                    if ((*itr)->GetAreaId() == 4538 || (*itr)->GetAreaId() == 4539)
                                                        _displayid = itr2->first;
                                                    else
                                                        _displayid = itr2->second;
                                                }
                                        }
                                        else
                                        {
                                            TeamPairMap::const_iterator itr2 = m_goDisplayPair.find((*itr)->GetGOInfo()->displayId);
                                            if (!m_goDisplayPair.empty())
                                                if (getDefenderTeam() == TEAM_HORDE)
                                                {
                                                    if ((*itr)->GetAreaId() == 4538 || (*itr)->GetAreaId() == 4539)
                                                        _displayid = itr2->second;
                                                    else
                                                        _displayid = itr2->first;
                                                }
                                                else
                                                {
                                                    if ((*itr)->GetAreaId() == 4538 || (*itr)->GetAreaId() == 4539)
                                                        _displayid = itr2->second;
                                                    else
                                                        _displayid = itr2->first;
                                                }
                                        }
                                        
                                        if ((*itr)->GetPhaseMask() == 2/* && (*itr)->GetUInt32Value(GAMEOBJECT_DISPLAYID) != _displayid*/)
                                        {
                                            (*itr)->SetUInt32Value(GAMEOBJECT_DISPLAYID, _displayid);
                                            (*itr)->SetPhaseMask(1, true);
                                        }
                                    }
                                }
                        }
        }
}

// Return false = Need to rebuild at battle End/Start
//        true  = no need to rebuild (ie: Banners or teleporters)
bool OutdoorPvPWG::UpdateGameObjectInfo(GameObject *go) const
{
    uint32 attFaction;
    uint32 defFaction;

    if (isWarTime())
    {
        attFaction = WintergraspFaction[getAttackerTeam()];
        defFaction = WintergraspFaction[getDefenderTeam()];
    }
    else
    {
        attFaction = 35;
        defFaction = 35;
    }

    if (!go || !go->GetGOInfo())
       return true;

    switch(go->GetGOInfo()->displayId)
    {
        case 8244: // Defender's Portal - Vehicle Teleporter
            go->SetUInt32Value(GAMEOBJECT_FACTION, WintergraspFaction[getDefenderTeam()]);
            return true;
        case 7967: // Titan relic
            go->SetUInt32Value(GAMEOBJECT_FACTION, attFaction);
            return true;
        case 5651: // Small Alliance Flag
        case 5652: // Small Horde Flag
        case 8256: // Big Alliance Flag
        case 8257: // Big Horde Flag
        {
           uint32 _displayid;
           bool cArea = (go->GetAreaId() == 4575 || go->GetAreaId() == 4539 || go->GetAreaId() == 4538); // Wintergrasp Fortress - The Broken Temple - The Sunken Ring

           if ((cArea || !cArea) && getDefenderTeam() == TEAM_ALLIANCE)
           {
                TeamPairMap::const_iterator itr = m_goDisplayPair.find(go->GetGOInfo()->displayId);
                if (itr == m_goDisplayPair.end())
                    return true;
                if (go->GetAreaId() != 4575 && go->GetAreaId() != 4539 && go->GetAreaId() != 4538)
                    _displayid = itr->second;
                if (go->GetAreaId() == 4575 || go->GetAreaId() == 4539 || go->GetAreaId() == 4538)
                    _displayid = itr->second;
           }

           if ((cArea || !cArea) && getDefenderTeam() == TEAM_HORDE)
           {
                TeamPairMap::const_iterator itr = m_goDisplayPair.find(go->GetGOInfo()->displayId);
                if (itr == m_goDisplayPair.end())
                    return true;
                if (go->GetAreaId() != 4575 && go->GetAreaId() != 4539 && go->GetAreaId() != 4538)
                    _displayid = itr->first;
                if (go->GetAreaId() == 4575 || go->GetAreaId() == 4539 || go->GetAreaId() == 4538)
                    _displayid = itr->first;
           }
           
           if (go->GetPhaseMask() == 1 && go->GetUInt32Value(GAMEOBJECT_DISPLAYID) != _displayid)
           {
               go->SetUInt32Value(GAMEOBJECT_DISPLAYID, _displayid);
               go->SummonGameObject(go->GetEntry(), go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), go->GetOrientation(), 0, 0, 0, 0, 0);
               go->RemoveFromWorld();
           }
           return true;
        }
        case 8165: // Wintergrasp Keep Door
        case 7877: // Wintergrasp Fortress Wall
        case 7878: // Wintergrasp Keep Tower
        case 7906: // Wintergrasp Fortress Gate
        case 7909: // Wintergrasp Wall
            go->SetUInt32Value(GAMEOBJECT_FACTION, defFaction);
            return false;
        case 7900: // Flamewatch Tower - Shadowsight Tower - Winter's Edge Tower
            go->SetUInt32Value(GAMEOBJECT_FACTION, attFaction);
            return false;
        case 8208: // Goblin Workshop
            if (OPvPCapturePointWG *workshop = GetWorkshopByGOGuid(go->GetGUID()))
                go->SetUInt32Value(GAMEOBJECT_FACTION, WintergraspFaction[workshop->m_buildingState->GetTeam()]);
            return false;

    }

    // Note: this is only for test, still need db support
    TeamPairMap::const_iterator itr = m_goDisplayPair.find(go->GetGOInfo()->displayId);
    if (itr != m_goDisplayPair.end())
    {
        go->SetUInt32Value(GAMEOBJECT_DISPLAYID, getDefenderTeam() == TEAM_ALLIANCE ?
            itr->second : itr->first);
        return true;
    }
    return false;
}

void OutdoorPvPWG::HandlePlayerEnterZone(Player * plr, uint32 zone)
{
    if (!sWorld->getBoolConfig(CONFIG_OUTDOORPVP_WINTERGRASP_ENABLED) || !plr)
        return;

    if (isWarTime())
    {
        if (plr->getLevel() < 75)
        {
            plr->CastSpell(plr, SPELL_TELEPORT_DALARAN, true);
            return;
        }
        if (plr->getLevel() > 74)
        {
            plr->KickedFromWG = false;
            if (plr->WGKickTimer > 20000 || plr->WGKickTimer <= 0)
                plr->WGKickTimer = 20000;
            if (plr->WGBattleBoxTimer > 20000 || plr->WGBattleBoxTimer <= 0)
                plr->WGBattleBoxTimer = 20000;
            if (plr->WGQueueBoxTimer > 20000 || plr->WGQueueBoxTimer <= 0)
                plr->WGQueueBoxTimer = 20000;

            if (!plr->HasAura(SPELL_RECRUIT) && !plr->HasAura(SPELL_CORPORAL) && !plr->HasAura(SPELL_LIEUTENANT))
                plr->CastSpell(plr, SPELL_RECRUIT, true);

            if (plr->GetTeamId() == getAttackerTeam())
            {
                if (m_towerDestroyedCount[getAttackerTeam()] < 3)
                    plr->SetAuraStack(SPELL_TOWER_CONTROL, plr, 3 - m_towerDestroyedCount[getAttackerTeam()]);
            } else if (m_towerDestroyedCount[getAttackerTeam()])
                    plr->SetAuraStack(SPELL_TOWER_CONTROL, plr, m_towerDestroyedCount[getAttackerTeam()]);
        }
    }

    SendInitWorldStatesTo(plr);
    OutdoorPvP::HandlePlayerEnterZone(plr, zone);
    UpdateTenacityStack();
}

void OutdoorPvPWG::HandlePlayerResurrects(Player* pPlayer, uint32 zone)
{
    if (!sWorld->getBoolConfig(CONFIG_OUTDOORPVP_WINTERGRASP_ENABLED))
        return;

    if (isWarTime() && pPlayer->getLevel() > 74)
    {
        // Tenacity
        if (pPlayer->GetTeamId() == TEAM_ALLIANCE && m_tenacityStack > 0 ||
            pPlayer->GetTeamId() == TEAM_HORDE && m_tenacityStack < 0)
        {
            if (pPlayer->HasAura(SPELL_TENACITY))
                pPlayer->RemoveAurasDueToSpell(SPELL_TENACITY);

            int32 newStack = m_tenacityStack < 0 ? -m_tenacityStack : m_tenacityStack;
            if (newStack > 20)
                newStack = 20;
            pPlayer->SetAuraStack(SPELL_TENACITY, pPlayer, newStack);
        }

        if (pPlayer->GetTeamId() == getAttackerTeam())
        {
            if (m_towerDestroyedCount[getAttackerTeam()] < 3)
                pPlayer->SetAuraStack(SPELL_TOWER_CONTROL, pPlayer, 3 - m_towerDestroyedCount[getAttackerTeam()]);
        } else if (m_towerDestroyedCount[getAttackerTeam()])
                pPlayer->SetAuraStack(SPELL_TOWER_CONTROL, pPlayer, m_towerDestroyedCount[getAttackerTeam()]);
    }
    OutdoorPvP::HandlePlayerResurrects(pPlayer, zone);
}

void OutdoorPvPWG::HandlePlayerLeaveZone(Player* pPlayer, uint32 zone)
{
    if (!sWorld->getBoolConfig(CONFIG_OUTDOORPVP_WINTERGRASP_ENABLED))
        return;

    if (!pPlayer->GetSession()->PlayerLogout())
    {
        if (pPlayer->GetVehicle())
            pPlayer->GetVehicle()->Dismiss();
        pPlayer->RemoveAurasDueToSpell(SPELL_RECRUIT);
        pPlayer->RemoveAurasDueToSpell(SPELL_CORPORAL);
        pPlayer->RemoveAurasDueToSpell(SPELL_LIEUTENANT);
        pPlayer->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);
        pPlayer->RemoveAurasDueToSpell(SPELL_SPIRITUAL_IMMUNITY);
    }
    pPlayer->RemoveAurasDueToSpell(SPELL_TENACITY);
    OutdoorPvP::HandlePlayerLeaveZone(pPlayer, zone);
    UpdateTenacityStack();
}

void OutdoorPvPWG::PromotePlayer(Player *killer) const
{
    Aura * aur;
    if (aur = killer->GetAura(SPELL_RECRUIT))
    {
        if (aur->GetStackAmount() >= 6)
        {
            killer->RemoveAura(SPELL_RECRUIT);
            killer->CastSpell(killer, SPELL_CORPORAL, true);
            killer->CastSpell(killer, SPELL_RANK_UP, true);
        } else killer->CastSpell(killer, SPELL_RECRUIT, true);
    }
    else if (aur = killer->GetAura(SPELL_CORPORAL))
    {
        if (aur->GetStackAmount() >= 10)
        {
            killer->RemoveAura(SPELL_CORPORAL);
            killer->CastSpell(killer, SPELL_LIEUTENANT, true);
            killer->CastSpell(killer, SPELL_RANK_UP, true);
        } else killer->CastSpell(killer, SPELL_CORPORAL, true);
    }
}

void OutdoorPvPWG::SendRankWarning(Player* killer)
{
    Aura * aur;
    if (aur = killer->GetAura(SPELL_RECRUIT))
    {
        if (aur->GetStackAmount() >= 6)
            SendWarningToPlayer(killer, WG_TEXT_FIRSTRANK);
    }
    else if (aur = killer->GetAura(SPELL_CORPORAL))
        if (aur->GetStackAmount() >= 10)
            SendWarningToPlayer(killer, WG_TEXT_SECONDRANK);
}

void OutdoorPvPWG::HandleKill(Player *killer, Unit *victim)
{
    if (!sWorld->getBoolConfig(CONFIG_OUTDOORPVP_WINTERGRASP_ENABLED) || !isWarTime())
        return;

    bool ok = false;
    if (victim->GetTypeId() == TYPEID_PLAYER)
    {
        if (victim->getLevel() >= 70)
            ok = true;
        killer->RewardPlayerAndGroupAtEvent(CRE_PVP_KILL, victim);
    } else {
        switch (GetCreatureType(victim->GetEntry()))
        {
            case CREATURE_SIEGE_VEHICLE:
                killer->RewardPlayerAndGroupAtEvent(CRE_PVP_KILL_V, victim);
                ok = true;
                break;
            case CREATURE_GUARD:
                killer->RewardPlayerAndGroupAtEvent(CRE_PVP_KILL, victim);
                ok = true;
                break;
            case CREATURE_TURRET:
                ok = true;
                break;
        }
    }

    if (!ok)
        return;

    if (Group *pGroup = killer->GetGroup())
    {
        for (GroupReference *itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
            if (itr->getSource()->IsAtGroupRewardDistance(killer) && itr->getSource()->getLevel() > 74)
            {
                SendRankWarning(itr->getSource());
                PromotePlayer(itr->getSource());
            }
    } else if (killer->getLevel() > 74)
    {
        SendRankWarning(killer);
        PromotePlayer(killer);
    }
}

// Recalculates Tenacity and applies it to Players / Vehicles
void OutdoorPvPWG::UpdateTenacityStack()
{
    if (!isWarTime())
        return;

    TeamId team = TEAM_NEUTRAL;
    uint32 allianceNum = 0;
    uint32 hordeNum = 0;
    int32 newStack = 0;

    for (PlayerSet::iterator itr = m_players[TEAM_ALLIANCE].begin(); itr != m_players[TEAM_ALLIANCE].end(); ++itr)
        if ((*itr)->getLevel() > 74)
            ++allianceNum;

    for (PlayerSet::iterator itr = m_players[TEAM_HORDE].begin(); itr != m_players[TEAM_HORDE].end(); ++itr)
        if ((*itr)->getLevel() > 74)
            ++hordeNum;

    if (allianceNum && hordeNum)
    {
        uint32 difference = 0;
        if (allianceNum < hordeNum)
            uint32 difference = hordeNum-allianceNum;
        else if (allianceNum > hordeNum)
            uint32 difference = allianceNum-hordeNum;

        if (allianceNum != hordeNum)
        {
            double parameter = 0.1071;
            int Stack = int(difference*parameter);
            if (Stack > 20)
                Stack = 20;
            newStack = Stack; // negative, should cast on alliance or horde
        }
    }

    if (newStack == m_tenacityStack)
        return;

    if (m_tenacityStack > 0 && newStack <= 0) // old buff was on alliance
        team = TEAM_ALLIANCE;
    else if (m_tenacityStack < 0 && newStack >= 0) // old buff was on horde
        team = TEAM_HORDE;

    m_tenacityStack = newStack;

    // Remove old buff
    if (team != TEAM_NEUTRAL)
    {
        for (PlayerSet::const_iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
            if ((*itr)->getLevel() > 74)
                (*itr)->RemoveAurasDueToSpell(SPELL_TENACITY);

        for (CreatureSet::const_iterator itr = m_vehicles[team].begin(); itr != m_vehicles[team].end(); ++itr)
            (*itr)->RemoveAurasDueToSpell(SPELL_TENACITY_VEHICLE);
    }

    // Apply new buff
    if (newStack)
    {
        team = newStack > 0 ? TEAM_ALLIANCE : TEAM_HORDE;
        if (newStack < 0)
            newStack = -newStack;
        if (newStack > 20)
            newStack = 20;

        for (PlayerSet::const_iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
            if ((*itr)->getLevel() > 74)
                (*itr)->SetAuraStack(SPELL_TENACITY, (*itr), newStack);

        for (CreatureSet::const_iterator itr = m_vehicles[team].begin(); itr != m_vehicles[team].end(); ++itr)
            (*itr)->SetAuraStack(SPELL_TENACITY_VEHICLE, (*itr), newStack);
    }
}

void OutdoorPvPWG::UpdateClockDigit(uint32 &timer, uint32 digit, uint32 mod)
{
    uint32 value = timer%mod;
    if (m_clock[digit] == value)
        return;

    m_clock[digit] = value;
    SendUpdateWorldState(ClockWorldState[digit], uint32(timer + time(NULL)));
    sWorld->SetWintergrapsTimer(uint32(timer + time(NULL)), digit);
}

void OutdoorPvPWG::UpdateClock()
{
    uint32 timer = m_timer / 1000;
    if (!isWarTime())
        UpdateClockDigit(timer, 1, 10);
    else
        UpdateClockDigit(timer, 0, 10);

    // Announce 15 minutes left
    if ((m_timer>900000) && (m_timer<901000) && (m_wartime==false))
        SendWarningToAllInZone(WG_TEXT_BEGIN2, "");
}

bool OutdoorPvPWG::Update(uint32 diff)
{
    if (!sWorld->getBoolConfig(CONFIG_OUTDOORPVP_WINTERGRASP_ENABLED))
        return false;

    if (m_timer > diff)
    {
        m_timer -= diff;

        if (isWarTime())
        {
            OutdoorPvP::Update(diff); // update capture points
            
            for (uint32 team = 0; team < 2; ++team)
            {
                for (PlayerSet::const_iterator itr = m_playersinqueue[team].begin(); itr != m_playersinqueue[team].end(); ++itr)
                    if(Player* plr = (*itr))
                    {
                        if (plr->WGQueueBox == true && plr->WGQueueBoxTimer < diff)
                        {
                            PlayerSet::const_iterator itr2 = m_players[team].find(plr);
                            if ((*itr2) == plr)
                                KickPlayerFromWG(plr, false);

                            m_playersinqueue[team].erase(plr);
                            plr->WGQueueBoxTimer = 20000;
                            plr->WGQueueBox = false;
                        }
                        else plr->WGQueueBoxTimer -= diff;
                    }
                        
                for (PlayerSet::const_iterator itr = m_playersinbattlebox[team].begin(); itr != m_playersinbattlebox[team].end(); ++itr)
                    if(Player* plr = (*itr))
                    {
                        if (plr->WGBattleBox == true && plr->WGBattleBoxTimer < diff)
                        {
                            PlayerSet::const_iterator itr2 = m_players[team].find(plr);
                            if ((*itr2) == plr)
                                KickPlayerFromWG(plr, false);

                            m_playersinbattlebox[team].erase(plr);
                            plr->WGBattleBoxTimer = 20000;
                            plr->WGBattleBox = false;
                        }
                        else plr->WGBattleBoxTimer -= diff;
                    }

                for (PlayerSet::const_iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
                    if(Player* plr = (*itr))
                    {
                        PlayerSet::const_iterator itr2 = m_playersinqueue[team].find(plr);
                        if ((*itr2) != plr)
                            if (plr->WGKickTimer < diff && plr->KickedFromWG == false)
                            {
                                KickPlayerFromWG(plr, false);
                                plr->KickedFromWG = true;
                            }
                            else plr->WGKickTimer -= diff;
                    }
            }

            /***************************************************************/
            /***      BATTLEGROUND VEHICLE TELEPORTER & KICK SYSTEM      ***/
            /***************************************************************/
            if (pMap)
            {
                Map::PlayerList const &PlayerList = pMap->GetPlayers();
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                {
                    if (getDefenderTeam() == TEAM_ALLIANCE)
                    {
                        if (i->getSource()->ToPlayer()->GetTeam() == ALLIANCE)
                        {
                            if (i->getSource()->GetDistance2d(5314.51f, 2703.69f) <= 5 && i->getSource()->IsOnVehicle(i->getSource()->GetVehicleCreatureBase()) && i->getSource()->isAlive())
                            {
                                Creature* Old = i->getSource()->GetVehicleCreatureBase();
                                if (Old->GetEntry() == 28319) // Alliance Turret
                                {
                                    i->getSource()->ExitVehicle();
                                    i->getSource()->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                    Old->SetVisible(true);
                                    Old->ToTempSummon()->UnSummon();
                                }
                                else
                                {
                                    Unit* Driver = Old->GetVehicleKit()->GetPassenger(0);
                                    Unit* Passenger1 = Old->GetVehicleKit()->GetPassenger(1);
                                    Unit* Passenger2 = Old->GetVehicleKit()->GetPassenger(2);
                                    Unit* Passenger3 = Old->GetVehicleKit()->GetPassenger(3);
                                    if (Driver && Driver->isAlive())
                                    {
                                        Creature* New = i->getSource()->SummonCreature(Old->GetEntry(), 5141.191406f, 2841.045410f, 408.703217f, 3.163321f, TEMPSUMMON_MANUAL_DESPAWN);
                                        m_vehicles[TEAM_ALLIANCE].insert(New);
                                        New->SetPower(POWER_MANA, Old->GetPower(POWER_MANA));
                                        New->SetPower(POWER_RAGE, Old->GetPower(POWER_RAGE));
                                        New->SetPower(POWER_FOCUS, Old->GetPower(POWER_FOCUS));
                                        New->SetPower(POWER_ENERGY, Old->GetPower(POWER_ENERGY));
                                        New->SetHealth(Old->GetHealth());
                                        New->SetRespawnTime((uint32)Old->GetRespawnTime());
                                        Old->GetVehicleKit()->Uninstall();
                                            /*if (Old->GetEntry() == 28312)
                                            {
                                                Creature* Turret = Old->GetGuardianPet();
                                                Turret->SetVisible(true);
                                                Turret->ForcedDespawn();
                                            }*/
                                        Old->SetVisible(true);
                                        Old->ForcedDespawn();
                                        Driver->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                        Driver->EnterVehicle(New->ToUnit(), 0);
                                        if (Passenger1 && Passenger1->isAlive())
                                            Passenger1->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                        //Passenger1->EnterVehicle(New->ToUnit(), 1); Seatid doesn't work :S
                                        if (Passenger2 && Passenger2->isAlive())
                                            Passenger2->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                        //Passenger2->EnterVehicle(New->ToUnit(), 2); Seatid doesn't work :S
                                        if (Passenger3 && Passenger3->isAlive())
                                            Passenger3->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                        //Passenger3->EnterVehicle(New->ToUnit(), 3); Seatid doesn't work :S
                                    }
                                }
                            }
                            if (i->getSource()->GetDistance2d(5316.25f, 2977.04f) <= 5 && i->getSource()->IsOnVehicle(i->getSource()->GetVehicleCreatureBase()))
                            {
                                Creature* Old = i->getSource()->GetVehicleCreatureBase();
                                if (Old->GetEntry() == 28319) // Alliance Turret
                                {
                                    i->getSource()->ExitVehicle();
                                    i->getSource()->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                    Old->SetVisible(true);
                                    Old->ToTempSummon()->UnSummon();
                                }
                                else
                                {
                                    Unit* Driver = Old->GetVehicleKit()->GetPassenger(0);
                                    Unit* Passenger1 = Old->GetVehicleKit()->GetPassenger(1);
                                    Unit* Passenger2 = Old->GetVehicleKit()->GetPassenger(2);
                                    Unit* Passenger3 = Old->GetVehicleKit()->GetPassenger(3);
                                    if (Driver && Driver->isAlive())
                                    {
                                        Creature* New = i->getSource()->SummonCreature(Old->GetEntry(), 5141.191406f, 2841.045410f, 408.703217f, 3.163321f, TEMPSUMMON_MANUAL_DESPAWN);
                                        m_vehicles[TEAM_ALLIANCE].insert(New);
                                        New->SetPower(POWER_MANA, Old->GetPower(POWER_MANA));
                                        New->SetPower(POWER_RAGE, Old->GetPower(POWER_RAGE));
                                        New->SetPower(POWER_FOCUS, Old->GetPower(POWER_FOCUS));
                                        New->SetPower(POWER_ENERGY, Old->GetPower(POWER_ENERGY));
                                        New->SetHealth(Old->GetHealth());
                                        New->SetRespawnTime((uint32)Old->GetRespawnTime());
                                        Old->GetVehicleKit()->Uninstall();
                                            /*if (Old->GetEntry() == 28312)
                                            {
                                                Creature* Turret = Old->GetGuardianPet();
                                                Turret->SetVisible(true);
                                                Turret->ForcedDespawn();
                                            }*/
                                        Old->SetVisible(true);
                                        Old->ForcedDespawn();
                                        Driver->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                        Driver->EnterVehicle(New->ToUnit(), 0);
                                        if (Passenger1 && Passenger1->isAlive())
                                            Passenger1->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                        //Passenger1->EnterVehicle(New->ToUnit(), 1); Seatid doesn't work :S
                                        if (Passenger2 && Passenger2->isAlive())
                                            Passenger2->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                        //Passenger2->EnterVehicle(New->ToUnit(), 2); Seatid doesn't work :S
                                        if (Passenger3 && Passenger3->isAlive())
                                            Passenger3->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                        //Passenger3->EnterVehicle(New->ToUnit(), 3); Seatid doesn't work :S
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if (i->getSource()->ToPlayer()->GetTeam() == HORDE)
                        {
                            if (i->getSource()->GetDistance2d(5314.51f, 2703.69f) <= 5 && i->getSource()->IsOnVehicle(i->getSource()->GetVehicleCreatureBase()) && i->getSource()->isAlive())
                            {
                                Creature* Old = i->getSource()->GetVehicleCreatureBase();
                                if (Old->GetEntry() == 32629) // Horde Turret
                                {
                                    i->getSource()->ExitVehicle();
                                    i->getSource()->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                    Old->SetVisible(true);
                                    Old->ToTempSummon()->UnSummon();
                                }
                                else
                                {
                                    Unit* Driver = Old->GetVehicleKit()->GetPassenger(0);
                                    Unit* Passenger1 = Old->GetVehicleKit()->GetPassenger(1);
                                    Unit* Passenger2 = Old->GetVehicleKit()->GetPassenger(2);
                                    Unit* Passenger3 = Old->GetVehicleKit()->GetPassenger(3);
                                    if (Driver && Driver->isAlive())
                                    {
                                        Creature* New = i->getSource()->SummonCreature(Old->GetEntry(), 5141.191406f, 2841.045410f, 408.703217f, 3.163321f, TEMPSUMMON_MANUAL_DESPAWN);
                                        m_vehicles[TEAM_HORDE].insert(New);
                                        New->SetPower(POWER_MANA, Old->GetPower(POWER_MANA));
                                        New->SetPower(POWER_RAGE, Old->GetPower(POWER_RAGE));
                                        New->SetPower(POWER_FOCUS, Old->GetPower(POWER_FOCUS));
                                        New->SetPower(POWER_ENERGY, Old->GetPower(POWER_ENERGY));
                                        New->SetHealth(Old->GetHealth());
                                        New->SetRespawnTime((uint32)Old->GetRespawnTime());
                                        Old->GetVehicleKit()->Uninstall();
                                            /*if (Old->GetEntry() == 28312)
                                            {
                                                //Vehicle *oldvehicle = Old->GetVehicleKit();
                                                Creature* Turret = Old->GetAllMinionsByEntry
                                                Turret->SetVisible(true);
                                                Turret->ForcedDespawn();
                                            }*/
                                        Old->SetVisible(true);
                                        Old->ForcedDespawn();
                                        Driver->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                        Driver->EnterVehicle(New->ToUnit(), 0);
                                        if (Passenger1 && Passenger1->isAlive())
                                            Passenger1->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                        //Passenger1->EnterVehicle(New->ToUnit(), 1); Seatid doesn't work :S
                                        if (Passenger2 && Passenger2->isAlive())
                                            Passenger2->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                        //Passenger2->EnterVehicle(New->ToUnit(), 2); Seatid doesn't work :S
                                        if (Passenger3 && Passenger3->isAlive())
                                            Passenger3->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                        //Passenger3->EnterVehicle(New->ToUnit(), 3); Seatid doesn't work :S
                                    }
                                }
                            }
                            if (i->getSource()->GetDistance2d(5316.25f, 2977.04f) <= 5 && i->getSource()->IsOnVehicle(i->getSource()->GetVehicleCreatureBase()))
                            {
                                Creature* Old = i->getSource()->GetVehicleCreatureBase();
                                if (Old->GetEntry() == 32629) // Horde Turret
                                {
                                    i->getSource()->ExitVehicle();
                                    i->getSource()->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                    Old->SetVisible(true);
                                    Old->ToTempSummon()->UnSummon();
                                }
                                else
                                {
                                    Unit* Driver = Old->GetVehicleKit()->GetPassenger(0);
                                    Unit* Passenger1 = Old->GetVehicleKit()->GetPassenger(1);
                                    Unit* Passenger2 = Old->GetVehicleKit()->GetPassenger(2);
                                    Unit* Passenger3 = Old->GetVehicleKit()->GetPassenger(3);
                                    if (Driver && Driver->isAlive())
                                    {
                                        Creature* New = i->getSource()->SummonCreature(Old->GetEntry(), 5141.191406f, 2841.045410f, 408.703217f, 3.163321f, TEMPSUMMON_MANUAL_DESPAWN);
                                        m_vehicles[TEAM_HORDE].insert(New);
                                        New->SetPower(POWER_MANA, Old->GetPower(POWER_MANA));
                                        New->SetPower(POWER_RAGE, Old->GetPower(POWER_RAGE));
                                        New->SetPower(POWER_FOCUS, Old->GetPower(POWER_FOCUS));
                                        New->SetPower(POWER_ENERGY, Old->GetPower(POWER_ENERGY));
                                        New->SetHealth(Old->GetHealth());
                                        New->SetRespawnTime((uint32)Old->GetRespawnTime());
                                        Old->GetVehicleKit()->Uninstall();
                                            /*if (Old->GetEntry() == 28312)
                                            {
                                                //Vehicle *oldvehicle = Old->GetVehicleKit();
                                                Creature* Turret = Old->GetAllMinionsByEntry
                                                Turret->SetVisible(true);
                                                Turret->ForcedDespawn();
                                            }*/
                                        Old->SetVisible(true);
                                        Old->ForcedDespawn();
                                        Driver->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                        Driver->EnterVehicle(New->ToUnit(), 0);
                                        if (Passenger1 && Passenger1->isAlive())
                                            Passenger1->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                        //Passenger1->EnterVehicle(New->ToUnit(), 1); Seatid doesn't work :S
                                        if (Passenger2 && Passenger2->isAlive())
                                            Passenger2->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                        //Passenger2->EnterVehicle(New->ToUnit(), 2); Seatid doesn't work :S
                                        if (Passenger3 && Passenger3->isAlive())
                                            Passenger3->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                        //Passenger3->EnterVehicle(New->ToUnit(), 3); Seatid doesn't work :S
                                    }
                                }
                            }
                        }
                    }
                }
            }

            /*********************************************************/
            /***        BATTLEGROUND RESSURECTION SYSTEM           ***/
            /*********************************************************/

            //this should be handled by spell system
            m_LastResurrectTime += diff;
            if (m_LastResurrectTime >= RESURRECTION_INTERVAL)
            {
                if (GetReviveQueueSize())
                {
                    for (std::map<uint64, std::vector<uint64> >::iterator itr = m_ReviveQueue.begin(); itr != m_ReviveQueue.end(); ++itr)
                    {
                        Creature* sh = NULL;
                        for (std::vector<uint64>::const_iterator itr2 = (itr->second).begin(); itr2 != (itr->second).end(); ++itr2)
                        {
                            Player *plr = sObjectMgr->GetPlayer(*itr2);
                            if (!plr)
                                continue;

                            if (!sh)
                                if (plr->IsInWorld())
                                {
                                    sh = plr->GetMap()->GetCreature(itr->first);
                                    // only for visual effect
                                    if (sh)
                                        sh->CastSpell(sh, SPELL_SPIRIT_HEAL, true); // Spirit Heal, effect 117
                                }

                            // Resurrection visual
                            if (sh)
                                if (plr->GetDistance2d(sh) <= 18.0f)
                                {
                                    plr->CastSpell(plr, SPELL_RESURRECTION_VISUAL, true);
                                    m_ResurrectQueue.push_back(*itr2);
                                }
                        }
                        (itr->second).clear();
                    }

                    m_ReviveQueue.clear();
                    m_LastResurrectTime = 0;
                } else m_LastResurrectTime = 0;
            }
            else if (m_LastResurrectTime > 500)    // Resurrect players only half a second later, to see spirit heal effect on NPC
            {
                for (std::vector<uint64>::const_iterator itr = m_ResurrectQueue.begin(); itr != m_ResurrectQueue.end(); ++itr)
                {
                    Player *plr = sObjectMgr->GetPlayer(*itr);
                    if (!plr)
                        continue;
                    plr->ResurrectPlayer(1.0f);
                    plr->CastSpell(plr, 6962, true);
                    plr->CastSpell(plr, SPELL_SPIRIT_HEAL_MANA, true);
                    sObjectAccessor->ConvertCorpseForPlayer(*itr);
                    RemovePlayerFromResurrectQueue(plr->GetGUID());
                }
                //m_ResurrectQueue.clear();
            }
        }
        UpdateClock();
    } else {
        m_sendUpdate = false;
        int32 entry = LANG_BG_WG_DEFENDED;

        bool Defended;

        if (m_changeDefender)
        {
            Captured = false;
            Defended = false;
            Changing = true;
            m_changeDefender = false;
            m_defender = getAttackerTeam();
            entry = LANG_BG_WG_CAPTURED;
           if (m_changeAlly == true || m_changeHorde == true) // If wg is switching (.wg switch)
           {
               RebuildAllBuildings();
               m_changeAlly = false;
               m_changeHorde = false;
           }
        }
        else
        {
            Defended = true;
            Captured = true;
            Changing = false;
        }
        if (isWarTime())
        {
            if (m_timer != 1) // 1 = forceStopBattle
            {
                sWorld->SendZoneText(ZONE_WINTERGRASP, fmtstring(sObjectMgr->GetVoragineStringForDBCLocale(entry), sObjectMgr->GetVoragineStringForDBCLocale(getDefenderTeam() == TEAM_ALLIANCE ? LANG_BG_AB_ALLY : LANG_BG_AB_HORDE)));
                if (Defended)
                    SendWarningToAllInZone(WG_TEXT_DEFENDED_FORTRESS, getDefenderTeam() == TEAM_ALLIANCE ? "The Alliance" : "The Horde");
            }
            EndBattle();
        } else {
            if (m_timer != 1)
                sWorld->SendZoneText(ZONE_WINTERGRASP, sObjectMgr->GetVoragineStringForDBCLocale(LANG_BG_WG_BATTLE_STARTS));
            StartBattle();
        }

        UpdateAllWorldObject();
        UpdateClock();
        SendInitWorldStatesTo();
        m_sendUpdate = true;
    }

    uint32 m_WSSaveTimer2 = 500;

    if (m_WSSaveTimer2 < diff)
        sWorld->setWorldState(WORLDSTATE_WINTERGRASP_WARTIME, m_wartime);
    else m_WSSaveTimer2 -= diff;

    if (m_WSSaveTimer < diff)
    {
        sWorld->setWorldState(WORLDSTATE_WINTERGRASP_WARTIME, m_wartime);
        sWorld->setWorldState(WORLDSTATE_WINTERGRASP_TIMER, m_timer);
        sWorld->setWorldState(WORLDSTATE_WINTERGRASP_DEFENDERS, m_defender);
        m_WSSaveTimer = sWorld->getIntConfig(CONFIG_OUTDOORPVP_WINTERGRASP_SAVESTATE_PERIOD);
    } else m_WSSaveTimer -= diff;

   return false;
}

void OutdoorPvPWG::forceStartBattle()
{// Uptime will do all the work
    m_wartime = false;

    if (m_timer != 1)
    {
        m_timer = 1;
        sWorld->SendZoneText(ZONE_WINTERGRASP, sObjectMgr->GetVoragineStringForDBCLocale(LANG_BG_WG_BATTLE_FORCE_START));
    }
    sWorld->SendWintergraspState();
}

void OutdoorPvPWG::forceStopBattle()
{// Uptime will do all the work.

    if (!isWarTime())
        m_wartime = true;
    if (m_timer != 1)
    {
        m_timer = 1;
        sWorld->SendZoneText(ZONE_WINTERGRASP, sObjectMgr->GetVoragineStringForDBCLocale(LANG_BG_WG_BATTLE_FORCE_STOP));
    }
       sWorld->SendWintergraspState();
}

void OutdoorPvPWG::forceChangeTeam()
{
    m_changeDefender = true;
    m_timer = 1;
    m_changeAlly = true;
    m_changeHorde = true;
    if (Captured == true)
        SendWarningToAllInZone(WG_TEXT_CAPTURED_FORTRESS, getAttackerTeam() == TEAM_ALLIANCE ? "The Alliance" : "The Horde");
    sWorld->SendZoneText(ZONE_WINTERGRASP, fmtstring(sObjectMgr->GetVoragineStringForDBCLocale(LANG_BG_WG_SWITCH_FACTION), sObjectMgr->GetVoragineStringForDBCLocale(getAttackerTeam() == TEAM_ALLIANCE ? LANG_BG_AB_ALLY : LANG_BG_AB_HORDE)));
    if (isWarTime())
        forceStartBattle();
    else
        forceStopBattle();
}

// Can be forced by gm's while in battle so have to reset in case it was wartime
void OutdoorPvPWG::StartBattle()
{
    uint32 CountDef=0;
    uint32 CountAtk=0;
    m_wartime = true;
    MaingateDestroyed = false;
    m_timer = sWorld->getIntConfig(CONFIG_OUTDOORPVP_WINTERGRASP_BATTLE_TIME) * MINUTE * IN_MILLISECONDS;

    for (PlayerSet::iterator itr = m_players[getDefenderTeam()].begin(); itr != m_players[getDefenderTeam()].end(); ++itr)
    {
        if ((*itr)->getLevel() < 75)
            (*itr)->CastSpell((*itr), SPELL_TELEPORT_DALARAN, true);
        else
        {
            CountDef++;
            if (!(*itr)->isInFlight())
                    if(isWarTime()==true && (*itr)->IsFlying())
                        if ((*itr)->GetPositionZ() < 619.244019)
                        {
                            (*itr)->RemoveAurasByType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED);
                            (*itr)->RemoveAurasByType(SPELL_AURA_FLY);
                            (*itr)->CastSpell((*itr), 45472, true); //prevent die if fall
                        }
            (*itr)->PlayDirectSound(OutdoorPvP_WG_SOUND_START_BATTLE); // START Battle
        }
    }

    for (PlayerSet::iterator itr = m_players[getAttackerTeam()].begin(); itr != m_players[getAttackerTeam()].end(); ++itr)
    {
        if ((*itr)->getLevel() < 75)
            (*itr)->CastSpell((*itr), SPELL_TELEPORT_DALARAN, true);
        else
        {
            CountAtk++;
            if (!(*itr)->isInFlight())
                    if(isWarTime()==true && (*itr)->IsFlying())
                        if ((*itr)->GetPositionZ() < 619.244019)
                        {
                            (*itr)->RemoveAurasByType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED);
                            (*itr)->RemoveAurasByType(SPELL_AURA_FLY);
                            (*itr)->CastSpell((*itr), 45472, true); //prevent die if fall
                        }
            (*itr)->PlayDirectSound(OutdoorPvP_WG_SOUND_START_BATTLE); // START Battle
        }
    }

    if (sWorld->getBoolConfig(CONFIG_CONFIG_OUTDOORPVP_WINTERGRASP_ANTIFARM_ENABLE))
    {
        if ((CountAtk < sWorld->getIntConfig(CONFIG_CONFIG_OUTDOORPVP_WINTERGRASP_ANTIFARM_ATK)) || (CountDef < sWorld->getIntConfig(CONFIG_CONFIG_OUTDOORPVP_WINTERGRASP_ANTIFARM_DEF)))
        {
            if (CountAtk<=CountDef)
                sWorld->SendWorldText(LANG_BG_WG_WORLD_NO_ATK);
            if (CountDef<CountAtk)
            {
                sWorld->SendWorldText(LANG_BG_WG_WORLD_NO_DEF);
                m_changeDefender=true;
            }
            forceStopBattle();
            return;
        }
    }

    SendWarningToAllInZone(WG_TEXT_BEGIN, "");

    //Uncomment to tele Defenders inside Fortress
    //TeamCastSpell(getDefenderTeam(), SPELL_TELEPORT_FORTRESS);

    //Remove Essence of Wintergrasp to all players
    sWorld->setWorldState(WORLDSTATE_WINTERGRASP_CONTROLING_FACTION, TEAM_NEUTRAL);
    sWorld->UpdateAreaDependentAuras();

    // Remove All Wintergrasp auras. Add Recruit rank and Tower Control
    for (PlayerSet::iterator itr = m_players[getAttackerTeam()].begin(); itr != m_players[getAttackerTeam()].end(); ++itr)
    {
        (*itr)->RemoveAurasDueToSpell(SPELL_RECRUIT);
        (*itr)->RemoveAurasDueToSpell(SPELL_CORPORAL);
        (*itr)->RemoveAurasDueToSpell(SPELL_LIEUTENANT);
        (*itr)->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);
        (*itr)->RemoveAurasDueToSpell(SPELL_SPIRITUAL_IMMUNITY);
        if ((*itr)->getLevel() > 74)
        {
            (*itr)->SetAuraStack(SPELL_TOWER_CONTROL, (*itr), 3);
            (*itr)->CastSpell(*itr, SPELL_RECRUIT, true);
        }
    }

    // Remove All Wintergrasp auras. Add Recruit rank
    for (PlayerSet::iterator itr = m_players[getDefenderTeam()].begin(); itr != m_players[getDefenderTeam()].end(); ++itr)
    {
        (*itr)->RemoveAurasDueToSpell(SPELL_RECRUIT);
        (*itr)->RemoveAurasDueToSpell(SPELL_CORPORAL);
        (*itr)->RemoveAurasDueToSpell(SPELL_LIEUTENANT);
        (*itr)->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);
        (*itr)->RemoveAurasDueToSpell(SPELL_SPIRITUAL_IMMUNITY);
        if ((*itr)->getLevel() > 74)
            (*itr)->CastSpell(*itr, SPELL_RECRUIT, true);
    }
    UpdateTenacityStack();
    // Update timer in players battlegrounds tab
    sWorld->SendWintergraspState();
}

void OutdoorPvPWG::InvitePlayerInZoneToQueue()
{
}

void OutdoorPvPWG::InvitePlayerInQueueToWar()
{
}

void OutdoorPvPWG::InvitePlayerInZoneToWar()
{
}

void OutdoorPvPWG::KickPlayerFromWG(Player* player, bool leave)
{
    OutdoorPvPWG *pvpWG = (OutdoorPvPWG*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(4197);
    if(!pvpWG || !player)
        return;

    //player->TeleportOutOfMap(player->GetMap()); There is a crash problem... I do not know why.
    
    if (leave == true)
    {
        PlayerSet::const_iterator itr = m_playersinwar[player->GetTeamId()].find(player);
        if ((*itr) == player)
            pvpWG->m_playersinwar[player->GetTeamId()].erase(player);
    }
}

void OutdoorPvPWG::QueueBoxSendedToPlayer(Player* player)
{
    if (!player)
        return;

    uint32 team;

    if (player->getFaction() == ALLIANCE)
        team = TEAM_ALLIANCE;
    else
        team = TEAM_HORDE;

    // Mark the player
    m_playersinqueue[team].insert(player);
    player->WGKickTimer = 20000;
    player->WGQueueBox = true;
}

void OutdoorPvPWG::PlayerAnsweredForInviteToQueue(Player* plr, bool Answer)
{
    OutdoorPvPWG *pvpWG = (OutdoorPvPWG*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(4197);
    if (!pvpWG || !plr)
        return;

    if (Answer == true)
    {
        uint32 team;

        if (plr->getFaction() == ALLIANCE)
            team = TEAM_ALLIANCE;
        else
            team = TEAM_HORDE;

        // Join queue
        m_playersinqueuelist[team].insert(plr);
        plr->WGKickTimer = 20000;
    }
    else
    {
        // If player is still in wintergrasp, kick him/her.
        PlayerSet::const_iterator itr = m_players[plr->GetTeamId()].find(plr);
        if ((*itr) == plr)
            pvpWG->KickPlayerFromWG(plr, false);
    }
}

void OutdoorPvPWG::SendWarningToPlayer(Player* plr,int32 entry, ...)
{
    const char *format = sObjectMgr->GetVoragineStringForDBCLocale(entry);
    va_list ap;
    char str [1024];
    va_start(ap, entry);
    vsnprintf(str,1024,format, ap);
    va_end(ap);
    std::string msg = (std::string)str;

    WorldPacket data = BuildWarningAnnPacket(msg);

    if(plr->GetSession())
        plr->GetSession()->SendPacket(&data);
}

void OutdoorPvPWG::SendWarningToAllInZone(int32 entry, const char *text)
{
    const char *format = fmtstring(sObjectMgr->GetVoragineStringForDBCLocale(entry), text);
    va_list ap;
    char str [1024];
    va_start(ap, entry);
    vsnprintf(str,1024,format,ap);
    va_end(ap);
    std::string msg = (std::string)str;
    WorldPacket data = BuildWarningAnnPacket(msg);
    BroadcastPacketZone(data);
}

void OutdoorPvPWG::BroadcastPacketZone(WorldPacket & data) const
{
    for (uint8 team = 0; team < 2; ++team)
        for (PlayerSet::const_iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
            if((*itr))
                (*itr)->GetSession()->SendPacket(&data);
}

WorldPacket OutdoorPvPWG::BuildWarningAnnPacket(std::string msg)
{
    WorldPacket data(SMSG_MESSAGECHAT, 200);

    data << (uint8)CHAT_MSG_RAID_BOSS_EMOTE;
    data << (uint32)LANG_UNIVERSAL;
    data << (uint64)0;
    data << (uint32)0; // 2.1.0
    data << (uint32)1;
    data << (uint8)0; 
    data << (uint64)0;
    data << (uint32)(strlen(msg.c_str())+1);
    data << msg.c_str();
    data << (uint8)0;

    return data;
}

void OutdoorPvPWG::EndBattle()
{
    // Cast Essence of Wintergrasp to all players (CheckCast will determine who to cast)
    sWorld->UpdateAreaDependentAuras();
    //Sound on End Battle
    for (PlayerSet::iterator itr = m_players[getDefenderTeam()].begin(); itr != m_players[getDefenderTeam()].end(); ++itr)
    {
        if (getDefenderTeam()==TEAM_ALLIANCE)
            TeamIDsound=OutdoorPvP_WG_SOUND_ALLIANCE_WINS; //Allience Win
        else
            TeamIDsound=OutdoorPvP_WG_SOUND_HORDE_WINS;  //Horde Win
        (*itr)->PlayDirectSound(TeamIDsound) ; // SoundOnEndWin
    }
    for (PlayerSet::iterator itr = m_players[getAttackerTeam()].begin(); itr != m_players[getAttackerTeam()].end(); ++itr)
        (*itr)->PlayDirectSound(OutdoorPvP_WG_SOUND_NEAR_VICTORY) ; // SoundOnEndLoose
    for (uint32 team = 0; team < 2; ++team)
    {
        // destroyed all vehicles
        while(!m_vehicles[team].empty())
        {
            Creature *veh = *m_vehicles[team].begin();
            m_vehicles[team].erase(m_vehicles[team].begin());
            veh->setDeathState(JUST_DIED);
        }

        if (m_players[team].empty())
            continue;

        for (PlayerSet::iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
        {
            // When WG ends the zone is cleaned including corpses, revive all players if dead
            if ((*itr)->isDead())
            {
                (*itr)->ResurrectPlayer(1.0f);
                sObjectAccessor->ConvertCorpseForPlayer((*itr)->GetGUID());
            }
            (*itr)->RemoveAurasDueToSpell(SPELL_TENACITY);
            (*itr)->CombatStop(true);
            (*itr)->getHostileRefManager().deleteReferences();
        }

        if (m_timer == 1) // Battle End was forced so no reward.
        {
            for (PlayerSet::iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
            {
                (*itr)->RemoveAurasDueToSpell(SPELL_RECRUIT);
                (*itr)->RemoveAurasDueToSpell(SPELL_CORPORAL);
                (*itr)->RemoveAurasDueToSpell(SPELL_LIEUTENANT);
                (*itr)->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);
                (*itr)->RemoveAurasDueToSpell(SPELL_SPIRITUAL_IMMUNITY);
            }
            continue;
        }

        // calculate rewards
        uint32 intactNum = 0;
        uint32 damagedNum = 0;
        for (OutdoorPvP::OPvPCapturePointMap::const_iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
            if (OPvPCapturePointWG *workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
                if (workshop->m_buildingState->GetTeam() == team)
                {
                    if (workshop->m_buildingState->damageState == DAMAGE_DAMAGED)
                        ++damagedNum;
                    else if (workshop->m_buildingState->damageState == DAMAGE_INTACT)
                        ++intactNum;
                }
        uint32 spellRewardId = team == getDefenderTeam() ? SPELL_VICTORY_REWARD : SPELL_DEFEAT_REWARD;
        uint32 baseHonor = 0;
        uint32 marks = 0;
        uint32 playersWithRankNum = 0;
        uint32 honor = 0;

        if (sWorld->getBoolConfig(CONFIG_OUTDOORPVP_WINTERGRASP_CUSTOM_HONOR))
        {
            // Calculate Level 70+ with Corporal or Lieutenant rank
            for (PlayerSet::iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
                if ((*itr)->getLevel() > 74 && ((*itr)->HasAura(SPELL_LIEUTENANT) || (*itr)->HasAura(SPELL_CORPORAL)))
                    ++playersWithRankNum;
            baseHonor = team == getDefenderTeam() ? sWorld->getIntConfig(CONFIG_OUTDOORPVP_WINTERGRASP_WIN_BATTLE) : sWorld->getIntConfig(CONFIG_OUTDOORPVP_WINTERGRASP_LOSE_BATTLE);
            baseHonor += (sWorld->getIntConfig(CONFIG_OUTDOORPVP_WINTERGRASP_DAMAGED_TOWER) * m_towerDamagedCount[OTHER_TEAM(team)]);
            baseHonor += (sWorld->getIntConfig(CONFIG_OUTDOORPVP_WINTERGRASP_DESTROYED_TOWER) * m_towerDestroyedCount[OTHER_TEAM(team)]);
            baseHonor += (sWorld->getIntConfig(CONFIG_OUTDOORPVP_WINTERGRASP_INTACT_BUILDING) * intactNum);
            baseHonor += (sWorld->getIntConfig(CONFIG_OUTDOORPVP_WINTERGRASP_DAMAGED_BUILDING) * damagedNum);
            if (playersWithRankNum)
                baseHonor /= playersWithRankNum;
        }

        for (PlayerSet::iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
        {
            if ((*itr)->getLevel() < 75)
                continue; // No rewards for level <75

            // give rewards
            if (sWorld->getBoolConfig(CONFIG_OUTDOORPVP_WINTERGRASP_CUSTOM_HONOR))
            {
                if (team == getDefenderTeam())
                {
                    if ((*itr)->HasAura(SPELL_LIEUTENANT))
                    {
                        marks = 3;
                        honor = baseHonor;
                    }
                    else if ((*itr)->HasAura(SPELL_CORPORAL))
                    {
                        marks = 2;
                        honor = baseHonor;
                    } else {
                        marks = 1;
                        honor = 0;
                    }
                } else {
                    if ((*itr)->HasAura(SPELL_LIEUTENANT))
                    {
                        marks = 1;
                        honor = baseHonor;
                    }
                    else if ((*itr)->HasAura(SPELL_CORPORAL))
                    {
                        marks = 1;
                        honor = baseHonor;
                    } else {
                        marks = 0;
                        honor = 0;
                    }
                }
                (*itr)->RewardHonor(NULL, 1, honor);
                RewardMarkOfHonor(*itr, marks);
                (*itr)->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, spellRewardId);
            } else {
                if ((*itr)->HasAura(SPELL_LIEUTENANT) || (*itr)->HasAura(SPELL_CORPORAL))
                {
                    // TODO - Honor from SpellReward should be shared by team players
                    // TODO - Marks should be given depending on Rank but 3 are given
                    // each time so Won't give any to recruits
                    (*itr)->CastSpell(*itr, spellRewardId, true);
                    for (uint32 i = 0; i < intactNum; ++i)
                        (*itr)->CastSpell(*itr, SPELL_INTACT_BUILDING, true);
                    for (uint32 i = 0; i < damagedNum; ++i)
                        (*itr)->CastSpell(*itr, SPELL_DAMAGED_BUILDING, true);
                    for (uint32 i = 0; i < m_towerDamagedCount[OTHER_TEAM(team)]; ++i)
                        (*itr)->CastSpell(*itr, SPELL_DAMAGED_TOWER, true);
                    for (uint32 i = 0; i < m_towerDestroyedCount[OTHER_TEAM(team)]; ++i)
                        (*itr)->CastSpell(*itr, SPELL_DESTROYED_TOWER, true);
                }
            }
            if (team == getDefenderTeam() && ((*itr)->HasAura(SPELL_LIEUTENANT) || (*itr)->HasAura(SPELL_CORPORAL)))
            {
                (*itr)->AreaExploredOrEventHappens(A_VICTORY_IN_WG);
                (*itr)->AreaExploredOrEventHappens(H_VICTORY_IN_WG);
            }
            (*itr)->RemoveAurasDueToSpell(SPELL_RECRUIT);
            (*itr)->RemoveAurasDueToSpell(SPELL_CORPORAL);
            (*itr)->RemoveAurasDueToSpell(SPELL_LIEUTENANT);
            (*itr)->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);
            (*itr)->RemoveAurasDueToSpell(SPELL_SPIRITUAL_IMMUNITY);
        }
    }

    m_wartime = false;
    m_timer = sWorld->getIntConfig(CONFIG_OUTDOORPVP_WINTERGRASP_INTERVAL) * MINUTE * IN_MILLISECONDS;
    RemoveOfflinePlayerWGAuras();
    // Update timer in players battlegrounds tab
    sWorld->SendWintergraspState();
    // update go factions
    for (GameObjectSet::iterator itr = m_gobjects.begin(); itr != m_gobjects.end(); ++itr)
       UpdateGameObjectInfo(*itr);
}

bool OutdoorPvPWG::CanBuildVehicle(OPvPCapturePointWG *workshop) const
{
    TeamId team = workshop->m_buildingState->GetTeam();
    if (team == TEAM_NEUTRAL)
        return false;

    return isWarTime()
        && workshop->m_buildingState->damageState != DAMAGE_DESTROYED
        && m_vehicles[team].size() < m_workshopCount[team] * MAX_VEHICLE_PER_WORKSHOP;
}

uint32 OutdoorPvPWG::GetData(uint32 id)
{
    // if can build more vehicles
    if (OPvPCapturePointWG *workshop = GetWorkshopByEngGuid(id))
        return CanBuildVehicle(workshop) ? 1 : 0;

    return 0;
}

void OutdoorPvPWG::RewardMarkOfHonor(Player *plr, uint32 count)
{
    // 'Inactive' this aura prevents the player from gaining honor points and battleground tokens
    if (plr->HasAura(SPELL_AURA_PLAYER_INACTIVE))
        return;
    if (count == 0)
        return;

    ItemPosCountVec dest;
    uint32 no_space_count = 0;
    uint8 msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, WG_MARK_OF_HONOR, count, &no_space_count);

    if (msg == EQUIP_ERR_ITEM_NOT_FOUND)
    {
        sLog->outErrorDb("Wintergrasp reward item (Entry %u) not exist in `item_template`.", WG_MARK_OF_HONOR);
        return;
    }

    if (msg != EQUIP_ERR_OK) // convert to possible store amount
        count -= no_space_count;

    if (count != 0 && !dest.empty()) // can add some
        if (Item* item = plr->StoreNewItem(dest, WG_MARK_OF_HONOR, true, 0))
            plr->SendNewItem(item, count, true, false);
}

void OutdoorPvPWG::LoadQuestGiverMap(uint32 guid, Position posHorde, Position posAlli)
{
    m_qgPosMap[std::pair<uint32, bool>(guid, true)] = posHorde,
    m_qgPosMap[std::pair<uint32, bool>(guid, false)] = posAlli,
    m_questgivers[guid] = NULL;
    if (getDefenderTeam() == TEAM_ALLIANCE)
        sObjectMgr->MoveCreData(guid, 571, posAlli);
    if (getDefenderTeam() == TEAM_HORDE)
        sObjectMgr->MoveCreData(guid, 571, posHorde);
}

OPvPCapturePointWG *OutdoorPvPWG::GetWorkshop(uint32 lowguid) const
{
    if (OPvPCapturePoint *cp = GetCapturePoint(lowguid))
        return dynamic_cast<OPvPCapturePointWG*>(cp);
    return NULL;
}

OPvPCapturePointWG *OutdoorPvPWG::GetWorkshopByEngGuid(uint32 lowguid) const
{
    for (OutdoorPvP::OPvPCapturePointMap::const_iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
        if (OPvPCapturePointWG *workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
            if (workshop->m_engGuid == lowguid)
                return workshop;
    return NULL;
}

OPvPCapturePointWG *OutdoorPvPWG::GetWorkshopByGOGuid(uint64 lowguid) const
{
    for (OutdoorPvP::OPvPCapturePointMap::const_iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
        if (OPvPCapturePointWG *workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
            if (workshop->m_workshopGuid == lowguid)
                return workshop;
    return NULL;
}

/*########################################################
 * Copy of Battleground system to make Spirit Guides Work
 *#######################################################*/
void OutdoorPvPWG::SendAreaSpiritHealerQueryOpcode(Player *pl, const uint64& guid)
{
    WorldPacket data(SMSG_AREA_SPIRIT_HEALER_TIME, 12);
    uint32 time_ = 30000 - GetLastResurrectTime();      // resurrect every 30 seconds
    if (time_ == uint32(-1))
        time_ = 0;
    data << guid << time_;
    pl->GetSession()->SendPacket(&data);
}

void OutdoorPvPWG::AddPlayerToResurrectQueue(uint64 npc_guid, uint64 player_guid)
{
    m_ReviveQueue[npc_guid].push_back(player_guid);
    Player *plr = sObjectMgr->GetPlayer(player_guid);
    if (!plr)
        return;

    plr->CastSpell(plr, SPELL_WAITING_FOR_RESURRECT, true);
}

void OutdoorPvPWG::RemovePlayerFromResurrectQueue(uint64 player_guid)
{
    for (std::map<uint64, std::vector<uint64> >::iterator itr = m_ReviveQueue.begin(); itr != m_ReviveQueue.end(); ++itr)
        for (std::vector<uint64>::iterator itr2 =(itr->second).begin(); itr2 != (itr->second).end(); ++itr2)
            if (*itr2 == player_guid)
            {
                (itr->second).erase(itr2);

                Player *plr = sObjectMgr->GetPlayer(player_guid);
                if (!plr)
                    return;

                plr->RemoveAurasDueToSpell(SPELL_WAITING_FOR_RESURRECT);
                return;
            }
}

void OutdoorPvPWG::RelocateAllianceDeadPlayers(Creature *cr)
{
    if (!cr || GetCreatureType(cr->GetEntry()) != CREATURE_SPIRIT_GUIDE)
        return;

    // Those who are waiting to resurrect at this node are taken to the closest own node's graveyard
    std::vector<uint64> ghost_list = m_ReviveQueue[cr->GetGUID()];
    if (!ghost_list.empty())
    {
        WorldSafeLocsEntry const *ClosestGrave = NULL;
        for (std::vector<uint64>::const_iterator itr = ghost_list.begin(); itr != ghost_list.end(); ++itr)
        {
            Player* plr = sObjectMgr->GetPlayer(*itr);
            if (!plr)
                continue;

            if (plr->getFaction() == ALLIANCE)
            {
                if (ClosestGrave)
                    plr->TeleportTo(plr->GetMapId(), ClosestGrave->x, ClosestGrave->y, ClosestGrave->z, plr->GetOrientation());
                else
                    ClosestGrave = sObjectMgr->GetClosestGraveYard(plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ(), plr->GetMapId(), plr->GetTeam());
            }
        }
    }
}

void OutdoorPvPWG::RelocateHordeDeadPlayers(Creature *cr)
{
    if (!cr || GetCreatureType(cr->GetEntry()) != CREATURE_SPIRIT_GUIDE)
        return;

    // Those who are waiting to resurrect at this node are taken to the closest own node's graveyard
    std::vector<uint64> ghost_list = m_ReviveQueue[cr->GetGUID()];
    if (!ghost_list.empty())
    {
        WorldSafeLocsEntry const *ClosestGrave = NULL;
        for (std::vector<uint64>::const_iterator itr = ghost_list.begin(); itr != ghost_list.end(); ++itr)
        {
            Player* plr = sObjectMgr->GetPlayer(*itr);
            if (!plr)
                continue;
            if (plr->getFaction() == HORDE)
            {
                if (ClosestGrave)
                    plr->TeleportTo(plr->GetMapId(), ClosestGrave->x, ClosestGrave->y, ClosestGrave->z, plr->GetOrientation());
                else
                    ClosestGrave = sObjectMgr->GetClosestGraveYard(plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ(), plr->GetMapId(), plr->GetTeam());
            }
        }
    }
}

OPvPCapturePointWG::OPvPCapturePointWG(OutdoorPvPWG *opvp, BuildingState *state) : OPvPCapturePoint(opvp), m_spiGuid(0), m_spiritguide(NULL), m_spiritguide_horde(NULL), m_spiritguide_alliance(NULL), m_engGuid(0), m_engineer(NULL), m_buildingState(state), m_wintergrasp(opvp) {}

void OPvPCapturePointWG::SetTeamByBuildingState()
{
    if (m_buildingState->GetTeam() == TEAM_ALLIANCE)
    {
        m_value = m_maxValue;
        m_State = OBJECTIVESTATE_ALLIANCE;
    }
    else if (m_buildingState->GetTeam() == TEAM_HORDE)
    {
        m_value = -m_maxValue;
        m_State = OBJECTIVESTATE_HORDE;
    } else {
        m_value = 0;
        m_State = OBJECTIVESTATE_NEUTRAL;
    }

    if (m_team != m_buildingState->GetTeam())
    {
        TeamId oldTeam = m_team;
        m_team = m_buildingState->GetTeam();
        ChangeTeam(oldTeam);
    }

    SendChangePhase();
}

void OPvPCapturePointWG::ChangeTeam(TeamId oldTeam)
{
    uint32 entry = 0;
    uint32 guide_entry = 0;
    uint32 guide_entry_fortress_horde = 0;
    uint32 guide_entry_fortress_alliance = 0;

    if (oldTeam != TEAM_NEUTRAL)
        m_wintergrasp->ModifyWorkshopCount(oldTeam, false);

    if (m_team != TEAM_NEUTRAL)
    {
        entry = m_team == TEAM_ALLIANCE ? CRE_ENG_A : CRE_ENG_H;
        guide_entry = m_team == TEAM_ALLIANCE ? CRE_SPI_A : CRE_SPI_H;
        guide_entry_fortress_horde = m_team == TEAM_HORDE ? CRE_SPI_H : CRE_SPI_H;
        guide_entry_fortress_alliance = m_team == TEAM_ALLIANCE ? CRE_SPI_A : CRE_SPI_A;
        m_wintergrasp->ModifyWorkshopCount(m_team, true);
    }

    if (m_capturePoint)
        GameObject::SetGoArtKit(CapturePointArtKit[m_team], m_capturePoint, m_capturePointGUID);

    m_buildingState->SetTeam(m_team);
    // TODO: this may be sent twice
    m_wintergrasp->BroadcastStateChange(m_buildingState);

    if (m_buildingState->building)
        m_buildingState->building->SetUInt32Value(GAMEOBJECT_FACTION, WintergraspFaction[m_team]);

    if (entry)
    {
        if (m_engGuid)
        {
            *m_engEntry = entry;
            _RespawnCreatureIfNeeded(m_engineer, entry);
            if (Changing == false)
            {
                CapturedWorkshop(m_engineer, entry);
                m_wintergrasp->ChangeFlags(m_engineer);
            }
        }
        if (m_spiGuid)
            if (m_wintergrasp->getAttackerTeam() == TEAM_ALLIANCE)
            {
                *m_spiEntry = guide_entry;
                _RespawnCreatureIfNeeded(m_spiritguide_horde, guide_entry_fortress_horde);
                m_wintergrasp->RelocateHordeDeadPlayers(m_spiritguide_horde); // Horde
                _RespawnCreatureIfNeeded(m_spiritguide, guide_entry);
                m_wintergrasp->RelocateAllianceDeadPlayers(m_spiritguide); // Alliance
            } 
            else 
            {
                *m_spiEntry = guide_entry;
                _RespawnCreatureIfNeeded(m_spiritguide_alliance, guide_entry_fortress_alliance);
                m_wintergrasp->RelocateAllianceDeadPlayers(m_spiritguide_alliance); // Alliance
                _RespawnCreatureIfNeeded(m_spiritguide, guide_entry);
                m_wintergrasp->RelocateHordeDeadPlayers(m_spiritguide); // Horde
            }
    } else if (m_engineer)
    {
        std::string name;
        switch (m_engineer->GetAreaId())
        {
            case 4538: // Sunken Ring
                name = "The Sunken Ring siege workshop";
                break;
            case 4539: // Broken Temple
                name = "The Broken Temple siege workshop";
                break;
            case 4611: // Westspark Workshop
                name = "The Westspark siege workshop";
                break;
            case 4612: // Eastspark Workshop
                name = "The Eastspark siege workshop";
                break;
        }
        if (m_engineer->GetEntry() == CRE_ENG_A)
            m_wintergrasp->SendWarningToAllInZone(WG_TEXT_HORDE_ATTACKED, name.c_str());
        else
            m_wintergrasp->SendWarningToAllInZone(WG_TEXT_ALLIANCE_ATTACKED, name.c_str());
        m_engineer->SetVisible(false);
        m_wintergrasp->ChangeFlags(m_engineer);
    }
}

class OutdoorPvP_wintergrasp : public OutdoorPvPScript
{
    public:
        OutdoorPvP_wintergrasp() : OutdoorPvPScript("outdoorpvp_wg") { }

        OutdoorPvP* GetOutdoorPvP() const
        {
            return new OutdoorPvPWG();
        }
};

void AddSC_outdoorpvp_wg()
{
    new OutdoorPvP_wintergrasp();
}
