#include "Common.h"
#include "LuaEngine.h"
#include "LuaInc.h"

void RegisterCreatureEvent(int entry, int type, const char* funcName)
{
    CreatureLuaScript cls;
    cls.type = type;
    cls.name = funcName;
    
    CreatureLuaScripts::iterator it = sLua->creaturescripts.find(entry);
    if(it != sLua->creaturescripts.end()) // Its already there
    {
        (*it).second.push_back(cls);
        sLua->creaturescripts[entry] = (*it).second;
        return;
    }
    else
    {
        CreatureLuaEvents cle;
        cle.push_back(cls);
        sLua->creaturescripts[entry] = cle;
    }
}

void RegisterPlayerEvent(int type, const char* funcName)
{
    PlayerLuaScript crls;
    
    crls.type = uint32(type);
    crls.name = funcName;
    
    sLua->InsertPlayerEvent(crls);
    sLog->outString("Added function %s for player",funcName);
}

/* Proxies */
void MonsterSay(Creature* cr, const char* text)
{
    cr->MonsterSay(text,0,NULL);
}

bool HasAura(Unit* pUnit, uint32 spellid)
{
    return pUnit->HasAura(spellid);
}

void SendGossipMenu(Player* plr, uint32 id, uint64 crGUID)
{
    plr->PlayerTalkClass->SendGossipMenu(id, crGUID);
}

void add_gossip_item(Player* plr, uint32 icon, const char* item, uint32 def)
{
    plr->ADD_GOSSIP_ITEM(icon,item,GOSSIP_SENDER_MAIN,def);
}

LuaEngine::LuaEngine() : m_luaState(NULL) { }

LuaEngine::~LuaEngine()
{
    lua_close(m_luaState);
    m_file = NULL;
    m_luaState = NULL;
}

void LuaEngine::Initialize()
{
    m_luaState = lua_open(); // Create the handle for Lua
    int iErr = 0;
    if(!m_luaState)
    {
        sLog->outString("Something wrong happened to the Lua engine");
        return;
    }
	luabind::open(m_luaState);
    luaL_openlibs(m_luaState); // Opent the correct libraries
    m_file = "./LuaScripts.lua"; // The file
    AddHooks(); // Call this to add the lua functions -> c++ functions hook
    if(luaL_dofile(m_luaState,m_file))
	{
		sLog->outString("Lua engine error: %s",lua_tostring(m_luaState,1));
		return;
	}
    sLog->outString("Lua engine initialized!");
}

void LuaEngine::AddHooks()
{
    sLog->outString("Initializing hooks");
    luabind::module(m_luaState)
		[
			luabind::def("RegisterCreatureEvent",&RegisterCreatureEvent),
			luabind::def("RegisterPlayerEvent",&RegisterPlayerEvent),
			luabind::def("outString",&LuaHandlers::outString),
            luabind::def("MonsterSay",&MonsterSay),
            luabind::def("HasAura",&HasAura),
            luabind::def("SendGossipMenu",&SendGossipMenu),
            luabind::def("ADD_GOSSIP_ITEM",&add_gossip_item)
        ];
    /* Player Class */
    luabind::module(m_luaState)
        [
            luabind::class_<Unit>("Unit"),
            luabind::class_<Player,Unit>("Player")
            .def((const char*)"GetName",&Player::GetName)
            .def((const char*)"GetUInt32Value",&Player::GetUInt32Value)
            .def((const char*)"GetUInt64Value",&Player::GetUInt64Value)
            .def((const char*)"SetUInt64Value",&Player::SetUInt64Value)
            .def((const char*)"SetUInt32Value",&Player::SetUInt32Value)
            .def((const char*)"TeleportTo",(bool(Player::*)(uint32,float,float,float,float,uint32))&Player::TeleportTo)
            .def((const char*)"GetGUID",&Player::GetGUID)
            .def((const char*)"GetGUIDLow",&Player::GetGUIDLow)
        ];
    /* Creature Class */
    luabind::module(m_luaState)
        [
            luabind::class_<Creature,Unit>("Creature")
            .def((const char*)"GetGUID",&Creature::GetGUID)
            .def((const char*)"GetGUIDLow",&Creature::GetGUIDLow)
            .def((const char*)"GetName",&Creature::GetName)
            .def((const char*)"GetEntry",&Creature::GetEntry)
        ];
    sLog->outString("Hooks initialized");
}
void LuaEngine::Reload()
{
    sLog->outString("Re-loading the lua scripts");
    lua_close(m_luaState);
    playerscripts.clear();
    creaturescripts.clear();
    m_luaState = lua_open();
    if(!m_luaState)
    {
        sLog->outString("Something wrong happened while reloading the Lua engine");
        return;
    }
	luabind::open(m_luaState);
    luaL_openlibs(m_luaState); // Opent the correct libraries
    AddHooks(); // Call this to add the lua functions -> c++ functions hook
    if(luaL_dofile(m_luaState,m_file))
	{
		sLog->outString("Lua engine error: %s",lua_tostring(m_luaState,1));
		return;
	}
    sLog->outString("Lua engine reloaded!");
}
/* Script system hooks */

