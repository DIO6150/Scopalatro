#pragma once

#include <stdint.h>

class Entity;
class IStatus;

class Damage
{
public:
	float value;

	Damage(Entity * source, float value)
		: source {.entity = source}
		, value {value}
	{
		setProperty(Flag_Entity);
	}

	Damage(IStatus * source, float value)
		: source {.status = source}
		, value {value}
	{
		unsetProperty(Flag_Entity);
	}

	Damage & echo() { return setProperty(Flag_Echo); }
	Damage & ignoreDef() { return setProperty(Flag_IgnoreDefense); }
	Damage & lethal() { return setProperty(Flag_Lethal); }
	Damage & nonLethal() { return unsetProperty(Flag_Lethal); }
	Damage & ignoreDamageMultiplier() { return setProperty(Flag_IgnoreDamageMultiplier); }
	Damage & cancel(IStatus * reason, float newValue = 0.0f)
	{
		value = newValue;
		cancelReason = reason;
		return setProperty(Flag_Canceled);
	}

	bool isSourceEntity() const { return getProperty(Flag_Entity); }
	bool isEcho() const { return getProperty(Flag_Echo); }
	bool canIgnoreDef() const { return getProperty(Flag_IgnoreDefense); }
	bool isLethal() const { return getProperty(Flag_Lethal); }
	bool isCanceled() const { return getProperty(Flag_Canceled); }
	bool hasValidSource() const { return source.entity || source.status; }
	bool hasValidValue() const { return value >= 0.0f; }

	Entity *  getEntity() { return source.entity; }
	IStatus * getStatus() { return source.status; }

	IStatus * getCancelReason()
	{
		if (isCanceled)
			return cancelReason;
		
		return nullptr;
	}

private:
	enum Property: uint64_t
	{
		Flag_Echo = 1 << 0,
		Flag_Entity = 1 << 1,
		Flag_IgnoreDefense = 1 << 2,
		Flag_IgnoreDamageMultiplier = 1 << 3,
		Flag_Canceled = 1 << 4,
		Flag_Lethal = 1 << 5,
	};

	Damage & setProperty(Property property)
	{
		properties |= property;
		return *this;
	}

	Damage & unsetProperty(Property property)
	{
		properties &= ~property;
		return *this;
	}

	bool getProperty(Property property) const
	{
		return properties & ~property == property;
	}

	union Source
	{
		Entity  * entity;
		IStatus * status;
	}        source;
	uint64_t properties;

	IStatus * cancelReason = nullptr;
};
