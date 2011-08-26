/* ScriptData
SDName: Gilneas City Quests
SD%Complete: 0
SDComment: 
SDCategory: Gilneas
EndScriptData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"
#include "Vehicle.h"

/*######
## Quest Evacuate the Merchant Square
######*/

#define SAY_TEXT1          "Protect me, please!"
#define SAY_TEXT2          "It's coming right for me!"
#define SAY_TEXT3          "What in the world? Let's get out of here!"
#define SAY_TEXT4          "Help!"
#define SAY_TEXT5          "Help me, please!"
#define SAY_TEXT6          "Worgen! Worgen everywhere!"
#define SAY_TEXT7          "Flee! They're everywhere!"

enum NpcEntry
{
    NPC_PANICKED_CITIZEN = 34851 //Panicked Citizen
};
static Position aSpawnGobjects[14]={
    {-1426.63f, 1436.45f, 36.4232f, 3.15599f},
    {-1421.11f, 1412.92f, 36.7947f, 1.55076f},
    {-1394.22f, 1403.21f, 36.4232f, 3.11791f},
    {-1486.21f, 1334.38f, 36.4893f, 1.83158f},
    {-1513.93f, 1371.12f, 36.535f,  0.002388f},
    {-1562.9f,  1409.65f, 36.6444f, 6.28119f},
    {-1537.49f, 1444.15f, 36.572f,  4.68448f},
    {-1463.98f, 1444.47f, 36.6447f, 4.67034f},
    {-1576.71f, 1335.87f, 36.5351f, 6.24978f},
    {-1569.81f, 1385.02f, 36.9232f, 1.54289f},
    {-1529.32f, 1322.13f, 36.5852f, 1.84564f},
    {-1583.43f, 1358.93f, 36.5342f, 1.5574f},
    {-1493.61f, 1371.21f, 36.5382f, 3.14155f}
};

static Position aSpawnNpc[14]={
    {-1423.519287f, 1432.359863f, 36.417496f},
    {-1425.657837f, 1409.786987f, 36.789055f},
    {-1391.449097f, 1398.799805f, 36.417553f},
    {-1489.789185f, 1330.539551f, 36.482567f},
    {-1516.506592f, 1375.060791f, 36.529503f},
    {-1565.980835f, 1414.078735f, 36.639523f},
    {-1533.081299f, 1447.566528f, 36.573128f},
    {-1459.259644f, 1447.721558f, 36.638252f},
    {-1579.594360f, 1340.718140f, 36.529320f},
    {-1574.331299f, 1382.006348f, 36.924595f},
    {-1533.135498f, 1318.141479f, 36.577988f},
    {-1587.891968f, 1355.486084f, 36.529217f},
    {-1490.507812f, 1366.389038f, 36.528957f}
};
static Position aNpcWayp1[14]={
    {-1425.469971f, 1436.560059f, 36.506599f},
    {-1421.184448f, 1409.983032f, 36.784653f},
    {-1392.102905f, 1403.186646f, 36.423553f},
    {-1485.378418f, 1331.381470f, 36.489300f},
    {-1515.93f,     1371.12f,     36.535f},
    {-1560.9f,      1409.65f,     36.6444f},
    {-1537.49f,     1446.15f,     36.572f},
    {-1463.98f,     1446.47f,     36.6447f},
    {-1578.71f,     1335.87f,     36.5351f},
    {-1569.81f,     1383.02f,     36.9232f},
    {-1529.32f,     1320.13f,     36.5852f},
    {-1583.43f,     1356.93f,     36.5342f},
    {-1491.61f,     1371.21f,     36.5382f}
};

