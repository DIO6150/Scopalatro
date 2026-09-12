#pragma once
#include <string>
#include <vector>
#include <memory>
#include <type_traits>
#include <string>

#include <CardCommon.hpp>
#include <EffectSystem.hpp>




class Card
{
public:
	Card(std::string name, std::string desc, Suit s = Suit::Denari, CardValue v = CardValue::Asso);

	//void Play(Entity& source, std::vector<Entity*> targets);

	void AddEffect(std::unique_ptr<Effect> effect);

	void AddEffect(Effect* effect);

	bool HasEffects() const;

	Suit      GetSuit() const;
	CardValue GetValue() const;
	int       GetNumericValue() const;

	std::string GetName() const;
	std::string GetDescription() const;
	std::string GetEffectsSummary() const;

	std::string GetKey() const;
	static std::string GetKey(Suit s, CardValue v);

	auto & GetEffects() { return m_effects; }

	void BoostValue(int amt) { valueBoost += amt; }

	void SetSuit(Suit s)
	{
		suit = s;
		name = valueToString(value) + " of " + suitToString(suit);
	}

	void SetValue(CardValue v)
	{
		value = v;
		name = valueToString(value) + " of " + suitToString(suit);
	}
	
private:
	std::string name;
	std::string description;

	Suit suit;
	CardValue value;
	int valueBoost = 0;

	std::vector<std::unique_ptr<Effect>> m_effects;
};

