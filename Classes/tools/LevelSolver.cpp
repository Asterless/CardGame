#include "tools/LevelSolver.h"

#include "cardgame/GameState.h"

#include <algorithm>
#include <set>
#include <sstream>
#include <vector>

namespace cardgame
{
namespace
{
std::string buildStateKey(const GameState& state)
{
    std::ostringstream stream;
    const std::unordered_map<int, CardData>& cards = state.getCards();

    stream << "T";
    const std::vector<int>& tableauSlots = state.getTableauSlots();
    for (std::size_t i = 0; i < tableauSlots.size(); ++i)
    {
        stream << "," << tableauSlots[i];
    }

    stream << "|S";
    const std::vector<int>& stockPile = state.getStockPile();
    for (std::size_t i = 0; i < stockPile.size(); ++i)
    {
        stream << "," << stockPile[i];
    }

    stream << "|W";
    const std::vector<int>& wastePile = state.getWastePile();
    for (std::size_t i = 0; i < wastePile.size(); ++i)
    {
        stream << "," << wastePile[i];
    }

    stream << "|F";
    std::vector<int> cardIds;
    cardIds.reserve(cards.size());
    for (std::unordered_map<int, CardData>::const_iterator it = cards.begin(); it != cards.end(); ++it)
    {
        cardIds.push_back(it->first);
    }

    std::sort(cardIds.begin(), cardIds.end());
    for (std::size_t i = 0; i < cardIds.size(); ++i)
    {
        const CardData* card = state.getCard(cardIds[i]);
        if (card == nullptr)
        {
            continue;
        }

        stream << "," << card->id << ":" << (card->faceUp ? 1 : 0) << ":" << static_cast<int>(card->zone);
    }

    return stream.str();
}

bool canClearTableauRecursive(GameState& state, std::set<std::string>& visited)
{
    if (state.isTableauCleared())
    {
        return true;
    }

    const std::string key = buildStateKey(state);
    if (!visited.insert(key).second)
    {
        return false;
    }

    const std::vector<int>& tableauSlots = state.getTableauSlots();
    for (std::size_t i = 0; i < tableauSlots.size(); ++i)
    {
        const int cardId = tableauSlots[i];
        if (cardId < 0 || !state.canMatchTableau(cardId))
        {
            continue;
        }

        GameState nextState = state;
        GameDelta delta;
        if (nextState.matchTableau(cardId, delta) && canClearTableauRecursive(nextState, visited))
        {
            return true;
        }
    }

    if (state.canDrawFromStock())
    {
        GameState nextState = state;
        GameDelta delta;
        if (nextState.drawFromStock(delta) && canClearTableauRecursive(nextState, visited))
        {
            return true;
        }
    }

    return false;
}
}

bool LevelSolver::canClearTableau(const LevelDefinition& definition)
{
    GameState state = GameState::createFromLevelDefinition(definition);
    std::set<std::string> visited;
    return canClearTableauRecursive(state, visited);
}
}
