#include "view/CardAssetCatalog.h"
#include "view/CardView.h"

#include <algorithm>
#include <new>

USING_NS_CC;

namespace cardgame
{
    CardView *CardView::create(const Size &size)
    {
        CardView *result = new (std::nothrow) CardView();
        if (result != nullptr && result->init(size))
        {
            result->autorelease();
            return result;
        }

        delete result;
        return nullptr;
    }

    bool CardView::init(const Size &size)
    {
        if (!Node::init())
        {
            return false;
        }

        _cardSize = size;
        setContentSize(size);
        setAnchorPoint(Vec2(0.5f, 0.5f));

        _backFrame = Sprite::create();
        addChild(_backFrame);

        _backAccent = DrawNode::create();
        addChild(_backAccent, 1);

        _frontFrame = Sprite::create();
        _frontFrame->setPosition(Vec2::ZERO);
        addChild(_frontFrame, 1);

        _smallNumberSprite = Sprite::create();
        _smallNumberSprite->setAnchorPoint(Vec2(0.0f, 1.0f));
        _smallNumberSprite->setPosition(Vec2(-size.width * 0.37f, size.height * 0.39f));
        addChild(_smallNumberSprite, 2);

        _bigNumberSprite = Sprite::create();
        _bigNumberSprite->setPosition(Vec2(0.0f, 22.0f));
        addChild(_bigNumberSprite, 2);

        _suitSprite = Sprite::create();
        _suitSprite->setPosition(Vec2(0.0f, -54.0f));
        addChild(_suitSprite, 2);

        _backLabel = Label::createWithTTF("BACK", "fonts/arial.ttf", 40);
        _backLabel->setPosition(Vec2::ZERO);
        _backLabel->setColor(Color3B::WHITE);
        _backLabel->setVisible(false);
        addChild(_backLabel, 2);

        _hintLabel = Label::createWithTTF("", "fonts/arial.ttf", 26);
        _hintLabel->setPosition(Vec2(0.0f, -size.height * 0.31f));
        _hintLabel->setVisible(false);
        addChild(_hintLabel, 3);

        setSpriteTexture(_frontFrame, CardAssetCatalog::getCardFrontPath());
        fitSprite(_frontFrame, size);
        setSpriteTexture(_backFrame, CardAssetCatalog::getCardFrontPath());
        fitSprite(_backFrame, size);
        updateBackAppearance();

        return true;
    }

    void CardView::syncWithCard(const CardData &card, bool clickable, bool isWasteTop)
    {
        _cardId = card.id;
        setVisible(true);
        setScale(1.0f);
        setOpacity(255);
        const bool isRed = isRedSuit(card.suit);
        std::string hintText;

        if (!card.faceUp)
        {
            hintText = "covered";
            _backFrame->setVisible(true);
            _backAccent->setVisible(true);
            _backLabel->setVisible(false);
            _frontFrame->setVisible(false);
            _smallNumberSprite->setVisible(false);
            _bigNumberSprite->setVisible(false);
            _suitSprite->setVisible(false);
            _hintLabel->setColor(Color3B(220, 235, 255));
        }
        else
        {
            hintText = card.zone == CardZone::Waste ? "current" : (card.zone == CardZone::Stock ? "draw" : "");

            _backFrame->setVisible(false);
            _backAccent->setVisible(false);
            _backLabel->setVisible(false);
            _frontFrame->setVisible(true);
            // 正面牌由通用数字和花色资源拼装，避免依赖整套 52 张成品图。
            setSpriteTexture(_smallNumberSprite, CardAssetCatalog::getNumberPath(card.rank, isRed, false));
            setSpriteTexture(_bigNumberSprite, CardAssetCatalog::getNumberPath(card.rank, isRed, true));
            setSpriteTexture(_suitSprite, CardAssetCatalog::getSuitPath(card.suit));
            fitSprite(_smallNumberSprite, Size(_cardSize.width * 0.18f, _cardSize.height * 0.18f));
            fitSprite(_bigNumberSprite, Size(_cardSize.width * 0.38f, _cardSize.height * 0.24f));
            fitSprite(_suitSprite, Size(_cardSize.width * 0.22f, _cardSize.width * 0.22f));
            _hintLabel->setColor(clickable ? Color3B(32, 122, 63) : Color3B(110, 118, 128));
        }

        updateBackAppearance();
        _hintLabel->setString("");
    }

    bool CardView::hitTest(const Vec2 &worldPoint) const
    {
        const Vec2 localPoint = convertToNodeSpace(worldPoint);
        const Rect rect(-_cardSize.width * 0.5f, -_cardSize.height * 0.5f, _cardSize.width, _cardSize.height);
        return rect.containsPoint(localPoint);
    }

    int CardView::getCardId() const
    {
        return _cardId;
    }

    void CardView::updateBackAppearance()
    {
        if (_backFrame != nullptr)
        {
            _backFrame->setColor(Color3B(116, 156, 205));
            _backFrame->setOpacity(255);
        }

        if (_backAccent == nullptr)
        {
            return;
        }

        _backAccent->clear();

        const float inset = 24.0f;
        const Vec2 origin(-_cardSize.width * 0.5f + inset, -_cardSize.height * 0.5f + inset);
        const Vec2 destination(_cardSize.width * 0.5f - inset, _cardSize.height * 0.5f - inset);
        _backAccent->drawSolidRect(origin, destination, Color4F(0.15f, 0.32f, 0.54f, 0.92f));
        _backAccent->drawRect(origin, destination, Color4F(0.83f, 0.90f, 0.98f, 0.85f));
    }

    void CardView::setSpriteTexture(Sprite *sprite, const std::string &path)
    {
        if (sprite == nullptr)
        {
            return;
        }

        Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(path);
        if (texture == nullptr)
        {
            sprite->setVisible(false);
            return;
        }

        sprite->setTexture(texture);
        const Size textureSize = texture->getContentSize();
        sprite->setTextureRect(Rect(0.0f, 0.0f, textureSize.width, textureSize.height));
        sprite->setVisible(true);
    }

    void CardView::fitSprite(Sprite *sprite, const Size &maxSize)
    {
        if (sprite == nullptr || !sprite->isVisible())
        {
            return;
        }

        const Size contentSize = sprite->getContentSize();
        if (contentSize.width <= 0.0f || contentSize.height <= 0.0f)
        {
            return;
        }

        const float scale = std::min(maxSize.width / contentSize.width, maxSize.height / contentSize.height);
        sprite->setScale(scale);
    }
}
