#ifndef CARDGAME_LEVEL_SOLVER_H
#define CARDGAME_LEVEL_SOLVER_H

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
