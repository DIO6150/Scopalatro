#include "Shop.hpp"
#include "Relic.hpp"
#include "Draft.hpp"
#include "Entity.hpp"
#include <iostream>

Shop::Shop() {
    m_items = {
        {
            "Blood Potion",
            "Heal 20% of your max HP.",
            0,
            [](Entity& p) {
                int amount = p.getMaxHp() / 5; 
                p.heal(amount);
            }
        },
        {
            "Vambraces",
            "The first time each combat you gain block, double the amount.",
            0,
            [](Entity& p) {
                p.addPassive("DoubleFirstBlock");
                std::cout << p.getName() << " equipped the Vambraces!\n";
            }
        },
        {
            "Small Shield",
            "Gain 8 Block going into the next fight.",
            0,
            [](Entity& p) { 
                p.addPendingBlock(8);
                std::cout << "Shield stowed for next combat.\n";
            }
        },
    };
}

void Shop::generateRelics(const Entity& player) {
	m_relics.clear();

	std::vector<std::shared_ptr<Relic>> pool;
	pool.push_back(std::make_shared<HiddenAdvantage>());
	pool.push_back(std::make_shared<DamageAndInterest>());
	pool.push_back(std::make_shared<GoldenScopa>());
	pool.push_back(std::make_shared<CursedThorns>());
	pool.push_back(std::make_shared<CocktailOverdose>());
	pool.push_back(std::make_shared<MolotovCocktail>());
	pool.push_back(std::make_shared<GlassCocktail>());
	pool.push_back(std::make_shared<BloodstainedDagger>());
	pool.push_back(std::make_shared<NDA>());
	pool.push_back(std::make_shared<GamblersFallacy>());
	pool.push_back(std::make_shared<Flashbang>());
	pool.push_back(std::make_shared<MagicTrick>());
	
	std::vector<std::shared_ptr<Relic>> available;
	for (auto& r : pool) {
		if (!player.hasObtainedRelic(r->name)) {
			available.push_back(r);
		}
	}
	
	for (int i = 0; i < 2 && !available.empty(); ++i) {
		int idx = rand() % available.size();
		m_relics.push_back(available[idx]);
		available.erase(available.begin() + idx);
	}
}


void Shop::displayShop(const Entity& player) const {
    std::cout << "============================================\n";
    std::cout << "                 SHOP\n                  \n";
    std::cout << " Gold: " << player.getGold() << " coins"
              << std::string(29 - std::to_string(player.getGold()).size(), ' ') << "\n";

    for (size_t i = 0; i < m_items.size(); ++i) {
        const auto& item = m_items[i];
        bool affordable = player.getGold() >= item.cost;
        std::string prefix = affordable ? " " : " [OUT OF FUNDS] ";
        std::cout << " [" << (i + 1) << "] " << item.name
                  << " (" << item.cost << "G) - " << item.description << "\n";
    }

	if (!m_relics.empty()) {
		std::cout << "			RELICS\n			\n";
		for (size_t i = 0; i < m_relics.size(); ++i) {
			const auto& relic = m_relics[i];
			bool affordable = player.getGold() >= relic->cost;
			int displayIdx = m_items.size() + i + 1;
			std::cout << "  [" << displayIdx << "] " << relic->name
				<< " (" << relic->cost << "G)";
			if (!affordable) std::cout << " [NOT ENOUGH GOLD]";
			std::cout << "\n      " << relic->description << "\n\n";
		}
	}

    std::cout << " [0] Leave Shop\n";
    std::cout << "============================================\n";
}

void Shop::run(Entity& player) {
	generateRelics(player);
    while (true) {
        displayShop(player);

        std::cout << "Make your choice: ";
        int choice = -1;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }

        if (choice == 0) {
            std::cout << "The die is cast.\n";
            break;
        }

		int totalOptions = m_items.size() + m_relics.size();
        if (choice < 1 || choice > totalOptions) {
            std::cout << "Unwise.\n";
            continue;
        }

		if (choice <= static_cast<int>(m_items.size())) {
			auto& item = m_items[choice - 1];
			if (player.getGold() < item.cost) {
				std::cout << "You don't have enough gold.\n";
				continue;
			}
			player.spendGold(item.cost);
			item.effect(player);
			std::cout << "It is done. Purchased: " << item.name << "\n";
		}
		else {
			int relicIdx = choice - 1 - m_items.size();
			auto relic = m_relics[relicIdx];
			if (player.getGold() < relic->cost) {
				std::cout << "You don't have enough gold.\n";
				continue;
			}
			player.spendGold(relic->cost);
			player.addRelic(relic);
			m_relics.erase(m_relics.begin() + relicIdx);
		}
    }
}