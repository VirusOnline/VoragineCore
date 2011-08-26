

#include "ScriptPCH.h"
#include "BattlefieldMgr.h"
#include "BattlefieldTB.h"
#include "Battlefield.h"
#include "ScriptSystem.h"
#include "WorldSession.h"
#include "ObjectMgr.h"

enum eTBqueuenpctext
{
    TB_NPCQUEUE_TEXT_H_NOWAR = 110006,
    TB_NPCQUEUE_TEXT_H_QUEUE = 110002,
    TB_NPCQUEUE_TEXT_H_WAR   = 110005,
    TB_NPCQUEUE_TEXT_A_NOWAR = 110003,
    TB_NPCQUEUE_TEXT_A_QUEUE = 110001,
    TB_NPCQUEUE_TEXT_A_WAR   = 110004,
    TB_NPCQUEUE_TEXTOPTION_JOIN = -1732009,
};

class npc_tb_spiritguide : public CreatureScript
{
public:
    npc_tb_spiritguide() : CreatureScript("npc_tb_spiritguide") { }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        if (pCreature->isQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetGUID());

        BattlefieldTB *BfTB = (BattlefieldTB*)sBattlefieldMgr.GetBattlefieldByBattleId(BATTLEFIELD_BATTLEID_TB);
        if (BfTB)
        {
            GraveYardVect gy=BfTB->GetGraveYardVect();
            for(uint8 i=0;i<gy.size();i++)
            {
                if(gy[i]->GetControlTeamId()==pPlayer->GetTeamId())
                {
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT,sObjectMgr->GetTrinityStringForDBCLocale(((BfGraveYardTB*)gy[i])->GetTextId()), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+i);
                }
            }
        }

        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* pPlayer, Creature* /*pCreature*/, uint32 /*uiSender*/, uint32 uiAction)
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        BattlefieldTB *BfTB = (BattlefieldTB*)sBattlefieldMgr.GetBattlefieldByBattleId(BATTLEFIELD_BATTLEID_TB);
        if (BfTB)
        {
            GraveYardVect gy=BfTB->GetGraveYardVect();
            for(uint8 i=0;i<gy.size();i++)
            {
                if(uiAction-GOSSIP_ACTION_INFO_DEF==i && gy[i]->GetControlTeamId()==pPlayer->GetTeamId())
                {
                    const WorldSafeLocsEntry* ws=sWorldSafeLocsStore.LookupEntry(gy[i]->GetGraveYardId());
                    pPlayer->TeleportTo(ws->map_id,ws->x,ws->y,ws->z,0);
                }
            }
        }
        return true;
    }
};

class npc_tol_barad_battlemage : public CreatureScript
{
public:
    npc_tol_barad_battlemage() : CreatureScript("npc_tol_barad_battlemage") { }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        if (pCreature->isQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetGUID());

        BattlefieldTB *BfTB = (BattlefieldTB*)sBattlefieldMgr.GetBattlefieldByBattleId(BATTLEFIELD_BATTLEID_TB);
        if (BfTB)
        {

            if(BfTB->IsWarTime())
            {
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT,sObjectMgr->GetTrinityStringForDBCLocale(TB_NPCQUEUE_TEXTOPTION_JOIN), GOSSIP_SENDER_MAIN,   GOSSIP_ACTION_INFO_DEF);
                pPlayer->SEND_GOSSIP_MENU(BfTB->GetDefenderTeam()?TB_NPCQUEUE_TEXT_H_WAR:TB_NPCQUEUE_TEXT_A_WAR, pCreature->GetGUID());
            }
            else
            {
                uint32 uiTime=BfTB->GetTimer()/1000;
                pPlayer->SendUpdateWorldState(4354,time(NULL)+uiTime);
                if(uiTime<15*MINUTE)
                {
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT,sObjectMgr->GetTrinityStringForDBCLocale(TB_NPCQUEUE_TEXTOPTION_JOIN), GOSSIP_SENDER_MAIN,   GOSSIP_ACTION_INFO_DEF);
                    pPlayer->SEND_GOSSIP_MENU(BfTB->GetDefenderTeam()?TB_NPCQUEUE_TEXT_H_QUEUE:TB_NPCQUEUE_TEXT_A_QUEUE, pCreature->GetGUID());
                }
                else
                {
                    pPlayer->SEND_GOSSIP_MENU(BfTB->GetDefenderTeam()?TB_NPCQUEUE_TEXT_H_NOWAR:TB_NPCQUEUE_TEXT_A_NOWAR, pCreature->GetGUID());
                }
            }
        }
        return true;
    }

    bool OnGossipSelect(Player* pPlayer, Creature* /*pCreature*/, uint32 /*uiSender*/, uint32 /*uiAction*/)
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        BattlefieldTB *BfTB = (BattlefieldTB*)sBattlefieldMgr.GetBattlefieldByBattleId(BATTLEFIELD_BATTLEID_TB);
        if (BfTB)
        {
            if(BfTB->IsWarTime()){
                BfTB->InvitePlayerToWar(pPlayer);
            }
            else
            {
                uint32 uiTime=BfTB->GetTimer()/1000;
                if(uiTime<15*MINUTE)
                    BfTB->InvitePlayerToQueue(pPlayer);
            }
        }
        return true;
    }
};

void AddSC_tol_barad()
{
   new npc_tol_barad_battlemage();
   new npc_tb_spiritguide();
}
