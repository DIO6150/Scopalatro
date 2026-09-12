#pragma once

#include "Entity.hpp"
#include "Card.hpp"
#include <vector>
#include <memory>
#include <functional>

class Draft {
public:
	void run(Entity& player,
		std::vector<std::unique_ptr<Card>>& draftedCards,
		std::vector<std::function<bool(Card*)>>& enchantments);
};