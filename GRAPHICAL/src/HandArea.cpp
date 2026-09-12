
#include <Combat/HandArea.hpp>

#include <Core/Logger.hpp>

#include <algorithm>

using namespace TLOT;

void HandArea::AddCard (CardModel * card)
{
	if (m_hand.find (card) == m_hand.end ())
	{
		m_hand.emplace (card, m_nextIndex++);
	}
}

void HandArea::RemoveCard (CardModel * card)
{
	if (m_hand.find (card) == m_hand.end ())
	{
		return;
	}

	m_hand.erase (card);
	if (card == m_hoveredCard)   m_hoveredCard = nullptr;
	if (card == m_draggedCard)   m_draggedCard = nullptr;
	if (card == m_resolvingCard) m_resolvingCard = nullptr;

	RecalculateIndices ();
	
	if (m_nextIndex != 0) m_nextIndex--;
}

size_t HandArea::GetHandSize ()
{
	return m_hand.size ();
}

glm::vec3 HandArea::GetCardPos (CardModel * card)
{
	if (m_hand.find(card) == m_hand.end())
        return glm::vec3{-1000.0f};

    size_t index = m_hand[card];

    float handSize = (float)GetHandSize();

    if (m_draggedCard || m_resolvingCard)
    {
        size_t draggedCardIndex =
            m_draggedCard ?
            m_hand.at(m_draggedCard) :
            m_hand.at(m_resolvingCard);

        handSize -= 1.0f;

        if (index > draggedCardIndex)
            index--;
        else if (index == draggedCardIndex)
            return card->GetPosition();
    }

    //--------------------------------------
    // Layout
    //--------------------------------------

    float spacing = cardSize;

    if (handSize > 1.0f)
    {
        float availableWidth = width - cardSize;

        spacing = availableWidth / (handSize - 1.0f);

        // Empêche les cartes de trop s'écarter
        spacing = std::min(spacing, cardSize * 1.1f);

        // Autorise le chevauchement quand la main grossit
        spacing = std::max(spacing, cardSize * 0.25f);
    }

    float totalWidth = cardSize;

    if (handSize > 1.0f)
        totalWidth += (handSize - 1.0f) * spacing;

    float startX = beginX + (width - totalWidth) * 0.5f;

    float x = startX + index * spacing + cardSize * 0.5f;
    float y = beginY + cardSize / 3.0f;
    float z = index * 0.1f;

    //--------------------------------------
    // Hover
    //--------------------------------------

    if (m_hoveredCard && !m_draggedCard && !m_resolvingCard)
    {
        auto hoveredCardIndex = m_hand.at(m_hoveredCard);

        float offsetX = cardSize * hoverFactor * 0.5f;

        if (index < hoveredCardIndex)
        {
            x -= offsetX;
        }
        else if (index > hoveredCardIndex)
        {
            x += offsetX;
        }
        else
        {
            y = cardSize + GetCardSize(card).y * 0.5f;
        }
    }

    return glm::vec3{x, y, z};
}

glm::vec3 HandArea::GetCardSize (CardModel * card)
{
	if (m_hand.find (card) == m_hand.end ()) return glm::vec3 {-1000.0f};

	if (card == m_hoveredCard || card == m_draggedCard)
	{
		return glm::vec3 {cardSize} * hoverFactor;
	}

	return glm::vec3 {cardSize};
}

size_t HandArea::GetIndex (CardModel * card)
{
	if (m_hand.find (card) == m_hand.end ()) return -1;

	return m_hand[card];
}

void HandArea::SetHover (CardModel * card)
{
	m_hoveredCard = card;
}

void HandArea::SetDrag (CardModel * card)
{
	m_hoveredCard = nullptr;
	m_draggedCard = card;
	m_resolvingCard = nullptr;
}

void HandArea::SetResolve (CardModel * card)
{
	m_draggedCard = nullptr;
	m_hoveredCard = nullptr;
	m_resolvingCard = card;
}

void HandArea::RecalculateIndices ()
{
	std::vector<std::pair<CardModel *, size_t>> sortedHand;
	sortedHand.reserve (m_hand.size ());

	for (auto & p : m_hand)
	{
		sortedHand.push_back (p);
	}

	std::sort (sortedHand.begin (), sortedHand.end (), [] (std::pair<CardModel *, size_t> & a, std::pair<CardModel *, size_t> & b) {
		return a.second < b.second;
	});

	m_hand.clear ();

	size_t index = 0;
	for (auto & [ID, _] : sortedHand)
	{
		m_hand[ID] = index++;
	}
}
