#pragma once

#include <map>
#include <memory>
#include <string>
#include <set>

#include <Combat/CombatHelper.hpp>
#include <Combat/CombatModelHelper.hpp>
#include <Combat/CombatModelListener.hpp>
#include <Combat/Entity.hpp>
#include <Combat/Enemy.hpp>
#include <Combat/CardPiles.hpp>


#include <Card.hpp>

class CombatModel
{
public:
	CombatModel(CombatModelListener * listener);

	void RegisterPlayerCard(Card * card);
	void RegisterEnemyCard(Card * card);


	void Init(Difficulty difficulty);


	void BeginPlayerTurn();
	void PlayCard(Card * cardID);
	void EndPlayerTurn();

	void UpdateEntitiesState();

	void BeginEnemyTurn();
	
	
	void DrawToTable(int count);
	void DrawToHand(int count);
	void DrawUntilHandFull();
	void Discard(Card * cardID);
	void Exhaust(Card * cardID);
	
	
	void Damage(Entity * dst, uint32_t calculatedDamage, Entity * source, bool isEcho);
	void Heal(Entity * dst, uint32_t amount);
	void ChangeCardSuit(Card * card, Suit suit);
	void ChangeCardValue(Card * card, CardValue value);
	
	
	int EffectiveHandSize() { return m_handSize; }
	std::string GetCardDescription(Card * card);
	std::string GetCardName(Card * card);
	std::vector<Card *> * GetPlayerCapturePile() { return &m_playerPiles.capturePile; }
	std::vector<Card *> * GetTable() { return &m_table; }

	void UpdateStatuses(Entity * entity);
	void AddStatus(Entity * target, std::string status, int duration, int magnitude);
	void RemoveStatus(Entity * target, std::string status);
	void DoubleDebuffs(Entity * target);
		
private:
	CombatModelListener * m_listener;
	std::set<Card *> m_registeredCards;
	
	void PrintPiles();
	std::vector<int> FindCaptureIndices(const Card * playedCard, int customTargetVal = -1);
	
	
	std::vector<Card *> m_table;
	
	CardPiles m_playerPiles;
	CardPiles m_enemyPiles;
	std::unique_ptr<Entity> m_player;
	std::unique_ptr<Enemy>  m_enemy;
	
	
	size_t m_handSize = 3;
	size_t m_maxHandSize = 10;
	int scopaCount = 0;
    int turn = 0;
	int playerCaptures = 0;
	int enemyCaptures  = 0;
};
