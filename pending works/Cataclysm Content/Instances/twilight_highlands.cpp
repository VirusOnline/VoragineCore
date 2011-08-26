
#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"

/*####
## npc_gurgthock2
## ToDo: Implement more texts
####*/

enum eGurgthock2
{
    QUEST_CRUCIBLE_CARNAGE_BLOODEYE_BRUISER       = 27863,
    QUEST_CRUCIBLE_CARNAGE_WAYWARD_WILDHAMMER_H   = 27865,
    QUEST_CRUCIBLE_CARNAGE_DEADLY_DRAGONMAW_A     = 27864,
    QUEST_CRUCUBLE_CARNAGE_CALDER_CREATION        = 27866,
    QUEST_CRUCIBLE_CARNAGE_EARL_EVISCERATION      = 27867,
    QUEST_CRUCIBLE_CARNAGE_FROM_TWILIGHT_TERROR   = 27868,

    NPC_HURPDERP                                  = 46944,
    NPC_SULLY_KNEECAPPER                          = 46946,
    NPC_TORG_DRAKEFLAYER                          = 46945,
    NPC_CADAVER_COLLAGE                           = 46947,
    NPC_GEOFFERY_TULVAN                           = 46948,
    NPC_EMBESCAR_DEVOURER                         = 46949,
    
    SAY_GEOFFERY_SPAWN                            = -1999928,
    SAY_GEOFFERY_DIE                              = -1999929,

    
};

const Position SpawnPosition[] =
{
    {-4178.053711f, -5161.670410f, -7.732315f, 2.808868f},
};

class npc_gurgthock2 : public CreatureScript
{
public:
    npc_gurgthock2() : CreatureScript("npc_gurgthock2") { }

    struct npc_gurgthock2AI : public ScriptedAI
    {
        npc_gurgthock2AI(Creature* pCreature) : ScriptedAI(pCreature) {}

        uint64 SummonGUID;
        uint64 uiPlayerGUID;

        uint32 uiTimer;
        uint32 uiPhase;
        uint32 uiRemoveFlagTimer;
        uint32 uiQuest;
        
        bool bRemoveFlag;

        void Reset()
        {
            SummonGUID = 0;
            uiPlayerGUID = 0;

            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            uiTimer = 0;
            uiPhase = 0;
            uiQuest = 0;
            uiRemoveFlagTimer = 5000;

            
            bRemoveFlag = false;
        }

        void SetGUID(const uint64 &guid, int32 /*id*/)
        {
            uiPlayerGUID = guid;
        }

