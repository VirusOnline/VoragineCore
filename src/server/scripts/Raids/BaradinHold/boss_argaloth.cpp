/*
 * Copyright (C) 2009-2011 CataProject
 */


#include "ScriptPCH.h"
#include "baradin_hold.h"

enum Spells
{
    SPELL_FEL_FIRESTORM       = 88972,
    SPELL_CONSUMING_DARKNESS  = 88954,
    SPELL_METEOR_SLASH        = 88942,
    SPELL_BERSERK             = 47008,
    // Mobs
    SPELL_ARCANE_AMPLIFIER    = 89354,
    SPELL_DEMON_REPELLENT_RAY = 89348
};

enum ePhases
{
    PHASE_1,
    PHASE_2,
};

class boss_argaloth : public CreatureScript
{
    public:
        boss_argaloth() : CreatureScript("boss_argaloth") { }

        struct boss_argalothAI : public BossAI
        {
            boss_argalothAI(Creature* creature) : BossAI(creature, DATA_ARGALOTH) {}

            uint8 Phase, PhaseCount;

            uint32 SlashTimer;
            uint32 ConsumingDarknessTimer;
            uint32 BerserkTimer;
            uint32 ResetPhaseTimer;

            void Reset()
            {
                PhaseCount = 0;
                SlashTimer = 10000;
                ConsumingDarknessTimer = 15000;
                BerserkTimer = 360000;
                Phase = PHASE_1;
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if(SlashTimer <= diff && Phase == PHASE_1)
                {
                    DoCastVictim(SPELL_METEOR_SLASH);
                    SlashTimer = 15000;
                } else SlashTimer -= diff;

                if(ConsumingDarknessTimer <= diff && Phase == PHASE_1)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        DoCast(target, SPELL_CONSUMING_DARKNESS, true);
                    ConsumingDarknessTimer = 15000;
                } else ConsumingDarknessTimer -= diff;

                if(BerserkTimer <= diff && Phase == PHASE_1)
                {
                    DoCast(me, SPELL_BERSERK);
                    BerserkTimer = 360000;
                } else BerserkTimer -= diff;

                if (HealthBelowPct(67) && Phase == PHASE_1 && PhaseCount == 0)
                {
                    PhaseCount++;
                    Phase = PHASE_2;
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();
                    DoCastAOE(SPELL_FEL_FIRESTORM);
                    ResetPhaseTimer = 16500;
                }

                if (HealthBelowPct(34) && Phase == PHASE_1 && PhaseCount == 1)
                {
                    PhaseCount++;
                    Phase = PHASE_2;
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();
                    DoCastAOE(SPELL_FEL_FIRESTORM);
                    ResetPhaseTimer = 16500;
                }

                if(ResetPhaseTimer <= diff)
                {
                    Phase = PHASE_1;
                    me->SetReactState(REACT_AGGRESSIVE);
                    SlashTimer = 10000;
                    ConsumingDarknessTimer = 15000;
                    ResetPhaseTimer = 360000;
                } else ResetPhaseTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_argalothAI(creature);
        }
};

/*######
##  Mob Archavon Warder
######*/
class mob_argaloth_warder : public CreatureScript
{
    public:
        mob_argaloth_warder() : CreatureScript("mob_argaloth_warder") { }

        struct mob_argaloth_warderAI : public ScriptedAI
        {
            mob_argaloth_warderAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            uint32 ArcaneAmplifierTimer;
            uint32 DemonRepellentRayTimer;

            void Reset()
            {
                ArcaneAmplifierTimer = 7500;
                DemonRepellentRayTimer = 15000;
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STAT_CASTING))
                    return;

                if(ArcaneAmplifierTimer <= diff)
                {
                    DoCastVictim(SPELL_ARCANE_AMPLIFIER);
                    ArcaneAmplifierTimer = 22500;
                } else ArcaneAmplifierTimer -= diff;

                if(DemonRepellentRayTimer <= diff)
                {
                    DoCastVictim(SPELL_DEMON_REPELLENT_RAY);
                    DemonRepellentRayTimer = 22500;
                } else DemonRepellentRayTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_argaloth_warderAI(creature);
        }
};

void AddSC_boss_argaloth()
{
    new boss_argaloth();
    new mob_argaloth_warder();
}
