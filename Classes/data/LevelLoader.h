#ifndef __CARDGAME_LEVEL_LOADER_H__
#define __CARDGAME_LEVEL_LOADER_H__

#include "data/LevelDefinition.h"

#include <string>

namespace cardgame
{
    class LevelLoader
    {
    public:
        static bool loadFromJsonFile(const std::string &path, LevelDefinition &outDefinition, std::string *outErrorMessage = nullptr);
    };
}

#endif
