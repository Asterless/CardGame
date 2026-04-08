#include "cardgame/GameState.h"

#include <algorithm>
#include <cmath>

namespace cardgame
{
namespace
{
CardData makeCardData(const LevelCardDefinition& definition)
{
    CardData card;
    card.id = definition.id;
    card.suit = definition.suit;
    card.rank = definition.rank;
    card.zone = definition.zone;
    card.faceUp = definition.faceUp;
    card.tableauIndex = definition.tableauIndex;
    card.blockers = definition.blockers;
    card.children = definition.children;
    return card;
}
}

GameState GameState::createDemoState()
{
    LevelDefinition definition;
    definition.tableauSlots = {1, 2, 3, 4, 5, 6, 7, 8};
    definition.stockPile = {9, 10, 11};
    definition.wastePile = {12};
    definition.cards.push_back(LevelCardDefinition{1, Suit::Diamonds, 3, CardZone::Tableau, true, 0, std::vector<int>(), {7}});
    definition.cards.push_back(LevelCardDefinition{2, Suit::Spades, 6, CardZone::Tableau, true, 1, std::vector<int>(), {8}});
    definition.cards.push_back(LevelCardDefinition{3, Suit::Hearts, 8, CardZone::Tableau, true, 2, std::vector<int>(), std::vector<int>()});
    definition.cards.push_back(LevelCardDefinition{4, Suit::Clubs, 10, CardZone::Tableau, true, 3, std::vector<int>(), std::vector<int>()});
    definition.cards.push_back(LevelCardDefinition{5, Suit::Spades, 5, CardZone::Tableau, true, 4, std::vector<int>(), std::vector<int>()});
    definition.cards.push_back(LevelCardDefinition{6, Suit::Diamonds, 11, CardZone::Tableau, true, 5, std::vector<int>(), std::vector<int>()});
    definition.cards.push_back(LevelCardDefinition{7, Suit::Hearts, 4, CardZone::Tableau, false, 6, {1}, std::vector<int>()});
    definition.cards.push_back(LevelCardDefinition{8, Suit::Clubs, 7, CardZone::Tableau, false, 7, {2}, std::vector<int>()});
    definition.cards.push_back(LevelCardDefinition{9, Suit::Diamonds, 9, CardZone::Stock, false, -1, std::vector<int>(), std::vector<int>()});
    definition.cards.push_back(LevelCardDefinition{10, Suit::Spades, 2, CardZone::Stock, false, -1, std::vector<int>(), std::vector<int>()});
    definition.cards.push_back(LevelCardDefinition{11, Suit::Hearts, 1, CardZone::Stock, true, -1, std::vector<int>(), std::vector<int>()});
    definition.cards.push_back(LevelCardDefinition{12, Suit::Clubs, 4, CardZone::Waste, true, -1, std::vector<int>(), std::vector<int>()});
    return createFromLevelDefinition(definition);
}

GameState GameState::createFromLevelDefinition(const LevelDefinition& definition)
{
    GameState state;
    state._tableauSlots = definition.tableauSlots;
    state._stockPile = definition.stockPile;
    state._wastePile = definition.wastePile;

    for (std::size_t i = 0; i < definition.cards.size(); ++i)
    {
        state.addCard(makeCardData(definition.cards[i]));
    }

    state.refreshPileFaces();
    return state;
}

const CardData* GameState::getCard(int cardId) const
{
    std::unordered_map<int, CardData>::const_iterator it = _cards.find(cardId);
    return it == _cards.end() ? nullptr : &it->second;
}

CardData* GameState::getMutableCard(int cardId)
{
    std::unordered_map<int, CardData>::iterator it = _cards.find(cardId);
    return it == _cards.end() ? nullptr : &it->second;
}

const std::unordered_map<int, CardData>& GameState::getCards() const
{
    return _cards;
}

const std::vector<int>& GameState::getTableauSlots() const
{
    return _tableauSlots;
}

const std::vector<int>& GameState::getStockPile() const
{
    return _stockPile;
}

const std::vector<int>& GameState::getWastePile() const
{
    return _wastePile;
}

int GameState::getWasteTopId() const
{
    return _wastePile.empty() ? -1 : _wastePile.back();
}

int GameState::getStockTopId() const
{
    return _stockPile.empty() ? -1 : _stockPile.back();
}

bool GameState::isTableauCleared() const
{
    for (std::size_t i = 0; i < _tableauSlots.size(); ++i)
    {
        if (_tableauSlots[i] >= 0)
        {
            return false;
        }
    }
    return true;
}

bool GameState::canDrawFromStock() const
{
    return !_stockPile.empty();
}

bool GameState::canMatchTableau(int cardId) const
{
    const CardData* card = getCard(cardId);
    const CardData* wasteTop = getCard(getWasteTopId());
    if (card == nullptr || wasteTop == nullptr)
    {
        return false;
    }

    if (card->zone != CardZone::Tableau || !card->faceUp || !isCardUnblocked(cardId))
    {
        return false;
    }

    return areRanksAdjacent(card->rank, wasteTop->rank);
}

bool GameState::drawFromStock(GameDelta& outDelta)
{
    if (_stockPile.empty())
    {
        return false;
    }

    const int cardId = _stockPile.back();
    _stockPile.pop_back();
    _wastePile.push_back(cardId);

    CardData* card = getMutableCard(cardId);
    if (card == nullptr)
    {
        return false;
    }

    card->zone = CardZone::Waste;
    outDelta.valid = true;
    outDelta.type = GameActionType::DrawStock;
    outDelta.movedCardId = cardId;
    outDelta.previousWasteTopId = _wastePile.size() > 1 ? _wastePile[_wastePile.size() - 2] : -1;
    outDelta.sourceIndex = static_cast<int>(_stockPile.size());
    refreshPileFaces();
    return true;
}

bool GameState::undoDrawFromStock(const GameDelta& delta)
{
    if (!delta.valid || delta.type != GameActionType::DrawStock || _wastePile.empty() || _wastePile.back() != delta.movedCardId)
    {
        return false;
    }

    _wastePile.pop_back();
    _stockPile.push_back(delta.movedCardId);

    CardData* card = getMutableCard(delta.movedCardId);
    if (card == nullptr)
    {
        return false;
    }

    card->zone = CardZone::Stock;
    refreshPileFaces();
    return true;
}

bool GameState::matchTableau(int cardId, GameDelta& outDelta)
{
    if (!canMatchTableau(cardId))
    {
        return false;
    }

    CardData* card = getMutableCard(cardId);
    if (card == nullptr)
    {
        return false;
    }

    _tableauSlots[card->tableauIndex] = -1;
    _wastePile.push_back(cardId);

    card->zone = CardZone::Waste;
    outDelta.valid = true;
    outDelta.type = GameActionType::MatchTableau;
    outDelta.movedCardId = cardId;
    outDelta.previousWasteTopId = _wastePile.size() > 1 ? _wastePile[_wastePile.size() - 2] : -1;
    outDelta.sourceIndex = card->tableauIndex;
    revealChildrenIfNeeded(cardId, outDelta.revealedCardIds);
    refreshPileFaces();
    return true;
}

bool GameState::undoMatchTableau(const GameDelta& delta)
{
    if (!delta.valid || delta.type != GameActionType::MatchTableau || _wastePile.empty() || _wastePile.back() != delta.movedCardId)
    {
        return false;
    }

    _wastePile.pop_back();
    _tableauSlots[delta.sourceIndex] = delta.movedCardId;

    CardData* card = getMutableCard(delta.movedCardId);
    if (card == nullptr)
    {
        return false;
    }

    card->zone = CardZone::Tableau;
    card->tableauIndex = delta.sourceIndex;
    card->faceUp = true;
    restoreHiddenState(delta.revealedCardIds);
    refreshPileFaces();
    return true;
}

void GameState::addCard(const CardData& card)
{
    _cards[card.id] = card;
}

bool GameState::areRanksAdjacent(int lhs, int rhs) const
{
    return std::abs(lhs - rhs) == 1;
}

bool GameState::isCardUnblocked(int cardId) const
{
    const CardData* card = getCard(cardId);
    if (card == nullptr)
    {
        return false;
    }

    for (std::size_t i = 0; i < card->blockers.size(); ++i)
    {
        const CardData* blocker = getCard(card->blockers[i]);
        if (blocker != nullptr && blocker->zone == CardZone::Tableau)
        {
            return false;
        }
    }
    return true;
}

void GameState::revealChildrenIfNeeded(int cardId, std::vector<int>& revealedCardIds)
{
    const CardData* parent = getCard(cardId);
    if (parent == nullptr)
    {
        return;
    }

    for (std::size_t i = 0; i < parent->children.size(); ++i)
    {
        CardData* child = getMutableCard(parent->children[i]);
        if (child != nullptr && child->zone == CardZone::Tableau && !child->faceUp && isCardUnblocked(child->id))
        {
            child->faceUp = true;
            revealedCardIds.push_back(child->id);
        }
    }
}

void GameState::restoreHiddenState(const std::vector<int>& cardIds)
{
    for (std::size_t i = 0; i < cardIds.size(); ++i)
    {
        CardData* card = getMutableCard(cardIds[i]);
        if (card != nullptr && card->zone == CardZone::Tableau)
        {
            card->faceUp = false;
        }
    }
}

void GameState::refreshPileFaces()
{
    for (std::size_t i = 0; i < _stockPile.size(); ++i)
    {
        CardData* card = getMutableCard(_stockPile[i]);
        if (card != nullptr)
        {
            card->faceUp = i + 1 == _stockPile.size();
        }
    }

    for (std::size_t i = 0; i < _wastePile.size(); ++i)
    {
        CardData* card = getMutableCard(_wastePile[i]);
        if (card != nullptr)
        {
            card->faceUp = true;
        }
    }
}
}
