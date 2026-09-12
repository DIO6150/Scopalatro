#include "Game.hpp"
#include "DeckBuilder.hpp"
#include "EffectSystem.hpp"
#include "Shop.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

Game::Game(Entity& p, Difficulty diff,
	const std::vector<std::unique_ptr<Card>>& draftedCards,
	const std::vector<std::function<bool(Card*)>>& enchantments)
    : player(p),
      enemy("Badalisk", 50, 0.5f),
      scopaCount(0),
      turn(1)
{
	float density = 0.5f;
	switch (diff) {
		case Difficulty::Tutorial: density = 0.0f; break;
		case Difficulty::Easy: density = 0.25f; break;
		case Difficulty::Normal: density = 0.50f; break;
		case Difficulty::Hard: density = 0.75f; break;
		case Difficulty::Elite: density = 1.0f; break;
	}
	enemy.setAttackDensity(density);

	EnemyAction bite {"Bite", 3, 6, "", 0, 0, 60};
    EnemyAction venom {"Venom Spit", 2, 2, "Poisoned", 2, 3, 25};
    EnemyAction crush {"Crush", 10, 14, "", 0, 0, 15};

    enemy.setActionWeights({ bite, venom, crush });
    enemy.buildDeck();

    deck = DeckBuilder::buildScopaDeck();

	std::cout << "Deck initialized with " << deck.size() << " cards." << std::endl;
	deck.shuffle();
	std::cout << "Deck shuffled.\n" << std::endl;

	std::cout << "Initializing Table..." << std::endl;
	for (int i = 0; i < 4; ++i) {
		if (!deck.isEmpty())
			table.push_back(deck.draw());
	}

	for (const auto& drafted : draftedCards) {
		deck.addCard(drafted->clone());
	}
	deck.shuffle();

	for (const auto& enchant : enchantments) {
		for (const auto& card : deck.getCards()) {
			if (enchant(card.get())) {
				break;
			}
		}
	}
}

void Game::run() {
    std::cout << "Cards Test\n" << std::endl;

    player.resetCombatState();
    player.printStatus();
    enemy.printStatus();

	for (auto& r : player.getRelics()) {
		r->onBattleStart(player);
	}

    while (player.isAlive() && enemy.isAlive()) {
        std::cout << "\n--- Turn " << turn++ << " ---" << std::endl;

		for (auto& r : player.getRelics()) {
			r->onTurnStart(player, enemy);
		}

        player.updateStatuses();
        enemy.updateStatuses();
		
		if (!player.isAlive()) {
			std::cout << "\n[" << player.getName() << " succumbed to their wounds!]" << std::endl;
			break;
		}
		if (!enemy.isAlive()) {
			std::cout << "\n[Enemy Defeated by Status Effects!]" << std::endl;
			calculateScoring();
			break;
		}

        drawPhase();

        if (hand.empty()) {
            std::cout << "Hand is empty and cannot draw." << std::endl;
            break;
        }
		
		enemy.planTurn(table);
        
        displayState();
		if (player.hasStatus("Stun")) {
			std::cout << "\n[" << player.getName() << 
" is stunned and cannot act this turn!]" << std::endl;
		} else {
			int cardIdx = playerSelectCard();
			if (player.hasStatus("Confusion")) {
				std::cout << "\n[" << player.getName() <<
"is Confused! Random card selected!]" << std::endl;
				cardIdx = rand() % hand.size();
			}
			resolvePlay(cardIdx);
		}

        if (!enemy.isAlive()) {
            std::cout << "\nEnemy Defeated!" << std::endl;
            calculateScoring();
            break;
        }

		if (enemy.hasStatus("Stun")) {
			std::cout << "\n[" << enemy.getName() <<
				" is stunned and cannot act this turn!]" << std::endl;
		} else {
			enemyTurnPhase();
		}

        if (!player.isAlive()) {
            std::cout << "\nYou have been defeated!" << std::endl;
            break;
        }
        endTurn();
    }
    if (player.isAlive() && enemy.isAlive()) {
        std::cout << "\nDeck depleted! Combat End" << std::endl;
        calculateScoring();
    }
}