        void SetData(uint32 uiId, uint32 uiValue)
        {
            bRemoveFlag = true;
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            
            Player* pPlayer = me->GetPlayer(*me, uiPlayerGUID);
            
            switch(uiId)
            {
                case 1:
                {
                    if (!pPlayer)
                        return;
                    std::string sText = ("The grand Crucible of Carnage awaits, " + std::string(pPlayer->GetName()) + ". Remember, once a battle starts you have to stay in the area. WIN OR DIE!");
                    me->MonsterSay(sText.c_str(),LANG_UNIVERSAL,0);
                    
                    switch(uiValue)
                    {
                        case QUEST_CRUCIBLE_CARNAGE_BLOODEYE_BRUISER:
                            //DoScriptText(SAY_QUEST_ACCEPT_TUSKARRMAGEDON, me);
                            uiPhase = 1;
                            uiTimer = 4000;
                            break;
                        case QUEST_CRUCIBLE_CARNAGE_WAYWARD_WILDHAMMER_H:
                            //DoScriptText(SAY_QUEST_ACCEPT_KORRAK_1, me);
                            uiPhase = 3;
                            uiTimer = 3000;
                            break;
                        case QUEST_CRUCIBLE_CARNAGE_DEADLY_DRAGONMAW_A:
                            uiPhase = 6;
                            uiTimer = 3000;
                            break;
                        case QUEST_CRUCUBLE_CARNAGE_CALDER_CREATION:
                            uiTimer = 5000;
                            uiPhase = 7;
                            break;
                        case QUEST_CRUCIBLE_CARNAGE_EARL_EVISCERATION:
                            uiTimer = 2000;
                            uiPhase = 12;
                            break;
                        case QUEST_CRUCIBLE_CARNAGE_FROM_TWILIGHT_TERROR:
                            uiTimer = 5000;
                            uiPhase = 15;
                            break;
                   }
                }
                        break;
                }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            ScriptedAI::UpdateAI(uiDiff);

            if (bRemoveFlag)
            {
                if (uiRemoveFlagTimer <= uiDiff)
                {
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    bRemoveFlag = false;

                    uiRemoveFlagTimer = 10000;
                } else uiRemoveFlagTimer -= uiDiff;
            }

            if (uiPhase)
            {
                Player* pPlayer = me->GetPlayer(*me, uiPlayerGUID);

                if (uiTimer <= uiDiff)
                {
                    switch(uiPhase)
                    {
                        case 1:
                            if (Creature* pSummon = me->SummonCreature(NPC_HURPDERP, SpawnPosition[0], TEMPSUMMON_CORPSE_DESPAWN, 1000))
                                SummonGUID = pSummon->GetGUID();
                            uiPhase = 2;
                            uiTimer = 4000;
                            break;
                        case 3:
                            if (Creature* pSummon = me->SummonCreature(NPC_SULLY_KNEECAPPER, SpawnPosition[0], TEMPSUMMON_CORPSE_DESPAWN, 1000))
                                SummonGUID = pSummon->GetGUID();
                            uiTimer = 3000;
                            uiPhase = 0;
                            break;
                        case 6:
                            {
                                if (Creature* pSummon = me->SummonCreature(NPC_TORG_DRAKEFLAYER, SpawnPosition[0], TEMPSUMMON_CORPSE_DESPAWN, 1000))
                                    SummonGUID = pSummon->GetGUID();
                                uiTimer = 3000;
                                uiPhase = 0;
                            }
                            break;
                        case 7:
                            {
                               if (Creature* pSummon = me->SummonCreature(NPC_CADAVER_COLLAGE, SpawnPosition[0], TEMPSUMMON_CORPSE_DESPAWN, 1000))
                                    SummonGUID = pSummon->GetGUID();
                                uiTimer = 3000;
                                uiPhase = 0;
                            }
                            break;
                        case 12:
                        {
                                DoScriptText(SAY_GEOFFERY_SPAWN,me);
                                if (Creature* pSummon = me->SummonCreature(NPC_GEOFFERY_TULVAN, SpawnPosition[0], TEMPSUMMON_CORPSE_DESPAWN, 1000))
                                    SummonGUID = pSummon->GetGUID();
                                uiTimer = 5000;
                                uiPhase = 0;
                        }
                        break;
                        case 15:
                            if (Creature* pSummon = me->SummonCreature(NPC_EMBESCAR_DEVOURER, SpawnPosition[0], TEMPSUMMON_CORPSE_DESPAWN, 1000))
                                    SummonGUID = pSummon->GetGUID();
                            uiTimer = 3000;
                            uiPhase = 0;
                            break;
                    }
                }else uiTimer -= uiDiff;
            }
        }
    };

    bool OnQuestAccept(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
    {
        switch (pQuest->GetQuestId())
        {
       
            case QUEST_CRUCIBLE_CARNAGE_BLOODEYE_BRUISER:
            case QUEST_CRUCIBLE_CARNAGE_WAYWARD_WILDHAMMER_H:
            case QUEST_CRUCIBLE_CARNAGE_DEADLY_DRAGONMAW_A:
            case QUEST_CRUCUBLE_CARNAGE_CALDER_CREATION:
            case QUEST_CRUCIBLE_CARNAGE_EARL_EVISCERATION:
            case QUEST_CRUCIBLE_CARNAGE_FROM_TWILIGHT_TERROR:
                pCreature->AI()->SetData(1, pQuest->GetQuestId());
                break;
        }

        pCreature->AI()->SetGUID(pPlayer->GetGUID());

        return false;
    }

    CreatureAI *GetAI(Creature *creature) const
    {
        return new npc_gurgthock2AI(creature);
    }
};

void AddSC_twilight_highlands()
{
    new npc_gurgthock2();
}
