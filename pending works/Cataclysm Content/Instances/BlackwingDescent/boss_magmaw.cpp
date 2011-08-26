/* ScriptData
SDName: Boss_magmaw
SD%Complete: 50%
SDComment: Need to see what other spells this boss cast, data extracted from here-> http://www.wowhead.com/npc=41570, is this ok?
SDCategory: Blackwing Descent
EndScriptData */

#include "ScriptPCH.h"
#include "ScriptedCreature.h"

enum eEnums
{
    SPELL_PILAR_OF_FLAME                = 78006,
    NPC_LAVA_PARASITE                   = 42321,
    SPELL_MANGLE                        = 89773,
    SPELL_LAVA_SPEW                     = 77690,
    SPELL_MAGMA_SPIT                    = 78068,
    SPELL_BESERK                        = 26662,
    SPELL_SWELTERING_ARMOR              = 78199,
    SPELL_CONSTRICTING_CHAINS           = 79589,
    SPELL_PARASITIC_INFECTION           = 94679, //Ouch, this must hurt :P
};

class boss_magmaw : public CreatureScript
{
public:
    boss_magmaw() : CreatureScript("boss_magmaw") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new boss_magmawAI (pCreature);
    }

    struct boss_magmawAI : public ScriptedAI
    {
        boss_magmawAI(Creature *c) : ScriptedAI(c) {}

        uint32 pilarofflame_cooldown;
        uint32 Beserk_Timer;
        uint32 m_phase;
        uint32 number_of_pilars;
        uint32 m_number_of_spawns;
        uint32 mangle_timer;
        uint32 eat_timer;
        uint32 spite_timer;
        uint32 spew_timer;
        bool ismangled;
        bool jumped;
        bool Beserk;

        void Reset()
        {
            m_number_of_spawns= 0;
            m_phase = 1;
            jumped = false;
            pilarofflame_cooldown = 30000;
            Beserk_Timer = 600000;
            Beserk = false;
            mangle_timer = 23000;
            ismangled = false;
            eat_timer = 30000;
            spite_timer = urand(20000,40000);
            spew_timer = urand(15000,25000);
            number_of_pilars = 0;
        }

        void JustSummoned(Creature* pSummon)
        {
            pSummon->SetReactState(REACT_AGGRESSIVE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;
        
            //phase 1?
            if (m_phase == 1)
            {
                // pilar of flame spell
                if (pilarofflame_cooldown <= diff)
                {
                    DoCast(me->getVictim(), SPELL_PILAR_OF_FLAME);
                    //ToDo: Script theese lava parasites =/
                    me->SummonCreature(NPC_LAVA_PARASITE,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ());
                    m_number_of_spawns += 1;
                    pilarofflame_cooldown = 30000;
                    number_of_pilars += 1;
                } else pilarofflame_cooldown -= diff;

                // Hit with Mangle to the main tank
                if (mangle_timer <= diff)
                {
                    if (Unit *pTarget = SelectTarget(SELECT_TARGET_TOPAGGRO))
                        DoCast(pTarget, SPELL_MANGLE);
                    mangle_timer = 23000;
                } else mangle_timer -= diff;

                //Beserk after 5 min
                if (Beserk_Timer <= diff)
                {
                    DoCast(me, SPELL_BESERK);
                } else Beserk_Timer -= diff;
                
                //He will eat your tank if he is already mangled
                if(ismangled)
                {
                    if (eat_timer <= diff)
                        if(Unit *pTarget = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            DoCast(pTarget,SPELL_SWELTERING_ARMOR);
                    else
                        eat_timer -= diff;
                }
                if (spew_timer <= diff)
                {
                    if (Unit *pTarget = SelectTarget(SELECT_TARGET_TOPAGGRO))
                        DoCast(pTarget, SPELL_LAVA_SPEW);
                    spew_timer = urand(15000,25000);;
                } else spew_timer -= diff;

                if (spite_timer <= diff)
                {
                    if (Unit *pTarget = SelectTarget(SELECT_TARGET_TOPAGGRO))
                        DoCast(pTarget, SPELL_MAGMA_SPIT);
                    spite_timer = urand(20000,40000);
                } else spite_timer -= diff;
                
                //Check for the phase 2...
                if(number_of_pilars >= 3)
                    m_phase = 2;
            }
            //Ok, phase 2
            else if(m_phase==2)
            {
                if(!jumped)
                {
                    jumped = true;
                    float speedxy = 1;
                    float speedz = 1;
                    me->JumpTo(speedxy,speedz,true);
                }
            // Need to fix this... he should give the players a spell and then unlearn them that spell
            m_phase = 1; // =/
            }
        }
      
    };

};

class lava_parasite : public CreatureScript
{
public:
    lava_parasite() : CreatureScript("lava_parasite") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new lava_parasiteAI (pCreature);
    }

    struct lava_parasiteAI : public ScriptedAI
    {
        lava_parasiteAI(Creature *c) : ScriptedAI(c) {}

        void UpdateAI(const uint32 diff)
        {
            if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM))
              DoCast(pTarget, SPELL_PARASITIC_INFECTION);
            me->DespawnOrUnsummon();
        }
    };

};

void AddSC_boss_magmaw()
{
    new lava_parasite();
    new boss_magmaw();
}