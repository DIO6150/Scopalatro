#pragma once

#include <vector>
#include <memory>

#include <Card.hpp>

class Deck {
public:
    Card * CreateScopaCard(Suit s, CardValue v);
	void BuildScopaDeck(bool withPlayerEffects = true);
	auto & GetCards() { return m_cards; }

private:
	std::vector<std::unique_ptr<Card>> m_cards;
};
