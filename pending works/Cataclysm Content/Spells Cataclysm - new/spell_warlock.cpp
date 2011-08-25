/*
 * Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
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

/*
 * Scripts for spells with SPELLFAMILY_WARLOCK and SPELLFAMILY_GENERIC spells used by warlock players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_warl_".
 */

#include "ScriptPCH.h"
#include "Spell.h"
#include "SpellAuraEffects.h"

enum WarlockSpells
{
    WARLOCK_DEMONIC_EMPOWERMENT_SUCCUBUS    = 54435,
    WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER  = 54443,
    WARLOCK_DEMONIC_EMPOWERMENT_FELGUARD    = 54508,
    WARLOCK_DEMONIC_EMPOWERMENT_FELHUNTER   = 54509,
    WARLOCK_DEMONIC_EMPOWERMENT_IMP         = 54444,
    WARLOCK_DEMON_SOUL_SUCCUBUS             = 79463,
    WARLOCK_DEMON_SOUL_VOIDWALKER           = 79464,
    WARLOCK_DEMON_SOUL_FELGUARD             = 79462,
    WARLOCK_DEMON_SOUL_FELHUNTER            = 79460,
    WARLOCK_DEMON_SOUL_IMP                  = 79459,
    //WARLOCK_IMPROVED_HEALTHSTONE_R1       = 18692,
    //WARLOCK_IMPROVED_HEALTHSTONE_R2       = 18693,
    WARLOCK_FELHUNTER_SHADOWBITE_R1         = 54049,
};

// 47193 Demonic Empowerment
class spell_warl_demonic_empowerment : public SpellScriptLoader
{
public:
    spell_warl_demonic_empowerment() : SpellScriptLoader("spell_warl_demonic_empowerment") { }

    class spell_warl_demonic_empowerment_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warl_demonic_empowerment_SpellScript)
        bool Validate(SpellEntry const * /*spellEntry*/)
        {
            if (!sSpellStore.LookupEntry(WARLOCK_DEMONIC_EMPOWERMENT_SUCCUBUS))
                return false;
            if (!sSpellStore.LookupEntry(WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER))
                return false;
            if (!sSpellStore.LookupEntry(WARLOCK_DEMONIC_EMPOWERMENT_FELGUARD))
                return false;
            if (!sSpellStore.LookupEntry(WARLOCK_DEMONIC_EMPOWERMENT_FELHUNTER))
                return false;
            if (!sSpellStore.LookupEntry(WARLOCK_DEMONIC_EMPOWERMENT_IMP))
                return false;
            return true;
        }

        void HandleScriptEffect(SpellEffIndex /*effIndex*/)
        {
            if (Creature* targetCreature = GetHitCreature())
            {
                if (targetCreature->isPet())
                {
                    CreatureTemplate const * ci = sObjectMgr->GetCreatureTemplate(targetCreature->GetEntry());
                    switch (ci->family)
                    {
                        case CREATURE_FAMILY_SUCCUBUS:
                            targetCreature->CastSpell(targetCreature, WARLOCK_DEMONIC_EMPOWERMENT_SUCCUBUS, true);
                            break;
                        case CREATURE_FAMILY_VOIDWALKER:
                        {
                            SpellEntry const* spellInfo = sSpellStore.LookupEntry(WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER);
                            int32 hp = int32(targetCreature->CountPctFromMaxHealth(GetCaster()->CalculateSpellDamage(targetCreature, spellInfo, 0)));
                            targetCreature->CastCustomSpell(targetCreature, WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER, &hp, NULL, NULL, true);
                            //unitTarget->CastSpell(unitTarget, 54441, true);
                            break;
                        }
                        case CREATURE_FAMILY_FELGUARD:
                            targetCreature->CastSpell(targetCreature, WARLOCK_DEMONIC_EMPOWERMENT_FELGUARD, true);
                            break;
                        case CREATURE_FAMILY_FELHUNTER:
                            targetCreature->CastSpell(targetCreature, WARLOCK_DEMONIC_EMPOWERMENT_FELHUNTER, true);
                            break;
                        case CREATURE_FAMILY_IMP:
                            targetCreature->CastSpell(targetCreature, WARLOCK_DEMONIC_EMPOWERMENT_IMP, true);
                            break;
                    }
                }
            }
        }

        void Register()
        {
            OnEffect += SpellEffectFn(spell_warl_demonic_empowerment_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warl_demonic_empowerment_SpellScript();
    }
};

// 47422 Everlasting Affliction
class spell_warl_everlasting_affliction : public SpellScriptLoader
{
public:
    spell_warl_everlasting_affliction() : SpellScriptLoader("spell_warl_everlasting_affliction") { }

    class spell_warl_everlasting_affliction_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warl_everlasting_affliction_SpellScript)
        void HandleScriptEffect(SpellEffIndex /*effIndex*/)
        {
            if (Unit* unitTarget = GetHitUnit())
                // Refresh corruption on target
                if (AuraEffect* aur = unitTarget->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_WARLOCK, 0x2, 0, 0, GetCaster()->GetGUID()))
                    aur->GetBase()->RefreshDuration();
        }

        void Register()
        {
            OnEffect += SpellEffectFn(spell_warl_everlasting_affliction_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warl_everlasting_affliction_SpellScript();
    }
};

// 6201 Create Healthstone
class spell_warl_create_healthstone : public SpellScriptLoader
{
public:
    spell_warl_create_healthstone() : SpellScriptLoader("spell_warl_create_healthstone") { }

