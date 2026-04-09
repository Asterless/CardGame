#include "view/GameOverlayView.h"

#include <new>

USING_NS_CC;

namespace cardgame
{
namespace
{
Node* createPanel(const Size& size, const Vec2& center, const Color4F& fillColor, const std::string& title)
{
    Node* panel = Node::create();
    panel->setPosition(center);

    DrawNode* background = DrawNode::create();
    background->drawSolidRect(Vec2(-size.width * 0.5f, -size.height * 0.5f), Vec2(size.width * 0.5f, size.height * 0.5f), fillColor);
    background->drawRect(Vec2(-size.width * 0.5f, -size.height * 0.5f), Vec2(size.width * 0.5f, size.height * 0.5f), Color4F(0.34f, 0.38f, 0.44f, 0.80f));
    panel->addChild(background);

    if (!title.empty())
    {
        DrawNode* badge = DrawNode::create();
        const Vec2 badgeOrigin(-size.width * 0.5f + 34.0f, size.height * 0.5f - 76.0f);
        badge->drawSolidRect(badgeOrigin, badgeOrigin + Vec2(212.0f, 52.0f), Color4F(1.0f, 1.0f, 1.0f, 0.40f));
        badge->drawRect(badgeOrigin, badgeOrigin + Vec2(212.0f, 52.0f), Color4F(0.34f, 0.38f, 0.44f, 0.45f));
        panel->addChild(badge, 1);

        Label* label = Label::createWithTTF(title, "fonts/arial.ttf", 26);
        label->setAnchorPoint(Vec2(0.0f, 0.5f));
        label->setColor(Color3B(48, 56, 66));
        label->setPosition(Vec2(-size.width * 0.5f + 52.0f, size.height * 0.5f - 50.0f));
        panel->addChild(label, 2);
    }

    return panel;
}
}

GameOverlayView* GameOverlayView::create(const Vec2& controlMenuPosition)
{
    GameOverlayView* result = new (std::nothrow) GameOverlayView();
    if (result != nullptr && result->init(controlMenuPosition))
    {
        result->autorelease();
        return result;
    }

    delete result;
    return nullptr;
}

bool GameOverlayView::init(const Vec2& controlMenuPosition)
{
    if (!Node::init())
    {
        return false;
    }

    buildControls(controlMenuPosition);
    buildCompletionOverlay();
    return true;
}

void GameOverlayView::setCallbacks(
    const std::function<void()>& onUndo,
    const std::function<void()>& onReset,
    const std::function<void()>& onReplay)
{
    _onUndo = onUndo;
    _onReset = onReset;
    _onReplay = onReplay;
}

void GameOverlayView::setUndoEnabled(bool enabled)
{
    if (_undoItem != nullptr)
    {
        _undoItem->setEnabled(enabled);
    }
}

void GameOverlayView::setLevelComplete(bool complete)
{
    if (_controlMenu != nullptr)
    {
        _controlMenu->setVisible(!complete);
    }

    if (_completionOverlay != nullptr)
    {
        _completionOverlay->setVisible(complete);
    }
}

void GameOverlayView::buildControls(const Vec2& controlMenuPosition)
{
    Label* undoLabel = Label::createWithTTF("Undo", "fonts/arial.ttf", 34);
    Label* resetLabel = Label::createWithTTF("Reset", "fonts/arial.ttf", 34);

    _undoItem = MenuItemLabel::create(undoLabel, CC_CALLBACK_1(GameOverlayView::onUndoClicked, this));
    _undoItem->setDisabledColor(Color3B(150, 150, 150));

    MenuItemLabel* resetItem = MenuItemLabel::create(resetLabel, CC_CALLBACK_1(GameOverlayView::onResetClicked, this));

    _controlMenu = Menu::create(_undoItem, resetItem, nullptr);
    _controlMenu->alignItemsHorizontallyWithPadding(62.0f);
    _controlMenu->setPosition(controlMenuPosition);
    addChild(_controlMenu, 0);
}

void GameOverlayView::buildCompletionOverlay()
{
    _completionOverlay = Node::create();
    _completionOverlay->setVisible(false);
    addChild(_completionOverlay, 1);

    LayerColor* dimLayer = LayerColor::create(Color4B(23, 29, 33, 168));
    _completionOverlay->addChild(dimLayer);

    Node* panel = createPanel(Size(620.0f, 420.0f), Vec2(540.0f, 1040.0f), Color4F(0.97f, 0.95f, 0.89f, 1.0f), "");
    _completionOverlay->addChild(panel, 1);

    Label* title = Label::createWithTTF("Level Complete", "fonts/arial.ttf", 56);
    title->setColor(Color3B(54, 62, 66));
    title->setPosition(Vec2(0.0f, 78.0f));
    panel->addChild(title, 1);

    Label* subtitle = Label::createWithTTF("All tableau cards have been cleared.", "fonts/arial.ttf", 28);
    subtitle->setColor(Color3B(102, 110, 118));
    subtitle->setPosition(Vec2(0.0f, 8.0f));
    panel->addChild(subtitle, 1);

    Label* replayLabel = Label::createWithTTF("Play Again", "fonts/arial.ttf", 34);
    MenuItemLabel* replayItem = MenuItemLabel::create(replayLabel, CC_CALLBACK_1(GameOverlayView::onReplayClicked, this));
    replayItem->setColor(Color3B(48, 56, 66));

    Menu* replayMenu = Menu::create(replayItem, nullptr);
    replayMenu->setPosition(Vec2(0.0f, -92.0f));
    panel->addChild(replayMenu, 2);
}

void GameOverlayView::onUndoClicked(Ref*)
{
    if (_onUndo)
    {
        _onUndo();
    }
}

void GameOverlayView::onResetClicked(Ref*)
{
    if (_onReset)
    {
        _onReset();
    }
}

void GameOverlayView::onReplayClicked(Ref*)
{
    if (_onReplay)
    {
        _onReplay();
    }
}
}
