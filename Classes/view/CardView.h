#ifndef CARDGAME_CARD_VIEW_H
#define CARDGAME_CARD_VIEW_H

#include "data/CardData.h"
#include "support/ObserverPtr.h"

#include "cocos2d.h"

namespace cardgame
{
    class CardView : public cocos2d::Node
    {
    public:
        static CardView *create(const cocos2d::Size &size);

        bool init(const cocos2d::Size &size);
        void syncWithCard(const CardData &card, bool clickable);
        bool hitTest(const cocos2d::Vec2 &worldPoint) const;

        int getCardId() const;

    private:
        // CardView 只处理单张牌的视觉表现，不承担任何玩法规则判断。
        void updateBackAppearance();
        void setSpriteTexture(cocos2d::Sprite *sprite, const std::string &path);
        void fitSprite(cocos2d::Sprite *sprite, const cocos2d::Size &maxSize);

    private:
        // Non-owning node references. cocos2d-x retains them via the scene graph.
        ObserverPtr<cocos2d::Sprite> _backFrame = nullptr;
        ObserverPtr<cocos2d::DrawNode> _backAccent = nullptr;
        ObserverPtr<cocos2d::Sprite> _frontFrame = nullptr;
        ObserverPtr<cocos2d::Sprite> _smallNumberSprite = nullptr;
        ObserverPtr<cocos2d::Sprite> _bigNumberSprite = nullptr;
        ObserverPtr<cocos2d::Sprite> _suitSprite = nullptr;
        ObserverPtr<cocos2d::Label> _backLabel = nullptr;
        ObserverPtr<cocos2d::Label> _hintLabel = nullptr;
        cocos2d::Size _cardSize;
        int _cardId = -1;
    };
}

#endif
