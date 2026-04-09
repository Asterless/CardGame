#include "data/LevelLoader.h"

#include "cocos2d.h"
#include "json/document.h"

namespace cardgame
{
namespace
{
bool fail(const std::string& message, std::string* outErrorMessage)
{
    if (outErrorMessage != nullptr)
    {
        *outErrorMessage = message;
    }
    return false;
}

bool readIntArray(const rapidjson::Value& value, std::vector<int>& outArray)
{
    if (!value.IsArray())
    {
        return false;
    }

    outArray.clear();
    for (rapidjson::SizeType i = 0; i < value.Size(); ++i)
    {
        if (!value[i].IsInt())
        {
            return false;
        }

        outArray.push_back(value[i].GetInt());
    }

    return true;
}

bool readFloat(const rapidjson::Value& value, float& outValue)
{
    if (!value.IsNumber())
    {
        return false;
    }

    outValue = value.GetFloat();
    return true;
}

bool readVector2(const rapidjson::Value& value, LevelVector2& outValue)
{
    if (!value.IsObject() || !value.HasMember("x") || !value.HasMember("y"))
    {
        return false;
    }

    return readFloat(value["x"], outValue.x) && readFloat(value["y"], outValue.y);
}

bool readSize(const rapidjson::Value& value, LevelSize& outSize)
{
    if (!value.IsObject() || !value.HasMember("width") || !value.HasMember("height"))
    {
        return false;
    }

    return readFloat(value["width"], outSize.width) && readFloat(value["height"], outSize.height);
}

bool readVector2Array(const rapidjson::Value& value, std::vector<LevelVector2>& outArray)
{
    if (!value.IsArray())
    {
        return false;
    }

    outArray.clear();
    for (rapidjson::SizeType i = 0; i < value.Size(); ++i)
    {
        LevelVector2 point;
        if (!readVector2(value[i], point))
        {
            return false;
        }

        outArray.push_back(point);
    }

    return true;
}

bool parseSuit(const std::string& text, Suit& outSuit)
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

bool parseZone(const std::string& text, CardZone& outZone)
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

bool LevelLoader::loadFromJsonFile(const std::string& path, LevelDefinition& outDefinition, std::string* outErrorMessage)
{
    const std::string content = cocos2d::FileUtils::getInstance()->getStringFromFile(path);
    if (content.empty())
    {
        return fail("Level file is empty or missing: " + path, outErrorMessage);
    }

    rapidjson::Document document;
    document.Parse<0>(content.c_str());
    if (document.HasParseError() || !document.IsObject())
    {
        return fail("Level JSON is invalid: " + path, outErrorMessage);
    }

    if (!document.HasMember("tableauSlots") || !readIntArray(document["tableauSlots"], outDefinition.tableauSlots))
    {
        return fail("tableauSlots must be an integer array.", outErrorMessage);
    }
    if (!document.HasMember("stockPile") || !readIntArray(document["stockPile"], outDefinition.stockPile))
    {
        return fail("stockPile must be an integer array.", outErrorMessage);
    }
    if (!document.HasMember("wastePile") || !readIntArray(document["wastePile"], outDefinition.wastePile))
    {
        return fail("wastePile must be an integer array.", outErrorMessage);
    }
    if (!document.HasMember("cards") || !document["cards"].IsArray())
    {
        return fail("cards must be an array.", outErrorMessage);
    }

    outDefinition.levelId = document.HasMember("levelId") && document["levelId"].IsString() ? document["levelId"].GetString() : "";
    outDefinition.cards.clear();
    outDefinition.layout = LevelLayoutDefinition();

    if (document.HasMember("layout"))
    {
        const rapidjson::Value& layout = document["layout"];
        if (!layout.IsObject())
        {
            return fail("layout must be an object.", outErrorMessage);
        }
        if (layout.HasMember("cardSize") && !readSize(layout["cardSize"], outDefinition.layout.cardSize))
        {
            return fail("layout.cardSize must contain numeric width and height.", outErrorMessage);
        }
        outDefinition.layout.hasCardSize = layout.HasMember("cardSize");
        if (layout.HasMember("stockBasePosition") && !readVector2(layout["stockBasePosition"], outDefinition.layout.stockBasePosition))
        {
            return fail("layout.stockBasePosition must contain numeric x and y.", outErrorMessage);
        }
        outDefinition.layout.hasStockBasePosition = layout.HasMember("stockBasePosition");
        if (layout.HasMember("wasteBasePosition") && !readVector2(layout["wasteBasePosition"], outDefinition.layout.wasteBasePosition))
        {
            return fail("layout.wasteBasePosition must contain numeric x and y.", outErrorMessage);
        }
        outDefinition.layout.hasWasteBasePosition = layout.HasMember("wasteBasePosition");
        if (layout.HasMember("stockPileDepthOffset") && !readVector2(layout["stockPileDepthOffset"], outDefinition.layout.stockPileDepthOffset))
        {
            return fail("layout.stockPileDepthOffset must contain numeric x and y.", outErrorMessage);
        }
        outDefinition.layout.hasStockPileDepthOffset = layout.HasMember("stockPileDepthOffset");
        if (layout.HasMember("wastePileDepthOffset") && !readVector2(layout["wastePileDepthOffset"], outDefinition.layout.wastePileDepthOffset))
        {
            return fail("layout.wastePileDepthOffset must contain numeric x and y.", outErrorMessage);
        }
        outDefinition.layout.hasWastePileDepthOffset = layout.HasMember("wastePileDepthOffset");
        if (layout.HasMember("coveredCardOffset") && !readVector2(layout["coveredCardOffset"], outDefinition.layout.coveredCardOffset))
        {
            return fail("layout.coveredCardOffset must contain numeric x and y.", outErrorMessage);
        }
        outDefinition.layout.hasCoveredCardOffset = layout.HasMember("coveredCardOffset");
        if (layout.HasMember("controlMenuPosition") && !readVector2(layout["controlMenuPosition"], outDefinition.layout.controlMenuPosition))
        {
            return fail("layout.controlMenuPosition must contain numeric x and y.", outErrorMessage);
        }
        outDefinition.layout.hasControlMenuPosition = layout.HasMember("controlMenuPosition");
        if (layout.HasMember("tableauPositions") && !readVector2Array(layout["tableauPositions"], outDefinition.layout.tableauPositions))
        {
            return fail("layout.tableauPositions must be an array of {x, y}.", outErrorMessage);
        }
    }

    const rapidjson::Value& cards = document["cards"];
    for (rapidjson::SizeType i = 0; i < cards.Size(); ++i)
    {
        const rapidjson::Value& cardValue = cards[i];
        if (!cardValue.IsObject())
        {
            return fail("Each card entry must be an object.", outErrorMessage);
        }

        LevelCardDefinition card;
        if (!cardValue.HasMember("id") || !cardValue["id"].IsInt())
        {
            return fail("Card id is missing or invalid.", outErrorMessage);
        }
        if (!cardValue.HasMember("suit") || !cardValue["suit"].IsString() || !parseSuit(cardValue["suit"].GetString(), card.suit))
        {
            return fail("Card suit is missing or invalid.", outErrorMessage);
        }
        if (!cardValue.HasMember("rank") || !cardValue["rank"].IsInt())
        {
            return fail("Card rank is missing or invalid.", outErrorMessage);
        }
        if (!cardValue.HasMember("zone") || !cardValue["zone"].IsString() || !parseZone(cardValue["zone"].GetString(), card.zone))
        {
            return fail("Card zone is missing or invalid.", outErrorMessage);
        }
        if (!cardValue.HasMember("faceUp") || !cardValue["faceUp"].IsBool())
        {
            return fail("Card faceUp is missing or invalid.", outErrorMessage);
        }

        card.id = cardValue["id"].GetInt();
        card.rank = cardValue["rank"].GetInt();
        card.faceUp = cardValue["faceUp"].GetBool();
        card.tableauIndex = cardValue.HasMember("tableauIndex") && cardValue["tableauIndex"].IsInt() ? cardValue["tableauIndex"].GetInt() : -1;

        if (cardValue.HasMember("blockers") && !readIntArray(cardValue["blockers"], card.blockers))
        {
            return fail("Card blockers must be an integer array.", outErrorMessage);
        }
        if (cardValue.HasMember("children") && !readIntArray(cardValue["children"], card.children))
        {
            return fail("Card children must be an integer array.", outErrorMessage);
        }

        outDefinition.cards.push_back(card);
    }

    return true;
}
}
