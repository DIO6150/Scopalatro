#pragma once

#include <string>
#include <vector>

#include <Combat/CombatHelper.hpp>

struct DamageContext;
struct DeathContext;

class CombatModelListener
{
public:
	virtual void OnMessage(std::string const message) = 0;


	virtual void OnCardsDrawnToHand(std::vector<Card *> cards, bool enemy=false) = 0;
	virtual void OnCardsDrawnToTable(std::vector<Card *> cards) = 0;
	virtual void OnCardsDiscarded(std::vector<Card *> cards, bool enemy=false) = 0;
	virtual void OnCardsCaptured(std::vector<Card *> cards, bool enemy=false) = 0;
	
	virtual void OnCardUpdate(Card * cards) = 0;

	virtual void OnCardResolving(Card * card, bool enemy=false) = 0;
	virtual void OnCardPlacedOnTable(Card * card, bool enemy=false) = 0;

	
	virtual void OnPlayerBeginTurn(int turnCount) = 0;
	virtual void OnPlayerHealthChange(int newHP, int newMaxHP) = 0;
	virtual void OnPlayerDeath() = 0;
	virtual void OnPlayerEndTurn()   = 0;
	
	virtual void OnEnemyBeginTurn() = 0;
	virtual void OnEnemyHealthChange(int newHP, int newMaxHP) = 0;
	virtual void OnEnemyDeath() = 0;
	virtual void OnEnemyEndTurn()   = 0;

};
