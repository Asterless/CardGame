#ifndef __CARDGAME_LEVEL_DEFINITION_H__
#define __CARDGAME_LEVEL_DEFINITION_H__

#include "data/CardTypes.h"

#include <string>
#include <vector>

namespace cardgame
{
struct LevelVector2
{
    float x = 0.0f;
    float y = 0.0f;
};

struct LevelSize
{
    float width = 0.0f;
    float height = 0.0f;
};

struct LevelLayoutDefinition
{
    // 关卡布局配置。未来新增牌位、牌堆位置时优先扩展这里，而不是写死在场景里。
    LevelSize cardSize;
    bool hasCardSize = false;
    LevelVector2 stockBasePosition;
    bool hasStockBasePosition = false;
    LevelVector2 wasteBasePosition;
    bool hasWasteBasePosition = false;
    LevelVector2 stockPileDepthOffset;
    bool hasStockPileDepthOffset = false;
    LevelVector2 wastePileDepthOffset;
    bool hasWastePileDepthOffset = false;
    LevelVector2 coveredCardOffset;
    bool hasCoveredCardOffset = false;
    LevelVector2 controlMenuPosition;
    bool hasControlMenuPosition = false;
    std::vector<LevelVector2> tableauPositions;
};

struct LevelCardDefinition
{
    // 关卡中的静态卡牌定义，负责描述初始状态，不直接参与运行时状态变更。
    int id = -1;
    Suit suit = Suit::Clubs;
    int rank = 1;
    CardZone zone = CardZone::Tableau;
    bool faceUp = false;
    int tableauIndex = -1;
    std::vector<int> blockers;
    std::vector<int> children;
};

struct LevelDefinition
{
    std::string levelId;
    LevelLayoutDefinition layout;
    std::vector<int> tableauSlots;
    std::vector<int> stockPile;
    std::vector<int> wastePile;
    std::vector<LevelCardDefinition> cards;
};
}

#endif
