#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "EffectSystem.hpp"
#include "Entity.hpp"
#include "ScopaDeck.hpp"

class Card {
public:
    Card(std::string name, std::string desc, Suit s = Suit::Denari, CardValue v = CardValue::Asso) 
        : name(name), description(desc), suit(s), value(v) {}

    
    void addEffect(std::unique_ptr<Effect> effect) {
        effects.push_back(std::move(effect));
    }

    void addEffect(Effect* effect) {
        effects.push_back(std::unique_ptr<Effect>(effect));
    }

	std::unique_ptr<Card> clone() const {
		auto cloned = std::make_unique<Card>(name, description, suit, value);
		for (const auto& e : effects) {
			cloned->addEffect(e->clone());
		}
		return cloned;
	}

    Suit getSuit() const { return suit; }
	Suit getEffectiveSuit(Entity* owner = nullptr) const {
		if (owner && owner->hasPassive("Magic Trick")) {
			if (suit == Suit::Bastoni) return Suit::Coppe;
			if (suit == Suit::Coppe) return Suit::Bastoni;
			if (suit == Suit::Spada) return Suit::Denari;
			if (suit == Suit::Denari) return Suit::Spada;
		}
		return suit;
	}
	
	void setSuit(Suit s) { 
		suit = s;
		
		size_t pos = name.find(" of ");
		if (pos != std::string::npos && name.find("Show of") ==
		std::string::npos) {
			name = name.substr(0, pos) + " of " + suitToString(s);
		}
	}

    CardValue getValue() const { return value; }
    int getNumericValue() const { return static_cast<int>(value) + valueBoost; }
	void boostValue(int amt) { valueBoost += amt; }

	void play(Entity& source, std::vector<Entity*> targets,
		std::function<void(int)> drawCallback = nullptr,
		std::vector<std::unique_ptr<Card>>* scorePile = nullptr,
		std::vector<std::unique_ptr<Card>>* table = nullptr) {
        std::cout << "\n> Playing card: " << name << std::endl;

        EffectContext ctx;
        ctx.source = &source;
        ctx.targets = targets;
        ctx.cardSource = this;
		ctx.drawCallback = drawCallback;
		ctx.scorePile = scorePile;
		ctx.table = table;

        for (auto& effect : effects) {
            std::cout << " [Effect]: " << effect->getDescription() << std::endl;
            effect->apply(ctx); 
        }
    }

    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
	void setName(const std::string& n) { name = n; }
    bool hasEffects() const { return !effects.empty(); }
    std::string getEffectsSummary() const {
        std::string result;
        for (const auto& e : effects) {
            if (!result.empty()) result += ", ";
            result += e->getDescription();
        }
        return result;
    }

private:
    std::string name;
    std::string description;
    Suit suit;
    CardValue value;
	int valueBoost = 0;
    std::vector<std::unique_ptr<Effect>> effects;
};

