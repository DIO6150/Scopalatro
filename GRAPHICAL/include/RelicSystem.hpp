#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cstdlib>

class Entity;
class Card;

enum class RelicTier { Common, Uncommon, Rare };

struct CaptureEvent {
	Entity * player;
	Entity * enemy;
	Card   * playedCard;
	std::vector<Card *> captured;
};

class Relic {
public:
	virtual	~Relic() = default;

	std::string name;
	std::string description;
	RelicTier tier;
	int cost;

	Relic(std::string n, std::string d, RelicTier t)
		: name(n), description(d), tier(t) {
		cost = rollPrice(t);
	}

	virtual void onPickup(Entity&) {}
	virtual void onTurnStart(Entity&, Entity&) {}
	virtual void onCapture(CaptureEvent&) {}
	virtual void onDamageDealt(Entity&, Entity&, int) {}
	virtual void onDamageReceived(Entity&, Entity&, int) {}
	virtual void onSpellCast(Card*, Entity&, Entity&) {}
	virtual void onBattleStart(Entity&) {}
	virtual void onBattleEnd(Entity&, int, int&) {}

	virtual int getExtraDraw() const { return 0; }
	virtual bool isExpired() const { return false; }

	static int rollPrice(RelicTier t) {
		switch (t) {
			case RelicTier::Common: return 3 + rand() % 4;
			case RelicTier::Uncommon: return 7 + rand() % 5;
			case RelicTier::Rare: return 12 + rand() % 5;
		}
		return 5;
	}
		 
};