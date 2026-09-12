#include <Combat/Enemy.hpp>
#include <EffectSystem.hpp>
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

void Enemy::buildDeck(std::vector<Card *> & cards)
{
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_real_distribution<float> probDist(0.0f, 1.0f);

	for (auto& c : cards) {
		if (probDist(rng) <= m_attackDensity) {
			EnemyAction action = rollAction();
			int dmg = rollDamage(action.minDamage, action.maxDamage);
			if (dmg > 0) {
				c->AddEffect(std::make_unique<DamageEffect>(dmg, TargetType::Enemy));
			}
			if (!action.status.empty()) {
				c->AddEffect(std::make_unique<ApplyStatusEffect>
					(action.status, action.statusDuration, action.statusMagnitude, TargetType::Enemy));
			}
		}
	}
}

static std::vector<int> findCaptureIndices(const Card * playedCard, const std::vector<Card *>& table, int customTargetVal) {
	
	int targetVal = customTargetVal > 0 ? customTargetVal : playedCard->GetNumericValue();

    for (size_t i = 0; i < table.size(); ++i) {
        if (table[i]->GetNumericValue() == targetVal) {
            return { static_cast<int>(i) };
        }
    }

    int n = table.size();
    for (int i = 1; i < (1 << n); ++i) {
        int currentSum = 0;
        std::vector<int> indices;
        for (int j = 0; j < n; ++j) {
            if ((i >> j) & 1) {
                currentSum += table[j]->GetNumericValue();
                indices.push_back(j);
            }
        }
        if (currentSum == targetVal) {
            return indices;
        }
    }        

    return {};
}

void Enemy::planTurn(std::vector<Card *> & hand, std::vector<Card *> & table) {
	if (hand.empty()) {
		m_plannedCardIdx = -1;
		return;
	}

	m_plannedCardIdx = 0;
	int bestCaptureValue = -1;

		for (size_t i = 0; i < hand.size(); ++i) {
			auto captureIndices = findCaptureIndices(hand[i], table, -1);

			if (!captureIndices.empty()) {
				int captureValue = 0;
				for (int idx : captureIndices) {
					captureValue += table[idx]->GetNumericValue();
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
		std::uniform_int_distribution<int> dist(0, hand.size() - 1);
		m_plannedCardIdx = dist(rng);
	}
}

const Card* Enemy::getPlannedCard(std::vector<Card *> & hand) const {
	if (m_plannedCardIdx >= 0 && m_plannedCardIdx < static_cast<int>(hand.size()))
	{
		return hand.at(m_plannedCardIdx);
	}
	return nullptr;
}

Card * Enemy::executeTurn(std::vector<Card *> & hand) {
	if (m_plannedCardIdx >= 0 && m_plannedCardIdx < static_cast<int>(hand.size()))
	{
		auto card = std::move(hand[m_plannedCardIdx]);
		hand.erase(hand.begin() + m_plannedCardIdx);
		m_plannedCardIdx = -1;

		return card;
	}

	return nullptr;
}
