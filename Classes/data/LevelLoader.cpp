#include "data/LevelLoader.h"

#include "cocos2d.h"
#include "json/document.h"

#include <utility>

namespace cardgame
{
    namespace
    {
        LevelLoadResult fail(const std::string &message)
        {
            return {nullopt, message};
        }

        Optional<std::vector<int>> readIntArray(const rapidjson::Value &value)
        {
            if (!value.IsArray())
            {
                return nullopt;
            }

            std::vector<int> outArray;
            outArray.reserve(value.Size());
            for (rapidjson::SizeType i = 0; i < value.Size(); ++i)
            {
                if (!value[i].IsInt())
                {
                    return nullopt;
                }

                outArray.push_back(value[i].GetInt());
            }

            return outArray;
        }

        Optional<float> readFloat(const rapidjson::Value &value)
        {
            if (!value.IsNumber())
            {
                return nullopt;
            }

            return value.GetFloat();
        }

        Optional<LevelVector2> readVector2(const rapidjson::Value &value)
        {
            if (!value.IsObject() || !value.HasMember("x") || !value.HasMember("y"))
            {
                return nullopt;
            }

            const auto x = readFloat(value["x"]);
            const auto y = readFloat(value["y"]);
            if (!x.has_value() || !y.has_value())
            {
                return nullopt;
            }

            return LevelVector2{*x, *y};
        }

        Optional<LevelSize> readSize(const rapidjson::Value &value)
        {
            if (!value.IsObject() || !value.HasMember("width") || !value.HasMember("height"))
            {
                return nullopt;
            }

            const auto width = readFloat(value["width"]);
            const auto height = readFloat(value["height"]);
            if (!width.has_value() || !height.has_value())
            {
                return nullopt;
            }

            return LevelSize{*width, *height};
        }

        Optional<std::vector<LevelVector2>> readVector2Array(const rapidjson::Value &value)
        {
            if (!value.IsArray())
            {
                return nullopt;
            }

            std::vector<LevelVector2> outArray;
            outArray.reserve(value.Size());
            for (rapidjson::SizeType i = 0; i < value.Size(); ++i)
            {
                const auto point = readVector2(value[i]);
                if (!point.has_value())
                {
                    return nullopt;
                }

                outArray.push_back(*point);
            }

            return outArray;
        }

        bool parseSuit(const std::string &text, Suit &outSuit)
        {
            if (text == "Hearts")
            {
                outSuit = Suit::Hearts;
                return true;
            }
            if (text == "Diamonds")
            {
                outSuit = Suit::Diamonds;
                return true;
            }
            if (text == "Clubs")
            {
                outSuit = Suit::Clubs;
                return true;
            }
            if (text == "Spades")
            {
                outSuit = Suit::Spades;
                return true;
            }

            return false;
        }

        bool parseZone(const std::string &text, CardZone &outZone)
        {
            if (text == "Tableau")
            {
                outZone = CardZone::Tableau;
                return true;
            }
            if (text == "Stock")
            {
                outZone = CardZone::Stock;
                return true;
            }
            if (text == "Waste")
            {
                outZone = CardZone::Waste;
                return true;
            }

            return false;
        }
    }