void Game::drawPhase() {
	int targetHandSize = 3;
	for (auto& r : player.getRelics()) {
		targetHandSize += r->getExtraDraw();
	}
	while (static_cast<int>(hand.size()) < targetHandSize && !deck.isEmpty()) {
		auto card = deck.draw();
        std::cout << "Drew: " << card->getName() << std::endl;
        hand.push_back(std::move(card));
    }
	enemy.drawCards();
}

void Game::displayState() const {
    std::cout << "\n[TABLE]: ";
    if (table.empty()) std::cout << "(Empty)";
    for (const auto& c : table)
        std::cout << "[" << c->getName() << "(" << c->getNumericValue() << ")] ";
    std::cout << std::endl;

    std::cout << "[HAND]: ";
    for (const auto& c : hand) {
        std::cout << "[" << c->getName() << "(" << c->getNumericValue() << ")";
        if (c->hasEffects()) std::cout << " | " << c->getEffectsSummary();
        std::cout << "] ";
    }
    std::cout << "\n" << std::endl; 

    const auto& intentCard = enemy.getPlannedCard();
	if (intentCard) {
		std::cout << "[" << enemy.getName() << " intends to play]: " 
<< intentCard->getName();
		if (intentCard->hasEffects()) {
			std::cout << " | " << intentCard->getEffectsSummary();
		}
		std::cout << "\n" << std::endl;
	}
}

void Game::enemyTurnPhase() {
	auto enemyCard = enemy.executeTurn();
	if (!enemyCard) return;

	std::vector<Entity*> targets = { &player };

	if (enemy.hasStatus("Confusion")) {
		std::cout << "\n[" << enemy.getName() <<
" is Confused and targets itself!]" << std::endl;
		targets = { &enemy };
	}

	std::cout << "[ENEMY] " << enemy.getName()
		<< " plays " << enemyCard->getName() << "!" << std::endl;

	enemyCard->play(enemy, targets, nullptr, &scorePile, &table);

	std::vector<int> capturedIndices =
		DeckBuilder::findCaptureIndices(enemyCard.get(), table);

	if (!capturedIndices.empty()) {
		enemyCaptures++;
		std::cout << " -> ENEMY CAPTURE! ";
		std::vector<std::unique_ptr<Card>> capturedCards;
		for (int idx : capturedIndices) {
			std::cout << table[idx]->getName() << " ";
			capturedCards.push_back(std::move(table[idx]));
		}
		std::cout << "!" << std::endl;

		std::sort(capturedIndices.rbegin(), capturedIndices.rend());
		for (int idx : capturedIndices)
			table.erase(table.begin() + idx);

		if (table.empty()) {
			std::cout << " ENEMY SCOPA! Table cleared! " << std::endl;
		}

		capturedCards.push_back(std::move(enemyCard));
		enemy.addScoreCards(std::move(capturedCards));
	} else {
		std::cout << " -> No capture. Card added to table." << std::endl;
		table.push_back(std::move(enemyCard));
	}
}

int Game::playerSelectCard() {
    int choice = -1;
    while (true) {
        std::cout << "Play which card? (1-" << hand.size() << "): ";
        if (std::cin >> choice && choice >= 1 && choice <= static_cast<int>(hand.size())) {
            return choice - 1;
        }
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "Invalid Input. ";
    }
}

