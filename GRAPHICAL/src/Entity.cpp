#include <Combat/Entity.hpp>
#include <Combat/Statuses/IStatus.hpp>

Entity::Entity(float hp, float maxHp, CombatModelListener * listener)
	: m_currentHp {hp}
	, m_maxHp     {maxHp}
	, m_listener  {listener}
{

}

// could be faster, but fast enough for our purpose + its clearer this way
Damage Entity::calculateDamage(Damage damage)
{
	if (!damage.hasValidSource() || !damage.hasValidValue())
		return damage;

	if (damage.isSourceEntity())
	{
		Entity * source = damage.getEntity();

		// ATTACKER : stuff like strength
		for (auto & status : source->m_statuses)
		{
			status->applyAttackDamageDealtModifier(this, damage, DamageModifierStage::AttackDamageFlat1);

			if (damage.isCanceled()) return damage;
		}

		// ATTACKER : idk
		for (auto & status : source->m_statuses)
		{
			status->applyAttackDamageDealtModifier(this, damage, DamageModifierStage::AttackDamageMultiplier1);

			if (damage.isCanceled()) return damage;
		}

		// ATTACKER : stuff like berserker
		for (auto & status : source->m_statuses)
		{
			status->applyAttackDamageDealtModifier(this, damage, DamageModifierStage::AttackDamageFlat2);

			if (damage.isCanceled()) return damage;
		}

		// ATTACKER : idk
		for (auto & status : source->m_statuses)
		{
			status->applyAttackDamageDealtModifier(this, damage, DamageModifierStage::AttackDamageMultiplier2);

			if (damage.isCanceled()) return damage;
		}

		// TARGET : idk
		for (auto & status : m_statuses)
		{
			status->applyAttackDamageTakenModifier(this, damage, DamageModifierStage::AttackDamageFlat1);

			if (damage.isCanceled()) return damage;
		}

		// TARGET : stuff like vuln here
		for (auto & status : m_statuses)
		{
			status->applyAttackDamageTakenModifier(this, damage, DamageModifierStage::AttackDamageMultiplier1);

			if (damage.isCanceled()) return damage;
		}

		// TARGET : idk
		for (auto & status : m_statuses)
		{
			status->applyAttackDamageTakenModifier(this, damage, DamageModifierStage::AttackDamageFlat2);

			if (damage.isCanceled()) return damage;
		}

		// TARGET : idk
		for (auto & status : m_statuses)
		{
			status->applyAttackDamageTakenModifier(this, damage, DamageModifierStage::AttackDamageMultiplier2);

			if (damage.isCanceled()) return damage;
		}
	} // do nothing if its a status

	if (damage.value < 0.0f)
		damage.value = 0.0f;

	return damage;
}

float Entity::calculateBlock()
{
	float block = m_block;

	// TARGET : block effects
	for (auto & status : m_statuses)
	{
		status->applyBlockModifier(this, block);
	}

	return block;
}

Damage Entity::applyDamage(Damage damage, float block)
{
	if (!damage.hasValidSource() || !damage.hasValidValue())
		return damage;

	if (damage.isSourceEntity())
	{
		Entity * source = damage.getEntity();

		for (auto & status : source->m_statuses)
		{
			status->onAttackDamageDealt(this, damage, block, DamageApplicationStage::BeforeAttackDamage);
		}

		for (auto & status : m_statuses)
		{
			status->onAttackDamageReceived(this, damage, block, DamageApplicationStage::BeforeAttackDamage);
		}
	}

	if (block > 0.0f && !damage.canIgnoreDef())
	{
		float absorbed = std::min(block, damage.value);
		m_block -= absorbed;
		m_block = std::max(m_block, 0.0f);
		damage.value -= absorbed;
	}

	if (damage.value > 0.0f)
		m_currentHp -= damage.value;

	if (m_currentHp <= 0.0f)
	{
		m_currentHp = 0.0f;
		damage.lethal();
	}

	if (damage.isSourceEntity())
	{
		Entity * source = damage.getEntity();

		for (auto & status : source->m_statuses)
		{
			status->onAttackDamageDealt(this, damage, block, DamageApplicationStage::AfterAttackDamage);
		}

		for (auto & status : m_statuses)
		{
			status->onAttackDamageReceived(this, damage, block, DamageApplicationStage::AfterAttackDamage);
		}
	}

	return damage;
}

/*
	// events
	if (damage.isSourceEntity()) m_listener->OnEntityDamage(this, damage, died);
	else m_listener->OnStatusDamage(this, damage, died);
*/


float Entity::gainBlock(float block)
{
	// statuses
	for (auto & status : m_statuses)
	{
		status->applyBlockGainModifier(this, block);
	}

	// application
	m_block += block;

	// event
	//m_listener->OnEntityGainBlock(this, m_block);

	return block;
}

float Entity::heal(float amount)
{
	m_currentHp += amount;
	m_currentHp = std::min(m_currentHp, m_maxHp);
}

bool Entity::isAlive() const
{
	return m_currentHp > 0;
}

float Entity::getCurrentHp() const
{
	return m_currentHp;
}

float Entity::getMaxHp() const
{
	return m_maxHp;
}

int Entity::getGold() const
{
	return m_gold;
}

void Entity::addGold(int amount)
{
	m_gold += amount;
}

void Entity::spendGold(int amount)
{
	m_gold -= amount;

	if (m_gold < 0)
		m_gold = 0;
}

void Entity::resetCombatState()
{
	m_currentHp = m_maxHp;
	m_block = 0.0f;
	m_statuses.clear();

	removeExpiredRelics();
}

void Entity::addRelic(Relic * relic)
{
	m_relics.push_back(relic);

	//relic->onPickup(this);
}

void Entity::removeExpiredRelics()
{
	std::vector<size_t> expired;

	size_t index = 0;
	for (auto & relic : m_relics)
	{
		if (relic->isExpired())
			expired.emplace_back(index);
		
		++index;
	}

	for (size_t index = m_relics.size() - 1; index < 0; --index)
	{
		m_relics.erase(m_relics.begin() + index);
	}
}
