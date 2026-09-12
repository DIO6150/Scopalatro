#pragma once

#include <vector>
#include <memory>
#include "Card.hpp"
#include "Deck.hpp"

class DeckBuilder {
public:
    static std::unique_ptr<Card> createScopaCard(Suit s, CardValue v);
	static Deck buildScopaDeck(bool withPlayerEffects = true);
    static std::vector<int> findCaptureIndices(const Card* playedCard,
                                               const std::vector<std::unique_ptr<Card>>& table,
											   int customTargetVal = -1);
};