#ifndef CARDGAME_CARD_ASSET_CATALOG_H
#define CARDGAME_CARD_ASSET_CATALOG_H

#include "data/CardTypes.h"

#include <string>

namespace cardgame
{
    class CardAssetCatalog
    {
    public:
        // 资源路径集中管理，替换美术资源时不需要改玩法逻辑。
        static std::string getCardFrontPath()
        {
            return "res/cards/frame/card_front.png";
        }

        static std::string getSuitPath(Suit suit)
        {
            switch (suit)
            {
            case Suit::Hearts:
                return "res/cards/suits/heart.png";
            case Suit::Diamonds:
                return "res/cards/suits/diamond.png";
            case Suit::Clubs:
                return "res/cards/suits/club.png";
            case Suit::Spades:
                return "res/cards/suits/spade.png";
            default:
                return "";
            }
        }

        static std::string getNumberPath(int rank, bool red, bool big)
        {
            const std::string sizeFolder = big ? "big" : "small";
            const std::string colorFolder = red ? "red" : "black";
            return "res/cards/numbers/" + sizeFolder + "/" + colorFolder + "/" + rankToText(rank) + ".png";
        }
    };
}

#endif
