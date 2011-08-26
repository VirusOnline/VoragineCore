/*
 * Copyright (C) 2008-2010 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Tirisfal_Glades
SD%Complete: 100
SDComment: Quest support: 590, 1819
SDCategory: Tirisfal Glades
EndScriptData */

/* ContentData
npc_calvin_montague
go_mausoleum_door
go_mausoleum_trigger
EndContentData */

#include "ScriptPCH.h"

/*######
## npc_calvin_montague
######*/

enum eCalvin
{
    SAY_COMPLETE        = -1000431,
    SPELL_DRINK         = 2639,                             // possibly not correct spell (but iconId is correct)
    QUEST_590           = 590,
    FACTION_HOSTILE     = 168
};

class npc_calvin_montague : public CreatureScript
{
public:
    npc_calvin_montague() : CreatureScript("npc_calvin_montague") { }

    bool OnQuestAccept(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
    {
        if (pQuest->GetQuestId() == QUEST_590)
        {
            pCreature->setFaction(FACTION_HOSTILE);
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
            CAST_AI(npc_calvin_montague::npc_calvin_montagueAI, pCreature->AI())->AttackStart(pPlayer);
        }
        return true;
    }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_calvin_montagueAI (pCreature);
    }

    struct npc_calvin_montagueAI : public ScriptedAI
    {
        npc_calvin_montagueAI(Creature* pCreature) : ScriptedAI(pCreature) { }

        uint32 m_uiPhase;
        uint32 m_uiPhaseTimer;
        uint64 m_uiPlayerGUID;

