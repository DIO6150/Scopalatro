#pragma once

#include <algorithm>
#include <vector>
#include <Card.hpp>
#include <chrono>
#include <random>

class CardPiles
{
public:
	enum PileType
	{
		Hand,
		Draw,
		Discard,
		Capture,
		Exhaust,
		None
	};

	void insert(Card * card, PileType pile)
	{
		auto p = getPile(pile);
		if (p) p->emplace_back(card);
	}

	void insert(std::vector<Card *> cards, PileType pile)
	{
		for (auto card : cards)
			insert(card, pile);
	}

	void remove(Card * card, PileType srcPile)
	{
		auto pile = getPile(srcPile);
		auto it = std::find(pile->begin(), pile->end(), card);
		if (it != pile->end())
			pile->erase(it);
	}

	std::vector<Card *> draw(int count)
	{
		std::vector<Card *> drawnCards;

		while(count > drawnCards.size())
		{
			if(drawPile.empty())
			{
				shuffle();

				if(drawPile.empty())
					break;
			}

			auto card = drawPile.back();
			drawPile.pop_back();

			drawnCards.push_back(card);
		}

		return drawnCards;
	}

	void shuffle()
	{
		std::vector<Card *> cards;
	
		cards.insert(cards.end(), drawPile.begin   (), drawPile.end   ());
		cards.insert(cards.end(), discardPile.begin(), discardPile.end());
		
		unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::shuffle(cards.begin(), cards.end(), std::default_random_engine(seed));

		drawPile.clear();
		discardPile.clear();

		drawPile = cards;
	}

	bool moveIn(Card * card, PileType dst, PileType srcHint)
	{
		if (dst == srcHint)
			return false;

		if (dst == None)
			return false;

		if (srcHint == None)
		{
			{
				auto itHand = std::find(hand.begin(), hand.end(), card);
				if (itHand != hand.end()) move(card, getPile(dst), &hand, itHand);

				return true;
			}

			{
				auto itDraw = std::find(drawPile.begin(), drawPile.end(), card);
				if (itDraw != drawPile.end()) move(card, getPile(dst), &drawPile, itDraw);

				return true;
			}

			{
				auto itDiscard = std::find(discardPile.begin(), discardPile.end(), card);
				if (itDiscard != discardPile.end()) move(card, getPile(dst), &discardPile, itDiscard);

				return true;
			}

			{
				auto itCapture = std::find(capturePile.begin(), capturePile.end(), card);
				if (itCapture != capturePile.end()) move(card, getPile(dst), &capturePile, itCapture);

				return true;
			}

			{
				auto itExhaust = std::find(exhaustPile.begin(), exhaustPile.end(), card);
				if (itExhaust != exhaustPile.end()) move(card, getPile(dst), &exhaustPile, itExhaust);

				return true;
			}

			return false;
		}

		auto pile = getPile(srcHint);
		auto it = std::find(pile->begin(), pile->end(), card);
		if (it != pile->end())
		{
			move(card, getPile(dst), pile, it);
			return true;
		}

		return false;
	}

	std::vector<Card *> hand;
	std::vector<Card *> drawPile;
	std::vector<Card *> discardPile;
	std::vector<Card *> capturePile;

	std::vector<Card *> exhaustPile;

private:
	std::vector<Card *> * getPile(PileType pile)
	{
		switch (pile)
		{
		case Hand: return &hand;
		case Draw: return &drawPile;
		case Discard: return &discardPile;
		case Capture: return &capturePile;
		case Exhaust: return &exhaustPile;
		
		default: return nullptr;
		}
	}

	void move
	(
		Card * card,
		std::vector<Card *> * destination,
		std::vector<Card *> * source,
		std::vector<Card *>::iterator sourceIterator
	)
	{
		if (!source || !destination)
			return;

		source->erase(sourceIterator);
		destination->emplace_back(card);
	}
};