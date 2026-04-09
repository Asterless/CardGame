#ifndef CARDGAME_CARD_DATA_H
#define CARDGAME_CARD_DATA_H

#include "data/CardTypes.h"

#include <vector>

namespace cardgame
{
    // 运行时卡牌数据模型。视图层和逻辑层都只依赖这份基础数据，不相互包含。
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
} // namespace cardgame

#endif
