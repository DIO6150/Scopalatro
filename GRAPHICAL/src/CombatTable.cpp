#include <Combat/CombatTable.hpp>

#include <algorithm>

void TableArea::AddCard(CardModel * card)
{
	if (m_table.contains(card))
		return;

	m_table[card] = m_nextIndex++;
}

void TableArea::RemoveCard(CardModel * card)
{
	auto it = m_table.find(card);

	if (it == m_table.end())
		return;

	size_t removedIndex = it->second;

	m_table.erase(it);

	// Décale toutes les cartes suivantes
	for (auto& [id, index] : m_table)
	{
		if (index > removedIndex)
			--index;
	}

	--m_nextIndex;
}

glm::vec3 TableArea::GetPos(CardModel * card)
{
	auto it = m_table.find(card);

	if (it == m_table.end())
		return {};

	size_t count = m_table.size();

	// largeur totale occupée
	float totalWidth = (count - 1) * cardAdvance;

	// centre la rangée autour de X = 0
	float startX = -totalWidth * 0.5f;

	float x = startX + static_cast<float>(it->second) * cardAdvance;

	return glm::vec3(beginX + x, beginY, 0.1f * it->second - 1.0f);
}

glm::vec3 TableArea::GetScale()
{
	return glm::vec3(cardSize);
}
