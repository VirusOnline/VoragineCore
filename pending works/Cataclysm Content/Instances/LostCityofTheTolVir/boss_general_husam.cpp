/* ScriptData
SDName: General Husam
SD%Complete: 90%
SDComment: Detonate dont work properly, affect only 1 trap for now.
SDCategory:
EndScriptData */

/*
NPC 44711 - Spawned before shockwave - targets.
NPC 44712 - Shockwave NPC.
*/
#include "ScriptPCH.h"
#include "ScriptedCreature.h"
#include "the_lost_city_of_tol_vir.h"

enum Spells
{
    SPELL_BAD_INTENTIONS                        = 83113,
    SPELL_SHOCKWAVE                             = 83445,
    SPELL_HAMMER_FIST                           = 83655,
    SPELL_MYSTIC_TRAP_PLANT                     = 83646,
    SPELL_MYSTIC_TRAP_EXPLODE                   = 83171,
    SPELL_DETONATE_TRAPS                        = 91263,
};

enum Yells
{
    SAY_AGGRO                   = -1755001,
    SAY_SHOCKWAVE               = -1755002,
    SAY_DETONATE                = -1755003,
    SAY_DEATH                   = -1755004,
};

enum Actions
{
    ACTION_DETONATE
}; 


class boss_general_husam : public CreatureScript
{
public:
    boss_general_husam() : CreatureScript("boss_general_husam") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new boss_general_husamAI (pCreature);
    }

    struct boss_general_husamAI : public ScriptedAI
    {
        InstanceScript* m_pInstance;
        
        boss_general_husamAI(Creature *c) : ScriptedAI(c)
        {
            m_pInstance = c->GetInstanceScript();
        }

        uint32 BadIntentionsTimer;
        uint32 ShockwaveTimer;
        uint32 HammerFistTimer;
        uint32 PlantTimer;
        uint32 DetonateTimer;

        void JustDied(Unit* /*Killer*/)
        {
            DoScriptText(SAY_DEATH, me);
        }

        void Reset()
        {
            BadIntentionsTimer = 8000;
            ShockwaveTimer = 18000;
            HammerFistTimer = 5000;
            PlantTimer = 3000;
            DetonateTimer = 23000;
        }

        void EnterCombat(Unit* pWho)
        {
            DoScriptText(SAY_AGGRO, me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if(BadIntentionsTimer <= diff)
            {
                if(Unit* pTar = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTar,SPELL_BAD_INTENTIONS);
                BadIntentionsTimer = 15000;
            }
            else
                BadIntentionsTimer -= diff;

            if(ShockwaveTimer <= diff)
            {
                Creature* Target1 = me->SummonCreature(44711, me->GetPositionX()+10,me->GetPositionY(),me->GetPositionZ(),0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10000);
                Creature* Target2 = me->SummonCreature(44711, me->GetPositionX()-10,me->GetPositionY(),me->GetPositionZ(),0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10000);
                Creature* Target3 = me->SummonCreature(44711, me->GetPositionX(),me->GetPositionY()+10,me->GetPositionZ(),0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10000);
                Creature* Target4 = me->SummonCreature(44711, me->GetPositionX(),me->GetPositionY()-10,me->GetPositionZ(),0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10000);
                DoScriptText(SAY_SHOCKWAVE, me);
                DoCastAOE(SPELL_SHOCKWAVE);
                DoCast(Target1, 83130);
                DoCast(Target2, 83130);
                DoCast(Target3, 83130);
                DoCast(Target4, 83130);
                if (Creature* Shockwave1 = me->SummonCreature(44712, me->GetPositionX(),me->GetPositionY(),me->GetPositionZ(),0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10000))
                    Shockwave1->GetMotionMaster()->MoveChase(Target1);
                if (Creature* Shockwave2 = me->SummonCreature(44712, me->GetPositionX(),me->GetPositionY(),me->GetPositionZ(),0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10000))
                    Shockwave2->GetMotionMaster()->MoveChase(Target2);
                if (Creature* Shockwave3 = me->SummonCreature(44712, me->GetPositionX(),me->GetPositionY(),me->GetPositionZ(),0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10000))
                    Shockwave3->GetMotionMaster()->MoveChase(Target3);
                if (Creature* Shockwave4 = me->SummonCreature(44712, me->GetPositionX(),me->GetPositionY(),me->GetPositionZ(),0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10000))
                    Shockwave4->GetMotionMaster()->MoveChase(Target4);
                ShockwaveTimer = urand(20000,40000);
            }
            else
                ShockwaveTimer -= diff;

            if (HammerFistTimer <= diff)
            {
                DoCast(me->getVictim(), SPELL_HAMMER_FIST);
                HammerFistTimer = 12000;
            }
            else
                HammerFistTimer -= diff;

            if (PlantTimer <= diff)
            {
                if(Unit* pTar = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    pTar->CastSpell(pTar, SPELL_MYSTIC_TRAP_PLANT, true);
                PlantTimer = 3000;
            }
            else
                PlantTimer -= diff;

            if (DetonateTimer <= diff && IsHeroic())
            {
                DoCast(me, SPELL_DETONATE_TRAPS);
                DoScriptText(SAY_DETONATE, me);
                DetonateTimer = 32000;
            }
            else
                DetonateTimer -= diff;

            DoMeleeAttackIfReady();
        }

    };

};

class npc_trap : public CreatureScript
{
    public:
        npc_trap() : CreatureScript("npc_trap") { }

        struct npc_trapAI : public ScriptedAI
        {
            npc_trapAI(Creature *pCreature) : ScriptedAI(pCreature) {}

            uint32 MysticTrap_ExplodeTimer;

            void Reset()
            {
                MysticTrap_ExplodeTimer = 4000;
            }

            void UpdateAI(const uint32 diff)
            {
                if (MysticTrap_ExplodeTimer <= diff)
                {
                    DoCastAOE(SPELL_MYSTIC_TRAP_EXPLODE);
                    me->DisappearAndDie();

                    MysticTrap_ExplodeTimer = 4000;
                }
                else
                    MysticTrap_ExplodeTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_trapAI(pCreature);
        }
};

void AddSC_boss_general_husam()
{
    new boss_general_husam();
    new npc_trap();
}