        void Reset()
        {
            m_uiPhase = 0;
            m_uiPhaseTimer = 5000;
            m_uiPlayerGUID = 0;

            me->RestoreFaction();

            if (!me->HasFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_OOC_NOT_ATTACKABLE))
                me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_OOC_NOT_ATTACKABLE);
        }

        void EnterCombat(Unit* /*who*/) {}

        void AttackedBy(Unit* pAttacker)
        {
            if (me->getVictim() || me->IsFriendlyTo(pAttacker))
                return;

            AttackStart(pAttacker);
        }

        void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
        {
            if (uiDamage > me->GetHealth() || me->HealthBelowPctDamaged(15, uiDamage))
            {
                uiDamage = 0;

                me->RestoreFaction();
                me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_OOC_NOT_ATTACKABLE);
                me->CombatStop(true);

                m_uiPhase = 1;

                if (pDoneBy->GetTypeId() == TYPEID_PLAYER)
                    m_uiPlayerGUID = pDoneBy->GetGUID();
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (m_uiPhase)
            {
                if (m_uiPhaseTimer <= uiDiff)
                    m_uiPhaseTimer = 7500;
                else
                {
                    m_uiPhaseTimer -= uiDiff;
                    return;
                }

                switch(m_uiPhase)
                {
                    case 1:
                        DoScriptText(SAY_COMPLETE, me);
                        ++m_uiPhase;
                        break;
                    case 2:
                        if (Player *pPlayer = Unit::GetPlayer(*me, m_uiPlayerGUID))
                            pPlayer->AreaExploredOrEventHappens(QUEST_590);

                        DoCast(me, SPELL_DRINK, true);
                        ++m_uiPhase;
                        break;
                    case 3:
                        EnterEvadeMode();
                        break;
                }

                return;
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

};


/*######
## go_mausoleum_door
## go_mausoleum_trigger
######*/

enum eMausoleum
{
    QUEST_ULAG      = 1819,
    NPC_ULAG        = 6390,
    GO_TRIGGER      = 104593,
    GO_DOOR         = 176594
};

class go_mausoleum_door : public GameObjectScript
{
public:
    go_mausoleum_door() : GameObjectScript("go_mausoleum_door") { }

    bool OnGossipHello(Player* pPlayer, GameObject* /*pGo*/)
    {
        if (pPlayer->GetQuestStatus(QUEST_ULAG) != QUEST_STATUS_INCOMPLETE)
            return false;

        if (GameObject* pTrigger = pPlayer->FindNearestGameObject(GO_TRIGGER, 30.0f))
        {
            pTrigger->SetGoState(GO_STATE_READY);
            pPlayer->SummonCreature(NPC_ULAG, 2390.26f, 336.47f, 40.01f, 2.26f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);
            return false;
        }

        return false;
    }

};

class go_mausoleum_trigger : public GameObjectScript
{
public:
    go_mausoleum_trigger() : GameObjectScript("go_mausoleum_trigger") { }

    bool OnGossipHello(Player* pPlayer, GameObject* pGo)
    {
        if (pPlayer->GetQuestStatus(QUEST_ULAG) != QUEST_STATUS_INCOMPLETE)
            return false;

        if (GameObject* pDoor = pPlayer->FindNearestGameObject(GO_DOOR, 30.0f))
        {
            pGo->SetGoState(GO_STATE_ACTIVE);
            pDoor->RemoveFlag(GAMEOBJECT_FLAGS,GO_FLAG_INTERACT_COND);
            return true;
        }

        return false;
    }

};

/*######
## npc_for_wakening_quest
######*/

enum eWakening
{
    QUEST_THE_WAKENING = 24960,
    TEXT_ID_VALDRED_1  = 31031,
    TEXT_ID_VALDRED_2  = 31025,
    TEXT_ID_VALDRED_3  = 31026,
    TEXT_ID_LILIAN_1   = 31027,
    TEXT_ID_LILIAN_2   = 31028,
    TEXT_ID_MARSHAL_1  = 31029,
    TEXT_ID_MARSHAL_2  = 31030,
    ACTION_START       = 1,
    ENTRY_VALDRED      = 49231,
    ENTRY_LILIAN       = 38895,
    ENTRY_MARSHAL      = 49230,
};

enum Events
{
    EVENT_START_VALDRED = 1,
    EVENT_START_LILIAN,
    EVENT_START_MARSHAL,
    EVENT_START_VALDRED_1,
    EVENT_START_VALDRED_2,
    EVENT_START_VALDRED_3,
    EVENT_START_VALDRED_4,
    EVENT_START_VALDRED_5,
    EVENT_START_LILIAN_1,
    EVENT_START_LILIAN_2,
    EVENT_START_MARSHAL_1,
    EVENT_START_MARSHAL_2,
};

#define GOSSIP_VALDRED_1 "Don't you remember? You died."
#define GOSSIP_VALDRED_2 "Calm down, Valdred. Undertaker Morodo probably sewed some news on for you."
#define GOSSIP_VALDRED_3 "You talk to Undertaker Morodo. He'll tell you what to do. That's all I know."
#define GOSSIP_LILIAN_1 "I'm not an abomination. I'm simply undead. I just want to speak with you."
#define GOSSIP_LILIAN_2 "Lilian, do you realize that you are undead yourself?"
#define GOSSIP_MARSHAL_1 "I'm not here to fight. I've only been asked to speak with you."
#define GOSSIP_MARSHAL_2 "You are free to do whatever you like."

Player* player = NULL;

class npc_for_wakening_quest : public CreatureScript
{
public:
    npc_for_wakening_quest() : CreatureScript("npc_for_wakening_quest") {}

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        uint32 Entry = pCreature->GetEntry();
        switch (uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                switch (Entry)
                {
                case ENTRY_VALDRED:
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_VALDRED_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_VALDRED_2, pCreature->GetGUID());
                    break;
                case ENTRY_LILIAN:
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LILIAN_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_LILIAN_2, pCreature->GetGUID());
                    break;
                case ENTRY_MARSHAL:
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_MARSHAL_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_MARSHAL_2, pCreature->GetGUID());
                    break;
                }
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                switch (Entry)
                {
                case ENTRY_VALDRED:
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_VALDRED_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
                    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_VALDRED_3, pCreature->GetGUID());
                    break;
                case ENTRY_LILIAN:
                    pPlayer->CLOSE_GOSSIP_MENU();
                    if (pCreature->AI())
                        pCreature->AI()->DoAction(ACTION_START);
                    pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    player = pPlayer;
                    break;
                case ENTRY_MARSHAL:
                    pPlayer->CLOSE_GOSSIP_MENU();
                    if (pCreature->AI())
                        pCreature->AI()->DoAction(ACTION_START);
                    pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    player = pPlayer;
                    break;
                }
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                pPlayer->CLOSE_GOSSIP_MENU();
                if (pCreature->AI())
                    pCreature->AI()->DoAction(ACTION_START);
                pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                player = pPlayer;
                break;
        }

        return true;
    }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        if (pCreature->isQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetGUID());

        QuestStatus status = pPlayer->GetQuestStatus(QUEST_THE_WAKENING);
        uint32 Entry = pCreature->GetEntry();
        uint32 RealTextID;
        const char* REAL_GOSSIP;
        switch (Entry)
        {
        case ENTRY_VALDRED:
            RealTextID = TEXT_ID_VALDRED_1;
            REAL_GOSSIP = GOSSIP_VALDRED_1;
            break;
        case ENTRY_LILIAN:
            RealTextID = TEXT_ID_LILIAN_1;
            REAL_GOSSIP = GOSSIP_LILIAN_1;
            break;
        case ENTRY_MARSHAL:
            RealTextID = TEXT_ID_MARSHAL_1;
            REAL_GOSSIP = GOSSIP_MARSHAL_1;
            break;
        }
        if (status == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, REAL_GOSSIP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        pPlayer->SEND_GOSSIP_MENU(RealTextID, pCreature->GetGUID());

        return true;
    }

    struct npc_for_wakening_questAI : public ScriptedAI
    {
        npc_for_wakening_questAI(Creature* pCreature) : ScriptedAI(pCreature) {}

        EventMap events;
        bool Valdred;
        bool Lilian;
        bool Marshal;
        uint32 uiTimer;
        uint8 uiPhase;

        void Reset()
        {
            events.Reset();
            Valdred = false;
            Lilian = false;
            Marshal = false;
            uiTimer = 0;
            uiPhase = 0;
        }

        void DoAction(const int32 actionId)
        {
            switch(actionId)
            {
            case ACTION_START:
                switch (me->GetEntry())
                {
                case ENTRY_VALDRED:
                    Valdred = true;
                    break;
                case ENTRY_LILIAN:
                    Lilian = true;
                    break;
                case ENTRY_MARSHAL:
                    Marshal = true;
                    break;
                }
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (uiTimer <= diff)
            {
                switch (uiPhase)
                {
                case 0:
                    if (Valdred == true)
                    {
                        uiPhase = EVENT_START_VALDRED;
                        uiTimer = 0;
                        Valdred = false;
                    }
                    if (Lilian == true)
                    {
                        uiPhase = EVENT_START_LILIAN;
                        uiTimer = 0;
                        Lilian = false;
                    }
                    if (Marshal == true)
                    {
                        uiPhase = EVENT_START_MARSHAL;
                        uiTimer = 0;
                        Marshal = false;
                    }
                    break;
                case EVENT_START_VALDRED:
                    me->MonsterSay("I see. Well then, let's get to work, Presea! The Dark Lady needs us, right?", LANG_UNIVERSAL, 0);
                    if (player)
                        me->SetOrientation(me->GetAngle(player));
                    me->SendMovementFlagUpdate();
                    uiPhase = EVENT_START_VALDRED_1;
                    uiTimer = 500;
                    break;
                case EVENT_START_VALDRED_1:
                    if (player)
                        player->KilledMonsterCredit(ENTRY_VALDRED, 0);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_NONE);
                    uiPhase = EVENT_START_VALDRED_2;
                    uiTimer = 1500;
                    break;
                case EVENT_START_VALDRED_2:
                    me->GetMotionMaster()->MovePath(ENTRY_VALDRED, false);
                    uiPhase = EVENT_START_VALDRED_3;
                    uiTimer = 5000;
                    break;
                case EVENT_START_VALDRED_3:
                    if (me->GetDistance2d(1692.620117f, 1675.563477f) <= 1)
                    {
                        uiPhase = EVENT_START_VALDRED_4;
                        uiTimer = 1000;
                    }
                    break;
                case EVENT_START_VALDRED_4:
                    me->MonsterYell("Valdred Moray, reporting for duty, sir!", LANG_UNIVERSAL, 0);
                    uiPhase = EVENT_START_VALDRED_5;
                    uiTimer = 1000;
                    break;
                case EVENT_START_VALDRED_5:
                    me->DespawnOrUnsummon(0);
                    uiPhase = 0;
                    uiTimer = 0;
                    break;

                case EVENT_START_LILIAN:
                    me->MonsterSay("No. You're lying! My father will protect me!", LANG_UNIVERSAL, 0);
                    if (player)
                        me->SetOrientation(me->GetAngle(player));
                    me->SendMovementFlagUpdate();
                    uiPhase = EVENT_START_LILIAN_1;
                    uiTimer = 500;
                    break;
                case EVENT_START_LILIAN_1:
                    if (player)
                        player->KilledMonsterCredit(ENTRY_LILIAN, 0);
                    uiPhase = EVENT_START_LILIAN_2;
                    uiTimer = 1500;
                    break;
                case EVENT_START_LILIAN_2:
                    me->HandleEmoteCommand(EMOTE_STATE_COWER);
                    me->GetMotionMaster()->MovePath(ENTRY_LILIAN, false);
                    me->DespawnOrUnsummon(10000);
                    uiPhase = 0;
                    uiTimer = 0;
                    break;
                case EVENT_START_MARSHAL:
                    me->MonsterSay("Who are you calling a monster? You're the monster! I'm just a man who died.", LANG_UNIVERSAL, 0);
                    if (player)
                        me->SetOrientation(me->GetAngle(player));
                    me->SendMovementFlagUpdate();
                    uiPhase = EVENT_START_MARSHAL_1;
                    uiTimer = 500;
                    break;
                case EVENT_START_MARSHAL_1:
                    if (player)
                        player->KilledMonsterCredit(ENTRY_MARSHAL, 0);
                    uiPhase = EVENT_START_MARSHAL_2;
                    uiTimer = 1500;
                    break;
                case EVENT_START_MARSHAL_2:
                    me->GetMotionMaster()->MovePath(ENTRY_MARSHAL, false);
                    me->DespawnOrUnsummon(8000);
                    uiPhase = 0;
                    uiTimer = 0;
                    break;
                }
            }
            else uiTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }

    };

    CreatureAI *GetAI(Creature *pCreature) const
    {
        return new npc_for_wakening_questAI(pCreature);
    }
};

