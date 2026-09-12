#pragma once

#include <Combat/Statuses/IStatus.hpp>
#include <Combat/Entity.hpp>


class Berserk: public MetaStatus<Berserk>
{
public:
	Berserk(int magnitude, int duration)
		: magnitude {magnitude}
		, duration  {duration}
	{

	}

	void stack(int magnitude, int duration)
	{
		this->magnitude += magnitude;
		this->duration  += duration;
	}
	
	bool hasEnded()
	{
		return duration == 0;
	}
	
	void applyAttackDamageDealtModifier(Entity * target, Damage & damage, DamageModifierStage stage) override
	{
		if (stage != DamageModifierStage::AttackDamageFlat2)
			return;

		damage.value += magnitude;
	}
	
	int magnitude = 0;
	int duration  = 0;
	
	inline static std::string const getName() { return "berk"; }
};