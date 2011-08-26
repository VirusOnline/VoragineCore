/* ScriptData
SDName: Boss_atramedes && its gongs
http://www.wowhead.com/npc=42960
SD%Complete: 52%
SDComment: http://www.wowhead.com/npc=41442, need to make him fly && add a function GetSound() in player class, find the correct opcode for this...
SDCategory: Blackwing Descent
EndScriptData */
/*
Add field sound into the characters table...
uint32 Player::GetSound()
{
    QueryResult result = CharacterDatabase.PQuery("SELECT sound FROM characters WHERE guid = %u",this->GetGUID());
    if(!result)
    {
    sLog.outDebug("Player %u, %s, error while loading the sound from DB",this->GetGUID(),this->GetName());
    return 0;
    }
    uint32 sound = result->Fetch()->GetUint32();
    return sound;

}
*/
#include "ScriptPCH.h"
#include "ScriptedCreature.h"

enum eEnums
{
    SPELL_MODULATION            = 77612,
    SPELL_SEARING_FLAME         = 77840,
    SPELL_SONAR_PULSE           = 77672,
    SPELL_SONIC_BREATH          = 78075,
    SPELL_TRACKING              = 78092,
    SPELL_ATRAMEDES_STUN        = 77611,
    ATRAMEDES_ENTRY             = 41442,

};

class boss_atramedes : public CreatureScript
{
public:
    boss_atramedes() : CreatureScript("boss_atramedes") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new boss_atramedesAI (pCreature);
    }

    struct boss_atramedesAI : public ScriptedAI
    {
        boss_atramedesAI(Creature *c) : ScriptedAI(c) {}

        bool ground;
        bool fly;
        uint32 ground_timer;
        uint32 fly_timer;
        uint32 sonar_timer;
        uint32 modulation_timer;
        uint32 sonic_timer;
        uint32 searing_timer;

        void Reset()
        {
            ground = true;
            fly = false;
            ground_timer = 80000;
            fly_timer = 40000;
            sonar_timer = urand(10000,15000);
            modulation_timer = urand(9000,11000);
            sonic_timer = urand(10000,20000);
            searing_timer = urand(5000,15000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;
            
            if(ground)
            {
                me->SetFlying(false);
                if(sonar_timer <= diff)
                {
                    //Random till the sound function is implemented
                    if(Unit* ptar = SelectTarget(SELECT_TARGET_RANDOM))
                        DoCast(ptar,SPELL_SONAR_PULSE);
                    sonar_timer = urand(10000,15000);
                }
                else
                    sonar_timer -= diff;

                if(modulation_timer <= diff)
                {
                    //Random till the sound function is implemented
                    if(Unit* ptar = SelectTarget(SELECT_TARGET_RANDOM))
                        DoCast(ptar,SPELL_MODULATION);
                    modulation_timer = urand(9000,11000);
                }
                else
                    modulation_timer -= diff;

                if(sonic_timer <= diff)
                {
                    //Random till the sound function is implemented
                    if(Unit* ptar = SelectTarget(SELECT_TARGET_RANDOM))
                        DoCast(ptar,SPELL_SONIC_BREATH);
                    sonic_timer = urand(10000,20000);
                }
                else
                    sonic_timer -= diff;

                if(searing_timer <= diff)
                {
                    //Random till the sound function is implemented
                    if(Unit* ptar = SelectTarget(SELECT_TARGET_RANDOM))
                        DoCast(ptar,SPELL_SEARING_FLAME);
                    searing_timer = urand(5000,15000);
                }
                else
                    searing_timer -= diff;

                fly_timer = 40000;
                if(ground_timer <= diff)
                {
                ground = false;
                fly = true;
                }
                else
                    ground_timer -= diff;
            }

            else if(fly)
            {
                // just 2 spells on phase 2
                me->SetFlying(true);
                if(sonic_timer <= diff)
                {
                    //Random till the sound function is implemented
                    if(Unit* ptar = SelectTarget(SELECT_TARGET_RANDOM))
                        DoCast(ptar,SPELL_SONIC_BREATH);
                    sonic_timer = urand(10000,20000);
                }
                else
                    sonic_timer -= diff;

                
                if(sonar_timer <= diff)
                {
                    //Random till the sound function is implemented
                    if(Unit* ptar = SelectTarget(SELECT_TARGET_RANDOM))
                        DoCast(ptar,SPELL_SONAR_PULSE);
                    sonar_timer = urand(10000,15000);
                }
                else
                    sonar_timer -= diff;
                ground_timer = 80000;
                if(fly_timer <= diff)
                {
                fly = false;
                ground = true;
                }
                else
                fly_timer -= diff;
            }
            DoMeleeAttackIfReady();
        }
    };
};


//Gongs!!!
class atramedes_gong : public CreatureScript
{
public:
    atramedes_gong() : CreatureScript("atramedes_gong") { }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        if(Creature* un = pCreature->FindNearestCreature(ATRAMEDES_ENTRY,90.0f))
        {
        un->StopMoving();
        un->CastStop();
        pCreature->CastSpell(un,SPELL_ATRAMEDES_STUN,true);
        pCreature->DisappearAndDie();
        }
        return true;
    }

};
void AddSC_boss_atramedes()
{
    new atramedes_gong();
    new boss_atramedes();
}