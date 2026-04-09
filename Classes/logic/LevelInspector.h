#ifndef __CARDGAME_LEVEL_INSPECTOR_H__
#define __CARDGAME_LEVEL_INSPECTOR_H__

#include "data/LevelDefinition.h"

#include <string>
#include <vector>

namespace cardgame
{
    struct LevelInspectionResult
    {
        std::string path;
        std::string levelId;
        bool loadSucceeded = false;
        bool valid = false;
        bool solvable = false;
        LevelDefinition definition;
        std::vector<std::string> errors;
    };

    class LevelInspector
    {
    public:
        static LevelInspectionResult inspectFile(const std::string &path);
        static std::vector<LevelInspectionResult> inspectDirectory(const std::string &directory);
    };
}

#endif
