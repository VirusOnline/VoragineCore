/*
 * Copyright (C) 2005-2011 MaNGOS <http://www.getmangos.com/>
 *
 * Copyright (C) 2008-2011 Trinity <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2011 Project Voragine <http://www.projectvoragine.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _CONFIGDATABASE_H
#define _CONFIGDATABASE_H

#include "DatabaseWorkerPool.h"
#include "MySQLConnection.h"

class ConfigDatabaseConnection : public MySQLConnection
{
    public:
        //- Constructors for sync and async connections
        ConfigDatabaseConnection(MySQLConnectionInfo& connInfo) : MySQLConnection(connInfo) {}
        ConfigDatabaseConnection(ACE_Activation_Queue* q, MySQLConnectionInfo& connInfo) : MySQLConnection(q, connInfo) {}

        //- Loads databasetype specific prepared statements
        void DoPrepareStatements();
};

typedef DatabaseWorkerPool<ConfigDatabaseConnection> ConfigDatabaseWorkerPool;

enum ConfigDatabaseStatements
{
    /*  Naming standard for defines:
        {DB}_{SET/DEL/ADD/REP}_{Summary of data changed}
        When updating more than one field, consider looking at the calling function
        name for a suiting suffix.
    */



	MAX_CONFIGDATABASE_STATEMENTS,

};
#endif