static Position aNpcWayp2[14]={
    {-1430.99f, 1436.28f, 35.9021f},
    {-1421.26f, 1417.07f, 35.5563f},
    {-1399.02f, 1402.98f, 35.5561f},
    {-1487.24f, 1338.15f, 35.5563f},
    {-1507.28f, 1370.93f, 35.5559f},
    {-1557.09f, 1409.64f, 35.5561f},
    {-1537.37f,  1439.3f, 35.9428f},
    {-1464.36f, 1436.51f, 35.8217f},
    {-1571.56f, 1335.88f, 35.6347f},
    {-1569.74f, 1389.45f, 36.2662f},
    {-1530.48f, 1327.55f,  35.556f},
    {-1583.63f, 1363.32f, 35.6643f},
    {-1499.58f, 1371.09f, 35.5563f}
};
static Position aNpcWayp3[14]={
    {-1438.7f, 1421.96f, 35.5563f},
    {-1435.26f,  1416.8f, 35.5563f},
    {-1405.71f, 1374.58f, 35.5561f},
    {-1469.41f, 1347.11f, 35.5563f},
    {-1507.08f, 1346.74f, 35.5559f},
    {-1533.0f, 1420.77f, 35.5546f},
    {-1523.91f, 1431.33f, 35.5563f},
    {-1457.38f, 1419.35f, 35.5561f},
    {-1570.9f, 1320.09f, 35.5564f},
    {-1554.69f, 1389.66f, 35.5917f},
    {-1546.91f,  1324.3f,  35.556f},
    {-1571.26f, 1363.71f, 35.6733f},
    {-1505.62f, 1346.96f, 35.5563f}
};

static Position aNpcWayp4[14]={
    {-1436.68f, 1364.87f, 35.5563f},
    {-1435.39f, 1365.52f, 35.5563f},
    {-1428.03f, 1363.38f, 35.5561f},
    {-1442.57f, 1356.91f, 35.5563f},
    {-1535.5f,  1339.6f, 35.5567f},
    {-1513.66f, 1418.35f, 35.5546f},
    {-1510.83f, 1420.87f, 35.5563f},
    {-1438.63f, 1364.84f, 35.5567f},
    {-1584.15f, 1317.07f, 30.4917f},
    {-1534.58f, 1392.73f,  35.557f},
    {-1570.89f, 1317.68f, 35.5558f},
    {-1569.13f, 1336.58f, 35.6914f},
    {-1533.98f,  1338.2f, 35.5563f}
};

class npc_panicked_citizen : public CreatureScript
{
public:
    npc_panicked_citizen() : CreatureScript("npc_panicked_citizen") { }

    struct npc_panicked_citizenAI : public npc_escortAI
    {
        npc_panicked_citizenAI(Creature* pCreature) : npc_escortAI(pCreature){
            me = pCreature;
        }
        int pObj;
        Position *poscc;

