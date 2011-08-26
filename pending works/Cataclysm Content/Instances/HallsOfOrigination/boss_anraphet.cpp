/* ScriptData
SDName: boss_anraphet
SD%Complete: 80%
SDComment: 
SDCategory: 
EndScriptData */

#include "ScriptPCH.h"

enum Texts
{
    SAY_INTRO                  = 0,
    SAY_AGGRO                  = 1,
    SAY_KILL_1                 = 2,
    SAY_KILL_2                 = 3,
    SAY_OMEGA                  = 4,
    SAY_DEATH                  = 5,
};

enum Spells
{
    SPELL_ALPHA_BEAMS          = 76184,
    SPELL_CRUMBLING_RUIN       = 75609,
    SPELL_DESTRUCTION_PROTOCOL = 77437,
    SPELL_NEMESIS_STRIKE       = 75604,
    SPELL_OMEGA_STANCE         = 75622,
};

class boss_anraphet : public CreatureScript
{
    public:
        boss_anraphet() : CreatureScript("boss_anraphet") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new boss_anraphetAI(pCreature);
        }

        struct boss_anraphetAI : public ScriptedAI
        {
            boss_anraphetAI(Creature* pCreature) : ScriptedAI(pCreature)
            {
                pInstance = pCreature->GetInstanceScript();
                SetCombatMovement(true);
            }

            std::list<uint64> SummonList;

            InstanceScript *pInstance;

            uint32 AplhaBeamsTimer;
            uint32 OmegaStanceTimer;
            uint32 NemesisStrikeTimer;
            uint32 CrumplingRuinTimer;

            void EnterCombat(Unit *who)
            {
                Talk(SAY_AGGRO);
            }

            void KilledUnit(Unit* victim)
            {
               Talk(RAND(SAY_KILL_1, SAY_KILL_2));
            }

            void JustDied(Unit* Killer)
            {
                Talk(SAY_DEATH);
                pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CRUMBLING_RUIN);
            }

            void Reset()
            {
                //if(pInstance && (pInstance->GetData(DATA_ANRAPHET_EVENT) != DONE && !check_in))
                //   pInstance->SetData(DATA_ANRAPHET_EVENT, NOT_STARTED);

                AplhaBeamsTimer = 8000+rand()%2000;
                OmegaStanceTimer = 10000;
                NemesisStrikeTimer = 12000;
                CrumplingRuinTimer = 15000;
                pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CRUMBLING_RUIN);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (AplhaBeamsTimer <= diff)
                {
                    DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true), SPELL_ALPHA_BEAMS);
                    AplhaBeamsTimer = 18000;
                    OmegaStanceTimer = 3000;
                } else AplhaBeamsTimer -= diff;

                if (OmegaStanceTimer <= diff)
                {
                    Talk(SAY_OMEGA);
                    DoCast(me, SPELL_OMEGA_STANCE);
                    OmegaStanceTimer = 20000;
                } else OmegaStanceTimer -= diff;

                if (NemesisStrikeTimer <= diff)
                {
                    DoCast(me->getVictim(), SPELL_NEMESIS_STRIKE);
                    NemesisStrikeTimer = 12000;
                } else NemesisStrikeTimer -= diff;

                if (CrumplingRuinTimer <= diff)
                {
                    DoCast(me->getVictim(), SPELL_CRUMBLING_RUIN);
                    CrumplingRuinTimer = 15000;
                } else CrumplingRuinTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };
};

void AddSC_boss_anraphet()
{
    new boss_anraphet();
}