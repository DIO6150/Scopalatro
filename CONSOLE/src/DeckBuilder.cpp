#include "DeckBuilder.hpp"
#include "EffectSystem.hpp"
#include <algorithm>
#include <map>
#include <vector>
#include <functional>

std::unique_ptr<Card> DeckBuilder::createScopaCard(Suit s, CardValue v) {
    std::string name = valueToString(v) + " of " + suitToString(s);
    return std::make_unique<Card>(name, "", s, v);
}

Deck DeckBuilder::buildScopaDeck(bool withPlayerEffects) {
    Deck deck;

    const std::vector<Suit> suits = {
        Suit::Denari, Suit::Coppe, Suit::Spada, Suit::Bastoni
    };
    const std::vector<CardValue> values = {
        CardValue::Asso,    CardValue::Due,     CardValue::Tre,
        CardValue::Quattro, CardValue::Cinque,  CardValue::Sei,
        CardValue::Sette,   CardValue::Fante,   CardValue::Cavallo,
        CardValue::Re
    };

    using CardKey = std::pair<Suit, CardValue>;
    using EffectInit = std::function<void(Card*)>;

    static const std::map<CardKey, std::vector<EffectInit>> specialCards = {
        {{Suit::Spada, CardValue::Asso}, {
            [](Card* c){ c->addEffect(std::make_unique<DamageEffect>(5)); }
        }},
        {{Suit::Denari, CardValue::Sette}, {
            [](Card* c){ c->addEffect(std::make_unique<HealEffect>(5)); }
        }},
        {{Suit::Denari, CardValue::Re}, {
            [](Card* c){ c->addEffect(std::make_unique<DamageEffect>(5)); },
            [](Card* c){ c->addEffect(std::make_unique<BlockEffect>(5)); }
        }},
        {{Suit::Coppe, CardValue::Asso}, {
            [](Card* c){ c->addEffect(std::make_unique<BlockEffect>(5)); }
        }},
        {{Suit::Denari, CardValue::Cavallo}, {
            [](Card* c){ c->addEffect(std::make_unique<BlockEffect>(5)); },
            [](Card* c){ c->addEffect(std::make_unique<HealEffect>(5)); }
        }},
        {{Suit::Coppe, CardValue::Cavallo}, {
            [](Card* c){ c->addEffect(std::make_unique<DamageEffect>(5, TargetType::AllEnemies)); },
            [](Card* c){ c->addEffect(std::make_unique<HealEffect>(5)); }
        }},
        {{Suit::Bastoni, CardValue::Fante}, {
            [](Card* c){ c->addEffect(std::make_unique<DamageEffect>(3)); },
            [](Card* c){ c->addEffect(std::make_unique<ApplyStatusEffect>("Poisoned", 2, 3)); }
        }}
    };

    for (Suit s : suits) {
        for (CardValue v : values) {
            auto card = createScopaCard(s, v);

			if (withPlayerEffects) {
				auto it = specialCards.find({ s, v });
				if (it != specialCards.end()) {
					for (auto& init : it->second) {
						init(card.get());
					}
				}
				else if (v == CardValue::Re) {
					card->addEffect(std::make_unique<DamageEffect>(10));
				}
			}
            
            deck.addCard(std::move(card));
        }
    }
    
    return deck;
}
           

std::vector<int> DeckBuilder::findCaptureIndices(const Card* playedCard,
                                                 const std::vector<std::unique_ptr<Card>>& table, int customTargetVal) {
	
	int targetVal = customTargetVal > 0 ? customTargetVal : playedCard->getNumericValue();

    for (size_t i = 0; i < table.size(); ++i) {
        if (table[i]->getNumericValue() == targetVal) {
            return { static_cast<int>(i) };
        }
    }

    int n = table.size();
    for (int i = 1; i < (1 << n); ++i) {
        int currentSum = 0;
        std::vector<int> indices;
        for (int j = 0; j < n; ++j) {
            if ((i >> j) & 1) {
                currentSum += table[j]->getNumericValue();
                indices.push_back(j);
            }
        }
        if (currentSum == targetVal) {
            return indices;
        }
    }        

    return {};
}