#pragma once

#include <string>
#include <vector>
#include <random>
#include <Combat/Entity.hpp>
#include <Deck.hpp>
#include <Card.hpp>
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

	void buildDeck(std::vector<Card *> & cards);
	void planTurn(std::vector<Card *> & hand, std::vector<Card *> & table);
	const Card* getPlannedCard(std::vector<Card *> & hand) const;
	Card * executeTurn(std::vector<Card *> & hand);

	void addScoreCards(std::vector<Card *> & capturePile, std::vector<Card *> captured)
	{
		for (auto& c : captured) capturePile.push_back(std::move(c));
	}

private:
	float m_attackDensity;
	std::vector<EnemyAction> m_actionWeights;
	int m_plannedCardIdx = -1;

    int rollDamage(int min, int max) const;
	EnemyAction rollAction() const;
};