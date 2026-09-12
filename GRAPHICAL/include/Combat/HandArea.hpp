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

class HandArea
{
public:
	float cardSize;
	float width;
	float beginX;
	float beginY = 0.0f;

	float hoverFactor = 2.0f;

	void AddCard(CardModel * card);

	void RemoveCard(CardModel * card);

	size_t GetHandSize();
	glm::vec3 GetCardPos(CardModel * card);

	glm::vec3 GetCardSize(CardModel * card);

	size_t GetIndex(CardModel * card);

	void SetHover(CardModel * card);
	void SetDrag(CardModel * card);
	void SetResolve(CardModel * card);
	auto begin() {return m_hand.begin(); }
	auto end  () {return m_hand.end  (); }

private:
	void RecalculateIndices();

	std::map<CardModel *, size_t> m_hand;

	CardModel * m_hoveredCard = nullptr;
	CardModel * m_draggedCard = nullptr;
	CardModel * m_resolvingCard = nullptr;

	size_t m_nextIndex = 0;
};
