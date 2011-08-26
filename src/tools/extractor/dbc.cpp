/*
 * Copyright (C) 2010-2011 CactusEMU <http://www.cactusemu.com/>
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

#include "dbc.h"
#include "misc.h"
#include "MPQ.h"
#include "DBCFile.h"

#include <set>
#include "Log.h"

void ExtractDataFiles(int locale, bool basicLocale)
{
    Log sLog;

    sLog.Write("Extracting data files...");
    
    std::set<std::pair<int, std::string> > datafiles;
    
    int foundCount = 0;
    
    for(int i = 0; i < PATCH_REV_COUNT + 1; i++)
    {
        TMPQArchive * archive = (TMPQArchive *)localeMPQ[i];
        TFileEntry * pFileEntry = archive->pFileTable;
        TFileEntry * pFileTableEnd = archive->pFileTable + archive->dwFileTableSize;
        
        // Parse the entire block table
        while(pFileEntry < pFileTableEnd)
        {
            // Only take existing files
            if( pFileEntry->dwFlags & MPQ_FILE_EXISTS && 
               (pFileEntry->dwFlags & MPQ_FILE_PATCH_FILE) == 0 && 
               (pFileEntry->dwFlags & MPQ_FILE_DELETE_MARKER) == 0 && 
               pFileEntry->szFileName != NULL)
            {
                std::string name = pFileEntry->szFileName;
                if (i != 0 && i < 7 )
                {
                    if(name.find(langs[locale]) == 0)
                        name = name.substr(strlen(langs[locale]) + 1);
                    else
                    {
                        pFileEntry++;
                        continue;
                    }
                }
                
                if (name.rfind(".dbc") == name.length() - strlen(".dbc") ||
                    name.rfind(".db2") == name.length() - strlen(".db2"))
                {
                    //Verify if this dbc isn't in the list yet. StormLibs return some extra dbcs :P
                    if(i != 0)
                    {
                        bool alreadyExist = false;
                        for(std::set<std::pair<int, std::string> >::iterator itr = datafiles.begin(); itr != datafiles.end(); itr++)
                        {
                            if(itr->second == name)
                            {
                                alreadyExist = true;
                                break;
                            }
                        }
                        if(alreadyExist)
                        {
                            pFileEntry++;
                            continue;
                        }
                    }
                    datafiles.insert(std::pair<int, std::string>(i, name));
                    foundCount++;
                }
            }
            
            // Move to the next file entry
            pFileEntry++;
        }
    }
    sLog.Write("Found %i data files", foundCount);
    
    std::string path = "./dbc/";
    if(!basicLocale)
    {
        path += langs[locale];
        path += "/";
    }
    CreateDir(path);
    
    // extract datas
    int count = 0;
    for (std::set<std::pair<int, std::string> >::iterator iter = datafiles.begin(); iter != datafiles.end(); ++iter)
    {
        std::string filename = path;
        filename += (iter->second.c_str() + strlen("DBFilesClient\\"));
        
        if(ExtractFileToHardDrive(localeMPQ[0], iter->second.c_str(), filename.c_str()) == ERROR_SUCCESS)
            ++count;
        else
        {
            assert(false);
        }
    }
    sLog.Write("Extracted %u data files\n", count);
}

uint32 ReadMapDBC()
{
    Log sLog;

    sLog.Write("Read Map.dbc file... ");
    DBCFile dbc("DBFilesClient\\Map.dbc");
    
    if(!dbc.open())
    {
        sLog.Write("Fatal error: Invalid Map.dbc file format!");
        exit(1);
    }
    
    size_t map_count = dbc.getRecordCount();
    map_ids = new map_id[map_count];
    for(uint32 x = 0; x < map_count; ++x)
    {
        map_ids[x].id = dbc.getRecord(x).getUInt(0);
        strcpy(map_ids[x].name, dbc.getRecord(x).getString(1));
    }
    sLog.Write("Done! (%u maps loaded)", map_count);
    return map_count;
}

void ReadAreaTableDBC()
{
    Log sLog;

    sLog.Write("Read AreaTable.dbc file...");
    DBCFile dbc("DBFilesClient\\AreaTable.dbc");
    
    if(!dbc.open())
    {
        sLog.Write("Fatal error: Invalid AreaTable.dbc file format!");
        assert(false);
    }
    
    size_t area_count = dbc.getRecordCount();
    size_t maxid = dbc.getMaxId();
    areas = new uint16[maxid + 1];
    memset(areas, 0xff, (maxid + 1) * sizeof(uint16));
    
    for(uint32 x = 0; x < area_count; ++x)
        areas[dbc.getRecord(x).getUInt(0)] = dbc.getRecord(x).getUInt(3);
    
    maxAreaId = dbc.getMaxId();
    
    sLog.Write("Done! (%u areas loaded)", area_count);
}

void ReadLiquidTypeTableDBC()
{
    Log sLog;

    sLog.Write("Read LiquidType.dbc file...");
    DBCFile dbc("DBFilesClient\\LiquidType.dbc");
    if(!dbc.open())
    {
        sLog.Write("Fatal error: Invalid LiquidType.dbc file format!");
        exit(1);
    }
    
    size_t LiqType_count = dbc.getRecordCount();
    size_t LiqType_maxid = dbc.getMaxId();
    LiqType = new uint16[LiqType_maxid + 1];
    memset(LiqType, 0xff, (LiqType_maxid + 1) * sizeof(uint16));
    
    for(uint32 x = 0; x < LiqType_count; ++x)
        LiqType[dbc.getRecord(x).getUInt(0)] = dbc.getRecord(x).getUInt(3);
    
    sLog.Write("Done! (%u LiqTypes loaded)", LiqType_count);
}