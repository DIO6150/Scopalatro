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
    
    void takeDamage(int amount, Entity* source = nullptr, bool isEcho = false) {
		if (amount <= 0) return;

		if (!isEcho) {
			if (hasStatus("Berserk")) {
				amount += getStatus("Berserk").magnitude;
			}
			if (hasStatus("Vulnerable")) {
				amount += (amount * getStatus("Vulnerable").magnitude) / 100;
			}
			if (hasStatus("Bleeding")) {
				amount += (amount * 25) / 100;
			}
			if (hasStatus("DEF")) {
				amount -= (amount * getStatus("DEF").magnitude) / 100;
			}
			if (amount < 0) amount = 0;
		}
        
		if (block > 0) {
            int absorbed = std::min(block, amount);
            block -= absorbed;
            amount -= absorbed;
            std::cout << name << " blocked " << absorbed << " damage! ";
            if (block > 0) std::cout << "(Shield remaining: " << block << ") \n";
            else std::cout << "(Shield broke!) \n";
        }

        if (amount > 0) {
            currentHp -= amount;
            std::cout << name << " took " << amount << " damage (HP: " << currentHp 
<< "/" << maxHp << ")" << std::endl;
			if (!isEcho && source) {
				for (auto& r : relics) {
					r->onDamageReceived(*this, *source, amount);
				}
			}
        } else {
            std::cout << std::endl;
        }

		if (currentHp < 0) currentHp = 0;

		if (!isEcho && hasStatus("Echo")) {
			int times = getStatus("Echo").magnitude;
			if (times == 0) times = 1;
			for (int i = 0; i < times; ++i) {
				if (currentHp <= 0) break;
				std::cout << name << "'s Echo resonates for " << amount <<
					" damage!" << std::endl;
				takeDamage(amount, source, true);
			}
		}
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

	void addStatus(const std::string& status, int duration, int magnitude = 0) {
		std::string adjective = getStatusAdjective(status);

        if (statuses.find(status) != statuses.end()) {
            statuses[status].duration += duration;
			if (magnitude > statuses[status].magnitude) {
				statuses[status].magnitude = magnitude;
			}
            std::cout << name << " is already " << adjective << ". Extended by " << duration << " turns." << std::endl;
        } else {
			statuses[status] = { duration, magnitude };
            std::cout << name << " is now " << adjective << " for " << duration << " turns." << std::endl;
        }
    }

	bool hasStatus(const std::string& status) const {
		return statuses.find(status) != statuses.end();
	}

	StatusData getStatus(const std::string& status) const {
		auto it = statuses.find(status);
		if (it != statuses.end()) return it->second;
		return { 0, 0 };
	}

	void removeStatus(const std::string& status) {
		auto it = statuses.find(status);
		if (it != statuses.end()) {
			std::cout << name << " lost " << status << "." << std::endl;
			statuses.erase(it);
		}
	}

	void doubleDebuffs() {
		static const std::vector<std::string> debuffs = {
			"Poisoned", "Burn", "Bleeding", "Vulnerable", "Stun", "Blindness", "Confusion"
		};
		for (const auto& debuff : debuffs) {
			auto it = statuses.find(debuff);
			if (it != statuses.end()) {
				it->second.duration *= 2;
				it->second.magnitude *= 2;
				std::cout << name << "'s " << debuff << " doubled! ("
					<< it->second.magnitude << " for " << it->second.duration << " turns)" << std::endl;
			}
		}
	}

    bool isAlive() const {
        return currentHp > 0;
    }

	int getHp() const { return currentHp; }
    std::string getName() const { return name; }

    int getGold() const { return gold; }
    void addGold(int amount) { gold += amount; }
    void spendGold(int amount) { gold -= amount; if (gold < 0) gold = 0; }

    int getMaxHp() const { return maxHp; }

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

    void updateStatuses() {
        auto it = statuses.begin();
        while (it != statuses.end()) {

			const std::string& statusName = it->first;
			StatusData& data = it->second;

			if (statusName == "Poisoned") {
				std::cout << name << " takes " << data.magnitude << " Poison damage!" << std::endl;
				takeDamage(data.magnitude, nullptr, true);
			}
			else if (statusName == "Burn") {
				std::cout << name << " takes " << data.magnitude << " Burn damage!" << std::endl;
				takeDamage(data.magnitude, nullptr, true);
			}
			else if (statusName == "Bleeding") {
				std::cout << name << " takes " << data.magnitude << " Bleed damage!" << std::endl;
				takeDamage(data.magnitude, nullptr, true);
			}
			else if (statusName == "Healing") {
				heal(data.magnitude);
			}

			data.duration--;
            if (data.duration <= 0) {
                std::cout << name << " is no longer " << getStatusAdjective(statusName) << "." << std::endl;
                it = statuses.erase(it);
            } else {
                ++it;
            }
        }
    }

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