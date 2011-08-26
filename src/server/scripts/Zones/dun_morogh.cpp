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

/* ScriptData
SDName: Dun_Morogh
SD%Complete: 50
SDComment: Quest support: 1783
SDCategory: Dun Morogh
EndScriptData */

/* ContentData
npc_narm_faulk
EndContentData */

#include "ScriptPCH.h"

/*######
## npc_narm_faulk
######*/

// signed for 6172
enum eNarmFaulkData
{
    SAY_HEAL = -1000187,
};

class npc_narm_faulk : public CreatureScript
{
public:
    npc_narm_faulk() : CreatureScript("npc_narm_faulk") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_narm_faulkAI (pCreature);
    }

    struct npc_narm_faulkAI : public ScriptedAI
    {
        uint32 lifeTimer;
        bool spellHit;

        npc_narm_faulkAI(Creature *c) : ScriptedAI(c) {}

        void Reset()
        {
            lifeTimer = 120000;
            me->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
            me->SetStandState(UNIT_STAND_STATE_DEAD);
            spellHit = false;
        }

        void EnterCombat(Unit * /*who*/)
        {
        }

        void MoveInLineOfSight(Unit * /*who*/)
        {
            return;
        }

        void UpdateAI(const uint32 diff)
        {
           if (me->IsStandState())
            {
                if (lifeTimer <= diff)
                {
                    EnterEvadeMode();
                    return;
                }
                else
                    lifeTimer -= diff;
            }
        }

        void SpellHit(Unit * /*Hitter*/, const SpellEntry *Spellkind)
        {
            if (Spellkind->Id == 8593 && !spellHit)
            {
                DoCast(me, 32343);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->SetUInt32Value(UNIT_DYNAMIC_FLAGS, 0);
                //me->RemoveAllAuras();
                DoScriptText(SAY_HEAL, me);
                spellHit = true;
            }
        }

    };

};

/*######
## npc_high_tinker_mekkatorque
######*/

enum eHighTinker
{
    ACTION_START              = 0,
    QUEST_THE_FIGHT_CONTINUES = 26208,
    TEXT_ID                   = 31032,
    NPC_IMAGE_1               = 42419,
    NPC_IMAGE_2               = 43131,
    NPC_IMAGE_3               = 42452,
    NPC_IMAGE_4               = 42423,
    NPC_IMAGE_5               = 42441,
    MODEL_IRRADIATOR          = 31878,
    // Need spell visuals(Teleport, explosion and blue thing)
};

static Player* player;

