
#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"

class npc_greymane_horse : public CreatureScript
{
public:
    npc_greymane_horse() : CreatureScript("npc_greymane_horse") { }

    struct npc_greymane_horseAI : public npc_escortAI
    {
        npc_greymane_horseAI(Creature* creature) : npc_escortAI(creature) {}

        void AttackStart(Unit* /*who*/) {}
        void EnterCombat(Unit* /*who*/) {}
        void EnterEvadeMode() {}

        void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply)
        {
            if (who->GetTypeId() == TYPEID_PLAYER)
            {
                if (apply)
                    Start(false, true, who->GetGUID());
            }
        }

        void WaypointReached(uint32 i)
        {
            Player* pPlayer = GetPlayerForEscort();

            switch(i)
            {
                case 7:
                    pPlayer->KilledMonsterCredit(35753, 0);
                    break;
            }
        }

        void JustDied(Unit* /*killer*/)
        {
        }

        void OnCharmed(bool /*apply*/)
        {
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_greymane_horseAI (creature);
    }
};

#define PHASE_1_WORGENS   RAND(35457,35456)
#define PHASE_2_WORGENS   RAND(35188,35170)

const Position spawns_coords[] = 
{
    {-1700.945f, 1470.472f, 52.2873f, 5.50f},
    {-1651.365f, 1421.122f, 52.2873f, 2.39f},
};

class npc_darius_crowley : public CreatureScript
{
public:
    npc_darius_crowley() : CreatureScript("npc_darius_crowley") { }

    bool OnQuestAccept(Player *pPlayer, Creature *pCreature, const Quest *pQuest)
    {
        if (pQuest->GetQuestId() == 14154)
        {
            CAST_AI(npc_darius_crowley::npc_darius_crowleyAI, pCreature->AI())->QuestInProgress = true;
            CAST_AI(npc_darius_crowley::npc_darius_crowleyAI, pCreature->AI())->phase = 1;
            CAST_AI(npc_darius_crowley::npc_darius_crowleyAI, pCreature->AI())->PlayerGUID = pPlayer->GetGUID();
            pPlayer->CastSpell(pCreature,68218,true);
        }

        return true;
    }

    struct npc_darius_crowleyAI : public ScriptedAI
    {
        npc_darius_crowleyAI(Creature* creature) : ScriptedAI(creature) { }

        std::list<uint64> SummonList;
        uint64 PlayerGUID;

        bool QuestInProgress;

        uint32 phase;
        uint32 count;

        uint32 nextsummon;
        uint32 CompleteTimer;

        void RemoveSummons()
        {
            if (SummonList.empty())
                return;

            for (std::list<uint64>::const_iterator itr = SummonList.begin(); itr != SummonList.end(); ++itr)
            {
                if (Creature* pTemp = Unit::GetCreature(*me, *itr))
                    if (pTemp)
                        pTemp->DisappearAndDie();
            }
            SummonList.clear();
        }

        void Reset()
        {
            phase = 1;
            count = 0;
            nextsummon = 1000;
            QuestInProgress = false;
            PlayerGUID = 0;
            RemoveSummons();
        }

        void JustSummoned(Creature *pSummoned)
        {
            if (Player *pPlayer = Unit::GetPlayer(*me, PlayerGUID))
            {
                pSummoned->AddThreat(pPlayer, 32.0f);
                pSummoned->AI()->AttackStart(me);
            }
            SummonList.push_back(pSummoned->GetGUID());
        }

        void JustDied(Unit * /*pKiller*/)
        {
            if (PlayerGUID)
                if (Player *pPlayer = Unit::GetPlayer(*me, PlayerGUID))
                    if (pPlayer->GetQuestStatus(14154) == QUEST_STATUS_INCOMPLETE)
                    {
                        pPlayer->FailQuest(14154);
                        RemoveSummons();
                    }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!QuestInProgress)
                return;

            if (!UpdateVictim())
                me->GetMotionMaster()->MoveTargetedHome();

            if (phase == 1)
            {
                for(uint32 x = 0; x<2; x++) // Bloodfang Bloodletter or Worgen Runt
                    me->SummonCreature(PHASE_1_WORGENS, RAND(spawns_coords[0],spawns_coords[1]), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 4000);

                phase = 2;
                nextsummon = 15000;
            }
            if (phase == 2 && count < 9)
            {
                if(nextsummon <= diff) //Lets summon another worgen
                {
                    for(uint32 x = 0; x<3; x++) // Worgen Alpha
                        me->SummonCreature(PHASE_2_WORGENS, RAND(spawns_coords[0],spawns_coords[1]), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 4000);

                    count++;
                    nextsummon = 20000;
                } else nextsummon -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_darius_crowleyAI (creature);
    }
};

// You Can't Take 'Em Alone - 14348

enum YouCantTakeEmAlone
{
    QUEST_14348_KILL_CREDIT            = 36233,
    SPELL_BARREL_KEG                   = 69094,
    QUEST_YOU_CANT_TAKE_EM_ALONE       = 14348,
};

#define SAY_BARREL_1 "I gots bad feeling...."
#define SAY_BARREL_2 "GAH! I CAN'T SEE IN HERE!"
#define SAY_BARREL_3 "Get back here! I smashes you!"
#define SAY_BARREL_4 "Uh-oh... this gonna hurts me..."
#define SAY_BARREL_5 "Barrel smell like gunpowder..."
#define SAY_BARREL_6 "This not be good..."

class npc_horrid_abomination : public CreatureScript
{
public:
    npc_horrid_abomination() : CreatureScript("npc_horrid_abomination") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_horrid_abominationAI(pCreature);
    }

