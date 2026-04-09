#include "CardGameScene.h"

#include "logic/LevelSessionLoader.h"
#include "view/CardView.h"
#include "view/GameOverlayView.h"

#include <algorithm>
#include <limits>
#include <sstream>

USING_NS_CC;

namespace
{
    const float kHeaderHeight = 230.0f;
    const float kBottomAreaHeight = 600.0f;
    const float kMoveDuration = 0.22f;

    Vec2 computeCenteredPilePosition(const Vec2 &pileBasePosition, const Size &cardSize, std::size_t pileSize, float depth, float depthXOffset, float depthYOffset)
    {
        const float centerOffsetX = static_cast<float>(pileSize - 1) * depthXOffset * 0.5f;
        const float centerOffsetY = static_cast<float>(pileSize - 1) * depthYOffset * 0.5f;
        const Vec2 pileCenter = pileBasePosition + Vec2(cardSize.width * 0.5f, cardSize.height * 0.5f);
        return pileCenter + Vec2(depth * depthXOffset - centerOffsetX,
                                 depth * depthYOffset - centerOffsetY);
    }

    std::string joinMessages(const std::vector<std::string> &messages)
    {
        std::ostringstream stream;
        bool isFirstMessage = true;
        for (const auto &message : messages)
        {
            if (!isFirstMessage)
            {
                stream << " | ";
            }
            stream << message;
            isFirstMessage = false;
        }

        return stream.str();
    }

    Node *createPanel(const Size &size, const Vec2 &center, const Color4F &fillColor, const std::string &title)
    {
        Node *panel = Node::create();
        panel->setPosition(center);

        DrawNode *background = DrawNode::create();
        background->drawSolidRect(Vec2(-size.width * 0.5f, -size.height * 0.5f), Vec2(size.width * 0.5f, size.height * 0.5f), fillColor);
        background->drawRect(Vec2(-size.width * 0.5f, -size.height * 0.5f), Vec2(size.width * 0.5f, size.height * 0.5f), Color4F(0.34f, 0.38f, 0.44f, 0.80f));
        panel->addChild(background);

        if (!title.empty())
        {
            DrawNode *badge = DrawNode::create();
            const Vec2 badgeOrigin(-size.width * 0.5f + 34.0f, size.height * 0.5f - 76.0f);
            badge->drawSolidRect(badgeOrigin, badgeOrigin + Vec2(212.0f, 52.0f), Color4F(1.0f, 1.0f, 1.0f, 0.40f));
            badge->drawRect(badgeOrigin, badgeOrigin + Vec2(212.0f, 52.0f), Color4F(0.34f, 0.38f, 0.44f, 0.45f));
            panel->addChild(badge, 1);

            Label *label = Label::createWithTTF(title, "fonts/arial.ttf", 26);
            label->setAnchorPoint(Vec2(0.0f, 0.5f));
            label->setColor(Color3B(48, 56, 66));
            label->setPosition(Vec2(-size.width * 0.5f + 52.0f, size.height * 0.5f - 50.0f));
            panel->addChild(label, 2);
        }

        return panel;
    }

    Node *createSlotDecoration(const Size &size, const Vec2 &center, const std::string &title)
    {
        Node *slot = Node::create();
        slot->setPosition(center);

        DrawNode *frame = DrawNode::create();
        frame->drawSolidRect(Vec2(-size.width * 0.5f, -size.height * 0.5f), Vec2(size.width * 0.5f, size.height * 0.5f), Color4F(1.0f, 1.0f, 1.0f, 0.12f));
        frame->drawRect(Vec2(-size.width * 0.5f, -size.height * 0.5f), Vec2(size.width * 0.5f, size.height * 0.5f), Color4F(0.39f, 0.43f, 0.48f, 0.55f));
        slot->addChild(frame);

        Label *label = Label::createWithTTF(title, "fonts/arial.ttf", 26);
        label->setColor(Color3B(88, 98, 110));
        label->setPosition(Vec2(0.0f, size.height * 0.5f + 28.0f));
        slot->addChild(label, 1);

        return slot;
    }

