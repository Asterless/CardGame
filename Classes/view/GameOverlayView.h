#ifndef CARDGAME_GAME_OVERLAY_VIEW_H
#define CARDGAME_GAME_OVERLAY_VIEW_H

#include "support/ObserverPtr.h"
#include "cocos2d.h"

#include <functional>

namespace cardgame
{
    class GameOverlayView : public cocos2d::Node
    {
    public:
        static GameOverlayView *create(const cocos2d::Vec2 &controlMenuPosition);

        bool init(const cocos2d::Vec2 &controlMenuPosition);
        void setCallbacks(
            const std::function<void()> &onUndo,
            const std::function<void()> &onReset,
            const std::function<void()> &onReplay);
        void setUndoEnabled(bool enabled);
        void setLevelComplete(bool complete);

    private:
        void buildControls(const cocos2d::Vec2 &controlMenuPosition);
        void buildCompletionOverlay();
        void onUndoClicked(cocos2d::Ref *sender);
        void onResetClicked(cocos2d::Ref *sender);
        void onReplayClicked(cocos2d::Ref *sender);

    private:
        // Non-owning node references. cocos2d-x retains them via the scene graph.
        ObserverPtr<cocos2d::MenuItemLabel> _undoItem = nullptr;
        ObserverPtr<cocos2d::Menu> _controlMenu = nullptr;
        ObserverPtr<cocos2d::Node> _completionOverlay = nullptr;
        std::function<void()> _onUndo;
        std::function<void()> _onReset;
        std::function<void()> _onReplay;
    };
}

#endif
