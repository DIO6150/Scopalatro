#pragma once

enum class Suit
{
    Coppe,   // Coupes
    Denari,  // Deniers
    Spada,   // Epées
    Bastoni, // Bâtons
};

enum class CardValue
{
    Asso = 1,
    Due = 2,
    Tre = 3,
    Quattro = 4,
    Cinque = 5,
    Sei = 6,
    Sette = 7,
    Fante = 8,   // Valet
    Cavallo = 9, // Cavalier
    Re = 10,     // Roi
};

inline std::string suitToString(Suit s) {
    switch(s) {
        case Suit::Coppe: return "cup";
        case Suit::Denari: return "deny";
        case Suit::Spada: return "sword";
        case Suit::Bastoni: return "club";
        default: return "unknown";
    }
}

inline std::string valueToString(CardValue v) {
    switch(v) {
        case CardValue::Fante: return "fante";
        case CardValue::Cavallo: return "cavallo";
        case CardValue::Re: return "re";
        case CardValue::Asso: return "ace";
        default: return std::to_string(static_cast<int>(v));
    }
}
