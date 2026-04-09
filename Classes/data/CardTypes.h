#ifndef __CARDGAME_CARD_TYPES_H__
#define __CARDGAME_CARD_TYPES_H__

#include <string>

namespace cardgame
{
enum class Suit
{
    Hearts,
    Diamonds,
    Clubs,
    Spades
};

enum class CardZone
{
    Tableau,
    Stock,
    Waste
};

inline bool isRedSuit(Suit suit)
{
    return suit == Suit::Hearts || suit == Suit::Diamonds;
}

inline std::string suitToText(Suit suit)
{
    switch (suit)
    {
    case Suit::Hearts:
        return "H";
    case Suit::Diamonds:
        return "D";
    case Suit::Clubs:
        return "C";
    case Suit::Spades:
        return "S";
    default:
        return "?";
    }
}

inline std::string rankToText(int rank)
{
    switch (rank)
    {
    case 1:
        return "A";
    case 11:
        return "J";
    case 12:
        return "Q";
    case 13:
        return "K";
    default:
        return std::to_string(rank);
    }
}

inline std::string cardToText(Suit suit, int rank)
{
    return suitToText(suit) + rankToText(rank);
}
}

#endif
