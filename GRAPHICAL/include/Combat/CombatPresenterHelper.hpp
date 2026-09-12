#pragma once

#include <set>

#include <Combat/CombatHelper.hpp>
#include <Combat/CombatModelHelper.hpp>
#include <Combat/CombatViewHelper.hpp>
#include <Combat/Enemy.hpp>

#include <Card.hpp>

struct EnnemyParam
{
	
};

struct CombatParams
{
	Difficulty difficulty = Difficulty::Easy;

	uint32_t playerHP;
	uint32_t playerMaxHP;

	std::vector<Card *> playerCards;
	std::vector<Card *> enemyCards;


	std::vector<int>    playerItems;
};