        void Reset()
        {
            poscc = new Position();
            poscc->m_positionX = me->GetPositionX();
            poscc->m_positionY = me->GetPositionY();
            poscc->m_positionZ = me->GetPositionZ();
            poscc->m_orientation = me->GetOrientation();
            pObj = GetNpcSpawn(poscc);
            if(pObj<14&&0<=pObj)
            {
                AddWaypoint(1,aNpcWayp1[pObj].GetPositionX(),
                    aNpcWayp1[pObj].GetPositionY(),
                    aNpcWayp1[pObj].GetPositionZ(),
                    1);
                AddWaypoint(2,aNpcWayp2[pObj].GetPositionX(),
                    aNpcWayp2[pObj].GetPositionY(),
                    aNpcWayp2[pObj].GetPositionZ(),
                    500);
                AddWaypoint(3,aNpcWayp3[pObj].GetPositionX(),
                    aNpcWayp3[pObj].GetPositionY(),
                    aNpcWayp3[pObj].GetPositionZ(),
                    1);
                AddWaypoint(4,aNpcWayp4[pObj].GetPositionX(),
                    aNpcWayp4[pObj].GetPositionY(),
                    aNpcWayp4[pObj].GetPositionZ(),
                    1);
            }
        }
        void SetHoldState(bool bOnHold){
            SetEscortPaused(bOnHold);
        }
        void WaypointReached(uint32 uiPointId)
        {
            switch(uiPointId)
            {
            case 1:
                break;
            case 2:
                Talk();
                break;
            case 3:
                break;
            case 4:
                me->DespawnOrUnsummon(100);
                break;
            }
        }
        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);
        }
        void Talk()
        {
            int Random = rand() % 7;
            switch(Random)
            {
            case 0:
                me->MonsterSay(SAY_TEXT1,
                    LANG_UNIVERSAL,0);
                break;
            case 1:
                me->MonsterSay(SAY_TEXT2,
                    LANG_UNIVERSAL,0);
                break;
            case 2:
                me->MonsterSay(SAY_TEXT3,
                    LANG_UNIVERSAL,0);
                break;
            case 3:
                me->MonsterSay(SAY_TEXT4,
                    LANG_UNIVERSAL,0);
                break;
            case 4:
                me->MonsterSay(SAY_TEXT5,
                    LANG_UNIVERSAL,0);
                break;
            case 5:
                me->MonsterSay(SAY_TEXT6,
                    LANG_UNIVERSAL,0);
                break;
            case 6:
                me->MonsterSay(SAY_TEXT7,
                    LANG_UNIVERSAL,0);
                break;
            }
        }
        int GetNpcSpawn(Position *Pox)
        {
            int num=0;
            for(int i=0;i<14;i++)
            {
                if(aSpawnNpc[i].GetPositionX()==Pox->GetPositionX())
                {
                    if(aSpawnNpc[i].GetPositionY()==Pox->GetPositionY())
                    {
                        num=i;
                        break;
                    }
                }
            }
            return num;
        }
    };
    CreatureAI *GetAI(Creature *creature) const
    {
        return new npc_panicked_citizenAI(creature);
    }
};
class   object_merchant_square_door : public GameObjectScript
{
public:
    object_merchant_square_door() : GameObjectScript("object_merchant_square_door")
    { }
    bool OnGossipHello(Player *pPlayer, GameObject *pObject)
    {
        if(pPlayer->GetQuestStatus(14098) == QUEST_STATUS_INCOMPLETE)
        {
            posc = new Position();
            posc->m_positionX = pObject->GetPositionX();
            posc->m_positionY = pObject->GetPositionY();
            posc->m_positionZ = pObject->GetPositionZ();
            posc->m_orientation = pObject->GetOrientation();
            trn = GetNpcSpawn(posc);
            if(pCreature = pPlayer->SummonCreature(NPC_PANICKED_CITIZEN,
                aSpawnNpc[trn].GetPositionX(), 
                aSpawnNpc[trn].GetPositionY(), 
                aSpawnNpc[trn].GetPositionZ(), 
                aSpawnNpc[trn].GetAngle(pPlayer),
                TEMPSUMMON_TIMED_DESPAWN, 7000))
            {
                if (npc_panicked_citizen::npc_panicked_citizenAI* pEscortAI = CAST_AI(npc_panicked_citizen::npc_panicked_citizenAI, pCreature->AI()))
                {
                    pEscortAI->Start(true,true,pPlayer->GetGUID(),0,false,false);
                    pEscortAI->SetDespawnAtEnd(true);
                }
            }
        }
        return false;
    }
    int GetNpcSpawn(Position *Pox)
    {
        int num=0;
        for(int i=0;i<14;i++)
        {
            if(aSpawnGobjects[i].GetPositionX()==Pox->GetPositionX())
            {
                if(aSpawnGobjects[i].GetPositionY()==Pox->GetPositionY())
                {
                    num=i;
                    break;
                }
            }
        }
        return num;
    }
private:
    Position *posc;
    Creature *pCreature;
    int trn;
};


/*#################
## Quest Sacrifices
#################*/

#define SAY_LORD_1     "Let's round up as many of them as we can. Every worgen chasing us is one less worgen chasing the survivors!"
#define YELL_LORD_1    "Come and get us, you motherless beasts!"
#define YELL_LORD_2    "You'll never catch us, you blasted mongrels!"
#define YELL_LORD_3    "Over here, you fleabags!"

enum NPCs
{
    NPC_CABALLO_1         = 44427,
    NPC_CABALLO_2         = 44428,
    NPC_LORD              = 35552,
};

enum PassagId
{
    P_LORD                = 0,
    P_PLAYER              = 1
};

