#if 0

#pragma once

#include <Combat/Damage.hpp>
#include <Combat/CharacterState.hpp>

struct BeginTurnContext
{	

};

struct EndTurnContext
{
	
};

struct DamageContext
{
	CharacterState & victim;
	DamageSourceType sourceType;
	CharacterState * source = nullptr;
	double damage;
	

	DamageContext (CharacterState & victim, DamageSourceType sourceType):
		victim {victim}, sourceType {sourceType} {}

	DamageContext (CharacterState & victim, DamageSourceType sourceType, CharacterState * source):
		victim {victim}, sourceType {sourceType}, source {source} {}

};

struct DeathContext
{
	DamageContext & source;
	
	
	DeathContext (DamageContext & context): source {context} {}
};

struct RetriggerPoison
{

};

#endif
