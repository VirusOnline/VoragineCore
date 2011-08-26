#include <string>
#include <iostream>

#include "TileAssembler.h"
#include "Log.h"

int main(int argc, char* argv[])
{
    Log sLog;

    if(argc != 3)
    {
        //printf("\nusage: %s <raw data dir> <vmap dest dir> [config file name]\n", argv[0]);
        //std::cout << "usage: " << argv[0] << " <raw data dir> <vmap dest dir>" << std::endl;
        sLog.Write("Usage: %c <raw data dir> <vmap dest dir>", argv[0]);
        return 1;
    }

    std::string src = argv[1];
    std::string dest = argv[2];

    //std::cout << "using " << src << " as source directory and writing output to " << dest << std::endl;
    sLog.Write("Using: %s as source directory and writing output to %s", src.c_str(), dest.c_str());
    VMAP::TileAssembler* ta = new VMAP::TileAssembler(src, dest);

    if(!ta->convertWorld2())
    {
        //std::cout << "exit with errors" << std::endl;
        sLog.Write("exit with errors");
        delete ta;
        return 1;
    }

    delete ta;
    //std::cout << "Ok, all done" << std::endl;
    sLog.Write("Ok, all done");
    return 0;
}
