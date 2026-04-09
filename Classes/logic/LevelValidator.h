#ifndef __CARDGAME_LEVEL_VALIDATOR_H__
#define __CARDGAME_LEVEL_VALIDATOR_H__

#include "data/LevelDefinition.h"

#include <string>
#include <vector>

namespace cardgame
{
    class LevelValidator
    {
    public:
        static bool validate(const LevelDefinition &definition, std::vector<std::string> &outErrors);
    };
}

#endif
