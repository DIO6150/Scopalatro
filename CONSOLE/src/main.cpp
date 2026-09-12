#include "Game.hpp"
#include "Entity.hpp"
#include "Shop.hpp"
#include "Draft.hpp"
#include "EffectSystem.hpp"
#include "Relic.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <functional>
#include <ctime>
#include <cstdlib>

int main() {
	srand(time(NULL));
    Entity player("Jaaj", 50);
    Shop shop;
	Draft draft;
    int fightCount = 1;

	std::vector<std::unique_ptr<Card>> draftedCards;
	std::vector<std::function<bool(Card*)>> enchantments;

    std::cout << " SCOPALATRO " << std::endl;

	Difficulty currentDiff = Difficulty::Normal;
	std::cout << "Select Difficulty:\n1. Tutorial\n2. Easy\n3. Normal\n4. Hard\n5. Elite\nChoice (1-5): ";
	int diffChoice = 3;
	if (std::cin >> diffChoice) {
		switch (diffChoice) {
		case 1: currentDiff = Difficulty::Tutorial; break;
		case 2: currentDiff = Difficulty::Easy; break;
		case 3: currentDiff = Difficulty::Normal; break;
		case 4: currentDiff = Difficulty::Hard; break;
		case 5: currentDiff = Difficulty::Elite; break;
		}
	}

	bool TEST_MODE = true;
	if (TEST_MODE) {
		std::cout << "\n[DEBUG] Pre-loading spells for testing...\n";
		player.addGold(999);

		auto injectSpell = [&](std::string name, Suit suit, CardValue val,
			std::unique_ptr<Effect> e1, std::unique_ptr<Effect> e2 = nullptr) {
				auto card = std::make_unique<Card>(name, "", suit, val);
				card->addEffect(std::move(e1));
				if (e2) card->addEffect(std::move(e2));
				draftedCards.push_back(std::move(card));
		};

		// RELIC TEST 1: Hidden Advantage (Extra Draw) WORKS
		// Should be 4 cards in hand instead of 3.
		// player.addRelic(std::make_shared<HiddenAdvantage>());
		
		// RELIC TEST 2: Damage and Interest (+1 Gold on Win) WORKS
		// Win the battle, check gold reward!
		// player.addRelic(std::make_shared<DamageAndInterest>());

		// RELIC TEST 3: Cursed Blade (33% to debuff attacker) WORKS
		// Do NOT play any Shield cards. Let the enemy hit your HP!
		// player.addRelic(std::make_shared<CursedThorns>());

		// RELIC TEST 4: Cocktail Overdose / Molotov / Glass Cocktail WORKS
		//player.addRelic(std::make_shared<CocktailOverdose>()); // <= 2 captures
		//player.addRelic(std::make_shared<MolotovCocktail>());  // == 2 captures
		//player.addRelic(std::make_shared<GlassCocktail>());    // >= 2 captures
		// Play this spell when there is exactly a 'Tre' (3) and an 'Asso' (1) on the board to capture 2 cards.
		//injectSpell("Tiny Cocktail (Value 1)", Suit::Coppe, CardValue::Asso, std::make_unique<DamageEffect>(1, TargetType::Enemy));
		// Use this 3 to capture a 1 and a 2 on the board (Triggers Overdose, Molotov, AND Glass!)
		//injectSpell("Medium Cocktail (Value 3)", Suit::Coppe, CardValue::Tre, std::make_unique<DamageEffect>(1, TargetType::Enemy));
		// Use this 6 to capture three smaller cards like 1, 2, and 3 (Triggers Glass)
		//injectSpell("Giant Cocktail (Value 6)", Suit::Coppe, CardValue::Sei, std::make_unique<DamageEffect>(1, TargetType::Enemy));

		// RELIC TEST 5: Bloodstained Dagger (Sword Capture -> Bleed) WORKS
		// player.addRelic(std::make_shared<BloodstainedDagger>());
		// injectSpell("Tiny Sword Spell", Suit::Spada, CardValue::Asso, std::make_unique<DamageEffect>(1, TargetType::Enemy));
		// injectSpell("Medium Sword Spell", Suit::Spada, CardValue::Tre, std::make_unique<DamageEffect>(2, TargetType::Enemy));
		// injectSpell("Giant Sword Spell", Suit::Spada, CardValue::Re, std::make_unique<DamageEffect>(3, TargetType::Enemy));
		// injectSpell("Test Sword Spell", Suit::Spada, CardValue::Due, std::make_unique<DamageEffect>(1, TargetType::Enemy));

		// RELIC TEST 6: NDA (+1 Spell Value every 5 captures) WORKS
		// player.addRelic(std::make_shared<NDA>());
		// Play this card when the board is full. The 'Demonstration' effect will change all table cards
		// to Coupes, and it will capture all of them! You should trigger the +1 boost instantly!
		// injectSpell("Mass Capture Spell", Suit::Coppe, CardValue::Re, std::make_unique<ChangeTableSuitEffect>(Suit::Coppe));

		// RELIC TEST 7: Gambler's Fallacy (Denari capture -> Double/Lose Gold)
		// player.addRelic(std::make_shared<GamblersFallacy>());
		// player.addGold(50);

		// RELIC TEST 8: Flashbang (4 Denari Spells Cast -> Blind) WORKS
		// player.addRelic(std::make_shared<Flashbang>());

		// Play this spell 4 times
		// injectSpell("Flash 1", Suit::Denari, CardValue::Asso, std::make_unique<DamageEffect>(1, TargetType::Enemy));
		// injectSpell("Flash 2", Suit::Denari, CardValue::Asso, std::make_unique<DamageEffect>(1, TargetType::Enemy));
		// injectSpell("Flash 3", Suit::Denari, CardValue::Asso, std::make_unique<DamageEffect>(1, TargetType::Enemy));
		// injectSpell("Flash 4", Suit::Denari, CardValue::Asso, std::make_unique<DamageEffect>(1, TargetType::Enemy));

		// RELIC TEST 9: Magic Trick (Suit Swapping)
		// player.addRelic(std::make_shared<MagicTrick>());
		// player.addRelic(std::make_shared<BloodstainedDagger>()); // Needs Spada (Sword) to trigger

		// Since MagicTrick swaps Denari (Gold) to Spada (Sword), playing this Denari spell WORKS
		// should successfully trigger the Bloodstained Dagger!
		// injectSpell("Disguised Sword Spell", Suit::Denari, CardValue::Asso, std::make_unique<DamageEffect>(1, TargetType::Enemy));

		//injectSpell("1. Poison Dart", std::make_unique<ApplyStatusEffect>("Poisoned", 3, 3, TargetType::Enemy));
		//injectSpell("2. Harvest Spell", std::make_unique<HarvestEffect>(TargetType::AllEnemies));
		//injectSpell("3. Show of Strength", std::make_unique<BoostTableEffect>(3), std::make_unique<DamageEffect>(5, TargetType::Enemy));
		//injectSpell("4. Demo of Intelligence", std::make_unique<ChangeTableSuitEffect>(Suit::Spada), std::make_unique<DamageEffect>(5, TargetType::Enemy));
		//injectSpell("5. Incurable Disease", std::make_unique<DoubleDebuffsEffect>(TargetType::Enemy), std::make_unique<BlockEffect>(10, TargetType::Self));
		//injectSpell("6. Poison Explosion", std::make_unique<PoisonExplosionEffect>(TargetType::Enemy));
		//injectSpell("7. Blindness", std::make_unique<ApplyStatusEffect>("Blindness", 3, 50, TargetType::Enemy));
		//injectSpell("8. Berserk", std::make_unique<ApplyStatusEffect>("Berserk", 3, 5, TargetType::Self));
		//injectSpell("9. Confusion", std::make_unique<ApplyStatusEffect>("Confusion", 2, 0, TargetType::Enemy));
		//injectSpell("10. Echo", std::make_unique<ApplyStatusEffect>("Echo", 2, 0, TargetType::Enemy));
		/*injectSpell("11. Defensive Pick",
			std::make_unique<BlockEffect>(15, TargetType::Self),
			std::make_unique<DrawCardEffect>(2, TargetType::Self));*/
	}

    while (player.isAlive()) {
        std::cout << "\n--- BATTLE " << fightCount << " ---" << std::endl;

        Game game(player, currentDiff, draftedCards, enchantments);
        game.run();

        if (player.isAlive()) {
            std::cout << "\nCombat won!" << std::endl;
			draft.run(player, draftedCards, enchantments);
			std::cout << "\nEntering the shop..." << std::endl;
            shop.run(player);


			int choice = 0;
			while (true) {
				std::cout << "\nPrepare for the next fight?" << std::endl;
				std::cout << "1. Yes\n2. No (Quit)" << std::endl;
				if (std::cin >> choice && (choice == 1 || choice == 2)) {
					break;
				}
				std::cin.clear();
				std::cin.ignore(1000, '\n');
				std::cout << "Invalid input." << std::endl;
			}
			if (choice == 2) {
				break;
			}
            fightCount++;
        } else {
            std::cout << "\nGAME OVER. You survived " << (fightCount - 1) << " fights." << std::endl;
        }
    }
    
    return 0;
}
