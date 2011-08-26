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

#include "headers.h"
#include "MPQ.h"
#include "Log.h"

void GetMPQFileName(char* filename, int rev, const char* input_path, const char* locale = NULL )
{
    
    if(rev >= 13914)
    {
        if(locale != NULL)
            sprintf(filename, "%s/Data/%s/wow-update-%s-%i.MPQ", input_path, locale, locale, rev);
        else
            sprintf(filename, "%s/Data/wow-update-base-%i.MPQ", input_path, rev);
        
        return;
    }

    sprintf(filename, "%s/Data/wow-update-%i.MPQ", input_path, rev);
}

void LoadLocaleMPQFiles(int const locale)
{
    Log sLog;

    char filename[512];
    
    //Locale-xxXX.MPQ
    sprintf(filename,"%s/Data/%s/locale-%s.MPQ", input_path, langs[locale], langs[locale]);
    sLog.Write("Loading %s", filename);
    if(!SFileOpenArchive(filename, 0, MPQ_OPEN_READ_ONLY, &localeMPQ[0]))
    {
        sLog.Write("%i", GetLastError());
        assert(false && "\nLoad of MPQ failed");
    }
    
    for(int i = 0; i < PATCH_REV_COUNT; ++i)
    {    
        bool result;
        if(patchRev[i] >= 13914)
        {
            GetMPQFileName(filename, patchRev[i], input_path, langs[locale]);    
            result = SFileOpenPatchArchive(localeMPQ[0], filename, NULL, MPQ_OPEN_READ_ONLY);
        }
        else
        {
            GetMPQFileName(filename, patchRev[i], input_path);
            result = SFileOpenPatchArchive(localeMPQ[0], filename, langs[locale], MPQ_OPEN_READ_ONLY);
        }
        if(!result)
        {
            sLog.Write("%i", GetLastError());
            assert(false && "Load of MPQ patch failed");
        }
    }
    if(!SFileIsPatchedArchive(localeMPQ[0]))
        assert(false && "An error occured");
    
    //Others
    for(int i = 0; i < PATCH_REV_COUNT; ++i)
    {
        GetMPQFileName(filename, patchRev[i], input_path, langs[locale]);
        sLog.Write("Loading %s", filename);
        if(!SFileOpenArchive(filename, 0, MPQ_OPEN_READ_ONLY, &localeMPQ[i+1]))
        {
            sLog.Write("%i", GetLastError());
            assert(false && "Load of MPQ failed");
        }
        
        for(int j = i + 1; j < PATCH_REV_COUNT; ++j)
        {
            bool result;
            if(patchRev[j] >= 13914)
            {
                GetMPQFileName(filename, patchRev[j], input_path, langs[locale]);    
                result = SFileOpenPatchArchive(localeMPQ[i+1], filename, NULL, MPQ_OPEN_READ_ONLY);
            }
            else
            {
                GetMPQFileName(filename, patchRev[j], input_path);
                result = SFileOpenPatchArchive(localeMPQ[i+1], filename, langs[locale], MPQ_OPEN_READ_ONLY);
            }
            if(!result)
            {
                sLog.Write("%j", GetLastError());
                assert(false && "Load of MPQ patch failed");
            }
        }
        if(!SFileIsPatchedArchive(localeMPQ[i+1]) && i != PATCH_REV_COUNT - 1)
            assert(false && "An error occured");
    }
}

