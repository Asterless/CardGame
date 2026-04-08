#ifndef __CARD_GAME_SCENE_H__
#define __CARD_GAME_SCENE_H__

#include "cardgame/CardView.h"
#include "cardgame/GameOverlayView.h"
#include "cardgame/GameCommand.h"
#include "cardgame/LevelSessionLoader.h"
#include "tools/LevelDefinition.h"

#include <functional>
#include <unordered_map>
#include <vector>

class CardGameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    bool init() override;

    CREATE_FUNC(CardGameScene);

private:
    struct CardPlacement
    {
        cocos2d::Vec2 position;
        int zOrder = 0;
        bool visible = true;
        bool clickable = false;
        bool isWasteTop = false;
    };

    void buildBackground();
    void createCardViews();
    void createTouchHandling();

    void onUndoRequested();
    void onResetRequested();
    void onReplayRequested();

    void applyLevelLayout(const cardgame::LevelDefinition& definition);
    void applyDefaultLayout();
    void updateCompletionOverlay();
    bool tryHandleCardTap(const cocos2d::Vec2& worldPoint);
    void executeCommand(const cardgame::GameCommandPtr& command);
    // 每次刷新都从当前状态重新计算目标位置，前进动画和 Undo 动画共用同一条刷新链路。
    void refreshAllViews(bool animated, const std::function<void()>& completion = std::function<void()>());
    cocos2d::Vec2 computeTableauPosition(const cardgame::CardData& card) const;
    CardPlacement computePlacement(const cardgame::CardData& card) const;
    void updateHudText(const std::string& message = "");

private:
    cardgame::GameState _state;
    cardgame::LevelDefinition _loadedLevelDefinition;
    cocos2d::Node* _cardLayer = nullptr;
    cardgame::GameOverlayView* _overlayView = nullptr;
    std::unordered_map<int, cardgame::CardView*> _cardViews;
    std::vector<cardgame::GameCommandPtr> _undoStack;
    std::vector<cocos2d::Vec2> _tableauPositions;
    cocos2d::Vec2 _stockBasePosition;
    cocos2d::Vec2 _wasteBasePosition;
    cocos2d::Vec2 _stockPileDepthOffset;
    cocos2d::Vec2 _wastePileDepthOffset;
    cocos2d::Vec2 _coveredCardOffset;
    cocos2d::Vec2 _controlMenuPosition;
    cocos2d::Size _cardSize;
    bool _hasLoadedLevelDefinition = false;
    bool _isLevelComplete = false;
    bool _inputLocked = false;
};

#endif
