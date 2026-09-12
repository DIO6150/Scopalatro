#include "Enemy.hpp"
#include "DeckBuilder.hpp"
#include "EffectSystem.hpp"
#include <iostream>
#include <numeric>
#include <stdexcept>

Enemy::Enemy(std::string name, int hp, float attackDensity) 
	: Entity(name, hp), m_attackDensity(attackDensity) {}

void Enemy::setActionWeights(std::vector<EnemyAction> weights) {
	if (weights.empty()) return;
	m_actionWeights = std::move(weights);
}

int Enemy::rollDamage(int min, int max) const {
    if (min == max) return min;
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

EnemyAction Enemy::rollAction() const {
	if (m_actionWeights.empty()) return { "Attack", 2, 4, "", 0, 1 };

	int totalWeight = 0;
	for (const auto& a : m_actionWeights) totalWeight += a.weight;

	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> dist(0, totalWeight - 1);
	int roll = dist(rng);

	int cumulative = 0;
	for (size_t i = 0; i < m_actionWeights.size(); ++i) {
		cumulative += m_actionWeights[i].weight;
		if (roll < cumulative) return m_actionWeights[i];
	}

	return m_actionWeights.back();
}

void Enemy::buildDeck() {
	m_deck = DeckBuilder::buildScopaDeck(false);
	
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_real_distribution<float> probDist(0.0f, 1.0f);

	auto& cards = m_deck.getCards();
	for (auto& c : cards) {
		if (probDist(rng) <= m_attackDensity) {
			EnemyAction action = rollAction();
			int dmg = rollDamage(action.minDamage, action.maxDamage);
			if (dmg > 0) {
				c->addEffect(std::make_unique<DamageEffect>(dmg, TargetType::Enemy));
			}
			if (!action.status.empty()) {
				c->addEffect(std::make_unique<ApplyStatusEffect>
					(action.status, action.statusDuration, action.statusMagnitude, TargetType::Enemy));
			}
			c->setName(c->getName() + " [" + action.name + "]");
		}
	}
	
	m_deck.shuffle();
}

void Enemy::drawCards() {
	while (m_hand.size() < 3 && !m_deck.isEmpty()) {
		m_hand.push_back(m_deck.draw());
	}
}

void Enemy::planTurn(const std::vector<std::unique_ptr<Card>>& table) {
	if (m_hand.empty()) {
		m_plannedCardIdx = -1;
		return;
	}

	m_plannedCardIdx = 0;
	int bestCaptureValue = -1;

		for (size_t i = 0; i < m_hand.size(); ++i) {
			auto captureIndices =
				DeckBuilder::findCaptureIndices(m_hand[i].get(), table);

			if (!captureIndices.empty()) {
				int captureValue = 0;
				for (int idx : captureIndices) {
					captureValue += table[idx]->getNumericValue();
				}
				if (captureValue > bestCaptureValue) {
					bestCaptureValue = captureValue;
					m_plannedCardIdx = static_cast<int>(i);
				}
			}
		}

	if (bestCaptureValue == -1) {
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> dist(0, m_hand.size() - 1);
		m_plannedCardIdx = dist(rng);
	}
}

const Card* Enemy::getPlannedCard() const {
	if (m_plannedCardIdx >= 0 && m_plannedCardIdx < static_cast<int>
		(m_hand.size())) {
		return m_hand[m_plannedCardIdx].get();
	}
	return nullptr;
}

std::unique_ptr<Card> Enemy::executeTurn() {
	if (m_plannedCardIdx >= 0 && m_plannedCardIdx < static_cast<int>
		(m_hand.size())) {
		auto card = std::move(m_hand[m_plannedCardIdx]);
		m_hand.erase(m_hand.begin() + m_plannedCardIdx);
		m_plannedCardIdx = -1;
		return card;
	}
	return nullptr;
}