    LevelLoadResult LevelLoader::loadFromJsonFile(const std::string &path)
    {
        const std::string content = cocos2d::FileUtils::getInstance()->getStringFromFile(path);
        if (content.empty())
        {
            return fail("Level file is empty or missing: " + path);
        }

        rapidjson::Document document;
        document.Parse<0>(content.c_str());
        if (document.HasParseError() || !document.IsObject())
        {
            return fail("Level JSON is invalid: " + path);
        }

        LevelDefinition definition;

        if (!document.HasMember("tableauSlots"))
        {
            return fail("tableauSlots must be an integer array.");
        }
        const auto tableauSlots = readIntArray(document["tableauSlots"]);
        if (!tableauSlots.has_value())
        {
            return fail("tableauSlots must be an integer array.");
        }
        definition.tableauSlots = *tableauSlots;

        if (!document.HasMember("stockPile"))
        {
            return fail("stockPile must be an integer array.");
        }
        const auto stockPile = readIntArray(document["stockPile"]);
        if (!stockPile.has_value())
        {
            return fail("stockPile must be an integer array.");
        }
        definition.stockPile = *stockPile;

        if (!document.HasMember("wastePile"))
        {
            return fail("wastePile must be an integer array.");
        }
        const auto wastePile = readIntArray(document["wastePile"]);
        if (!wastePile.has_value())
        {
            return fail("wastePile must be an integer array.");
        }
        definition.wastePile = *wastePile;

        if (!document.HasMember("cards") || !document["cards"].IsArray())
        {
            return fail("cards must be an array.");
        }

        definition.levelId = document.HasMember("levelId") && document["levelId"].IsString() ? document["levelId"].GetString() : "";

        if (document.HasMember("layout"))
        {
            const rapidjson::Value &layout = document["layout"];
            if (!layout.IsObject())
            {
                return fail("layout must be an object.");
            }

            if (layout.HasMember("cardSize"))
            {
                const auto cardSize = readSize(layout["cardSize"]);
                if (!cardSize.has_value())
                {
                    return fail("layout.cardSize must contain numeric width and height.");
                }
                definition.layout.cardSize = *cardSize;
            }

            if (layout.HasMember("stockBasePosition"))
            {
                const auto stockBasePosition = readVector2(layout["stockBasePosition"]);
                if (!stockBasePosition.has_value())
                {
                    return fail("layout.stockBasePosition must contain numeric x and y.");
                }
                definition.layout.stockBasePosition = *stockBasePosition;
            }

            if (layout.HasMember("wasteBasePosition"))
            {
                const auto wasteBasePosition = readVector2(layout["wasteBasePosition"]);
                if (!wasteBasePosition.has_value())
                {
                    return fail("layout.wasteBasePosition must contain numeric x and y.");
                }
                definition.layout.wasteBasePosition = *wasteBasePosition;
            }

            if (layout.HasMember("stockPileDepthOffset"))
            {
                const auto stockPileDepthOffset = readVector2(layout["stockPileDepthOffset"]);
                if (!stockPileDepthOffset.has_value())
                {
                    return fail("layout.stockPileDepthOffset must contain numeric x and y.");
                }
                definition.layout.stockPileDepthOffset = *stockPileDepthOffset;
            }

            if (layout.HasMember("wastePileDepthOffset"))
            {
                const auto wastePileDepthOffset = readVector2(layout["wastePileDepthOffset"]);
                if (!wastePileDepthOffset.has_value())
                {
                    return fail("layout.wastePileDepthOffset must contain numeric x and y.");
                }
                definition.layout.wastePileDepthOffset = *wastePileDepthOffset;
            }

            if (layout.HasMember("coveredCardOffset"))
            {
                const auto coveredCardOffset = readVector2(layout["coveredCardOffset"]);
                if (!coveredCardOffset.has_value())
                {
                    return fail("layout.coveredCardOffset must contain numeric x and y.");
                }
                definition.layout.coveredCardOffset = *coveredCardOffset;
            }

            if (layout.HasMember("controlMenuPosition"))
            {
                const auto controlMenuPosition = readVector2(layout["controlMenuPosition"]);
                if (!controlMenuPosition.has_value())
                {
                    return fail("layout.controlMenuPosition must contain numeric x and y.");
                }
                definition.layout.controlMenuPosition = *controlMenuPosition;
            }

            if (layout.HasMember("tableauPositions"))
            {
                const auto tableauPositions = readVector2Array(layout["tableauPositions"]);
                if (!tableauPositions.has_value())
                {
                    return fail("layout.tableauPositions must be an array of {x, y}.");
                }
                definition.layout.tableauPositions = *tableauPositions;
            }
        }

        const rapidjson::Value &cards = document["cards"];
        definition.cards.clear();
        definition.cards.reserve(cards.Size());
        for (rapidjson::SizeType i = 0; i < cards.Size(); ++i)
        {
            const rapidjson::Value &cardValue = cards[i];
            if (!cardValue.IsObject())
            {
                return fail("Each card entry must be an object.");
            }

            LevelCardDefinition card;
            if (!cardValue.HasMember("id") || !cardValue["id"].IsInt())
            {
                return fail("Card id is missing or invalid.");
            }
            if (!cardValue.HasMember("suit") || !cardValue["suit"].IsString() || !parseSuit(cardValue["suit"].GetString(), card.suit))
            {
                return fail("Card suit is missing or invalid.");
            }
            if (!cardValue.HasMember("rank") || !cardValue["rank"].IsInt())
            {
                return fail("Card rank is missing or invalid.");
            }
            if (!cardValue.HasMember("zone") || !cardValue["zone"].IsString() || !parseZone(cardValue["zone"].GetString(), card.zone))
            {
                return fail("Card zone is missing or invalid.");
            }
            if (!cardValue.HasMember("faceUp") || !cardValue["faceUp"].IsBool())
            {
                return fail("Card faceUp is missing or invalid.");
            }

            card.id = cardValue["id"].GetInt();
            card.rank = cardValue["rank"].GetInt();
            card.faceUp = cardValue["faceUp"].GetBool();
            card.tableauIndex = cardValue.HasMember("tableauIndex") && cardValue["tableauIndex"].IsInt() ? cardValue["tableauIndex"].GetInt() : -1;

            if (cardValue.HasMember("blockers"))
            {
                const auto blockers = readIntArray(cardValue["blockers"]);
                if (!blockers.has_value())
                {
                    return fail("Card blockers must be an integer array.");
                }
                card.blockers = *blockers;
            }

            if (cardValue.HasMember("children"))
            {
                const auto children = readIntArray(cardValue["children"]);
                if (!children.has_value())
                {
                    return fail("Card children must be an integer array.");
                }
                card.children = *children;
            }

            definition.cards.push_back(card);
        }

        return {std::move(definition), ""};
    }
}