class npc_high_tinker_mekkatorque : public CreatureScript
{
public:
    npc_high_tinker_mekkatorque() : CreatureScript("npc_high_tinker_mekkatorque") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_high_tinker_mekkatorqueAI (pCreature);
    }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        if (pCreature->isQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetGUID());

        pPlayer->SEND_GOSSIP_MENU(TEXT_ID, pCreature->GetGUID());

        return true;
    }

    bool OnQuestAccept(Player* pPlayer, Creature* creature, const Quest * quest)
    {
        if (quest->GetQuestId() == QUEST_THE_FIGHT_CONTINUES)
        {
            player = pPlayer;
            Position pos = creature->GetHomePosition();
            if (creature->AI())
                creature->AI()->DoAction(ACTION_START);
        }

        return true;
    }

    struct npc_high_tinker_mekkatorqueAI : public ScriptedAI
    {
        npc_high_tinker_mekkatorqueAI(Creature *c) : ScriptedAI(c) {}

        uint32 uiTimer;
        uint8 uiPhase;
        Creature* Image[8];
        bool QuestAccepted;

        void Reset()
        {
            uiTimer = 0;
            uiPhase = 0;
            QuestAccepted = false;
            me->SetOrientation(2.303830f);
            me->SendMovementFlagUpdate();
        }

        void DoAction(const int32 actionId)
        {
            switch(actionId)
            {
            case ACTION_START:
                QuestAccepted = true;
                break;
            }
        }

        void Say(Creature* creature, const char *text, uint32 language, uint64 TargetGuid)
        {
            if (creature)
                creature->MonsterSay(text, language, TargetGuid);
        }

        void UpdateAI(const uint32 diff)
        {
            if (QuestAccepted == true)
            {
                if (uiTimer <= diff)
                {
                    switch (uiPhase)
                    {
                    case 0:
                        me->GetMotionMaster()->MovePath(me->GetEntry(), false);
                        me->SendMovementFlagUpdate();
                        ++uiPhase;
                        uiTimer = 5000;
                        break;
                    case 1:
                        me->MonsterSay("We assembled a mighty force and defeated Thermaplugg's servants, fighting our way into the city. But, the usurper Thermaplugg had one last trick up his sleeve...", LANG_UNIVERSAL, 0);
                        ++uiPhase;
                        uiTimer = 4000;
                        break;
                    case 2:
                        me->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                        ++uiPhase;
                        uiTimer = 1000;
                        break;
                    case 3:
                        // blue thing is here
                        Image[0] = me->SummonCreature(NPC_IMAGE_1, -5134.890625f, 495.697418f, 396.451538f, 5.601433f);
                        Image[1] = me->SummonCreature(NPC_IMAGE_2, -5135.135254f, 494.831726f, 396.451538f, 5.601433f);
                        Image[2] = me->SummonCreature(NPC_IMAGE_2, -5134.188965f, 496.109772f, 396.450714f, 5.601433f);
                        Image[3] = me->SummonCreature(NPC_IMAGE_3, -5132.813965f, 493.770477f, 396.451019f, 2.469979f);
                        Image[5] = me->SummonCreature(NPC_IMAGE_5, -5135.094727f, 494.066895f, 396.450562f, 2.072694f);
                        Image[6] = me->SummonCreature(NPC_IMAGE_5, -5132.917480f, 495.923645f, 396.450562f, 5.458562f);
                        Image[3]->SetDisplayId(MODEL_IRRADIATOR);
                        me->MonsterSay("Take a look at this holotable. I want to show you a bit about Operation: Gnomeregan.", LANG_UNIVERSAL, 0);
                        ++uiPhase;
                        uiTimer = 6000;
                        break;
                    case 4:
                        Say(Image[0], "It's too quiet. Where are Thermaplugg's defense forces?", LANG_UNIVERSAL, 0);
                        me->PlayDirectSound(17552);
                        ++uiPhase;
                        uiTimer = 5000;
                        break;
                    case 5:
                        Image[4] = me->SummonCreature(NPC_IMAGE_4, -5133.591797f, 494.378143f, 396.450989f, 2.352166f);
                        ++uiPhase;
                        uiTimer = 3000;
                        break;
                    case 6:
                        Say(Image[4], "No! No, no NO!!! I won't allow you into my kingdom, usurper! I WON'T ALLOW IT!", LANG_UNIVERSAL, 0);
                        me->PlayDirectSound(17570);
                        ++uiPhase;
                        uiTimer = 5500;
                        break;
                    case 7:
                        Say(Image[4], "Activate the Ultimate Atomic Protection System.", LANG_UNIVERSAL, 0);
                        ++uiPhase;
                        uiTimer = 5000;
                        break;
                    case 8:
                        Say(Image[4], "...DISINTEGRATE THEM ALL!", LANG_UNIVERSAL, 0);
                        ++uiPhase;
                        uiTimer = 4000;
                        break;
                    case 9:
                        Say(Image[0], "IT'S A TRAP! That's a fully functional irradiator!", LANG_UNIVERSAL, 0);
                        me->PlayDirectSound(17554);
                        ++uiPhase;
                        uiTimer = 5000;
                        break;
                    case 10:
                        Say(Image[3], "Irradiator 3000 activated. Attention. Emergency. You now have ten minutes to reach minimum safe distance.", LANG_UNIVERSAL, 0);
                        //need sound
                        ++uiPhase;
                        uiTimer = 11000;
                        break;
                    case 11:
                        Say(Image[0], "Ten minutes? Plenty of time to disarm the device.", LANG_UNIVERSAL, 0);
                        me->PlayDirectSound(17556);
                        ++uiPhase;
                        uiTimer = 7000;
                        break;
                    case 12:
                        Say(Image[0], "Hinkles, hand me that Whirring Bronze Gizmo and some Fresh Spring Water. Cogspin, toss me that Dirty Trogg Cloth and a Handful of Copper Bolts...", LANG_UNIVERSAL, 0);
                        ++uiPhase;
                        uiTimer = 10000;
                        break;
                    case 13:
                        Say(Image[4], "TEN MINUTES?! You left the factory setting on? Give me that controller, you idiot!", LANG_UNIVERSAL, 0);
                        me->PlayDirectSound(17572);
                        ++uiPhase;
                        uiTimer = 1500;
                        break;
                    case 14:
                        //delete blue thing
                        ++uiPhase;
                        uiTimer = 4000;
                        break;
                    case 15:
                        Say(Image[3], "*BEEP* Attention. Emergency. You now have ten seconds to reach minimum safe distance.", LANG_UNIVERSAL, 0);
                        //need sound
                        ++uiPhase;
                        uiTimer = 4000;
                        break;
                    case 16:
                        Say(Image[0], "Whuh-oh! We've got to get out of here! Lapforge, beam us back, quickly!", LANG_UNIVERSAL, 0);
                        me->PlayDirectSound(17557);
                        ++uiPhase;
                        uiTimer = 5000;
                        break;
                    case 17:
                        //Image[0], Image[1], Image[2] cast teleport effect
                        ++uiPhase;
                        uiTimer = 1000;
                        break;
                    case 18:
                        if (Image[0] && Image[1] && Image[2])
                            for (int i = 0; i < 3; ++i)
                                Image[i]->ForcedDespawn();
                        ++uiPhase;
                        uiTimer = 1000;
                        break;
                    case 19:
                        //Image[3] cast explosion effect
                        me->PlayDirectSound(17555);
                        ++uiPhase;
                        uiTimer = 1000;
                        break;
                    case 20:
                        if (Image[3] && Image[4] && Image[5] && Image[6])
                            for (int i = 3; i < 7; ++i)
                                Image[i]->ForcedDespawn();
                        ++uiPhase;
                        uiTimer = 2000;
                        break;
                    case 21:
                        if (player)
                            if (player->IsInWorld())
                                player->KilledMonsterCredit(42463, 0);
                        me->MonsterSay("We were forced to retreat. Thermaplugg continues to hide behind the radiation, sending his crony, Crushcog, to harass us here on the surface.", LANG_UNIVERSAL, 0);
                        //need sound
                        ++uiPhase;
                        uiTimer = 3000;
                        break;
                    case 22:
                        QuestAccepted = false;
                        EnterEvadeMode();
                        me->SetOrientation(2.303830f);
                        me->SendMovementFlagUpdate();
                        ++uiPhase;
                        uiTimer = 1000;
                        break;
                    }
                }
                else uiTimer -= diff;
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

};

void AddSC_dun_morogh()
{
    new npc_narm_faulk();
    new npc_high_tinker_mekkatorque();
}