/*######
## npc_darnell
######*/

enum eDarnell
{
    QUEST_SHADOW_GRAVE = 28608,
    QUEST_BEYOND_GRAVES= 25089,
    QUEST_RECRUITMENT  = 26800,
    NPC_MORDO          = 1568,
    NPC_DARNELL_2      = 49141,
    SAY_GREETINGS      = -1000022,
    SAY_1              = -1000023,
    SAY_2              = -1000024,
    SAY_3              = -1000025,
    SAY_4              = -1000026,
    SAY_5              = -1000027,
    SAY_6              = -1000028,
    SAY_7              = -1000029,
    SAY_COMPLETE_1     = -1000030,
    SAY_COMPLETE_2     = -1000031,
    SAY_HELLO          = -1000032,
    SAY_8              = -1000033,
    SAY_9              = -1000034,
    SAY_10             = -1000035,
    SAY_11             = -1000036,
    SAY_12             = -1000037,
    FIRST_DARNELL_PATH         = 49337,
    SECOND_DARNELL_PATH        = 49338,
    THIRD_DARNELL_PATH         = 49339,
};

class npc_darnell : public CreatureScript
{
public:
    npc_darnell() : CreatureScript("npc_darnell") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_darnellAI (pCreature);
    }

    struct npc_darnellAI : public ScriptedAI
    {
        npc_darnellAI(Creature* pCreature) : ScriptedAI(pCreature) {}

        bool Spawned;
        bool MordoEmote;
        bool Next;
        bool Next2;
        bool NewPath;
        uint32 uiDespawnTimer;
        uint32 uiSpawnedTimer;
        uint8 uiSpawnedPhase;
        uint32 uiTimer;
        uint8 uiPhase;
        uint32 uiNewPathTimer;
        uint8 uiNewPathPhase;
        uint32 uiNext2Timer;
        uint8 uiNext2Phase;

        void Reset()
        {
            Spawned = true;
            MordoEmote = false;
            Next = false;
            Next2 = false;
            NewPath = false;
            uiDespawnTimer = 500;
            uiSpawnedTimer = 0;
            uiSpawnedPhase = 0;
            uiTimer = 0;
            uiPhase = 0;
            uiNewPathTimer = 0;
            uiNewPathPhase = 0;
            uiNext2Timer = 0;
            uiNext2Phase = 0;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (me->GetOwner())
                if (me->GetOwner()->GetTypeId() == TYPEID_PLAYER)
                {
                    Player* player = me->GetOwner()->ToPlayer();
                    QuestStatus status = player->GetQuestStatus(QUEST_SHADOW_GRAVE);
                    QuestStatus status2 = player->GetQuestStatus(QUEST_BEYOND_GRAVES);
                    if (status2 == QUEST_STATUS_INCOMPLETE)
                        me->UpdateEntry(NPC_DARNELL_2);
                    if (status == QUEST_STATUS_REWARDED || (status != QUEST_STATUS_INCOMPLETE && status == QUEST_STATUS_REWARDED))
                    {
                        if (MordoEmote == false)
                        {
                            Creature* Mordo = me->FindNearestCreature(NPC_MORDO, 10, true);
                            if (Mordo)
                                Mordo->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                            MordoEmote = true;
                        }
                        if (uiDespawnTimer <= uiDiff)
                        {
                            me->DespawnOrUnsummon(0);
                            uiDespawnTimer = 2000;
                        }
                        else uiDespawnTimer -= uiDiff;
                    }
                }

            if (Spawned == true)
            {
                if (uiSpawnedTimer <= uiDiff)
                {
                    switch (uiSpawnedPhase)
                    {
                    case 0:
                        // The mob must stop here. (NEED A FLAG OR SOMETHING ELSE, BY THE WAY UNIT_FLAG_DISABLE_MOVE ISN'T WORKING I GUESS)
                        ++uiSpawnedPhase;
                        uiSpawnedTimer = 1000;
                        break;
                    case 1:
                        if (me->GetOwner())
                            if (me->GetOwner()->GetTypeId() == TYPEID_PLAYER)
                            {
                                Player* pPlayer = me->GetOwner()->ToPlayer();
                                me->MonsterSay(SAY_GREETINGS, LANG_UNIVERSAL, pPlayer->GetGUID());
                            }
                        // The mob can move here. (NEED A REMOVEFLAG OR SOMETHING ELSE, BY THE WAY UNIT_FLAG_DISABLE_MOVE ISN'T WORKING I GUESS)
                        me->GetMotionMaster()->MoveFollow(me->GetOwner(), 2, 360);
                        Next = true;
                        Spawned = false;
                        ++uiSpawnedPhase;
                        uiSpawnedTimer = 1000;
                        break;
                    }
                }
                else uiSpawnedTimer -= uiDiff;
            }

            if (Next == true)
            {
                if (uiTimer <= uiDiff)
                {
                    switch (uiPhase)
                    {
                    case 0:
                        if (me->GetDistance2d(1665.450928f, 1662.900879f) < 6 || me->GetDistance2d(1673.012085f, 1663.741211f) < 6 || me->GetDistance2d(1679.169067f, 1665.848511f) < 6 || me->GetDistance2d(1685.645630f, 1663.395508f) < 6 || me->GetDistance2d(1693.048340f, 1660.157593f) < 6)
                        {
                            if (me->GetOwner())
                                if (me->GetOwner()->GetTypeId() == TYPEID_PLAYER)
                                {
                                    Player* pPlayer = me->GetOwner()->ToPlayer();
                                    me->MonsterSay(SAY_1, LANG_UNIVERSAL, pPlayer->GetGUID());
                                }
                            me->GetMotionMaster()->MovePoint(0, 1662.562988f, 1662.813477f, 141.851440f);
                            ++uiPhase;
                            uiTimer = 500;
                        }
                        break;
                    case 1:
                        if (me->GetDistance2d(1662.562988f, 1662.813477f) < 1 && me->IsStopped())
                        {
                            me->SetOrientation(6.227751f);
                            me->SendMovementFlagUpdate();
                            ++uiPhase;
                            uiTimer = 500;
                        }
                        break;
                    case 2:
                        if (me->GetOwner())
                            if (me->GetOwner()->GetDistance2d(me->GetPositionX(), me->GetPositionY()) < 1)
                            {
                                ++uiPhase;
                                uiTimer = 0;
                            }
                        break;
                    case 3:
                        me->GetMotionMaster()->MovePath(FIRST_DARNELL_PATH, false);
                        ++uiPhase;
                        uiTimer = 7000;
                        break;
                    case 4:
                        Next = false;
                        NewPath = true;
                        break;
                    }
                }
                else uiTimer -= uiDiff;
            }

            if (NewPath == true)
            {
                if (uiNewPathTimer <= uiDiff)
                {
                    switch (uiNewPathPhase)
                    {
                    case 0:
                        me->GetMotionMaster()->MovePath(SECOND_DARNELL_PATH, false);
                        ++uiNewPathPhase;
                        uiNewPathTimer = 2500;
                        break;
                    case 1:
                        if (me->GetOwner())
                            if (me->GetOwner()->GetTypeId() == TYPEID_PLAYER)
                            {
                                Player* pPlayer = me->GetOwner()->ToPlayer();
                                me->MonsterSay(RAND(SAY_2, SAY_3, SAY_4), LANG_UNIVERSAL, pPlayer->GetGUID());
                            }
                        ++uiNewPathPhase;
                        uiNewPathTimer = 12000;
                        break;
                    case 2:
                        if (me->GetOwner())
                            if (me->GetOwner()->GetTypeId() == TYPEID_PLAYER)
                            {
                                Player* pPlayer = me->GetOwner()->ToPlayer();
                                me->MonsterSay(RAND(SAY_5, SAY_6, SAY_7), LANG_UNIVERSAL, pPlayer->GetGUID());
                            }
                        ++uiNewPathPhase;
                        uiNewPathTimer = 15500;
                        break;
                    case 3:
                        me->GetMotionMaster()->MovePath(49338, false);
                        uiNewPathPhase = 1;
                        uiNewPathTimer = 4500;
                        break;

                    }
                }
                else uiNewPathTimer -= uiDiff;

                if (me->GetOwner())
                    if (me->GetOwner()->GetTypeId() == TYPEID_PLAYER)
                    {
                        Player* player = me->GetOwner()->ToPlayer();
                        QuestStatus status = player->GetQuestStatus(QUEST_SHADOW_GRAVE);
                        if (status == QUEST_STATUS_COMPLETE)
                        {
                            NewPath = false;
                            Next2 = true;
                        }
                    }
            }

            if (Next2 == true)
            {
                if (uiNext2Timer <= uiDiff)
                {
                    switch (uiNext2Phase)
                    {
                    case 0:
                        me->GetMotionMaster()->MoveFollow(me->GetOwner(), 2, 360);
                        me->MonsterSay(SAY_COMPLETE_1, LANG_UNIVERSAL, 0);
                        ++uiNext2Phase;
                        uiNext2Timer = 3000;
                        break;
                    case 1:
                        me->MonsterSay(SAY_COMPLETE_2, LANG_UNIVERSAL, 0);
                        ++uiNext2Phase;
                        uiNext2Timer = 1000;
                        break;
                    }
                }
                else uiNext2Timer -= uiDiff;
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

/*######
## npc_darnell2
######*/

class npc_darnell2 : public CreatureScript
{
public:
    npc_darnell2() : CreatureScript("npc_darnell2") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_darnell2AI (pCreature);
    }

    struct npc_darnell2AI : public ScriptedAI
    {
        npc_darnell2AI(Creature* pCreature) : ScriptedAI(pCreature) {}

        bool Next;
        bool Spawned;
        bool Recruitment;
        uint32 uiSpawnedTimer;
        uint8 uiSpawnedPhase;
        uint32 uiDespawnTimer;
        uint32 uiTimer;
        uint8 uiPhase;

        void Reset()
        {
            Spawned = true;
            Next = false;
            Recruitment = false;
            uiSpawnedTimer = 0;
            uiSpawnedPhase = 0;
            uiDespawnTimer = 0;
            uiTimer = 0;
            uiPhase = 0;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (me->GetOwner())
                if (me->GetOwner()->GetTypeId() == TYPEID_PLAYER)
                {
                    Player* player = me->GetOwner()->ToPlayer();
                    QuestStatus status = player->GetQuestStatus(QUEST_BEYOND_GRAVES);
                    QuestStatus status2 = player->GetQuestStatus(QUEST_RECRUITMENT);
                    if (status == QUEST_STATUS_REWARDED && status2 == QUEST_STATUS_INCOMPLETE)
                        Recruitment = true;
                    if (status2 != QUEST_STATUS_INCOMPLETE && status2 == QUEST_STATUS_REWARDED)
                        Recruitment = false;

                    if (Recruitment == false)
                        if (status != QUEST_STATUS_INCOMPLETE && status == QUEST_STATUS_REWARDED)
                        {
                            if (uiDespawnTimer <= uiDiff)
                            {
                                me->DespawnOrUnsummon(0);
                                uiDespawnTimer = 2000;
                            }
                            else uiDespawnTimer -= uiDiff;
                        }
                }

            if (Spawned == true)
            {
                if (uiSpawnedTimer <= uiDiff)
                {
                    switch (uiSpawnedPhase)
                    {
                    case 0:
                        // The mob must stop here. (NEED A FLAG OR SOMETHING ELSE, BY THE WAY UNIT_FLAG_DISABLE_MOVE ISN'T WORKING I GUESS)
                        ++uiSpawnedPhase;
                        uiSpawnedTimer = 1000;
                        break;
                    case 1:
                        me->MonsterSay(SAY_HELLO, LANG_UNIVERSAL, 0);
                        me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
                        // The mob can move here. (NEED A REMOVEFLAG OR SOMETHING ELSE, BY THE WAY UNIT_FLAG_DISABLE_MOVE ISN'T WORKING I GUESS)
                        me->GetMotionMaster()->MoveFollow(me->GetOwner(), 2, 360);
                        Next = true;
                        Spawned = false;
                        ++uiSpawnedPhase;
                        uiSpawnedTimer = 1000;
                        break;
                    }
                }
                else uiSpawnedTimer -= uiDiff;
            }

            if (Next == true)
            {
                if (Recruitment == false)
                {
                    if (uiTimer <= uiDiff)
                    {
                        switch (uiPhase)
                        {
                        case 0:
                            if (me->GetDistance2d(1712.541138f, 1639.811035f) <= 15)
                            {
                                me->MonsterSay(SAY_8, LANG_UNIVERSAL, 0);
                                me->GetMotionMaster()->MovePoint(0, 1723.532349f, 1631.841553f, 119.999260f);
                                ++uiPhase;
                                uiTimer = 0;
                            }
                            break;
                        case 1:
                            if (me->GetDistance2d(1723.532349f, 1631.841553f) <= 1)
                            {
                                me->GetMotionMaster()->MovePoint(0, 1733.724731f, 1626.103149f, 117.776321f);
                                ++uiPhase;
                                uiTimer = 0;
                            }
                            break;
                        case 2:
                            if (me->GetDistance2d(1733.724731f, 1626.103149f) <= 1)
                            {
                                me->GetMotionMaster()->MovePoint(0, 1762.803833f, 1602.697021f, 110.071106f);
                                ++uiPhase;
                                uiTimer = 0;
                            }
                            break;
                        case 3:
                            if (me->GetDistance2d(1762.803833f, 1602.697021f) <= 1)
                            {
                                me->GetMotionMaster()->MovePoint(0, 1789.149292f, 1593.908325f, 103.833618f);
                                ++uiPhase;
                                uiTimer = 0;
                            }
                            break;
                        case 4:
                            if (me->GetDistance2d(1789.149292f, 1593.908325f) <= 1)
                            {
                                me->GetMotionMaster()->MovePoint(0, 1817.585815f, 1589.575317f, 96.273346f);
                                ++uiPhase;
                                uiTimer = 0;
                            }
                            break;
                        case 5:
                            if (me->GetDistance2d(1817.585815f, 1589.575317f) <= 1)
                            {
                                me->SetOrientation(2.977333f);
                                me->SendMovementFlagUpdate();
                                ++uiPhase;
                                uiTimer = 500;
                            }
                            break;
                        case 6:
                            if (me->GetOwner())
                                if (me->GetOwner()->GetDistance2d(me->GetPositionX(), me->GetPositionY()) < 15)
                                {
                                    me->MonsterSay(SAY_9, LANG_UNIVERSAL, 0);
                                    ++uiPhase;
                                    uiTimer = 0;
                                }
                            break;
                        case 7:
                            me->GetMotionMaster()->MovePoint(0, 1857.578369f, 1600.913696f, 94.799179f);
                            ++uiPhase;
                            uiTimer = 6000;
                            break;
                        case 8:
                            me->MonsterSay(SAY_10, LANG_UNIVERSAL, 0);
                            me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                            ++uiPhase;
                            uiTimer = 6000;
                            break;
                        case 9:
                            me->GetMotionMaster()->MoveFollow(me->GetOwner(), 2, 360);
                            Spawned = true;
                            Next = false;
                            ++uiPhase;
                            uiTimer = 0;
                            break;
                        }
                    }
                    else uiTimer -= uiDiff;
                }
                else
                {
                    if (uiTimer <= uiDiff)
                    {
                        switch (uiPhase)
                        {
                        case 0:
                            if (me->GetDistance2d(1883.312256f, 1587.775879f) <= 10)
                            {
                                me->MonsterSay(SAY_11, LANG_UNIVERSAL, me->GetOwner()->ToPlayer()->GetGUID());
                                me->GetMotionMaster()->MovePoint(0, 1895.683594f, 1587.493164f, 87.803123f);
                                ++uiPhase;
                                uiTimer = 0;
                            }
                            break;
                        case 1:
                            if (me->GetDistance2d(1895.683594f, 1587.493164f) <= 1)
                            {
                                ++uiPhase;
                                uiTimer = 500;
                            }
                            break;
                        case 2:
                            me->MonsterSay(SAY_12, LANG_UNIVERSAL, me->GetOwner()->ToPlayer()->GetGUID());
                            ++uiPhase;
                            uiTimer = 5500;
                            break;
                        case 3:
                            me->GetMotionMaster()->MoveFollow(me->GetOwner(), 2, 360);
                            ++uiPhase;
                            uiTimer = 0;
                            break;
                        }
                    }
                    else uiTimer -= uiDiff;
                }
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

/*######
## npc_deathguard_oliver
######*/

enum eOliver
{
    FIRST_OLIVER_PATH  = 1737,
    SECONDD_OLIVER_PATH = 1738,
};

class npc_deathguard_oliver : public CreatureScript
{
public:
    npc_deathguard_oliver() : CreatureScript("npc_deathguard_oliver") {}

    struct npc_deathguard_oliverAI : public ScriptedAI
    {
        npc_deathguard_oliverAI(Creature* pCreature) : ScriptedAI(pCreature) {}

        uint32 uiTimer;
        uint8 uiPhase;

        void Reset()
        {
            uiTimer = 0;
            uiPhase = 0;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!me->isInCombat())
                if (uiTimer <= diff)
                {
                    switch (uiPhase)
                    {
                    case 0:
                        me->GetMotionMaster()->MovePath(FIRST_OLIVER_PATH, false);
                        ++uiPhase;
                        uiTimer = 10000;
                        break;
                    case 1:
                        if (me->GetDistance2d(1827.560669f, 1596.134033f) <= 1)
                        {
                            ++uiPhase;
                            uiTimer = 0;
                        }
                        break;
                    case 2:
                        me->GetMotionMaster()->MoveRandom(5.0f);
                        ++uiPhase;
                        uiTimer = 17000;
                        break;
                    case 3:
                        me->GetMotionMaster()->MovePath(SECONDD_OLIVER_PATH, false);
                        ++uiPhase;
                        uiTimer = 5000;
                        break;
                    case 4:
                        if (me->GetDistance2d(1807.130005f, 1605.920044f) <= 1)
                        {
                            uiPhase = 0;
                            uiTimer = 0;
                        }
                        break;
                    }
                }
                else uiTimer -= diff;
        }
    };

    CreatureAI *GetAI(Creature *pCreature) const
    {
        return new npc_deathguard_oliverAI(pCreature);
    }
};

/*######
## npc_aradne
######*/

enum eAradne
{
    VALKYR_RESURRECTION = 73524,
    ARADNE_PATH         = 50372,
    NPC_RISEN_DEAD      = 50374,
    SAY_RESURRECTION_1  = -1000038,
    SAY_RESURRECTION_2  = -1000039,
    SAY_RESURRECTION_3  = -1000040,
    SAY_RESURRECTION_4  = -1000041,
    SAY_RESURRECTION_5  = -1000042,
};

Position FirstPos   = {1707.505493f, 1703.844482f, 134.933426f, 4.633843f};
Position SecondPos  = {1696.426392f, 1688.603271f, 135.187225f, 6.243160f};
Position ThirdPos   = {1695.348267f, 1681.100342f, 134.929855f, 6.059628f};
Position FourthPos  = {1682.933350f, 1687.017334f, 137.869354f, 5.974266f};

class npc_aradne : public CreatureScript
{
public:
    npc_aradne() : CreatureScript("npc_aradne") {}

    struct npc_aradneAI : public ScriptedAI
    {
        npc_aradneAI(Creature* pCreature) : ScriptedAI(pCreature) 
        {
            me->GetMotionMaster()->MovePath(ARADNE_PATH, true);
            me->SetSpeed(MOVE_WALK, 0.8f);
        }

        bool Action;
        bool First;
        bool Second;
        bool Third;
        bool Fourth;
        Creature* RisenDead;
        Position RealPos;
        uint32 uiTimer;
        uint8 uiPhase;

        void Reset()
        {
            Action = false;
            First = false;
            Second = false;
            Third = false;
            Fourth = false;
            uiTimer = 0;
            uiPhase = 0;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!me->isMoving() && Action == false)
            {
                if (me->GetDistance2d(1707.439941f, 1700.439941f) <= 1)
                {
                    RealPos.m_positionX = FirstPos.GetPositionX();
                    RealPos.m_positionY = FirstPos.GetPositionY();
                    RealPos.m_positionZ = FirstPos.GetPositionZ();
                    RealPos.m_orientation = FirstPos.GetOrientation();
                    uiTimer = 500;
                    First = true;
                    Action = true;
                }
                if (me->GetDistance2d(1698.250000f, 1689.310059f) <= 1)
                {
                    RealPos.m_positionX = SecondPos.GetPositionX();
                    RealPos.m_positionY = SecondPos.GetPositionY();
                    RealPos.m_positionZ = SecondPos.GetPositionZ();
                    RealPos.m_orientation = SecondPos.GetOrientation();
                    uiTimer = 500;
                    Second = true;
                    Action = true;
                }
                if (me->GetDistance2d(1697.109985f, 1681.760010f) <= 1 && me->GetPositionZ() < 138.0f)
                {
                    RealPos.m_positionX = ThirdPos.GetPositionX();
                    RealPos.m_positionY = ThirdPos.GetPositionY();
                    RealPos.m_positionZ = ThirdPos.GetPositionZ();
                    RealPos.m_orientation = ThirdPos.GetOrientation();
                    uiTimer = 500;
                    Third = true;
                    Action = true;
                }
                if (me->GetDistance2d(1685.013672f, 1685.938232f) <= 1)
                {
                    RealPos.m_positionX = FourthPos.GetPositionX();
                    RealPos.m_positionY = FourthPos.GetPositionY();
                    RealPos.m_positionZ = FourthPos.GetPositionZ();
                    RealPos.m_orientation = FourthPos.GetOrientation();
                    uiTimer = 500;
                    Fourth = true;
                    Action = true;
                }
            }

            if (Action == true)
            {
                if (uiTimer <= diff)
                {
                    switch (uiPhase)
                    {
                    case 0:
                        ++uiPhase;
                        if (First == true)
                            me->SetOrientation(1.460806f);
                        if (Second == true)
                            me->SetOrientation(2.847034f);
                        if (Third == true)
                            me->SetOrientation(2.878449f);
                        if (Fourth == true)
                            me->SetOrientation(2.741005f);
                        me->SendMovementFlagUpdate();
                        me->SendMonsterStop(false);
                        ++uiPhase;
                        uiTimer = 1500;
                        break;
                    case 1:
                        ++uiPhase;
                        uiTimer = 1000;
                        break;
                    case 2:
                        me->CastSpell(RealPos.GetPositionX(), RealPos.GetPositionY(), RealPos.GetPositionZ(), VALKYR_RESURRECTION, false);
                        ++uiPhase;
                        uiTimer = 3000;
                        break;
                    case 3:
                        DoScriptText(RAND(SAY_RESURRECTION_1, SAY_RESURRECTION_2, SAY_RESURRECTION_3, SAY_RESURRECTION_4, SAY_RESURRECTION_5), me);
                        me->SummonCreature(NPC_RISEN_DEAD, RealPos.GetPositionX(), RealPos.GetPositionY(), RealPos.GetPositionZ(), RealPos.GetOrientation());
                        ++uiPhase;
                        if (First == true)
                            uiTimer = 12000;
                        if (Second == true)
                            uiTimer = 7000;
                        if (Third == true)
                            uiTimer = 7000;
                        if (Fourth == true)
                            uiTimer = 6000;
                        break;
                    case 4:
                        First = false;
                        Second = false;
                        Third = false;
                        Fourth = false;
                        uiPhase = 0;
                        Action = false;
                        uiTimer = 0;
                        break;
                    }
                }else uiTimer -= diff;
            }
            
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI *GetAI(Creature *pCreature) const
    {
        return new npc_aradneAI(pCreature);
    }
};

/*######
## npc_risen_dead
######*/

enum eRisen
{
    SPELL_ENRAGE        = 44427,
    MODEL_ZOMBIE        = 10973,
    SAY_FIRST_ACTION_1  = -1000043,
    SAY_FIRST_ACTION_2  = -1000044,
    SAY_SECOND_ACTION_1 = -1000045,
    SAY_SECOND_ACTION_2 = -1000046,
    SAY_SECOND_ACTION_3 = -1000047,
    SAY_SECOND_ACTION_4 = -1000048,
    SAY_SECOND_ACTION_5 = -1000049,
    SAY_THIRD_ACTION_1  = -1000050,
    PATH_FIRST          = 50373,
    PATH_SECOND         = 50374,
    ACTION_FIRST        = 3,
    ACTION_SECOND       = 4,
    ACTION_SECOND_2     = 5,
    ACTION_SECOND_3     = 6,
    ACTION_THIRD        = 7,
    ACTION_THIRD_2      = 8,
    ACTION_THIRD_3      = 9,
    ACTION_END          = 10,
};

class npc_risen_dead : public CreatureScript
{
public:
    npc_risen_dead() : CreatureScript("npc_risen_dead") {}

    struct npc_risen_deadAI : public ScriptedAI
    {
        npc_risen_deadAI(Creature* pCreature) : ScriptedAI(pCreature) 
        {
            Random = urand(0,2);
            me->ForcedDespawn(35000);
        }

        uint32 Random;
        uint32 uiTimer;
        uint8 uiPhase;

        void Reset()
        {
            uiTimer = 0;
            uiPhase = 0;
        }

        void UpdateAI(const uint32 diff)
        {
            if (uiTimer <= diff)
            {
                switch (uiPhase)
                {
                case 0:
                    ++uiPhase;
                    uiTimer = 1000;
                    break;
                case 1:
                    if (Random == 0)
                        DoScriptText(SAY_FIRST_ACTION_1, me);
                    if (Random == 1)
                        DoScriptText(RAND(SAY_SECOND_ACTION_1, SAY_SECOND_ACTION_2, SAY_SECOND_ACTION_3, SAY_SECOND_ACTION_4), me);
                    if (Random == 2)
                    {
                        uiPhase = ACTION_THIRD;
                        DoScriptText(SAY_THIRD_ACTION_1, me);
                        uiTimer = 3000;
                    }
                    else
                    {
                        ++uiPhase;
                        uiTimer = 4000;
                    }
                    break;
                case 2:
                    if (Random == 0)
                    {
                        DoScriptText(SAY_FIRST_ACTION_2, me);
                        uiPhase = ACTION_FIRST;
                        uiTimer = 3000;
                    }
                    if (Random == 1)
                    {
                        DoScriptText(SAY_SECOND_ACTION_5, me);
                        uiPhase = ACTION_SECOND;
                        uiTimer = 4000;
                    }
                    break;
                case ACTION_FIRST:
                    me->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_DEAD);
                    me->ForcedDespawn(14000);
                    uiPhase = ACTION_END;
                    uiTimer = 0;
                    break;
                case ACTION_SECOND:
                    if (me->GetDistance2d(1707.505493f, 1703.844482f) <= 1)
                    {
                        me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                        me->GetMotionMaster()->MovePoint(0, 1711.796631f, 1699.298462f, 133.015518f);
                        uiPhase = ACTION_SECOND_3;
                        uiTimer = 0;
                    }
                    else
                    {
                        me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                        me->GetMotionMaster()->MovePoint(0, 1701.912231f, 1685.268311f, 134.184265f);
                        uiPhase = ACTION_SECOND_3;
                        uiTimer = 0;
                    }
                    break;
                case ACTION_SECOND_2:
                    if (me->GetDistance2d(1698.519775f, 1653.410400f) <= 1)
                    {
                        me->ForcedDespawn();
                        uiPhase = ACTION_END;
                        uiTimer = 0;
                    }
                    break;
                case ACTION_SECOND_3:
                    if (me->GetDistance2d(1711.796631f, 1699.298462f) <= 1 || me->GetDistance2d(1701.912231f, 1685.268311f) <= 1)
                    {
                        me->GetMotionMaster()->MovePath(PATH_FIRST, false);
                        uiPhase = ACTION_SECOND_2;
                        uiTimer = 0;
                    }
                    break;
                case ACTION_THIRD:
                    me->CastSpell(me, SPELL_ENRAGE, false);
                    me->SetDisplayId(MODEL_ZOMBIE);
                    if (me->GetDistance2d(1682.736450f, 1687.085083f) <= 1)
                    {
                        me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                        me->GetMotionMaster()->MovePoint(0, 1695.912354f, 1686.218506f, 135.147675f);
                        uiPhase = ACTION_THIRD_2;
                        uiTimer = 1500;
                    }
                    else
                    {
                        me->GetMotionMaster()->MovePath(PATH_SECOND, false);
                        uiPhase = ACTION_THIRD_2;
                        uiTimer = 0;
                    }
                    break;
                case ACTION_THIRD_2:
                    if (me->GetDistance2d(1670.371826f, 1662.767578f) <= 1)
                    {
                        me->ForcedDespawn();
                        uiPhase = ACTION_END;
                        uiTimer = 0;
                    }
                    break;
                case ACTION_THIRD_3:
                    me->GetMotionMaster()->MovePath(PATH_SECOND, false);
                    uiPhase = ACTION_THIRD_2;
                    uiTimer = 0;
                    break;
                case ACTION_END:
                    break;
                }
            }
            else uiTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI *GetAI(Creature *pCreature) const
    {
        return new npc_risen_deadAI(pCreature);
    }
};

void AddSC_tirisfal_glades()
{
    new npc_calvin_montague();
    new go_mausoleum_door();
    new go_mausoleum_trigger();
    new npc_for_wakening_quest();
    new npc_darnell();
    new npc_darnell2();
    new npc_deathguard_oliver();
    new npc_aradne();
    new npc_risen_dead();
}
