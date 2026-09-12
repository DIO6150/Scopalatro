#pragma once
#include <string>

enum class Suit {
    Coppe,   // Coupes
    Denari,  // Deniers
    Spada,   // Epées
    Bastoni, // Bâtons
};

enum class CardValue {
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
        case Suit::Coppe: return "Coupes";
        case Suit::Denari: return "Deniers";
        case Suit::Spada: return "Epees";
        case Suit::Bastoni: return "Batons";
        default: return "Unknown";
    }
}

inline std::string valueToString(CardValue v) {
    switch(v) {
        case CardValue::Fante: return "Valet";
        case CardValue::Cavallo: return "Cavalier";
        case CardValue::Re: return "Roi";
        default: return std::to_string(static_cast<int>(v));
    }
}