void LoadMapMPQFiles()
{
    Log sLog;

    char filename[512];
    
    //Locale-xxXX.MPQ
    sprintf(filename,"%s/Data/world.MPQ", input_path);
    sLog.Write("Loading %s", filename);
    if(!SFileOpenArchive(filename, 0, MPQ_OPEN_READ_ONLY, &WorldMPQ))
    {
        sLog.Write("%i", GetLastError());
        assert(false && "\nLoad of MPQ failed");
    }
    
    for(int i = 0; i < PATCH_REV_COUNT; ++i)
    {
        char ext[7] = "";
        sprintf(ext, "-%i", patchRev[i]);
        
        sprintf(filename,"%s/Data/wow-update%s%s.MPQ", input_path, i < 6 ? "" : "-base", ext); // IMPORTANT NOTE: WE MUST CHECK FILE PATH AT EVERY PATCH
        //sLog.Write("    -%i", patchRev[i]); //  THIS IS NOT AN ERROR!! JUST A REPORT.
        if(!SFileOpenPatchArchive(WorldMPQ, filename, i < 6 ? "base" : NULL, MPQ_OPEN_READ_ONLY))
        {
            sLog.Write("%i", GetLastError());
            assert(false && "Load of MPQ patch failed");
        }
    }
    if(!SFileIsPatchedArchive(WorldMPQ))
        assert(false && "An error occured");
    
    for(int j = 0; j < MAX_EXPANSION; j++)
    {
        sprintf(filename, "%s/Data/expansion%u.MPQ", input_path, j+1);
        sLog.Write("Loading %s", filename);
        if(!SFileOpenArchive(filename, 0, MPQ_OPEN_READ_ONLY, &ExpansionsMPQ[j]))
        {
            sLog.Write("%i", GetLastError());
            assert(false && "\nLoad of MPQ failed");
        }
        
        if(!IsValidMpqHandle((TMPQArchive*)ExpansionsMPQ[j]))
        {
            sLog.Write("Load of Expansion%u.MPQ Failed!", j+1);
            sLog.Write("Please verify you downloaded all the MPQs. You should replace\n'SET accountType \"xx\"'\nin your WTF/config.wtf and WTF/launcher.wtf by\n'SET accountType \"CT\"'\nand then restart your launcher");
            exit(1);
        }
        
        for(int i = 0; i < PATCH_REV_COUNT; ++i)
        {
            char ext[7] = "";
            sprintf(ext, "-%i", patchRev[i]);
            
            sprintf(filename,"%s/Data/wow-update%s%s.MPQ", input_path, i < 6 ? "" : "-base", ext); // IMPORTANT NOTE: WE MUST CHECK FILE PATH AT EVERY PATCH
            //sLog.Write("    -%i", patchRev[i]); //  THIS IS NOT AN ERROR!! JUST A REPORT.
            if(!SFileOpenPatchArchive(ExpansionsMPQ[j], filename, i < 6 ? "base" : NULL, MPQ_OPEN_READ_ONLY))
            {
                sLog.Write("%i", GetLastError());
                assert(false && "Load of MPQ patch failed");
            }
        }
    }
}

int ExtractFileToHardDrive(HANDLE &MPQ_handle, const char * szArchivedFile, const char * szFileName)
{
    Log sLog;

    HANDLE hFile  = NULL;          // Archived file handle
    TFileStream* handle = NULL;          // Disk file handle
    int    nError = ERROR_SUCCESS; // Result value
    
    if(nError == ERROR_SUCCESS)            
    {
        if(!SFileOpenFileEx(MPQ_handle, szArchivedFile, SFILE_OPEN_PATCHED_FILE, &hFile))
            nError = GetLastError();
    }
    
    // Create the target file
    if(nError == ERROR_SUCCESS)
    {
        handle = FileStream_CreateFile(szFileName);
        if(handle == NULL)
            nError = GetLastError();
    }
    
    // Read the file from the archive
    if(nError == ERROR_SUCCESS)
    {
        // Get the size of the full patched file
        DWORD dwFileSize = SFileGetFileSize(hFile, NULL);
        if(dwFileSize != 0)
        {
            // Allocate space for the full file
            BYTE * pbFullFile = new BYTE[dwFileSize];
            if(!SFileReadFile(hFile, pbFullFile, dwFileSize))
            {           
                nError = GetLastError();
                sLog.Write("Failed to read full patched file data \"%s\"", szFileName);
                assert(false);
            }
            FileStream_Write(handle, NULL, pbFullFile, dwFileSize);
            delete [] pbFullFile;
        }
    }
    
    // Cleanup and exit
    if(handle != NULL)
        FileStream_Close(handle);
    if(hFile != NULL)
        SFileCloseFile(hFile);
    
    return nError;
}

char* ExtractFileToMemory(HANDLE &MPQ_handle, const char * szArchivedFile, int & size )
{
    Log sLog;

    HANDLE hFile  = NULL;          // Archived file handle
    int    nError = ERROR_SUCCESS; // Result value
    char * pbFullFile = NULL;
    
    if(nError == ERROR_SUCCESS)            
    {
        if(!SFileOpenFileEx(MPQ_handle, szArchivedFile, SFILE_OPEN_PATCHED_FILE, &hFile))
            nError = GetLastError();
    }
    
    // Read the file from the archive
    if(nError == ERROR_SUCCESS)
    {
        // Get the size of the full patched file
        size = SFileGetFileSize(hFile, NULL);
        if(size != 0)
        {
            // Allocate space for the full file
            pbFullFile = new char[size];
            if(!SFileReadFile(hFile, pbFullFile, size))
            {           
                nError = GetLastError();
                sLog.Write("Failed to read full patched file data \"%s\"", szArchivedFile);
                assert(false);
            }
        }
    }
    
    // Cleanup and exit
    if(nError == ERROR_SUCCESS && hFile != NULL)
        SFileCloseFile(hFile);
    
    return pbFullFile;
}