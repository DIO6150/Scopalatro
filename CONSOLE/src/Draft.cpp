#include "Draft.hpp"
#include "DeckBuilder.hpp"
#include "EffectSystem.hpp"
#include <iostream>
#include <random>

void Draft::run(Entity& player,
	std::vector<std::unique_ptr<Card>>& draftedCards,
	std::vector<std::function<bool(Card*)>>& enchantments) {
	
	std::cout << "LOOT REWARDS\n";
	std::cout << "\n[" << player.getName() << "] HP: " << player.getHp() << "/" << player.getMaxHp() 
			  << " | Gold: " << player.getGold() << "\n\n";
	std::cout << "Choose a card to add to your deck or enchant an existing card.\n\n";

	std::random_device rd;
	std::mt19937 rng(rd());

	const std::vector<Suit> suits = { Suit::Denari, Suit::Coppe,
		Suit::Spada, Suit::Bastoni };
	const std::vector<CardValue> values = {
		CardValue::Asso, CardValue::Due, CardValue::Tre, CardValue::Quattro,
		CardValue::Cinque, CardValue::Sei, CardValue::Sette, CardValue::Fante,
		CardValue::Cavallo, CardValue::Re
	};

	std::uniform_int_distribution<int> suitDist(0, 3);
	std::uniform_int_distribution<int> valDist(0, 9);

	auto generateRandomCard = [&](std::mt19937& gen) -> std::unique_ptr<Card> {
		auto card = DeckBuilder::createScopaCard(suits[suitDist(gen)], values[valDist(gen)]);
		std::uniform_int_distribution<int> typeDist(0, 13);
		int type = typeDist(gen);

		if (type == 0) {
			std::uniform_int_distribution<int> dmgDist(5, 12);
			card->addEffect(std::make_unique<DamageEffect>(dmgDist(gen), TargetType::Enemy));
			card->addEffect(std::make_unique<ApplyStatusEffect>("Vulnerable", 2, 50, TargetType::Enemy));
			card->setName(card->getName() + " [Attack]");
		}
		else if (type == 1) {
			std::uniform_int_distribution<int> dmgDist(5, 12);
			card->addEffect(std::make_unique<DamageEffect>(dmgDist(gen), TargetType::Enemy));
			card->addEffect(std::make_unique<ApplyStatusEffect>("Poisoned", 3, 3, TargetType::Enemy));
			card->setName(card->getName() + " [Poison]");
		}
		else if (type == 2) {
			std::uniform_int_distribution<int> dmgDist(3, 8);
			card->addEffect(std::make_unique<DamageEffect>(dmgDist(gen), TargetType::Enemy));
			card->addEffect(std::make_unique<ApplyStatusEffect>("Stun", 1, 0, TargetType::Enemy));
			card->setName(card->getName() + " [Slow]");
		}
		else if (type == 3) {
			std::uniform_int_distribution<int> blkDist(8, 15);
			card->addEffect(std::make_unique<BlockEffect>(blkDist(gen), TargetType::Self));
			card->addEffect(std::make_unique<DrawCardEffect>(2, TargetType::Self));
			card->setName(card->getName() + " [Defensive Pick]");
		}
		else if (type == 4) {
			std::uniform_int_distribution<int> healDist(3, 6);
			card->addEffect(std::make_unique<ApplyStatusEffect>("Healing", 3, healDist(gen), TargetType::Self));
			card->setName(card->getName() + " [Care Factor]");
		}
		else if (type == 5) {
			card->addEffect(std::make_unique<HarvestEffect>(TargetType::AllEnemies));
			card->setName(card->getName() + " [Harvest]");
		}
		else if (type == 6) {
			std::uniform_int_distribution<int> boostDist(2, 4);
			std::uniform_int_distribution<int> dmgDist(3, 8);
			card->addEffect(std::make_unique<BoostTableEffect>(boostDist(gen)));
			card->addEffect(std::make_unique<DamageEffect>(dmgDist(gen), TargetType::Enemy));
			card->setName(card->getName() + " [Show of Strength]");
		}
		else if (type == 7) {
			Suit randomSuit = suits[suitDist(gen)];
			std::uniform_int_distribution<int> dmgDist(3, 8);
			card->addEffect(std::make_unique<ChangeTableSuitEffect>(randomSuit));
			card->addEffect(std::make_unique<DamageEffect>(dmgDist(gen), TargetType::Enemy));
			card->setName(card->getName() + " [Demonstration of Intelligence]");
		}
		else if (type == 8) {
			std::uniform_int_distribution<int> blkDist(5, 10);
			card->addEffect(std::make_unique<DoubleDebuffsEffect>(TargetType::Enemy));
			card->addEffect(std::make_unique<BlockEffect>(blkDist(gen), TargetType::Self));
			card->setName(card->getName() + " [Incurable Disease]");
		}
		else if (type == 9) {
			card->addEffect(std::make_unique<PoisonExplosionEffect>(TargetType::Enemy));
			card->setName(card->getName() + " [Poison Explosion]");
		}
		else if (type == 10) {
			card->addEffect(std::make_unique<ApplyStatusEffect>("Blindness", 3, 50, TargetType::Enemy));
			card->setName(card->getName() + " [Blindness]");
		}
		else if (type == 11) {
			card->addEffect(std::make_unique<ApplyStatusEffect>("Berserk", 3, 5, TargetType::Self));
			card->setName(card->getName() + " [Berserk]");
		}
		else if (type == 12) {
			card->addEffect(std::make_unique<ApplyStatusEffect>("Confusion", 2, 0, TargetType::Enemy));
			card->setName(card->getName() + " [Confusion]");
		}
		else if (type == 13) {
			card->addEffect(std::make_unique<ApplyStatusEffect>("Echo", 2, 0, TargetType::Enemy));
			card->setName(card->getName() + " [Echo]");
		}
		return card;
	};

	auto card1 = generateRandomCard(rng);
	auto card2 = generateRandomCard(rng);

	Suit enchSuit = suits[suitDist(rng)];
	CardValue enchVal = values[valDist(rng)];
	std::string enchTargetName = valueToString(enchVal) + " of " + suitToString(enchSuit);

	std::uniform_int_distribution<int> enchTypeDist(0, 1);
	int enchType = enchTypeDist(rng);

	std::cout << " [1] Draft New Card : " << card1->getName() << "\n";
	std::cout << "		" << card1->getEffectsSummary() << "\n\n";

	std::cout << " [2] Draft New Card : " << card2->getName() << "\n";
	std::cout << "		" << card2->getEffectsSummary() << "\n\n";

	std::cout << " [3] Enchantment : Empower your " << enchTargetName << "\n";
	if (enchType == 0) {
		std::cout << " Adds: Deal 5 damage to target, apply Bleeding (3 dmg, 2 turns).\n\n";
	}
	else {
		std::cout << " Adds: Grant 5 Block, draw 1 card.\n\n";
	}
	
	std::cout << " [0] Skip\n";
	std::cout << "============================================\n";

	int choice = -1;
	while (true) {
		std::cout << "Choose (0-3): ";
		if (std::cin >> choice && choice >= 0 && choice <= 3) {
			break;
		}
		std::cin.clear();
		std::cin.ignore(1000, '\n');
	}

	if (choice == 1) {
		std::cout << "Added " << card1->getName() << " to your deck!\n";
		draftedCards.push_back(std::move(card1));
	} else if (choice == 2) {
		std::cout << "Added " << card2->getName() << " to your deck!\n";
		draftedCards.push_back(std::move(card2));
	} else if (choice == 3) {
		std::cout << "Enchantment applied! Your " << enchTargetName <<
			" is now empowered!\n";
		enchantments.push_back([enchSuit, enchVal, enchType](Card* c) -> bool {
			if (c->getSuit() == enchSuit && c->getValue() == enchVal) {
				if (enchType == 0) {
					c->addEffect(std::make_unique<DamageEffect>(5, TargetType::Enemy));
					c->addEffect(std::make_unique<ApplyStatusEffect>("Bleeding", 2, 3, TargetType::Enemy));
					c->setName(c->getName() + " [Bleeding]");
				}
				else {
					c->addEffect(std::make_unique<BlockEffect>(5, TargetType::Self));
					c->addEffect(std::make_unique<DrawCardEffect>(1, TargetType::Self));
					c->setName(c->getName() + " [Defence]");
				}
				return true;
			}
			return false;
		});
	} else {
		std::cout << "You skipped the rewards.\n";
	}
}