#pragma once

#include <Combat/Statuses/IStatus.hpp>
#include <Combat/Entity.hpp>


class Blindness: public MetaStatus<Blindness>
{
public:
	Blindness(int magnitude, int duration)
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
	
	void onAttackDamageDealt(Entity * target, Damage & damage, float & block, DamageApplicationStage stage) override
	{
		if (stage != DamageApplicationStage::BeforeAttackDamage)
			return;

		if (rand() % 100 < magnitude)
			damage.cancel(this, 0.0f);
	}
	
	int magnitude = 0;
	int duration  = 0;
	
	inline static std::string const getName() { return "flashpod"; }
};