    class spell_warl_create_healthstone_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warl_create_healthstone_SpellScript)

        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            if (Unit* unitTarget = GetHitUnit())
                CreateItem(effIndex, 5512);
        }

        void Register()
        {
            OnEffect += SpellEffectFn(spell_warl_create_healthstone_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warl_create_healthstone_SpellScript();
    }
};

// 54049 Shadow Bite
class spell_warl_shadow_bite : public SpellScriptLoader
{
public:
    spell_warl_shadow_bite() : SpellScriptLoader("spell_warl_shadow_bite") { }

    class spell_warl_shadow_bite_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warl_shadow_bite_SpellScript)
        bool Validate(SpellEntry const * /*spellEntry*/)
        {
            if (!sSpellStore.LookupEntry(WARLOCK_FELHUNTER_SHADOWBITE_R1))
                return false;
            return true;
        }

        void HandleScriptEffect(SpellEffIndex /*effIndex*/)
        {
            //Unit *caster = GetCaster();
            // Get DoTs on target by owner (15% increase by dot)
            // need to get this here from SpellEffects.cpp ?
            //damage *= float(100.f + 15.f * caster->getVictim()->GetDoTsByCaster(caster->GetOwnerGUID())) / 100.f;
        }

        // Improved Felhunter parts commented--deprecated. removed in cataclysm
        // For Improved Felhunter
        void HandleAfterHitEffect()
        {
            Unit *caster = GetCaster();
            if(!caster) { return; };

            // break if our caster is not a pet
            if(!(caster->GetTypeId() == TYPEID_UNIT && caster->ToCreature()->isPet())) { return; };

            // break if pet has no owner and/or owner is not a player
            Unit *owner = caster->GetOwner();
            if(!(owner && (owner->GetTypeId() == TYPEID_PLAYER))) { return; };
            

            /*int32 amount;
            // rank 1 - 4%
            if(owner->HasAura(WARLOCK_IMPROVED_FELHUNTER_R1)) { amount = 5; };*/

            /*// rank 2 - 8%
            if(owner->HasAura(WARLOCK_IMPROVED_FELHUNTER_R2)) { amount = 9; };*/
            
            // Finally return the Mana to our Caster
            /*if(AuraEffect * aurEff = owner->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_WARLOCK, 214, 0))
                caster->CastCustomSpell(caster,WARLOCK_IMPROVED_FELHUNTER_EFFECT,&amount,NULL,NULL,true,NULL,aurEff,caster->GetGUID());*/
        }

        void Register()
        {
            //OnEffect += SpellEffectFn(spell_warl_shadow_bite_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            AfterHit += SpellHitFn(spell_warl_shadow_bite_SpellScript::HandleAfterHitEffect);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warl_shadow_bite_SpellScript();
    }
};

// 77801 Demon Soul
class spell_warl_demon_soul : public SpellScriptLoader
{
public:
    spell_warl_demon_soul() : SpellScriptLoader("spell_warl_demon_soul") { }

    class spell_warl_demon_soul_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warl_demon_soul_SpellScript)
        bool Validate(SpellEntry const * /*spellEntry*/)
        {
            if (!sSpellStore.LookupEntry(WARLOCK_DEMON_SOUL_SUCCUBUS))
                return false;
            if (!sSpellStore.LookupEntry(WARLOCK_DEMON_SOUL_VOIDWALKER))
                return false;
            if (!sSpellStore.LookupEntry(WARLOCK_DEMON_SOUL_FELGUARD))
                return false;
            if (!sSpellStore.LookupEntry(WARLOCK_DEMON_SOUL_FELHUNTER))
                return false;
            if (!sSpellStore.LookupEntry(WARLOCK_DEMON_SOUL_IMP))
                return false;
            return true;
        }

        void HandleScriptEffect(SpellEffIndex /*effIndex*/)
        {
            if (Creature* targetCreature = GetHitCreature())
            {
                if (targetCreature->isPet())
                {
                    CreatureTemplate const *ci = sObjectMgr->GetCreatureTemplate(targetCreature->GetEntry());
                    Unit *caster = GetCaster();
                    switch (ci->family)
                    {
                        case CREATURE_FAMILY_SUCCUBUS:
                            caster->CastSpell(caster, WARLOCK_DEMON_SOUL_SUCCUBUS, true);
                            break;
                        case CREATURE_FAMILY_VOIDWALKER:
                            caster->CastSpell(caster, WARLOCK_DEMON_SOUL_VOIDWALKER, true);
                            break;
                        case CREATURE_FAMILY_FELGUARD:
                            caster->CastSpell(caster, WARLOCK_DEMON_SOUL_FELGUARD, true);
                            break;
                        case CREATURE_FAMILY_FELHUNTER:
                            caster->CastSpell(caster, WARLOCK_DEMON_SOUL_FELHUNTER, true);
                            break;
                        case CREATURE_FAMILY_IMP:
                            caster->CastSpell(caster, WARLOCK_DEMON_SOUL_IMP, true);
                            break;
                    }
                }
            }
        }

        void Register()
        {
            OnEffect += SpellEffectFn(spell_warl_demon_soul_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warl_demon_soul_SpellScript();
    }
};

void AddSC_warlock_spell_scripts()
{
    new spell_warl_demonic_empowerment();
    new spell_warl_everlasting_affliction();
    new spell_warl_create_healthstone();
    new spell_warl_shadow_bite();
}
