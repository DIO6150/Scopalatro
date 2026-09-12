#pragma once
#include "RelicSystem.hpp"
#include "Entity.hpp"
#include "Card.hpp"
#include <iostream>

class HiddenAdvantage : public Relic {
public:
	HiddenAdvantage() : Relic("Hidden Advantage", "Draw an extra card card at the start of your turn.",
		RelicTier::Uncommon) {}

	int getExtraDraw() const override {
		return 1;
	}
};

class DamageAndInterest : public Relic {
public:
	DamageAndInterest() : Relic("Damage and Interest", "Earn an extra 1 gold at the end of a battle.",
		RelicTier::Common) {}

	void onBattleEnd(Entity& player, int scopaCount, int& goldBonus) override {
		goldBonus += 1;
		std::cout << "[Damage and Interest] collected 1 extra Gold!" << std::endl;
	}
};

class GoldenScopa : public Relic {
public:
	GoldenScopa() : Relic("Golden Scopa",
		"Earn an extra 5 gold permanently if 3 or more Scopa have been achieved during a battle.", RelicTier::Common) {}

	void onBattleEnd(Entity& player, int scopaCount, int& goldBonus) override {
		if (scopaCount >= 3) {
			goldBonus += 5;
			std::cout << "[Golden Scopa] 3+ Scopas achieved! Rewarded 5 extra Gold!" << std::endl;
		}
	}
};

class CursedThorns : public Relic {
public:
	CursedThorns() : Relic("Cursed Thorns", 
		"Each instance of unblocked damage taken has a 33% chance of inflicting Bleeding, Poison, or Burn on the attacker.",
		RelicTier::Uncommon) {}

	void onDamageReceived(Entity& target, Entity& attacker, int amount) override {
		if (rand() % 100 < 33) {
			int r = rand() % 3;
			if (r == 0) {
				attacker.addStatus("Bleeding", 2, 3);
			} else if (r == 1) {
				attacker.addStatus("Poisoned", 2, 3);
			} else {
				attacker.addStatus("Burn", 2, 3);
			}
			std::cout << "[Cursed Thorns] The thorns striked back at " <<
				attacker.getName() << "!" << std::endl;
		}
	}
};

class CocktailOverdose : public Relic {
public:
	CocktailOverdose() : Relic("Cocktail Overdose",
		"When a Cup spell captures up to two cards, 25 % chance to Poison the enemy.",
		RelicTier::Uncommon) {}

	void onCapture(CaptureEvent& evt) override {
		if (evt.playedCard->getEffectiveSuit(evt.player) == Suit::Coppe && evt.playedCard->hasEffects()) {
			if (evt.captured.size() <= 2) {
				if (rand() % 100 < 25) { //Change to 25
					evt.enemy->addStatus("Poisoned", 2, 3);
					std::cout << "[Cocktail Overdose] Enemy Poisoned!" << std::endl;
				}
			}
		}
	}
};

class MolotovCocktail : public Relic {
public:
	MolotovCocktail() : Relic("Molotov Cocktail",
		"When a Cup spell captures exactly two cards, 25% chance to Burn the enemy.",
		RelicTier::Uncommon) {}

	void onCapture(CaptureEvent& evt) override {
		if (evt.playedCard->getEffectiveSuit(evt.player) == Suit::Coppe && evt.playedCard->hasEffects()) {
			if (evt.captured.size() == 2) {
				if (rand() % 100 < 25) { // 25
					evt.enemy->addStatus("Burn", 2, 3);
					std::cout << "[Molotov Cocktail] Enemy Burned!" << std::endl;
				}
			}
		}
	}
};

class GlassCocktail : public Relic {
public:
	GlassCocktail() : Relic("Glass Cocktail",
		"When a Cup spell captures at least two cards, 25% chance to Bleed the enemy.",
		RelicTier::Uncommon) {
	}

	void onCapture(CaptureEvent& evt) override {
		if (evt.playedCard->getEffectiveSuit(evt.player) == Suit::Coppe && evt.playedCard->hasEffects()) {
			if (evt.captured.size() >= 2) {
				if (rand() % 100 < 25) { // 25
					evt.enemy->addStatus("Bleeding", 2, 3);
					std::cout << "[Glass Cocktail] Enemy Bleeding!" << std::endl;
				}
			}
		}
	}
};

