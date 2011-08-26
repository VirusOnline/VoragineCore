/*
 * Copyright (C) 2010-2011 VoragineCore <http://www.projectvoragine.com/>
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

#include "Log.h"
#include <stdarg.h>

using namespace std;

Log::Log() :
    logfile(NULL)
{
    Initialize();
}

void Log::Initialize()
{
    remove("vmap3assembler.log");
    logfile = fopen("vmap3assembler.log", "a");
}

Log::~Log()
{
    if(logfile != NULL)
        fclose(logfile);
    logfile = NULL;
}

void Log::Write(char* format, ...)
{
    if (!format)
        return;

    va_list ap;
    va_start(ap, format);
    vfprintf(logfile, format, ap);
    vfprintf(stdout, format, ap); // To screen
    printf("\n");
    va_end(ap);
    fprintf(logfile, "\n");

    fflush(logfile);
}
