#pragma once

#include <string>
#include <vector>
#include <random>
#include "Entity.hpp"
#include "Deck.hpp"
#include "Card.hpp"
#include <memory>

struct EnemyAction {
    std::string name;
    int minDamage = 0;
    int maxDamage = 0;
    std::string status = "";
    int statusDuration = 0;
	int statusMagnitude = 0;
    int weight = 1;
};

class Enemy : public Entity {
public:
    Enemy(std::string name, int hp, float attackDensity = 0.5f);

	void setActionWeights(std::vector<EnemyAction> weights);
	void setAttackDensity(float density) { m_attackDensity = density; }

	void buildDeck();
	void drawCards();
	void planTurn(const std::vector<std::unique_ptr<Card>>& table);
	const Card* getPlannedCard() const;
	std::unique_ptr<Card> executeTurn();

	int getHandSize() const { return m_hand.size(); }
	void addScoreCards(std::vector<std::unique_ptr<Card>> captured) {
		for (auto& c : captured) m_scorePile.push_back(std::move(c));
	}
	const std::vector<std::unique_ptr<Card>>& getScorePile() const {
		return m_scorePile;
	}

private:
	float m_attackDensity;
	std::vector<EnemyAction> m_actionWeights;

	Deck m_deck;
	std::vector<std::unique_ptr<Card>> m_hand;
	std::vector<std::unique_ptr<Card>> m_scorePile;
	int m_plannedCardIdx = -1;

    int rollDamage(int min, int max) const;
	EnemyAction rollAction() const;
};