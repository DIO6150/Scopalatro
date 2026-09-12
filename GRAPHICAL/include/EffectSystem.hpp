#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>

#include <CardCommon.hpp>

class Entity;
class Card;
class CombatModel;

enum class TargetType {
    Enemy,
    Self,
    AllEnemies,
    All
};

struct EffectContext {
	CombatModel * observer;
    Entity      * entitySource;
    Card        * cardSource;
    std::vector<Entity *> targets;

    std::vector<Entity*> getTargets(TargetType type) {
        switch (type) {
            case TargetType::Self: return {entitySource};
            case TargetType::Enemy: return targets; 
            case TargetType::AllEnemies: return targets;
            case TargetType::All: {
                std::vector<Entity*> all = targets;
                all.push_back(entitySource);
                return all;
            }
            default: return targets;
        }
    }
};

class Effect {
protected:
    TargetType targetType;
public:
    Effect(TargetType target = TargetType::Enemy) : targetType(target) {}
    virtual ~Effect() = default;

    virtual void apply(EffectContext& ctx) = 0;
    virtual std::string getDescription() const = 0;
    virtual std::unique_ptr<Effect> clone() const = 0;
};

class DamageEffect : public Effect {
    int amount;
public:
    DamageEffect(int amt, TargetType target = TargetType::Enemy) 
        : Effect(target), amount(amt) {}

    void apply(EffectContext& ctx) override;

    std::string getDescription() const override {
        std::string t = (targetType == TargetType::Self) ? "Self" : "Target";
        return "Deal " + std::to_string(amount) + " damage to " + t + ".";
    }

    std::unique_ptr<Effect> clone() const override {
        return std::make_unique<DamageEffect>(amount, targetType);
    }
};

class HealEffect : public Effect {
    int amount;
public:
    HealEffect(int amt, TargetType target = TargetType::Self) 
        : Effect(target), amount(amt) {}

    void apply(EffectContext& ctx) override;

    std::string getDescription() const override {
        std::string t = (targetType == TargetType::Self) ? "Self" : "Target";
        return "Heal " + t + " for " + std::to_string(amount) + " HP.";
    }

    std::unique_ptr<Effect> clone() const override {
        return std::make_unique<HealEffect>(amount, targetType);
    }
};

class ApplyStatusEffect : public Effect {
    std::string statusName;
    int duration;
	int magnitude;
public:
	ApplyStatusEffect(std::string name, int dur, int mag = 0, TargetType target = TargetType::Enemy)
        : Effect(target), statusName(name), duration(dur), magnitude(mag) {}

    void apply(EffectContext& ctx) override;

    std::string getDescription() const override {
        std::string t = (targetType == TargetType::Self) ? "Self" : "Target";
		std::string desc = "Apply " + statusName;
		if (magnitude > 0) desc += " (" + std::to_string(magnitude) + ")";
		desc += " to " + t + " for " + std::to_string(duration) + " turns.";
		return desc;
    }

    std::unique_ptr<Effect> clone() const override {
        return std::make_unique<ApplyStatusEffect>(statusName, duration, magnitude, targetType);
    }
};

class BlockEffect : public Effect {
    int amount;
public:
    BlockEffect(int amt, TargetType target = TargetType::Self)
        : Effect(target), amount(amt) {}

    void apply(EffectContext& ctx) override;

    std::string getDescription() const override {
        std::string t = (targetType == TargetType::Self) ? "Self" : "Target";
        return "Grant " + std::to_string(amount) + " Shield to " + t + ".";
    }

    std::unique_ptr<Effect> clone() const override {
        return std::make_unique<BlockEffect>(amount, targetType);
    }
};

class DrawCardEffect : public Effect {
	int amount;
public:
	DrawCardEffect(int amt, TargetType target = TargetType::Self)
		: Effect(target), amount(amt) {}

	void apply(EffectContext& ctx) override;

	std::string getDescription() const override {
		return "Draw " + std::to_string(amount) + " cards.";
	}

	std::unique_ptr<Effect> clone() const override {
		return std::make_unique<DrawCardEffect>(amount, targetType);
	}
};

class HarvestEffect : public Effect {
public:
	HarvestEffect(TargetType target = TargetType::AllEnemies)
		: Effect(target) {}

	void apply(EffectContext& ctx) override;

	std::string getDescription() const override {
		return "Deal damage equal to total capture value to all enemies.";
	}

	std::unique_ptr<Effect> clone() const override {
		return std::make_unique<HarvestEffect>(targetType);
	}
};

class BoostTableEffect : public Effect {
	int boostAmount;
public:
	BoostTableEffect(int amt, TargetType target = TargetType::Self)
		: Effect(target), boostAmount(amt) {}

	void apply(EffectContext& ctx) override;
		
	std::string getDescription() const override {
		return "Boost all table cards by +" + std::to_string(boostAmount) + ".";
	}

	std::unique_ptr<Effect> clone() const override {
		return std::make_unique<BoostTableEffect>(boostAmount, targetType);
	}
};

class ChangeTableSuitEffect : public Effect {
	Suit newSuit;
public:
	ChangeTableSuitEffect(Suit s, TargetType target = TargetType::Self)
		: Effect(target), newSuit(s) {}
	
	void apply(EffectContext& ctx) override;
	
	std::string getDescription() const override;
	
	std::unique_ptr<Effect> clone() const override {
		return std::make_unique<ChangeTableSuitEffect>(newSuit, targetType);
	}
};

class DoubleDebuffsEffect : public Effect {
public:
	DoubleDebuffsEffect(TargetType target = TargetType::Enemy)
		: Effect(target) {}
	
	void apply(EffectContext& ctx) override;
	
	std::string getDescription() const override {
		return "Double all debuffs on Target.";
	}
	
	std::unique_ptr<Effect> clone() const override {
		return std::make_unique<DoubleDebuffsEffect>(targetType);
	}
};

class PoisonExplosionEffect : public Effect {
public:
	PoisonExplosionEffect(TargetType target = TargetType::Enemy)
		: Effect(target) {}
	
	void apply(EffectContext& ctx) override;
	
	std::string getDescription() const override {
		return "Detonate Poison on Target for burst damage.";
	}
	
	std::unique_ptr<Effect> clone() const override {
		return std::make_unique<PoisonExplosionEffect>(targetType);
	}
};
