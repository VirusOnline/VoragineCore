#ifndef DEF_THELOSTCITYOFTOLVIR_H
#define DEF_THELOSTCITYOFTOLVIR_H

enum Creatures
{
    // Husam encounter
    CREATURE_HUSAM                    = 44577,
    CREATURE_LAND_MINE                = 44840,
    // Barim encounter
    CREATURE_BARIM                    = 43612,
    CREATURE_BLAZE_OF_HEAVENS         = 48906,
    CREATURE_HARBINGER_OF_DARKNESS    = 43927,
    CREATURE_SOUL_FRAGMENT            = 43934,
    CREATURE_VEIL_OF_TWILIGHT         = 43926,
    CREATURE_HEAVENS_FURY             = 43801,
    // Lockmaw encounter
    CREATURE_LOCKMAW                  = 43614,
    CREATURE_FRENZIED_CROCOLISK       = 43658,
    CREATURE_AUGH                     = 49045,
    CREATURE_AUGH_SCENT               = 45379,
    CREATURE_AUGH_WHIRLWIND           = 45378,
    // Siamat encounter
    CREATURE_SIAMAT                   = 44819,
};

enum Data
{
    DATA_HUSAM      = 0,
    DATA_BARIM      = 1,
    DATA_LOCKMAW    = 2,
    DATA_AUGH       = 3,
    DATA_SIAMAT     = 4
};

#define MAX_ENCOUNTER 5

#endif