    Node *createInfoChip(const Size &size, const Vec2 &center, const std::string &text)
    {
        Node *chip = Node::create();
        chip->setPosition(center);

        DrawNode *background = DrawNode::create();
        background->drawSolidRect(Vec2(-size.width * 0.5f, -size.height * 0.5f), Vec2(size.width * 0.5f, size.height * 0.5f), Color4F(1.0f, 1.0f, 1.0f, 0.55f));
        background->drawRect(Vec2(-size.width * 0.5f, -size.height * 0.5f), Vec2(size.width * 0.5f, size.height * 0.5f), Color4F(0.34f, 0.38f, 0.44f, 0.38f));
        chip->addChild(background);

        Label *label = Label::createWithTTF(text, "fonts/arial.ttf", 24);
        label->setWidth(size.width - 44.0f);
        label->setAlignment(TextHAlignment::CENTER);
        label->setColor(Color3B(66, 72, 82));
        label->setPosition(Vec2::ZERO);
        chip->addChild(label, 1);

        return chip;
    }
}

Scene *CardGameScene::createScene()
{
    return CardGameScene::create();
}

bool CardGameScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    const cardgame::LevelSessionLoadResult loadResult = cardgame::LevelSessionLoader::load("res/levels/", "res/levels/demo_level.json");
    for (const auto &log : loadResult.logs)
    {
        CCLOG("%s", log.c_str());
    }

    if (!loadResult.errors.empty())
    {
        CCLOG("Level load failed: %s", joinMessages(loadResult.errors).c_str());
        applyDefaultLayout();
        _state = loadResult.state;
    }
    else
    {
        CCLOG("Level '%s' clearable: %s",
              loadResult.definition.levelId.c_str(),
              loadResult.solvable ? "true" : "false");
        _loadedLevelDefinition = loadResult.definition;
        _hasLoadedLevelDefinition = loadResult.hasLoadedDefinition;
        applyLevelLayout(loadResult.definition);
        _state = loadResult.state;
    }

    buildBackground();

    _cardLayer = Node::create();
    addChild(_cardLayer, 5);

    _overlayView = cardgame::GameOverlayView::create(_controlMenuPosition);
    _overlayView->setCallbacks(
        std::bind(&CardGameScene::onUndoRequested, this),
        std::bind(&CardGameScene::onResetRequested, this),
        std::bind(&CardGameScene::onReplayRequested, this));
    addChild(_overlayView, 20);

    createCardViews();
    createTouchHandling();
    refreshAllViews(false);
    updateHudText();
    return true;
}

void CardGameScene::applyLevelLayout(const cardgame::LevelDefinition &definition)
{
    // 先落默认值，再用关卡配置覆盖，避免关卡漏字段时场景不可用。
    applyDefaultLayout();

    if (definition.layout.cardSize.has_value() &&
        definition.layout.cardSize->width > 0.0f &&
        definition.layout.cardSize->height > 0.0f)
    {
        _cardSize = Size(definition.layout.cardSize->width, definition.layout.cardSize->height);
    }

    if (definition.layout.stockBasePosition.has_value())
    {
        _stockBasePosition = Vec2(definition.layout.stockBasePosition->x, definition.layout.stockBasePosition->y);
    }

    if (definition.layout.wasteBasePosition.has_value())
    {
        _wasteBasePosition = Vec2(definition.layout.wasteBasePosition->x, definition.layout.wasteBasePosition->y);
    }

    if (definition.layout.stockPileDepthOffset.has_value())
    {
        _stockPileDepthOffset = Vec2(definition.layout.stockPileDepthOffset->x, definition.layout.stockPileDepthOffset->y);
    }

    if (definition.layout.wastePileDepthOffset.has_value())
    {
        _wastePileDepthOffset = Vec2(definition.layout.wastePileDepthOffset->x, definition.layout.wastePileDepthOffset->y);
    }

    if (definition.layout.coveredCardOffset.has_value())
    {
        _coveredCardOffset = Vec2(definition.layout.coveredCardOffset->x, definition.layout.coveredCardOffset->y);
    }

    if (definition.layout.controlMenuPosition.has_value())
    {
        _controlMenuPosition = Vec2(definition.layout.controlMenuPosition->x, definition.layout.controlMenuPosition->y);
    }

    if (!definition.layout.tableauPositions.empty())
    {
        _tableauPositions.clear();
        for (const auto &point : definition.layout.tableauPositions)
        {
            _tableauPositions.push_back(Vec2(point.x, point.y));
        }
    }
}

