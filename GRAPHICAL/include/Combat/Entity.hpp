#pragma once
#include <iostream>
#include <map>
#include <string>
#include <set>
#include <vector>
#include <memory>
#include "RelicSystem.hpp"

#include <Combat/Statuses/IStatus.hpp>
#include <Combat/Damage.hpp>
#include <Combat/CardPiles.hpp>

struct StatusData {
	int duration;
	int magnitude;
};

class CombatModelListener;

class Entity {
public:
	Entity(float hp, float maxHp, CombatModelListener * listener);

	Damage calculateDamage(Damage damage);
	float  calculateBlock();
	Damage applyDamage(Damage damage, float block);

    float gainBlock(float amount);
    float heal(float amount);

	template<class Status, class ... Args>
	void addStatus(Args ... args);

    bool isAlive() const;

	float getCurrentHp() const;
    float getMaxHp() const;
	float getBlock() const { return m_block; }

    int getGold() const;
    auto getStatuses() const { return m_statuses; }

    void addGold(int amount);
    void spendGold(int amount);

    void resetCombatState();

	void addRelic(Relic * relic);
	void removeExpiredRelics();

private:
    float m_currentHp = 0.0f;
    float m_maxHp     = 0.0f;
    float m_block     = 0.0f;
	
    int m_gold = 0;

	CardPiles m_inventory;

    std::vector<IStatus *> m_statuses;
	std::vector<Relic *> m_relics;

	CombatModelListener * m_listener;
};