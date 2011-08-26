/*
#include "grimbatol.h"
#include "ScriptPCH.h"

enum Texts
{
    SAY_AGGRO         = 1,
    SAY_DUAL_SWORDS,
    SAY_MAZE,
    SAY_SHIELD,
    SAY_DEATH,
};

enum Spells
{
    SPELL_MIGHTY_STOMP      = 74984,
    SPELL_PICK_WEAPON       = 75000,
    SPELL_ENCUMBERED        = 75007, // Only maze
    SPELL_IMPALING_SLAM     = 90756, // Only maze
    SPELL_DISORIENTING_ROAR = 74976, // Only dual-blades
    
};
class boss_forgemaster_throngus : public CreatureScript
{
    public:
        boss_forgemaster_throngus() : CreatureScript("boss_forgemaster_throngus") { }

        struct boss_forgemaster_throngusAI : public BossAI
        {
            boss_forgemaster_throngusAI(Creature* creature) : BossAI(creature, DATA_FORGEMASTER_THRONGUS)
            {
                
            }

            void Reset()
            {
                _Reset();
                
            }

            void EnterCombat(Unit* who)
            {
                me->setActive(true);
                Talk(SAY_AGGRO);
            }
            
            void JustDied(Unit* killer)
            {
                _JustDied();
                Talk(SAY_DEATH);
            }

            void JustReachedHome()
            {
                _JustReachedHome();
            }

            void KilledUnit(Unit* victim)
            {
                
            }

            void EnterEvadeMode()
            {
                
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_forgemaster_throngusAI(creature);
        }
};

void AddSC_forgemaster_throngus()
{
    new boss_forgemaster_throngus();
}
*/