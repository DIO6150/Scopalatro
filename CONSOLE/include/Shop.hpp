#pragma once
#include "Entity.hpp"
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include "RelicSystem.hpp"

struct ShopItem {
    std::string name;
    std::string description;
    int cost;
    std::function<void(Entity&)> effect;
};

class Shop {
public:
    Shop();
    void run(Entity& player);

private:
    void displayShop(const Entity& player) const;
	void generateRelics(const Entity& player);
    std::vector<ShopItem> m_items;
	std::vector<std::shared_ptr<Relic>> m_relics;
};