#ifndef __CARDGAME_GAME_OVERLAY_VIEW_H__
#define __CARDGAME_GAME_OVERLAY_VIEW_H__

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
        cocos2d::MenuItemLabel *_undoItem = nullptr;
        cocos2d::Menu *_controlMenu = nullptr;
        cocos2d::Node *_completionOverlay = nullptr;
        std::function<void()> _onUndo;
        std::function<void()> _onReset;
        std::function<void()> _onReplay;
    };
}

#endif
