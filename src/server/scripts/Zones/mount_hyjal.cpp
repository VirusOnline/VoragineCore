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

/*######
## npc_lost_warden
######*/

enum eWarden
{
    NPC_TEXT_1         = 49296,
    NPC_TEXT_2         = 49297,
    NPC_TEXT_3         = 49298,
    NPC_TEXT_4         = 49299,
    QUEST_LOST_WARDENS = 25881,
    ACTION_START       = 1,
    SAY_BYE_1          = -1999971,
    SAY_BYE_2          = -1999972,
    SAY_BYE_3          = -1999973,
    SAY_BYE_4          = -1999974,
    SAY_BYE_5          = -1999975,
    SAY_BYE_6          = -1999976,
    SAY_BYE_7          = -1999977,
    KILL_CREDIT        = 41510,
};

#define GOSSIP_TEXT "On you feet soldier, soldier. The ancients are with us, and the mountains is healed!"

class npc_lost_warden : public CreatureScript
{
public:
    npc_lost_warden() : CreatureScript("npc_lost_warden") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_lost_wardenAI (pCreature);
    }

    struct npc_lost_wardenAI : public ScriptedAI
    {
        npc_lost_wardenAI(Creature *c) : ScriptedAI(c)
        {
            me->SetReactState(REACT_PASSIVE);
            EnterEvadeMode();
            me->SetHealth(me->GetMaxHealth()*10/100); // %10 of maximum health
            uint32 Random = urand(0,3);
            switch (Random)
            {
            case 0:
                me->SetRandomTextID(NPC_TEXT_1);
                break;
            case 1:
                me->SetRandomTextID(NPC_TEXT_2);
                break;
            case 2:
                me->SetRandomTextID(NPC_TEXT_3);
                break;
            case 3:
                me->SetRandomTextID(NPC_TEXT_4);
                break;
            }
        }
    };

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        switch (uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                pPlayer->CLOSE_GOSSIP_MENU();
                pPlayer->KilledMonsterCredit(KILL_CREDIT, 0);
                pCreature->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                DoScriptText(RAND(SAY_BYE_1, SAY_BYE_2, SAY_BYE_3, SAY_BYE_4, SAY_BYE_5, SAY_BYE_6, SAY_BYE_7), pCreature, pPlayer);
                pCreature->ForcedDespawn(6000);
                pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                break;
        }

        return true;
    }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        if (pCreature->isQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetGUID());

        QuestStatus status = pPlayer->GetQuestStatus(QUEST_LOST_WARDENS);
        if (status == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TEXT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        pPlayer->SEND_GOSSIP_MENU(pCreature->GetRandomTextID(), pCreature->GetGUID());

        return true;
    }
};

void AddSC_mount_hyjal()
{
    new npc_lost_warden();
}