void Game::resolvePlay(int cardIdx) {
    auto cardToPlay = std::move(hand[cardIdx]);
	hand.erase(hand.begin() + cardIdx);

    std::vector<Entity*> targets = { &enemy };

    std::cout << "[PLAYER] " << player.getName()
              << " plays " << cardToPlay->getName() << "!" << std::endl;

	auto drawCb = [this](int amt) {
		std::cout << "Drawing " << amt << " cards..." << std::endl;
		for (int i = 0; i < amt; ++i) {
			if (!deck.isEmpty()) {
				auto card = deck.draw();
				std::cout << "Drew: " << card->getName() << std::endl;
				hand.push_back(std::move(card));
			} else {
				std::cout << "Deck is empty!" << std::endl;
				break;
			}
		}
	};

	cardToPlay->play(player, targets, drawCb, &scorePile, &table);

	if (cardToPlay->hasEffects()) {
		for (auto& r : player.getRelics()) {
			r->onSpellCast(cardToPlay.get(), player, enemy);
		}
	}

	int targetVal = cardToPlay->getNumericValue();
	if (cardToPlay->hasEffects()) {
		targetVal += player.getSpellValueBoost();
	}

    std::vector<int> capturedIndices = 
        DeckBuilder::findCaptureIndices(cardToPlay.get(), table, targetVal);

    if (!capturedIndices.empty()) {
		playerCaptures++;
        std::cout << " -> CAPTURE! ";

		CaptureEvent evt;
		evt.playedCard = cardToPlay.get();
		evt.player = &player;
		evt.enemy = &enemy;
		evt.scorePile = &scorePile;

        for (int idx : capturedIndices) {
            std::cout << table[idx]->getName() << " ";
			evt.captured.push_back(table[idx].get());
            scorePile.push_back(std::move(table[idx]));
        }
        std::cout << "!" << std::endl;

		for (auto& r : player.getRelics()) {
			r->onCapture(evt);
		}

        std::sort(capturedIndices.rbegin(), capturedIndices.rend());
        for (int idx : capturedIndices)
            table.erase(table.begin() + idx);
        
        if (table.empty()) {
            ++scopaCount;
            std::cout << " SCOPA! Table cleared (" << scopaCount
                      << " sweep" << (scopaCount > 1 ? "s" : "") << " total) " << std::endl;
        }

        scorePile.push_back(std::move(cardToPlay));
    
    } else {
        std::cout << " -> No capture. Card added to table." << std::endl;
        table.push_back(std::move(cardToPlay));
    }
}

void Game::endTurn() {
    player.printStatus();
    enemy.printStatus();
    std::cout << "[End of Turn] Hand: " << hand.size()
			  << " | Enemy Hand: " << enemy.getHandSize()
              << " | Table: " << table.size()
              << " | Captures: " << playerCaptures << "\n" << std::endl;
}

void Game::calculateScoring() {
    std::cout << " VICTORY ! \n";

    int numDenari = 0;
    bool hasSettebello = false;

    for (const auto& card : scorePile) {
        if (!card) continue;
        if (card->getSuit() == Suit::Denari) {
            numDenari++;
            if (card->getNumericValue() == 7) {
                hasSettebello = true;
            }
        }
    }

    int goldGained = 5;

    bool wonCards = (playerCaptures > enemyCaptures);
    bool wonDenari = (numDenari > 5);

    std::cout << "- Most Captures (You: " << playerCaptures << " | Enemy: "
<< enemyCaptures << ") : " << (wonCards ? "YES (+1 Gold)" : "NO") << "\n";
    std::cout << "- Most Denari (" << std::setw(2) << numDenari << ")  : " << 
(wonDenari ? "YES (+1 Gold)" : "NO") << "\n";
    std::cout << "- Settebello        : " << (hasSettebello ? "YES (+1 Gold)" : "NO") 
<< "\n";
    std::cout << "- Scopas (Sweeps)   : " << scopaCount << "   (+" << scopaCount 
<< " Gold)\n";
    std::cout << "--------------------------------------------\n";

    if (wonCards) goldGained++;
    if (wonDenari) goldGained++;
    if (hasSettebello) goldGained++;
    goldGained += scopaCount;

	int bonusGold = 0;
	for (auto& r : player.getRelics()) {
		r->onBattleEnd(player, scopaCount, bonusGold);
	}

	if (bonusGold > 0) {
		std::cout << "[Relics] Bonus Gold: +" << bonusGold << " Gold\n";
		goldGained += bonusGold;
	}

    player.addGold(goldGained);

    std::cout << "Total Earned: " << goldGained << " Gold\n";
    std::cout << "Current Gold: " << player.getGold() << " Gold\n";
    std::cout << "============================================\n";
} 