void CardGameScene::applyDefaultLayout()
{
    _cardSize = Size(190.0f, 266.0f);
    _stockBasePosition = Vec2(290.0f, 206.0f);
    _wasteBasePosition = Vec2(790.0f, 206.0f);
    _stockPileDepthOffset = Vec2(-8.0f, -10.0f);
    _wastePileDepthOffset = Vec2(0.0f, 0.0f);
    _coveredCardOffset = Vec2(-8.0f, -10.0f);
    _controlMenuPosition = Vec2(870.0f, 468.0f);
    _tableauPositions = {
        Vec2(270.0f, 1480.0f),
        Vec2(450.0f, 1480.0f),
        Vec2(630.0f, 1480.0f),
        Vec2(810.0f, 1480.0f),
        Vec2(360.0f, 1140.0f),
        Vec2(540.0f, 1140.0f),
        Vec2(270.0f, 1320.0f),
        Vec2(450.0f, 1320.0f)};
}

void CardGameScene::buildBackground()
{
    LayerColor *background = LayerColor::create(Color4B(235, 240, 232, 255));
    addChild(background);

    DrawNode *headerBand = DrawNode::create();
    headerBand->drawSolidRect(Vec2(0.0f, 2080.0f - kHeaderHeight), Vec2(1080.0f, 2080.0f), Color4F(0.84f, 0.90f, 0.87f, 1.0f));
    headerBand->drawRect(Vec2(0.0f, 2080.0f - kHeaderHeight), Vec2(1080.0f, 2080.0f), Color4F(0.32f, 0.38f, 0.36f, 0.45f));
    addChild(headerBand, 0);

    const float mainAreaHeight = 2080.0f - kHeaderHeight - kBottomAreaHeight;
    Node *tableauPanel = createPanel(Size(1080.0f, mainAreaHeight), Vec2(540.0f, kBottomAreaHeight + mainAreaHeight * 0.5f), Color4F(0.88f, 0.93f, 0.90f, 1.0f), "");
    addChild(tableauPanel, 0);

    Node *bottomPanel = createPanel(Size(1080.0f, kBottomAreaHeight), Vec2(540.0f, kBottomAreaHeight * 0.5f), Color4F(0.95f, 0.92f, 0.85f, 1.0f), "");
    addChild(bottomPanel, 0);

    DrawNode *centerDivider = DrawNode::create();
    centerDivider->drawSegment(Vec2(540.0f, 72.0f), Vec2(540.0f, kBottomAreaHeight - 88.0f), 1.4f, Color4F(0.44f, 0.43f, 0.38f, 0.35f));
    addChild(centerDivider, 1);

    addChild(createSlotDecoration(_cardSize, _stockBasePosition, ""), 1);
    addChild(createSlotDecoration(_cardSize, _wasteBasePosition, ""), 1);
}

void CardGameScene::createCardViews()
{
    const auto &cards = _state.getCards();
    for (const auto &entry : cards)
    {
        auto *view = cardgame::CardView::create(_cardSize);
        _cardLayer->addChild(view);
        _cardViews[entry.first] = view;
    }
}

