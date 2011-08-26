
#include "ScriptPCH.h"

class npc_flameward : public CreatureScript
{
public:
        npc_flameward() : CreatureScript("npc_flameward") { }

    struct npc_flamewardAI : public ScriptedAI
    {
        npc_flamewardAI(Creature* pCreature) : ScriptedAI(pCreature) { }

        uint32 timer;
        bool QuestInProgress;
        uint64 PlayerGUID;
        
        void Reset()
        {
            PlayerGUID = NULL;
            QuestInProgress = false;
            timer = 60000;
        }
        void UpdateAI(const uint32 diff)
        {
            if(timer <= diff)
            {
                if (PlayerGUID)
                    if (Player *pPlayer = Unit::GetPlayer(*me, PlayerGUID))
                        if (pPlayer->GetQuestStatus(25502) == QUEST_STATUS_INCOMPLETE)
                            pPlayer->CastSpell(pPlayer,75471,true);
                            
            } else timer-=diff;
        }
    };

    CreatureAI *GetAI(Creature *creature) const
    {
        return new npc_flamewardAI(creature);
    }

};
class go_flameward : public GameObjectScript
{
public:
    go_flameward() : GameObjectScript("go_flameward") { }

    
    bool OnGossipHello(Player* player, GameObject* go)
    {
        if(player->GetQuestStatus(25502) == QUEST_STATUS_INCOMPLETE)
        {
            player->CastSpell(player,75470,true);
            if(Creature* flamew = player->SummonCreature(40460, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), go->GetOrientation(), TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 61000)) //1 minute
            {
                CAST_AI(npc_flameward::npc_flamewardAI, flamew->AI())->QuestInProgress = true;
                CAST_AI(npc_flameward::npc_flamewardAI, flamew->AI())->PlayerGUID = player->GetGUID();
            }
        }
        return true;
    }
};

void AddSC_mount_hyjal()
{
    new go_flameward;
    new npc_flameward;
}