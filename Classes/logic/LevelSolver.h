#ifndef __CARDGAME_LEVEL_SOLVER_H__
#define __CARDGAME_LEVEL_SOLVER_H__

#include "data/LevelDefinition.h"

namespace cardgame
{
    class LevelSolver
    {
    public:
        static bool canClearTableau(const LevelDefinition &definition);
    };
}

#endif
