/*
 * Copyright (C) 2010-2011 SkyFire <http://www.projectskyfire.org/>
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

#include"ScriptPCH.h"
#include"halls_of_origination.h"

enum ScriptTexts
{
    SAY_AGGRO          = 0,
    SAY_KILL_1         = 1,
    SAY_KILL_2         = 2,
    SAY_DEATH          = 3,
};

enum Spells
{
    SPELL_FLAME_BOLT   = 77370,
    SPELL_RAGING_SMASH = 83650,
    SPELL_EARTH_SPIKE  = 94974,
};

enum BossPhases
{
    PHASE_NORMAL = 1,
    PHASE_50_PER = 2,
};

const Position SpawnPosition[] =
{
    {-542.85f, -389.745f, 158.871f},
    {-526.344f, -430.457f, 155.513f},
    {-521.232f, -376.484f, 156.975f},
    {-516.064f, -405.301f, 155.591f},
    {-503.511f, -373.971f, 155.352f},
    {-493.174f, -433.977f, 156.578f},
    {-479.587f, -398.807f, 156.877f},
    {-475.642f, -423.303f, 158.438f}
};

class boss_earthrager_ptah : public CreatureScript
{
    public:
        boss_earthrager_ptah() : CreatureScript("boss_earthrager_ptah") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new boss_earthrager_ptahAI(pCreature);
        }
        struct boss_earthrager_ptahAI : public ScriptedAI
        {
            boss_earthrager_ptahAI(Creature* pCreature) : ScriptedAI(pCreature)
            {
                pInstance = pCreature->GetInstanceScript();
            }

            std::list<uint64> SummonList;

            InstanceScript *pInstance;

            uint8 Phase;
            bool Phased;
            uint8 SpawnCount;
            uint8 PhaseCount;

            uint32 FlameBoltTimer;
            uint32 RagingSmashTimer;
            uint32 EarthSpikeTimer;

            void Reset()
            {
                if(pInstance)
                   pInstance->SetData(DATA_EARTHRAGER_PTAH_EVENT, NOT_STARTED);

                Phase = PHASE_NORMAL;
                SpawnCount = 3;
                Phased = false;
                PhaseCount = 0;
                FlameBoltTimer = 7000;
                RagingSmashTimer = 4000;
                EarthSpikeTimer = 12000;
                RemoveSummons();
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE,0);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            }

            void SummonedCreatureDespawn(Creature* summon)
            {
                switch(summon->GetEntry())
                {
                    case NPC_DUSTBONE_HORROR:
                    case NPC_JEWELED_SCARAB:
                        SpawnCount--;
                        break;
                }
            }

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

            void JustSummoned(Creature* pSummon)
            {
                switch (pSummon->GetEntry())
                {
                    case NPC_DUSTBONE_HORROR:
                    case NPC_JEWELED_SCARAB:
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                            pSummon->AI()->AttackStart(pTarget);
                        SummonList.push_back(pSummon->GetGUID());
                        break;
                }
                SummonList.push_back(pSummon->GetGUID());
            }

            void ChangePhase()
            {
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE,65); // Dead
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE + UNIT_FLAG_NON_ATTACKABLE);
                SetCombatMovement(false);
                me->SetReactState(REACT_PASSIVE);

                for(uint32 x = 0; x<6; ++x)
                    me->SummonCreature(NPC_JEWELED_SCARAB,SpawnPosition[x],TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);

                for(uint32 x = 0; x<2; ++x)
                    me->SummonCreature(NPC_DUSTBONE_HORROR,SpawnPosition[6+x],TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);

                me->SummonCreature(NPC_DUSTBONE_HORROR,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ(),0,TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);
                PhaseCount++;
                Phase = PHASE_50_PER;
            }

            void KilledUnit(Unit* /*Killed*/)
            {
                DoScriptText(RAND(SAY_KILL_1, SAY_KILL_2), me);
            }

            void JustDied(Unit* /*Kill*/)
            {
                DoScriptText(SAY_DEATH, me);
                if (pInstance)
                    pInstance->SetData(DATA_EARTHRAGER_PTAH_EVENT, DONE);
            }

            void EnterCombat(Unit* /*Ent*/)
            {
                DoScriptText(SAY_AGGRO, me);
                if (pInstance)
                    pInstance->SetData(DATA_EARTHRAGER_PTAH_EVENT, IN_PROGRESS);

                DoZoneInCombat();
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                   return;

                if (me->HealthBelowPct(51) && Phase == PHASE_NORMAL && PhaseCount == 0)
                {
                    ChangePhase();
                }

                if (SpawnCount == 0 && Phase == PHASE_50_PER)
                {
                    me->SetUInt32Value(UNIT_NPC_EMOTESTATE,0);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE + UNIT_FLAG_NON_ATTACKABLE);

                    SpawnCount = 8;
                    SetCombatMovement(true);
                    Phase = PHASE_NORMAL;
                    Phased = false;
                    me->SetReactState(REACT_AGGRESSIVE);
                    FlameBoltTimer = 7000;
                    RagingSmashTimer = 4000;
                    EarthSpikeTimer = 12000;
                }

                if (FlameBoltTimer <= diff && Phase == PHASE_NORMAL)
                {
                    DoCastAOE(SPELL_FLAME_BOLT);
                    FlameBoltTimer = 12000;
                } else FlameBoltTimer -= diff;

                if (RagingSmashTimer <= diff && Phase == PHASE_NORMAL)
                {
                    DoCastVictim(SPELL_RAGING_SMASH);
                    RagingSmashTimer = 8000;
                } else RagingSmashTimer -= diff;

                if (EarthSpikeTimer <= diff && Phase == PHASE_NORMAL)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        DoCast(target, SPELL_EARTH_SPIKE);
                    EarthSpikeTimer = 15000;
                } else EarthSpikeTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };
};

void AddSC_boss_earthrager_ptah()
{
    new boss_earthrager_ptah();
}
