#pragma once

#include <vector>
#include <memory>
#include <functional>
#include "Entity.hpp"
#include "Card.hpp"
#include "Deck.hpp"
#include "Enemy.hpp"

enum class Difficulty {
	Tutorial, // 0.0f
	Easy, // 0.25f
	Normal, // 0.50f
	Hard, // 0.75f
	Elite // 1.0f
};

class Game {
public:
    Game(Entity& player, Difficulty diff,
		const std::vector<std::unique_ptr<Card>>& draftedCards,
		const std::vector<std::function<bool(Card*)>>& enchantments);
    void run();

private:
    void drawPhase();
    void displayState() const;
    int  playerSelectCard();
    void resolvePlay(int cardIdx);
    void enemyTurnPhase();
    void endTurn();
    void calculateScoring();

    Entity& player;
    Enemy enemy;
    Deck deck;
    std::vector<std::unique_ptr<Card>> hand;
    std::vector<std::unique_ptr<Card>> table;
    std::vector<std::unique_ptr<Card>> scorePile;
    int scopaCount;
    int turn;
	int playerCaptures = 0;
	int enemyCaptures = 0;
};