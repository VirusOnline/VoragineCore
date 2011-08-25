#ifndef TRINITYCORE_LUA_H
#define TRINITYCORE_LUA_H

#include "Common.h"
#include <ace/Singleton.h>
#include "LuaInc.h"
#include "Player.h"
#include "Creature.h"
#include "Unit.h"
#include "GossipDef.h"
#include "NPCHandler.h"
#include "Pet.h"
#include "QuestDef.h"
#include "ReputationMgr.h"
#include "Util.h"                                           // for Tokens typedef
#include "WorldSession.h"
#include "ScriptedGossip.h"

class PlayerMenu;

struct CreatureLuaScript
{
	uint32 type;
    const char* name;
};

struct PlayerLuaScript
{
	uint32 type;
    const char* name;
};

enum ePlayerLuaEvents
{
    eLuaPlayerChat                 = 0,
    eLuaPlayerPvPKill              = 1,
    eLuaPlayerCreatureKill         = 2,
    eLuaPlayerKilledByCreature     = 3,
    eLuaPlayerLevelChanged         = 4,
};

enum eCreatureLuaEvents
{
    /* AI functions */
    eLuaCreatureEnterCombat        = 0,
    eLuaCreatureLeaveCombat        = 1,
    eLuaCreatureKilled             = 2,
    eLuaCreatureKill               = 3,
    /* Gossip functions */
    eLuaCreatureGossipHello        = 4,
};

typedef std::list<CreatureLuaScript> CreatureLuaEvents;
typedef UNORDERED_MAP<uint32,CreatureLuaEvents> CreatureLuaScripts;
typedef std::list<PlayerLuaScript> PlayerLuaScripts;

class LuaEngine
{
    friend class ACE_Singleton<LuaEngine, ACE_Thread_Mutex>;
    LuaEngine();
    ~LuaEngine();
    
    public:
        void Initialize(); // Initializes the Lua engine
        void AddHooks(); // Adds the hooks to the functions in lua
        void Reload(); // Reload the lua files
        void InsertPlayerEvent(PlayerLuaScript pls)
        {
            playerscripts.push_back(pls);
        }
        
        /* Player Scripts */
        void LuaPlayerChat(Player* plr, uint32 type, uint32 lang, std::string msg);
        void LuaPlayerPvPKill(Player *killer, Player *killed);
        void LuaPlayerCreatureKill(Player* killer, Creature* killed);
        void LuaPlayerKilledByCreature(Creature* killer, Player* killed);
        void LuaPlayerLevelChanged(Player* plr, uint8 newlevel);
        /* Creature Scripts*/
        void LuaCreatureEnterCombat(Creature* cr, Unit* victim);
        bool LuaCreatureGossipHello(Player* player, Creature* creature);
    protected:
        lua_State* m_luaState;
        const char* m_file;
    public:
        CreatureLuaScripts creaturescripts;
        PlayerLuaScripts playerscripts;
};

class LuaHandlers
{
    LuaHandlers();
    ~LuaHandlers();
    
    public:
        static void outString(const char* msg) // Handles the output from lua
		{
			sLog->outString("%s",msg);
		}
};

#define sLua ACE_Singleton<LuaEngine, ACE_Thread_Mutex>::instance()

#endif