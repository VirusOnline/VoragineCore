#include "ScriptPCH.h"
#include "the_lost_city_of_tol_vir.h"

class instance_the_lost_city_of_tol_vir : public InstanceMapScript
{
    public:
        instance_the_lost_city_of_tol_vir() : InstanceMapScript("instance_the_lost_city_of_tol_vir", 755) { }

        struct instance_the_lost_city_of_tol_vir_InstanceMapScript : public InstanceScript
        {
            instance_the_lost_city_of_tol_vir_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(MAX_ENCOUNTER);
            }

            void Initialize()
            {
            }

            /*void OnCreatureCreate(Creature* creature)
            {
                switch (creature->GetEntry())
                {
                    default:
                        break;
                }
            }

            uint64 GetData64(uint32 identifier)
            {
                switch (identifier)
                {
                    default:
                        break;
                }

                return 0;
            }*/

        private:
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_the_lost_city_of_tol_vir_InstanceMapScript(map);
        }
};

void AddSC_instance_the_lost_city_of_tol_vir()
{
    new instance_the_lost_city_of_tol_vir();
}