class BloodstainedDagger : public Relic {
public:
	BloodstainedDagger() : Relic("Bloodstained Dagger",
		"When a Sword spell captures, apply 1 Bleeding stack per card captured.",
		RelicTier::Rare) {}

	void onCapture(CaptureEvent& evt) override {
		if (evt.playedCard->getEffectiveSuit(evt.player) == Suit::Spada && evt.playedCard->hasEffects()) {
			int stacks = evt.captured.size();
			evt.enemy->addStatus("Bleeding", 2, stacks * 3);
			std::cout << "[Bloodstained Dagger] Applied " << stacks << " stacks of Bleeding!" << std::endl;
		}
	}
};

class NDA : public Relic {
	int m_captureCount = 0;
public:
	NDA() : Relic("NDA", "The capture value of Spells increases by 1 for every 5 cards captured.",
		RelicTier::Uncommon) {}

	void onCapture(CaptureEvent& evt) override {
		m_captureCount += evt.captured.size();
		while (m_captureCount >= 5) {
			m_captureCount -= 5;
			evt.player->addSpellValueBoost(1);
			std::cout << "[NDA] Spells permanently gained +1 Capture Value!" << std::endl;
		}
	}
};

class GamblersFallacy : public Relic {
	int m_battlesLeft = 3;
public:
	GamblersFallacy() : Relic("Gambler's Fallacy",
		"For 3 battles, each Denari card captured gives a 50% chance to double your gold, or lose it all.",
		RelicTier::Common) {}

	void onBattleEnd(Entity& player, int scopaCount, int& goldBonus) override {
		m_battlesLeft--;
	}

	bool isExpired() const override {
		return m_battlesLeft <= 0;
	}

	void onCapture(CaptureEvent& evt) override {
		for (auto* c : evt.captured) {
			if (c->getEffectiveSuit(evt.player) == Suit::Denari) {
				if (rand() % 100 < 50) {
					int gold = evt.player->getGold();
					evt.player->addGold(gold);
					std::cout << "[Gambler's Fallacy] Luck is on your side! Gold doubled to " <<
						evt.player->getGold() << "!" << std::endl;
				}
				else {
					evt.player->spendGold(evt.player->getGold());
					std::cout << "[Gambler's Fallacy] You lost the bet! All gold lost!" << std::endl;
				}
				std::cout << "[Gambler's Fallacy] " << c->getName() << " was consumed!" << std::endl;
				auto it = evt.scorePile->begin();
				while (it != evt.scorePile->end()) {
					if (it->get() == c) {
						evt.scorePile->erase(it);
						break;
					}
					++it;
				}
			}
		}
	}
};

class Flashbang : public Relic {
	int m_casts = 0;
	int m_triggers = 0;
public:
	Flashbang() : Relic("Flashbang", "Every 4 Gold Spells cast, applies Blind to all enemies (max 2/battle).",
		RelicTier::Rare) {}

	void onBattleStart(Entity& player) override {
		m_casts = 0;
		m_triggers = 0;
	}

	void onSpellCast(Card* spell, Entity& player, Entity& enemy) override {
		if (spell->getEffectiveSuit(&player) == Suit::Denari) {
			m_casts++;
			if (m_casts >= 4) {
				m_casts = 0;
				if (m_triggers < 2) {
					m_triggers++;
					enemy.addStatus("Blindness", 2, 50);
					std::cout << "[Flashbang] FLASH! Enemy is Blinded and a Gypceros is heard laughing in the distance! (" <<
						m_triggers << "/2)" << std::endl;
				}
			}
		}
	}
};

class MagicTrick : public Relic {
public:
	MagicTrick() : Relic("Magic Trick", "Clubs spells are considered Cups, Cups as Clubs, Swords as Gold, Gold as Swords.",
		RelicTier::Rare) {}

	void onPickup(Entity& player) override {
		player.addPassive("Magic Trick");
	}
};