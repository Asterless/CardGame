#include "logic/GameCommand.h"

namespace cardgame
{
    const GameDelta &GameCommand::getDelta() const
    {
        return _delta;
    }

    bool DrawStockCommand::execute(GameState &state)
    {
        _delta = GameDelta();
        return state.drawFromStock(_delta);
    }

    bool DrawStockCommand::undo(GameState &state)
    {
        return state.undoDrawFromStock(_delta);
    }

    MatchTableauCommand::MatchTableauCommand(int cardId)
        : _cardId(cardId)
    {
    }

    bool MatchTableauCommand::execute(GameState &state)
    {
        _delta = GameDelta();
        return state.matchTableau(_cardId, _delta);
    }

    bool MatchTableauCommand::undo(GameState &state)
    {
        return state.undoMatchTableau(_delta);
    }
}
