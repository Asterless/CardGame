#include "logic/GameCommand.h"

namespace cardgame
{
    const Optional<GameDelta> &GameCommand::getDelta() const
    {
        return _delta;
    }

    bool DrawStockCommand::execute(GameState &state)
    {
        _delta = state.drawFromStock();
        return _delta.has_value();
    }

    bool DrawStockCommand::undo(GameState &state)
    {
        return _delta.has_value() && state.undoDrawFromStock(*_delta);
    }

    MatchTableauCommand::MatchTableauCommand(int cardId)
        : _cardId(cardId)
    {
    }

    bool MatchTableauCommand::execute(GameState &state)
    {
        _delta = state.matchTableau(_cardId);
        return _delta.has_value();
    }

    bool MatchTableauCommand::undo(GameState &state)
    {
        return _delta.has_value() && state.undoMatchTableau(*_delta);
    }
}
