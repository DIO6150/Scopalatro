#pragma once
#include <iostream>
#include <map>
#include <string>
#include <set>
#include <vector>
#include <memory>
#include "RelicSystem.hpp"

struct StatusData {
	int duration;
	int magnitude;
};

class Entity {
public:
    Entity(std::string name, int hp)
        : name(name), currentHp(hp), maxHp(hp), gold(0), block(0) {}

	int calculateDamage(int baseAmount) const {
		if (hasStatus("Blindness")) {
			if (rand() % 100 < getStatus("Blindness").magnitude) {
				std::cout << name << " is Blinded and missed the attack!" << std::endl;
				return 0;
			}
		}
		int finalAmount = baseAmount;
		if (hasStatus("Power")) {
			finalAmount += (baseAmount * getStatus("Power").magnitude) / 100;
		}
		if (hasStatus("Berserk")) {
			finalAmount += getStatus("Berserk").magnitude;
		}
		if (hasStatus("Burn")) {
			finalAmount -= getStatus("Burn").magnitude;
		}
		if (finalAmount < 0) finalAmount = 0;
		return finalAmount;
	}

    void gainBlock(int amount) {
        if (hasPassive("DoubleFirstBlock") && !m_doubleBlockSpent) {
            amount *= 2;
            m_doubleBlockSpent = true;
            std::cout << "(Vambraces) Block doubled! ";
        }
        block += amount;
        std::cout << name << " gained " << amount << " Shield! (Total: " << block <<
")" << std::endl;
    }

    void heal(int amount) {
        currentHp += amount;
        if (currentHp > maxHp) currentHp = maxHp;
        std::cout << name << " healed for " << amount << " (HP: " << currentHp << "/" << maxHp << ")" << std::endl;
    }

	std::string getStatusAdjective(const std::string& statusName) const {
		if (statusName == "Confusion") return "Confused";
		if (statusName == "Blindness") return "Blinded";
		if (statusName == "Burn") return "Burned";
		if (statusName == "Echo") return "Echoing";
		return statusName;
	}

	bool hasStatus(const std::string& status) const {
		return statuses.find(status) != statuses.end();
	}

	StatusData getStatus(const std::string& status) const {
		auto it = statuses.find(status);
		if (it != statuses.end()) return it->second;
		return { 0, 0 };
	}

    bool isAlive() const {
        return currentHp > 0;
    }

	int& getHp() { return currentHp; }
    std::string getName() const { return name; }

    int getGold() const { return gold; }
    void addGold(int amount) { gold += amount; }
    void spendGold(int amount) { gold -= amount; if (gold < 0) gold = 0; }

    int& getMaxHp() { return maxHp; }

	void setLastAttackDamage(int dmg) { lastAttackDamageTaken = dmg; }
	int getLastAttackDamage() const { return lastAttackDamageTaken; }

    void increaseMaxHp(int amount) {
        maxHp += amount;
        currentHp += amount;
        std::cout << name << " max HP increased to " << maxHp << "!" << std::endl;
    }

	int getSpellValueBoost() const { return m_spellValueBoost; }
	void addSpellValueBoost(int amount) {
		m_spellValueBoost += amount;
		std::cout << name << " gained +" << amount << " Spell Capture Value! (Total Bonus: +" <<
			m_spellValueBoost << ")" << std::endl;
	}

    void addPassive(const std::string& p) { m_passives.insert(p); }
    bool hasPassive(const std::string& p) const { return m_passives.count(p) > 0; }

    void addPendingBlock(int amount) { m_pendingBlock += amount; }

    void resetCombatState() {
        m_doubleBlockSpent = false;
        block = 0;
        statuses.clear();
        if (m_pendingBlock > 0) {
            std::cout << "[Shop Bonus] ";
            gainBlock(m_pendingBlock);
            m_pendingBlock = 0;
        }
		removeExpiredRelics();
    }

	void addRelic(std::shared_ptr<Relic> relic) {
		relics.push_back(relic);
		obtainedRelics.insert(relic->name);
		relic->onPickup(*this);
		std::cout << name << " obtained Relic: " << relic->name << "!" << std::endl;
	}

	bool hasObtainedRelic(const std::string& name) const {
		return obtainedRelics.find(name) != obtainedRelics.end();
	}

	std::vector<std::shared_ptr<Relic>>& getRelics() { return relics; }
	const std::vector<std::shared_ptr<Relic>>& getRelics() const { return relics; }

	void removeExpiredRelics() {
		auto it = relics.begin();
		while (it != relics.end()) {
			if ((*it)->isExpired()) {
				std::cout << (*it)->name << "has expired and is removed." << std::endl;
				it = relics.erase(it);
			} else {
				++it;
			}
		}
	}

    void printStatus() const {
        std::cout << "[" << name << "] HP: " << currentHp << "/" << maxHp;
        if (block > 0) {
            std::cout << " | Shield: " << block;
        }
    
        if (!statuses.empty()) {
            std::cout << " | Statuses: ";
            for (const auto& pair : statuses) {
                std::cout << pair.first << " (" << pair.second.duration << " turns) ";
            }
        }
        std::cout << std::endl;
    }

    auto & getStatuses() { return statuses; }

	auto & getBlock() { return block; }

private:
    std::string name;
    int currentHp;
    int maxHp;
    int gold;
    int block;
	int lastAttackDamageTaken = 0;
    int m_pendingBlock = 0;
	int m_spellValueBoost = 0;
    bool m_doubleBlockSpent = false;
    std::set<std::string> m_passives;
	std::set<std::string> obtainedRelics;
    std::map<std::string, StatusData> statuses;
	std::vector<std::shared_ptr<Relic>> relics;
};