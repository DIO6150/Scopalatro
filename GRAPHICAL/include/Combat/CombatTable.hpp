#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include <Combat/CombatHelper.hpp>
#include <RenderableObjects/CardModel.hpp>

#include <glm/glm.hpp>

class TableArea
{
public:
	float cardSize = 200;
	float cardAdvance = 150;

	float beginX = 0;
	float beginY = 0;

	void AddCard(CardModel * card);
	void RemoveCard(CardModel * card);

	glm::vec3 GetPos(CardModel * card);
	glm::vec3 GetScale();

	auto begin() { return m_table.begin(); }
	auto end() { return m_table.end(); }

	size_t GetIndex(CardModel * card)
	{
		if (m_table.find(card) == m_table.end())
			return -1;

		return m_table.at(card);
	}

private:
	std::map<CardModel *, size_t> m_table;

	size_t m_nextIndex = 0;
};