// Crowley's Horse
class npc_crowleyshorse : public CreatureScript
{
public:
    npc_crowleyshorse() : CreatureScript("npc_crowleyshorse") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_crowleyshorseAI(pCreature);
    }
    struct npc_crowleyshorseAI : public npc_escortAI
    {
        npc_crowleyshorseAI(Creature *pCreature) : npc_escortAI(pCreature),vehicle(pCreature->GetVehicleKit())
        {
            ASSERT(vehicle);
            Reset();
        }
        void Reset()
        {
            uiPhaseTimer = 1000;
            vehicle->Install();
            cCreature = vehicle->GetPassenger(P_LORD);
        }

        void EnterCombat(Unit* /*who*/){}
        void JustDied(Unit * /*killer*/){}
        void SpawnTimeRift(uint32 timeRiftID, uint64* guidVector){}
        void SpawnWaveGroup(uint32 waveID, uint64* guidVector){}
        void SetHoldState(bool bOnHold){
            SetEscortPaused(bOnHold);
        }

        void JumpToNextStep(uint32 uiTimer)
        {
            uiPhaseTimer = uiTimer;
            ++uiStep;
        }

        void WaypointReached(uint32 uiPointId)
        {
            Player* pPlayer = GetPlayerForEscort();
            if (!pPlayer)
                return;

            switch(uiPointId)
            {
                case 0:
                    break;
                case 1:
                    //jump
                    break;
                case 2:
                    break;
                case 3:
                    break;
                case 4:
                    break;
                case 5:
                    break;
                case 6:
                    break;
                case 7:
                    break;
                case 8:
                    cCreature->MonsterSay(SAY_LORD_1,LANG_UNIVERSAL,NULL);
                    break;
                case 9:
                    break;
                case 10:
                    break;
                case 11:
                    break;
                case 12:
                    break;
                case 13:
                    break;
                case 14:
                    break;
                case 15:
                    break;
                case 16:
                    break;
                case 17:
                    break;
                case 18: 
                    break;
                case 19:
                    break;
                case 20:
                    break;
                case 21:
                    break;
                case 22:
                    //jump
                    break;
                case 23:
                    break;
                case 24:
                    break;
                case 25:
                    break;
                case 26:
                    break;
                case 27:
                     cCreature->MonsterYell(YELL_LORD_1,LANG_UNIVERSAL,NULL);
                    break;
                case 28:
                    break;
                case 29:
                    break;
                case 30:
                    break;
                case 31:
                    break;
                case 32:
                    break;
                case 33:
                    break;
                case 34:
                    break;
                case 35:
                    break;
                case 36:
                    break;
                case 37:
                    break;
                case 38:
                    break;
                case 39:
                    pPlayer->ExitVehicle();
                    me->DespawnOrUnsummon(100);
                    vehicle->RemoveAllPassengers();
                    cCreature->RemoveFromWorld();
                    break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);
            DoMeleeAttackIfReady();
        }
        private:
        bool bStepping;
        uint32 uiStep;
        uint32 uiPhaseTimer;
        Vehicle *vehicle;
        Unit *cCreature;
    };

};

// Crowley's Horse2
class npc_crowleyshorse2 : public CreatureScript
{
public:
    npc_crowleyshorse2() : CreatureScript("npc_crowleyshorse2") { }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        if(pPlayer->GetQuestStatus(14212) == QUEST_STATUS_INCOMPLETE)
        {
            if(Creature *xCreature = pPlayer->SummonCreature(NPC_CABALLO_2,
                pPlayer->GetPositionX(), 
                pPlayer->GetPositionY(), 
                pPlayer->GetPositionZ(), 
                pPlayer->GetAngle(pPlayer),
                TEMPSUMMON_MANUAL_DESPAWN))
            {
                if (npc_crowleyshorse::npc_crowleyshorseAI* pEscortAI = CAST_AI(npc_crowleyshorse::npc_crowleyshorseAI, xCreature->AI()))
                {
                    pEscortAI->Start(true,true,pPlayer->GetGUID(),0,false,false);
                    pEscortAI->SetDespawnAtEnd(true);
                    pPlayer->EnterVehicle(xCreature, P_PLAYER);
                }
            }
        }
        return true;
    }
};

void AddSC_gilneas_quest_script()
{
    //Quest Evacuate the merchant square
    new object_merchant_square_door();
    new npc_panicked_citizen();
    //Quest Sacrifices
    new npc_crowleyshorse();
    new npc_crowleyshorse2();
}