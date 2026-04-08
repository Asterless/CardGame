#ifndef __CARDGAME_GAME_STATE_H__
#define __CARDGAME_GAME_STATE_H__

#include "tools/LevelDefinition.h"
#include "CardTypes.h"

#include <unordered_map>
#include <vector>

namespace cardgame
{
// 运行时动作类型。每一种可回退操作都应映射到一个明确的动作类型。
enum class GameActionType
{
    DrawStock,
    MatchTableau
};

struct GameDelta
{
    bool valid = false;
    GameActionType type = GameActionType::DrawStock;
    int movedCardId = -1;
    int previousWasteTopId = -1;
    int sourceIndex = -1;
    // 因本次操作被连带翻开的牌。Undo 时必须恢复这些牌的朝向。
    std::vector<int> revealedCardIds;
};

struct CardData
{
    int id = -1;
    Suit suit = Suit::Clubs;
    int rank = 1;
    CardZone zone = CardZone::Tableau;
    bool faceUp = false;
    int tableauIndex = -1;
    // blockers 表示压住当前牌的牌。只要这些牌还在主牌区，当前牌就不可操作。
    std::vector<int> blockers;
    // children 表示当前牌移除后，可能因此翻开的下层牌。
    std::vector<int> children;
};

class GameState
{
public:
    static GameState createDemoState();
    static GameState createFromLevelDefinition(const LevelDefinition& definition);

    const CardData* getCard(int cardId) const;
    CardData* getMutableCard(int cardId);

    const std::unordered_map<int, CardData>& getCards() const;
    const std::vector<int>& getTableauSlots() const;
    const std::vector<int>& getStockPile() const;
    const std::vector<int>& getWastePile() const;

    int getWasteTopId() const;
    int getStockTopId() const;
    bool isTableauCleared() const;

    bool canDrawFromStock() const;
    bool canMatchTableau(int cardId) const;

    bool drawFromStock(GameDelta& outDelta);
    bool undoDrawFromStock(const GameDelta& delta);

    bool matchTableau(int cardId, GameDelta& outDelta);
    bool undoMatchTableau(const GameDelta& delta);

private:
    // 运行时统一用 id -> CardData 管理卡牌，避免视图层直接持有业务状态。
    void addCard(const CardData& card);
    bool areRanksAdjacent(int lhs, int rhs) const;
    bool isCardUnblocked(int cardId) const;
    void revealChildrenIfNeeded(int cardId, std::vector<int>& revealedCardIds);
    void restoreHiddenState(const std::vector<int>& cardIds);
    void refreshPileFaces();

private:
    std::unordered_map<int, CardData> _cards;
    std::vector<int> _tableauSlots;
    std::vector<int> _stockPile;
    std::vector<int> _wastePile;
};
}

#endif
