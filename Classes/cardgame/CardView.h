#ifndef __CARDGAME_CARD_VIEW_H__
#define __CARDGAME_CARD_VIEW_H__

#include "GameState.h"

#include "cocos2d.h"

namespace cardgame
{
class CardView : public cocos2d::Node
{
public:
    static CardView* create(const cocos2d::Size& size);

    bool init(const cocos2d::Size& size);
    void syncWithCard(const CardData& card, bool clickable, bool isWasteTop);
    bool hitTest(const cocos2d::Vec2& worldPoint) const;

    int getCardId() const;

private:
    // CardView 只处理单张牌的视觉表现，不承担任何玩法规则判断。
    void updateBackAppearance();
    void setSpriteTexture(cocos2d::Sprite* sprite, const std::string& path);
    void fitSprite(cocos2d::Sprite* sprite, const cocos2d::Size& maxSize);

private:
    cocos2d::Sprite* _backFrame = nullptr;
    cocos2d::DrawNode* _backAccent = nullptr;
    cocos2d::Sprite* _frontFrame = nullptr;
    cocos2d::Sprite* _smallNumberSprite = nullptr;
    cocos2d::Sprite* _bigNumberSprite = nullptr;
    cocos2d::Sprite* _suitSprite = nullptr;
    cocos2d::Label* _backLabel = nullptr;
    cocos2d::Label* _hintLabel = nullptr;
    cocos2d::Size _cardSize;
    int _cardId = -1;
};
}

#endif
