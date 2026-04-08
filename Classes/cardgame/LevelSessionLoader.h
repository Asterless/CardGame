#ifndef __CARDGAME_LEVEL_SESSION_LOADER_H__
#define __CARDGAME_LEVEL_SESSION_LOADER_H__

#include "GameState.h"
#include "tools/LevelDefinition.h"

#include <string>
#include <vector>

namespace cardgame
{
struct LevelSessionLoadResult
{
    GameState state;
    LevelDefinition definition;
    bool hasLoadedDefinition = false;
    bool solvable = false;
    std::vector<std::string> logs;
    std::vector<std::string> errors;
};

class LevelSessionLoader
{
public:
    static LevelSessionLoadResult load(const std::string& levelDirectory, const std::string& levelPath);
};
}

#endif
