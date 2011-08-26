#include "grimbatol.h"
#include "ScriptPCH.h"

class instance_grimbatol : public InstanceMapScript
{
public:
    instance_grimbatol() : InstanceMapScript("instance_grimbatol", 670) { }

    InstanceScript* GetInstanceScript(InstanceMap* pMap) const
    {
        return new instance_grimbatol_InstanceMapScript(pMap);
    }

    struct instance_grimbatol_InstanceMapScript : public InstanceScript
    {
        instance_grimbatol_InstanceMapScript(Map* pMap) : InstanceScript(pMap)
        {
            SetBossNumber(MAX_ENCOUNTER);
        }
        
        uint64 uiGeneralUmbriss;
        uint64 uiForgemasterThrongus;
        uint64 uiDrahgaShadowburner;
        uint64 uiErudax;
        
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        
        std::string str_data;
        
        void Initialize()
        {
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                m_auiEncounter[i] = NOT_STARTED;
                
            uiGeneralUmbriss = 0;
            uiForgemasterThrongus = 0;
            uiDrahgaShadowburner = 0;
            uiErudax = 0;
        }
        
        bool IsEncounterInProgress() const
        {
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                if (m_auiEncounter[i] == IN_PROGRESS)
                    return true;
            return false;
        }
        
        void OnCreatureCreate(Creature* creature)
        {
            switch(creature->GetEntry())
            {
                case BOSS_GENERAL_UMBRISS:         uiGeneralUmbriss = creature->GetGUID(); break;
                case BOSS_FORGEMASTER_THRONGUS:    uiForgemasterThrongus = creature->GetGUID(); break;
                case BOSS_DRAHGA_SHADOWBURNER:     uiDrahgaShadowburner = creature->GetGUID(); break;
                case BOSS_ERUDAX:                  uiErudax = creature->GetGUID(); break;
            }
        }
        
        void SetData(uint32 type, uint32 data)
        {
            switch(type)
            {
                case DATA_GENERAL_UMBRISS_EVENT:
                    m_auiEncounter[0] = data;
                    break;
                case DATA_FORGEMASTER_THRONGUS_EVENT:
                    m_auiEncounter[1] = data;
                    break;
                case DATA_DRAHGA_SHADOWBURNER_EVENT:
                    m_auiEncounter[2] = data;
                    break;
                case DATA_ERUDAX_EVENT:
                    m_auiEncounter[3] = data;
                    break;
                }

                if (data == DONE)
                    SaveToDB();
        }
        
        uint32 GetData(uint32 type)
        {
            switch(type)
            {
                case DATA_GENERAL_UMBRISS_EVENT:               return m_auiEncounter[0];
                case DATA_FORGEMASTER_THRONGUS_EVENT:          return m_auiEncounter[1];
                case DATA_DRAHGA_SHADOWBURNER_EVENT:           return m_auiEncounter[2];
                case DATA_ERUDAX_EVENT:                        return m_auiEncounter[3];
            }
            return 0;
        }
        
        uint64 GetData64(uint32 identifier)
        {
            switch(identifier)
            {
                case DATA_GENERAL_UMBRISS:             return uiGeneralUmbriss;
                case DATA_FORGEMASTER_THRONGUS:        return uiForgemasterThrongus;
                case DATA_DRAHGA_SHADOWBURNER:         return uiDrahgaShadowburner;
                case DATA_ERUDAX:                      return uiErudax;
            }
            return 0;
        }
        
        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << "G B " << m_auiEncounter[0] << " " << m_auiEncounter[1] << " "
                << m_auiEncounter[2] << " " << m_auiEncounter[3];

            str_data = saveStream.str();

            OUT_SAVE_INST_DATA_COMPLETE;
            return str_data;
        }

        void Load(const char* in)
        {
            if (!in)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return;
            }

            OUT_LOAD_INST_DATA(in);

            char dataHead1, dataHead2;
            uint16 data0, data1, data2, data3;

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3;

            if (dataHead1 == 'G' && dataHead2 == 'B')
            {
                m_auiEncounter[0] = data0;
                m_auiEncounter[1] = data1;
                m_auiEncounter[2] = data2;
                m_auiEncounter[3] = data3;

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (m_auiEncounter[i] == IN_PROGRESS)
                        m_auiEncounter[i] = NOT_STARTED;

            } else OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };
};

void AddSC_instance_grimbatol()
{
    new instance_grimbatol();
}
