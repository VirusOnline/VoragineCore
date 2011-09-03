/*
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
// tst
#include "ScriptMgr.h"
#include <cstring>
#include <cstring>
#include <stdio.h>
#include <time.h>


#define OFFSET_THEME 10000 

class npc_arena_theme : public CreatureScript
{
    public:
    npc_arena_theme() : CreatureScript("npc_arena_theme") { }

    int GetLastThemeTime()
    {
        QueryResult result;
        result = WorldDatabase.PQuery("SELECT `time` FROM `gurubashi_lastspawned`");

        if (result)
        {
            Field *fields = result->Fetch();
            return fields[0].GetInt32();
        }
        else return 0;

    }

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        if (uiAction > OFFSET_THEME)
        {
            QueryResult result;
            result = WorldDatabase.PQuery("DELETE FROM `gurubashi_lastspawned`");
            result = WorldDatabase.PQuery("INSERT INTO `gurubashi_lastspawned` VALUES (%u)", time (NULL));
            result = WorldDatabase.PQuery("SELECT `x`, `y`, `z`, `o`, `entry` FROM `gurubashi_spawns` WHERE `theme` = %u", uiAction - OFFSET_THEME);
            if (result)
            {
                pCreature->MonsterSay("Spawning gameobjects..", LANG_UNIVERSAL, pPlayer->GetGUID());
                do
                {
                    Field *fields = result->Fetch();
                    pCreature->SummonGameObject(fields[4].GetInt32(), fields[0].GetFloat(), fields[1].GetFloat(), fields[2].GetFloat(), fields[3].GetFloat(), 0, 0, 0, 0, 600); 
                }
                while (result->NextRow());
            }
            else
            {
                pCreature->MonsterSay("No gameobjects found.", LANG_UNIVERSAL, pPlayer->GetGUID());
            }
        }
        else
        {
            switch (uiAction)
            {
            case GOSSIP_ACTION_INFO_DEF + 1:
                return false;
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                return false;
                break;
            }
        }

        return true;
    }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        if (GetLastThemeTime() + 600 <= time (NULL))
        {
            QueryResult result;
            result = WorldDatabase.PQuery("SELECT `id`, `name` FROM `gurubashi_themes`");
            if (result)
            {
                do
                {
                    Field *fields = result->Fetch();
                    pPlayer->ADD_GOSSIP_ITEM(4, fields[1].GetString(), GOSSIP_SENDER_MAIN, OFFSET_THEME + fields[0].GetInt32());
                }
                while (result->NextRow());
            }
        }
        else
        {
            char msg[100];
            int time2 = GetLastThemeTime() + 600 - (int)time (NULL);
            if (time2 < 60)
                sprintf(msg, "Próximo cambio posible en menos de un minuto.");
            else
                sprintf(msg, "Proximo cambio posible en %u minuto/s.", time2 / 60);
            pPlayer->ADD_GOSSIP_ITEM(0, msg, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        }
        pPlayer->ADD_GOSSIP_ITEM(0, "Hasta Luego", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(1,pCreature->GetGUID());
        return true;
    }
};
void AddSC_npc_arena_theme()
{
    new npc_arena_theme();
}