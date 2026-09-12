#include <EffectSystem.hpp>
#include <Deck.hpp>
#include <algorithm>
#include <map>
#include <vector>
#include <functional>

Card * Deck::CreateScopaCard(Suit s, CardValue v) {
    std::string name = valueToString(v) + " of " + suitToString(s);
	m_cards.emplace_back(std::make_unique<Card>(name, "", s, v));

    return m_cards.back().get();
}

void Deck::BuildScopaDeck(bool withPlayerEffects) {
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
			[](Card* c){ c->AddEffect(std::make_unique<DamageEffect>(5)); }
		}},
		{{Suit::Denari, CardValue::Sette}, {
			[](Card* c){ c->AddEffect(std::make_unique<HealEffect>(5)); }
		}},
		{{Suit::Denari, CardValue::Re}, {
			[](Card* c){ c->AddEffect(std::make_unique<DamageEffect>(5)); },
			[](Card* c){ c->AddEffect(std::make_unique<BlockEffect>(5)); }
		}},
		{{Suit::Coppe, CardValue::Asso}, {
			[](Card* c){ c->AddEffect(std::make_unique<BlockEffect>(5)); }
		}},
		{{Suit::Denari, CardValue::Cavallo}, {
			[](Card* c){ c->AddEffect(std::make_unique<BlockEffect>(5)); },
			[](Card* c){ c->AddEffect(std::make_unique<HealEffect>(5)); }
		}},
		{{Suit::Coppe, CardValue::Cavallo}, {
			[](Card* c){ c->AddEffect(std::make_unique<DamageEffect>(5, TargetType::AllEnemies)); },
			[](Card* c){ c->AddEffect(std::make_unique<HealEffect>(5)); }
		}},
		{{Suit::Bastoni, CardValue::Fante}, {
			[](Card* c){ c->AddEffect(std::make_unique<DamageEffect>(3)); },
			[](Card* c){ c->AddEffect(std::make_unique<ApplyStatusEffect>("Poisoned", 2, 3)); }
		}}
	};

	for (Suit s : suits) {
		for (CardValue v : values) {
			auto card = CreateScopaCard(s, v);

			if (withPlayerEffects) {
				auto it = specialCards.find({ s, v });
				if (it != specialCards.end()) {
					for (auto& init : it->second) {
						init(card);
					}
				}
				else if (v == CardValue::Re) {
					card->AddEffect(std::make_unique<DamageEffect>(10));
				}
			}
		}
	}
}
