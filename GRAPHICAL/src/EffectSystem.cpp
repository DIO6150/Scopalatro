#include <EffectSystem.hpp>
#include <Combat/Entity.hpp>
#include <Combat/CombatModel.hpp>
#include <Card.hpp>

void DamageEffect::apply(EffectContext& ctx) {
	int finalAmount = amount;
	if(ctx.entitySource)
	{
		finalAmount = ctx.entitySource->calculateDamage(amount);
	}

    auto targets = ctx.getTargets(targetType);

    for(auto* target : targets) {
		if(target)
		{
			ctx.observer->Damage(target, finalAmount, ctx.entitySource, false);
		}
    }
}

void HealEffect::apply(EffectContext& ctx) {
    auto targets = ctx.getTargets(targetType);
    for(auto* target : targets) {
        if(target)
		{
			ctx.observer->Heal(target, amount);
		}
    }
}

void ApplyStatusEffect::apply(EffectContext& ctx) {
    auto targets = ctx.getTargets(targetType);
    for(auto* target : targets)
	{
        if(target)
		{
			ctx.observer->AddStatus(target, statusName, duration, magnitude);
		}
    }
}

void BlockEffect::apply(EffectContext& ctx) {
    auto targets = ctx.getTargets(targetType);
    for(auto* target : targets) {
        if(target) target->gainBlock(amount);
    }
}

void DrawCardEffect::apply(EffectContext& ctx) {
	if(ctx.observer )
	{
		ctx.observer->DrawToHand(amount);
	}
	else
	{
		std::cout << "ERROR, no observer" << std::endl;
	}
}

void HarvestEffect::apply(EffectContext& ctx) {
	if (!ctx.observer)
	{
		std::cout << "ERROR, no observer" << std::endl;
		return;
	}

	if(!ctx.observer->GetPlayerCapturePile()) {
		std::cout << "No score pile available to Harvest!" << std::endl;
		return;
	}

	int totalValue = 0;

	for(const auto& card : *ctx.observer->GetPlayerCapturePile())
	{
		if(card) totalValue += card->GetNumericValue();
	}

	std::cout << "Harvest! Total capture value: " << totalValue << " damage!" << std::endl;
	auto targets = ctx.getTargets(targetType);
	for(auto* target : targets)
	{
		if(target)
			ctx.observer->Damage(target, totalValue, ctx.entitySource, false);
	}
}

void BoostTableEffect::apply(EffectContext& ctx) {
	if(!ctx.observer->GetTable()) {
		std::cout << "No table available!" << std::endl;
		return;
	}
	std::cout << "Show of Strength! All table cards boosted by +" << boostAmount << "!" << std::endl;
	for(auto& card : *ctx.observer->GetTable()) {
		if(card) {
			card->BoostValue(boostAmount);
			std::cout << "	" << card->GetName() << " -> value " << card->GetNumericValue() << std::endl;
		}
	}
}

std::string ChangeTableSuitEffect::getDescription() const {
	return "Change all table cards to " + suitToString(newSuit) + ".";
}

void ChangeTableSuitEffect::apply(EffectContext& ctx) {
	if(!ctx.observer->GetTable()) {
		std::cout << "No table available." << std::endl;
		return;
	}
	std::cout << "Demonstration of Intelligence! All table cards become " << suitToString(newSuit) << "!" << std::endl;
	for(auto& card : *ctx.observer->GetTable()) {
		if(card)
		{
			ctx.observer->ChangeCardSuit(card, newSuit);
		}
	}
}

void DoubleDebuffsEffect::apply(EffectContext& ctx) {
	auto targets = ctx.getTargets(targetType);
	for(auto* target : targets) {
		if(target) {
			std::cout << "Incurable Disease! Doubling debuffs on " << target->getName() << "!" << std::endl;
			ctx.observer->DoubleDebuffs(target);
		}
	}
}

void PoisonExplosionEffect::apply(EffectContext& ctx) {
	auto targets = ctx.getTargets(targetType);
	for(auto* target : targets) {
		if(!target) continue;
		if(target->hasStatus("Poisoned")) {
			auto poison = target->getStatus("Poisoned");
			int burstDmg = poison.magnitude * poison.duration;

			ctx.observer->Damage(target, burstDmg, nullptr, true);
			ctx.observer->RemoveStatus(target, "Poison");
		}
		else
		{
			std::cout << target->getName() << " has no Poison to detonate." << std::endl;
		}
	}
}