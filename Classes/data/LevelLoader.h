#ifndef CARDGAME_LEVEL_LOADER_H
#define CARDGAME_LEVEL_LOADER_H

#include "data/LevelDefinition.h"
#include "support/Optional.h"

#include <string>

namespace cardgame
{
    struct LevelLoadResult
    {
        Optional<LevelDefinition> definition;
        std::string errorMessage;

        explicit operator bool() const
        {
            return definition.has_value();
        }
    };

    class LevelLoader
    {
    public:
        static LevelLoadResult loadFromJsonFile(const std::string &path);
    };
}

#endif
