/*
 * Copyright (C) 2005-2011 MaNGOS <http://www.getmangos.com/>
 *
 * Copyright (C) 2008-2011 Trinity <http://www.trinitycore.org/>
 *
 * Copyright (C) 2006-2011 ScriptDev2 <http://www.scriptdev2.com/>
 *
 * Copyright (C) 2010-2011 VoragineCore <http://www.projectvoragine.com/>
 *
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

#include "ScriptPCH.h"
#include "vault_of_archavon.h"
#include "OutdoorPvPMgr.h"
#include "OutdoorPvPWG.h"

/* Vault of Archavon encounters:
1 - Archavon the Stone Watcher event
2 - Emalon the Storm Watcher event
3 - Koralon the Flame Watcher event
4 - Toravon the Ice Watcher event
*/

class instance_archavon : public InstanceMapScript
{
    public:
        instance_archavon() : InstanceMapScript("instance_archavon", 624) { }

        struct instance_archavon_InstanceMapScript : public InstanceScript
        {
            instance_archavon_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(MAX_ENCOUNTER);
            }

            uint32 m_auiEncounter[MAX_ENCOUNTER];

            void Initialize()
            {
                EmalonGUID = 0;
                ToravonGUID = 0;
            }

            void OnPlayerEnter(Player *m_player)
            {
                if (sWorld->getBoolConfig(CONFIG_OUTDOORPVP_WINTERGRASP_ENABLED))
                    if(OutdoorPvPWG *pvpWG = (OutdoorPvPWG*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(4197))
                    {
                        if ((pvpWG->getDefenderTeam()==TEAM_ALLIANCE) && (m_player->ToPlayer()->GetTeam() == ALLIANCE))
                            return;
                        else if ((pvpWG->getDefenderTeam()!=TEAM_ALLIANCE) && (m_player->ToPlayer()->GetTeam() == HORDE))
                            return;
                        else m_player->CastSpell(m_player, SPELL_TELEPORT_FORTRESS, true);
                    }
            }

            void OnCreatureCreate(Creature* creature)
            {
                switch (creature->GetEntry())
                {
                    case CREATURE_EMALON:
                        EmalonGUID = creature->GetGUID();
                        break;
                    case CREATURE_TORAVON:
                        ToravonGUID = creature->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            uint64 GetData64(uint32 identifier)
            {
                switch (identifier)
                {
                    case DATA_EMALON:
                        return EmalonGUID;
                    case DATA_TORAVON:
                        return ToravonGUID;
                    default:
                        break;
                }

                return 0;
            }

            void SetData(uint32 type, uint32 data)
            {
                switch(type)
                {
                case DATA_ARCHAVON:
                    m_auiEncounter[0] = data;
                    break;
                case DATA_EMALON:
                    m_auiEncounter[1] = data;
                    break;
                case DATA_KORALON:
                    m_auiEncounter[2] = data;
                    break;
                case DATA_TORAVON:
                    m_auiEncounter[3] = data;
                    break;
                }
            }

            uint32 GetData(uint32 type)
            {
                switch(type)
                {
                    case DATA_ARCHAVON:       return m_auiEncounter[0];
                    case DATA_EMALON:         return m_auiEncounter[1];
                    case DATA_KORALON:        return m_auiEncounter[2];
                    case DATA_TORAVON:        return m_auiEncounter[3];
                }

                return 0;
            }

            bool IsEncounterInProgress() const
            {
                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (m_auiEncounter[i] == IN_PROGRESS) return true;

                return false;
            }

        private:
            uint64 EmalonGUID;
            uint64 ToravonGUID;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_archavon_InstanceMapScript(map);
        }
};

void AddSC_instance_archavon()
{
    new instance_archavon();
}