// Called when a player chats
void LuaEngine::LuaPlayerChat(Player* plr, uint32 type, uint32 lang, std::string msg)
{
    sLog->outString("Calling LuaPlayerChat");
    for(PlayerLuaScripts::iterator itr = playerscripts.begin(); itr != playerscripts.end(); ++itr)
        if((*itr).type == eLuaPlayerChat)
            luabind::call_function<void>(m_luaState,(*itr).name,plr,type,lang,msg);
}

// Called when a player kills another player in PvP
void LuaEngine::LuaPlayerPvPKill(Player *killer, Player *killed)
{
    for(PlayerLuaScripts::iterator itr = playerscripts.begin(); itr != playerscripts.end(); ++itr)
        if((*itr).type == eLuaPlayerPvPKill)
            luabind::call_function<void>(m_luaState,(*itr).name,killer,killed);
}

// Called when a player kills a creature
void LuaEngine::LuaPlayerCreatureKill(Player* killer, Creature* killed)
{
    try
    {
        for(PlayerLuaScripts::iterator itr = playerscripts.begin(); itr != playerscripts.end(); ++itr)
            if((*itr).type == eLuaPlayerCreatureKill)
                luabind::call_function<void>(m_luaState,(*itr).name,killer,killed);
    }
    catch(luabind::cast_failed& e)
    {
        sLog->outString("LuaBind cast_failed Exception: %s",e.what());
    }
}

// Called when a creature kills a player
void LuaEngine::LuaPlayerKilledByCreature(Creature* killer, Player* killed)
{
    for(PlayerLuaScripts::iterator itr = playerscripts.begin(); itr != playerscripts.end(); ++itr)
        if((*itr).type == eLuaPlayerKilledByCreature)
            luabind::call_function<void>(m_luaState,(*itr).name,killer,killed);
}

// Called when a player changes its level
void LuaEngine::LuaPlayerLevelChanged(Player* plr, uint8 newlevel)
{
    for(PlayerLuaScripts::iterator itr = playerscripts.begin(); itr != playerscripts.end(); ++itr)
        if((*itr).type == eLuaPlayerLevelChanged)
            luabind::call_function<void>(m_luaState,(*itr).name,plr,newlevel);
}

// Called when a creature engages combat
void LuaEngine::LuaCreatureEnterCombat(Creature* cr, Unit* victim)
{
    for(CreatureLuaScripts::iterator itr = creaturescripts.begin(); itr != creaturescripts.end(); ++itr)
        if((*itr).first == cr->GetEntry())
            for(CreatureLuaEvents::iterator it2 = (*itr).second.begin(); it2 != (*itr).second.end(); ++it2)
                if((*it2).type == eLuaCreatureEnterCombat)
                    luabind::call_function<void>(m_luaState,(*it2).name,cr,victim);
}

// Called when a player opens a gossip window with the creature
bool LuaEngine::LuaCreatureGossipHello(Player* player, Creature* creature)
{
    for(CreatureLuaScripts::iterator itr = creaturescripts.begin(); itr != creaturescripts.end(); ++itr)
        if((*itr).first == creature->GetEntry())
            for(CreatureLuaEvents::iterator it2 = (*itr).second.begin(); it2 != (*itr).second.end(); ++it2)
                if((*it2).type == eLuaCreatureGossipHello)
                {
                    luabind::call_function<void>(m_luaState,(*it2).name,player,creature);
                    return true;
                }
    return false;
}