    struct npc_horrid_abominationAI : public ScriptedAI
    {
        npc_horrid_abominationAI(Creature *c) : ScriptedAI(c) {}

        uint32 DieTimer;

        void Reset ()
        {
            me->ClearUnitState(UNIT_STAT_ROOT | UNIT_STAT_STUNNED);
            DieTimer = 5000;
        }

        void SpellHit(Unit* caster, const SpellEntry * spell)
        {
            if (spell->Id == SPELL_BARREL_KEG && caster->GetTypeId() == TYPEID_PLAYER
                && caster->ToPlayer()->GetQuestStatus(QUEST_YOU_CANT_TAKE_EM_ALONE) == QUEST_STATUS_INCOMPLETE)
            {
                caster->ToPlayer()->KilledMonsterCredit(QUEST_14348_KILL_CREDIT, 0);
                me->AddUnitState(UNIT_STAT_ROOT | UNIT_STAT_STUNNED);
                switch (urand(0,5))
                {
                    case 0:
                        me->MonsterYell(SAY_BARREL_1, LANG_UNIVERSAL, 0);
                        break;
                    case 1:
                        me->MonsterYell(SAY_BARREL_2, LANG_UNIVERSAL, 0);
                        break;
                    case 2:
                        me->MonsterYell(SAY_BARREL_3, LANG_UNIVERSAL, 0);
                        break;
                    case 3:
                        me->MonsterYell(SAY_BARREL_4, LANG_UNIVERSAL, 0);
                        break;
                    case 4:
                        me->MonsterYell(SAY_BARREL_5, LANG_UNIVERSAL, 0);
                        break;
                    case 5:
                        me->MonsterYell(SAY_BARREL_6, LANG_UNIVERSAL, 0);
                        break;
                }
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (DieTimer <= uiDiff)
            {
                if (me->HasAura(SPELL_BARREL_KEG))
                    me->DisappearAndDie();
                else
                    DieTimer = 1000;
            }
            else
                DieTimer -= uiDiff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

// UPDATE `creature_template` SET `ScriptName` = 'npc_crowleys_horse' WHERE `entry` = 44428;
class npc_crowleys_horse : public CreatureScript
{
public:
    npc_crowleys_horse() : CreatureScript("npc_crowleys_horse") { }

    struct npc_crowleys_horseAI : public npc_escortAI
    {
        npc_crowleys_horseAI(Creature* creature) : npc_escortAI(creature) {}

        void AttackStart(Unit* /*who*/) {}
        void EnterCombat(Unit* /*who*/) {}
        void EnterEvadeMode() {}

        void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply)
        {
            if (who->GetTypeId() == TYPEID_PLAYER)
            {
                if (apply)
                    Start(false, true, who->GetGUID());
            }
        }

        void WaypointReached(uint32 /*i*/)
        {
        }

        void JustDied(Unit* /*killer*/)
        {
        }

        void OnCharmed(bool /*apply*/)
        {
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crowleys_horseAI (creature);
    }
};

// UPDATE `creature_template` SET `ScriptName` = 'npc_swift_mountain_horse' WHERE `entry` = 36741;
class npc_swift_mountain_horse : public CreatureScript
{
public:
    npc_swift_mountain_horse() : CreatureScript("npc_swift_mountain_horse") { }

    struct npc_swift_mountain_horseAI : public npc_escortAI
    {
        npc_swift_mountain_horseAI(Creature* creature) : npc_escortAI(creature) {}

        void AttackStart(Unit* /*who*/) {}
        void EnterCombat(Unit* /*who*/) {}
        void EnterEvadeMode() {}

        void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply)
        {
            if (who->GetTypeId() == TYPEID_PLAYER)
            {
                if (apply)
                    Start(false, true, who->GetGUID());
            }
        }

        void WaypointReached(uint32 i)
        {
            Player* pPlayer = GetPlayerForEscort();

            switch(i)
            {
                case 8:
                    pPlayer->TeleportTo(654, pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), pPlayer->GetOrientation());
                    break;
            }
        }

        void JustDied(Unit* /*killer*/)
        {
        }

        void OnCharmed(bool /*apply*/)
        {
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_swift_mountain_horseAI (creature);
    }
};

class npc_stout_mountain_horse : public CreatureScript
{
public:
    npc_stout_mountain_horse() : CreatureScript("npc_stout_mountain_horse") { }

    struct npc_stout_mountain_horseAI : public npc_escortAI
    {
        npc_stout_mountain_horseAI(Creature* creature) : npc_escortAI(creature) {}

        void AttackStart(Unit* /*who*/) {}
        void EnterCombat(Unit* /*who*/) {}
        void EnterEvadeMode() {}

        void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply)
        {
            if (who->GetTypeId() == TYPEID_PLAYER)
            {
                if (apply)
                    Start(false, true, who->GetGUID());
            }
        }

        void WaypointReached(uint32 /*wp*/)
        {
        }

        void JustDied(Unit* /*killer*/)
        {
        }

        void OnCharmed(bool /*apply*/)
        {
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stout_mountain_horseAI (creature);
    }
};

void AddSC_gilneas()
{
    new npc_greymane_horse;
    new npc_darius_crowley;
    new npc_horrid_abomination;
    new npc_crowleys_horse;
    new npc_swift_mountain_horse;
    new npc_stout_mountain_horse;
}