void CardGameScene::createTouchHandling()
{
    EventListenerTouchOneByOne *listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(false);

    listener->onTouchBegan = [this](Touch *, Event *)
    {
        return !_inputLocked && !_isLevelComplete;
    };

    listener->onTouchEnded = [this](Touch *touch, Event *)
    {
        if (!_inputLocked)
        {
            tryHandleCardTap(touch->getLocation());
        }
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void CardGameScene::onUndoRequested()
{
    if (_inputLocked)
    {
        return;
    }

    if (_undoStack.empty())
    {
        updateHudText("Nothing to undo.");
        return;
    }

    const cardgame::GameCommandPtr command = _undoStack.back();
    if (!command->undo(_state))
    {
        updateHudText("Undo failed because the current state is out of sync.");
        return;
    }

    _undoStack.pop_back();
    refreshAllViews(true, [this]()
                    { updateHudText("Undo completed."); });
}

void CardGameScene::onResetRequested()
{
    if (_inputLocked)
    {
        return;
    }

    _state = _hasLoadedLevelDefinition
                 ? cardgame::GameState::createFromLevelDefinition(_loadedLevelDefinition)
                 : cardgame::GameState::createDemoState();
    _undoStack.clear();
    refreshAllViews(false);
    updateHudText("Demo reset.");
}

void CardGameScene::onReplayRequested()
{
    onResetRequested();
}

bool CardGameScene::tryHandleCardTap(const Vec2 &worldPoint)
{
    if (_isLevelComplete)
    {
        return false;
    }

    const int stockTopId = _state.getStockTopId();
    if (stockTopId >= 0)
    {
        cardgame::CardView *stockView = _cardViews[stockTopId];
        if (stockView != nullptr && stockView->isVisible() && stockView->hitTest(worldPoint))
        {
            executeCommand(std::make_shared<cardgame::DrawStockCommand>());
            return true;
        }
    }

    const auto &tableauSlots = _state.getTableauSlots();
    std::vector<int> hitOrder;
    hitOrder.reserve(tableauSlots.size());
    for (const int cardId : tableauSlots)
    {
        if (cardId < 0)
        {
            continue;
        }

        hitOrder.push_back(cardId);
    }

    // 点击判定按当前可见 z 序从上到下检测，避免被覆盖的牌抢到点击。
    std::sort(hitOrder.begin(), hitOrder.end(), [this](int lhs, int rhs)
              {
        cardgame::CardView* lhsView = _cardViews[lhs];
        cardgame::CardView* rhsView = _cardViews[rhs];
        const int lhsZOrder = lhsView == nullptr ? std::numeric_limits<int>::min() : lhsView->getLocalZOrder();
        const int rhsZOrder = rhsView == nullptr ? std::numeric_limits<int>::min() : rhsView->getLocalZOrder();
        if (lhsZOrder != rhsZOrder)
        {
            return lhsZOrder > rhsZOrder;
        }

        return lhs > rhs; });

    for (const int cardId : hitOrder)
    {
        cardgame::CardView *view = _cardViews[cardId];
        const cardgame::CardData *card = _state.getCard(cardId);
        if (view == nullptr || card == nullptr || !view->isVisible() || !view->hitTest(worldPoint))
        {
            continue;
        }

        if (!card->faceUp)
        {
            updateHudText("This tableau card is still covered.");
            return true;
        }

        if (!_state.canMatchTableau(cardId))
        {
            updateHudText("Tableau cards only move when their rank is one away from the waste top.");
            return true;
        }

        executeCommand(std::make_shared<cardgame::MatchTableauCommand>(cardId));
        return true;
    }

    return false;
}

void CardGameScene::executeCommand(const cardgame::GameCommandPtr &command)
{
    if (command == nullptr || _inputLocked)
    {
        return;
    }

    if (!command->execute(_state))
    {
        updateHudText("The requested move is not valid.");
        return;
    }

    _undoStack.push_back(command);
    refreshAllViews(true, [this]()
                    { updateHudText("Move completed."); });
}

void CardGameScene::refreshAllViews(bool animated, const std::function<void()> &completion)
{
    _inputLocked = animated;

    struct RefreshState
    {
        int pendingAnimations = 0;
        bool completionCalled = false;
    };

    const auto refreshState = std::make_shared<RefreshState>();
    std::function<void()> finish = [this, completion, refreshState]()
    {
        if (refreshState->completionCalled)
        {
            return;
        }

        refreshState->completionCalled = true;
        _inputLocked = false;
        updateCompletionOverlay();
        updateHudText();

        if (completion)
        {
            completion();
        }
    };

    // 每次都从最新状态重新计算所有牌的位置，这样 Undo 只需要恢复数据，不需要记录动画轨迹。
    const auto &cards = _state.getCards();
    for (const auto &entry : cards)
    {
        const auto &card = entry.second;
        cardgame::CardView *view = _cardViews[card.id];
        if (view == nullptr)
        {
            continue;
        }

        const CardPlacement placement = computePlacement(card);
        view->syncWithCard(card, placement.clickable);
        view->setLocalZOrder(placement.zOrder);
        view->setVisible(placement.visible);
        view->stopAllActions();

        if (!placement.visible)
        {
            continue;
        }

        if (!animated || view->getPosition().fuzzyEquals(placement.position, 0.5f))
        {
            view->setPosition(placement.position);
            continue;
        }

        ++refreshState->pendingAnimations;
        view->runAction(Sequence::create(
            MoveTo::create(kMoveDuration, placement.position),
            CallFunc::create([refreshState, finish]()
                             {
                --refreshState->pendingAnimations;
                if (refreshState->pendingAnimations == 0)
                {
                    finish();
                } }),
            nullptr));
    }

    if (!animated || refreshState->pendingAnimations == 0)
    {
        finish();
    }
}

cocos2d::Vec2 CardGameScene::computeTableauPosition(const cardgame::CardData &card) const
{
    if (card.blockers.empty() || !card.children.empty())
    {
        return _tableauPositions[card.tableauIndex];
    }

    Vec2 blockerCenter = Vec2::ZERO;
    int validBlockerCount = 0;
    for (const int blockerId : card.blockers)
    {
        const cardgame::CardData *blocker = _state.getCard(blockerId);
        if (blocker == nullptr || blocker->tableauIndex < 0 || blocker->tableauIndex >= static_cast<int>(_tableauPositions.size()))
        {
            continue;
        }

        blockerCenter += _tableauPositions[blocker->tableauIndex];
        ++validBlockerCount;
    }

    if (validBlockerCount == 0)
    {
        return _tableauPositions[card.tableauIndex];
    }

    blockerCenter *= 1.0f / static_cast<float>(validBlockerCount);
    return blockerCenter + _coveredCardOffset;
}

CardGameScene::CardPlacement CardGameScene::computePlacement(const cardgame::CardData &card) const
{
    CardPlacement placement;

    if (card.zone == cardgame::CardZone::Tableau)
    {
        placement.position = computeTableauPosition(card);
        placement.zOrder = 1000 + static_cast<int>(placement.position.y);
        placement.visible = true;
        placement.clickable = _state.canMatchTableau(card.id);
        return placement;
    }

    if (card.zone == cardgame::CardZone::Stock)
    {
        const std::vector<int> &stockPile = _state.getStockPile();
        for (std::size_t i = 0; i < stockPile.size(); ++i)
        {
            if (stockPile[i] != card.id)
            {
                continue;
            }

            const float depth = static_cast<float>(stockPile.size() - 1 - i);
            placement.position = computeCenteredPilePosition(
                _stockBasePosition,
                _cardSize,
                stockPile.size(),
                depth,
                _stockPileDepthOffset.x,
                _stockPileDepthOffset.y);
            placement.zOrder = 200 + static_cast<int>(i);
            placement.visible = true;
            placement.clickable = i + 1 == stockPile.size();
            return placement;
        }
    }

    const std::vector<int> &wastePile = _state.getWastePile();
    for (std::size_t i = 0; i < wastePile.size(); ++i)
    {
        if (wastePile[i] != card.id)
        {
            continue;
        }

        const float depth = static_cast<float>(wastePile.size() - 1 - i);
        // 手牌区牌面保持完全重叠，确保当前顶部牌不会产生视觉漂移。
        placement.position = computeCenteredPilePosition(
            _wasteBasePosition,
            _cardSize,
            wastePile.size(),
            depth,
            _wastePileDepthOffset.x,
            _wastePileDepthOffset.y);
        placement.zOrder = 300 + static_cast<int>(i);
        placement.visible = true;
        placement.clickable = false;
        return placement;
    }

    placement.visible = false;
    return placement;
}

void CardGameScene::updateCompletionOverlay()
{
    // 通关状态只取决于主牌区是否清空，界面显示始终从状态推导，不单独缓存额外流程。
    _isLevelComplete = _state.isTableauCleared();

    if (_overlayView != nullptr)
    {
        _overlayView->setLevelComplete(_isLevelComplete);
    }
}

void CardGameScene::updateHudText(const std::string &message)
{
    if (_overlayView != nullptr)
    {
        _overlayView->setUndoEnabled(!_undoStack.empty());
    }

    CC_UNUSED_PARAM(message);
}
