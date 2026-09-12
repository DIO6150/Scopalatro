#pragma once

#include <string>
#include <functional>
#include <list>
#include <memory>
#include <map>
#include <vector>

class Damage;
class Entity;

enum class DamageModifierStage
{
	AttackDamageFlat1,
	AttackDamageMultiplier1,
	AttackDamageFlat2,
	AttackDamageMultiplier2,

	StatusDamageFlat1,
	StatusDamageMultiplier1,
	StatusDamageFlat2,
	StatusDamageMultiplier2,
};

enum class DamageApplicationStage
{
	BeforeAttackDamage,
	AfterAttackDamage,

	BeforeStatusDamage,
	AfterStatusDamage,
};

class IStatus
{
public:

	virtual ~IStatus() = 0;

	virtual void applyAttackDamageTakenModifier(
		Entity * target,
		Damage & damage,
		DamageModifierStage stage
	) {}

	virtual void applyAttackDamageDealtModifier(
		Entity * target,
		Damage & damage,
		DamageModifierStage stage
	) {}

	virtual void applyBlockModifier(
		Entity * target,
		float & block
	) {}

	virtual void onAttackDamageReceived(
		Entity * target,
		Damage & damage,
		float & block,
		DamageApplicationStage stage
	) {}

	virtual void onAttackDamageDealt(
		Entity * target,
		Damage & damage,
		float & block,
		DamageApplicationStage stage
	) {}

	virtual void applyBlockGainModifier(
		Entity * target,
		float & block
	) {}

	virtual void onTurnStart(
		Entity * target,
		int turnCount
	) {}

	virtual void onTurnEnd(
		Entity * target,
		int turnCount
	) {}
};

template<class Derived>
class MetaStatus : public IStatus
{
protected:
	MetaStatus() = default;
	MetaStatus(MetaStatus const &) = default;
	MetaStatus(MetaStatus &&) = default;
public:
	static std::string const getName()
	{
		Derived::getName();
	}

	template<class ... Args>
	void stack(Args && ... args)
	{
		static_cast<Derived *>(this)->stack(std::forward<Args>(args)...);
	}

	bool hasEnded()
	{
		return static_cast<Derived *>(this)->hasEnded();
	}
};

class StatusList
{
public:
	template<class Status, class ... Args>
	void addStatus(Args && ... args)
	{
		std::string key = MetaStatus<Status>::getName();

		if (m_list.contains(key))
		{
			IStatus * aStatus = m_list.at(key);
			MetaStatus<Status> * status = static_cast<MetaStatus<Status> *>(aStatus);
			status->stack(std::forward<Args>(args)...);

			return;
		}

		m_list.try_emplace(
			std::piecewise_construct,
			std::make_tuple(key),
			std::make_tuple(std::forward<Args>(args)...);
		)
	}

	IStatus * getStatus(std::string key)
	{
		if (m_list.contains(key))
			return m_list.at(key);

		return nullptr;
	}

	auto begin() { return m_list.begin(); }
	auto end  () { return m_list.end  (); }


private:
	std::map<std::string, IStatus *> m_list;
};