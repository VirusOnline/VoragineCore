
#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"
#include "ScriptedFollowerAI.h"

// http://www.youtube.com/watch?v=x_kHGGUHwNg - The defense of Nahom
// Need to spawn correct npcs in zone and find the correct phasemask
#define DEFENSE_NAHOM_EVENT_START "I'm ready to battle" // NOT BLIZZLIKE
#define SPELL_START 85208
// ToDo: Move to DB
#define SAY_START01 "It...It worked! We will defend Nahom with the power stored within this temple!"
// We need to find the correct vehicle id for this npc: 45670
#define NPC_NEFERSET_INFANTRY 48463
#define NPC_COLOSUS 45586
#define GREATER_COLOSUS 48490
#define SPAWN_COORDS_ENEMY1 -9712.055664f, -1617.849243f, 24.623856f, 5.205875f
#define SPAWN_COORDS_ENEMY2 -9712.055664f, -1617.849243f, 24.623856f, 5.205875f
#define SPAWN_COORDS_ENEMY RAND(SPAWN_COORDS_ENEMY1,SPAWN_COORDS_ENEMY2)


// Update creature_template set scriptname = 'npc_ramkahen_sergeant' where entry = 49228
class npc_ramkahen_sergeant : public CreatureScript
{
public:
    npc_ramkahen_sergeant() : CreatureScript("npc_ramkahen_sergeant") { }
    
    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_ramkahen_sergeantAI (pCreature);
    }

    struct npc_ramkahen_sergeantAI : public ScriptedAI
    {
        npc_ramkahen_sergeantAI(Creature *c) : ScriptedAI(c) {}
        
        bool QuestInProgress;
        uint64 plrGUID;
        uint32 wave;
        uint32 ui_Phase;
        uint32 nextwave;
        void Reset()
        {
            nextwave = 20000;
            ui_Phase = 0;
            wave = 0;
            QuestInProgress = false;
            plrGUID = 0;
        }
        void UpdateAI(const uint32 diff)
        {
            if(!QuestInProgress)
                return;
            if(!plrGUID)
                return;
                
            if(nextwave <= diff)
            {
                if(ui_Phase == 1)
                {
                    wave = 1;
                    for(uint32 x = 0; x<8; ++x)
                        me->SummonCreature(NPC_NEFERSET_INFANTRY, SPAWN_COORDS_ENEMY1, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 4000);
                    ui_Phase = 2;
                    nextwave = 20000;
                }
                else if(ui_Phase == 2)
                {
                    wave = 2;
                    for(uint32 x = 0; x<8; ++x)
                        me->SummonCreature(NPC_NEFERSET_INFANTRY, SPAWN_COORDS_ENEMY1, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 4000);
                    ui_Phase = 3;
                    nextwave = 12000;
                }
                else if(ui_Phase == 3)
                {
                    wave = 3;
                    for(uint32 x = 0; x<8; ++x)
                        me->SummonCreature(NPC_NEFERSET_INFANTRY, SPAWN_COORDS_ENEMY, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 4000);
                    me->SummonCreature(NPC_COLOSUS,SPAWN_COORDS_ENEMY,TEMPSUMMON_CORPSE_TIMED_DESPAWN, 4000);
                    ui_Phase = 4;
                    nextwave = 9000;
                }
                else if(ui_Phase == 3)
                {
                    wave = 3;
                    for(uint32 x = 0; x<8; ++x)
                        me->SummonCreature(NPC_NEFERSET_INFANTRY, SPAWN_COORDS_ENEMY, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 4000);
                    for(uint32 x = 0; x<8; ++x)
                        me->SummonCreature(NPC_NEFERSET_INFANTRY, SPAWN_COORDS_ENEMY, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 4000);
                    for(uint32 x = 0; x<3; ++x)
                        me->SummonCreature(NPC_COLOSUS,SPAWN_COORDS_ENEMY,TEMPSUMMON_CORPSE_TIMED_DESPAWN, 4000);
                    ui_Phase = 4;
                    nextwave = 6000;
                }
                else if(ui_Phase == 4) //Final, spawn greater colosus
                {
                    wave = 4;
                    me->SummonCreature(GREATER_COLOSUS,SPAWN_COORDS_ENEMY,TEMPSUMMON_CORPSE_TIMED_DESPAWN, 4000);
                    ui_Phase = 5;
                }
            } else nextwave -= diff;
        }
    };
    
    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        if (uiAction == 1000) //Start battle
        {
            CAST_AI(npc_ramkahen_sergeant::npc_ramkahen_sergeantAI, pCreature->AI())->QuestInProgress = true;
            CAST_AI(npc_ramkahen_sergeant::npc_ramkahen_sergeantAI, pCreature->AI())->plrGUID = pPlayer->GetGUID();
            pCreature->CastSpell(pPlayer,SPELL_START,true);
        }

        return true;
    }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        if (pCreature->isQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetGUID());

        if(pPlayer->GetQuestStatus(28501) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, DEFENSE_NAHOM_EVENT_START, GOSSIP_SENDER_MAIN, 1000);

        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

        return true;
    }

};

// Update creature_template set scriptname = 'npc_greater_colosus' where entry = 48490
class npc_greater_colosus : public CreatureScript
{
public:
    npc_greater_colosus() : CreatureScript("npc_greater_colosus") { }
    
    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_greater_colosusAI(pCreature);
    }

    struct npc_greater_colosusAI : public ScriptedAI
    {
        npc_greater_colosusAI(Creature *c) : ScriptedAI(c) {}
        
        void JustDied(Unit* /*killer*/)
        {
            uint64 plrGUID = CAST_AI(npc_ramkahen_sergeant::npc_ramkahen_sergeantAI, me->AI())->plrGUID;
            if(plrGUID)
                if (Player *pPlayer = Unit::GetPlayer(*me, plrGUID))
                    if(pPlayer->GetQuestStatus(28501) == QUEST_STATUS_INCOMPLETE)
                        pPlayer->CastSpell(pPlayer,91824,true); //Defense of Nahom credit
        }
    };
};


void AddSC_uldum()
{
    new npc_ramkahen_sergeant;
    new npc_greater_colosus;
}