#pragma once

#include <Combat/Statuses/IStatus.hpp>
#include <Combat/Entity.hpp>


class Poison: public MetaStatus<Poison>
{
public:
	Poison(int magnitude, int duration)
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
	
	void onTurnStart(Entity * target, int turnCount) override
	{
		Damage dmg  = target->calculateDamage(Damage {this, static_cast<float>(magnitude)});
		float block = target->calculateBlock();
		target->applyDamage(dmg, block);

		--duration;
	}
	
	int magnitude = 0;
	int duration  = 0;
	
	inline static std::string const getName() { return "poison"; }
};
