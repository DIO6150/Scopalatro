#pragma once

#include <Combat/CombatModel.hpp>
#include <Combat/CombatView.hpp>
#include <Combat/CombatPresenterHelper.hpp>
#include <Combat/CharacterState.hpp>

#include <memory>
#include <string>
#include <string_view>

#include <Debugger/SceneInspector.hpp>
#include <Renderer/Renderer.hpp>
#include <RenderContext.hpp>

class CombatPresenter: public CombatViewListener, public CombatModelListener
{
public:
	
	void Init();
	void Update();
	void Render();

	void Begin(CombatParams params);

	CombatPresenter(TLOT::RenderContext * context, TLOT::Renderer * renderer, TLOT::SceneInspector * inspector);

private:
	// View events

	void OnCardDropInPlayArea(CardModel * actor) override;
	void DebugDrawCard() override;
	void Pause() override;

	// Model events
	
	void OnMessage(std::string const message) override;
	void OnCardsDrawnToHand(std::vector<Card *> cards, bool enemy=false) override;
	void OnCardsDrawnToTable(std::vector<Card *> cards) override;
	void OnCardsDiscarded(std::vector<Card *> cards, bool enemy=false) override;
	void OnCardsCaptured(std::vector<Card *> cards, bool enemy=false) override;
	void OnCardUpdate(Card * cards) override;
	void OnCardResolving(Card * card, bool enemy=false) override;
	void OnCardPlacedOnTable(Card * card, bool enemy=false) override;
	void OnPlayerBeginTurn(int turnCount) override;
	void OnPlayerHealthChange(int newHP, int newMaxHP) override;
	void OnPlayerDeath() override;
	void OnPlayerEndTurn() override;
	void OnEnemyBeginTurn() override;
	void OnEnemyHealthChange(int newHP, int newMaxHP) override;
	void OnEnemyDeath() override;
	void OnEnemyEndTurn() override;

	// Members
	TLOT::RenderContext * m_context;
	TLOT::Renderer * m_renderer;

	std::unique_ptr<CombatView>  m_view;
	std::unique_ptr<CombatModel> m_model;

	std::map<Card *, CardModel *> m_cardToActor;
	std::map<CardModel *, Card *> m_actorToCard;

	Card      * Convert(CardModel * actor);
	CardModel * Convert(Card * card);
	std::vector<Card *>      Convert(std::vector<CardModel *> actors);
	std::vector<CardModel *> Convert(std::vector<Card *> cards);

	TaskManager m_taskManager;

	std::vector<std::function<TaskID()>> m_animationStack;
	TaskID m_blockingAnimation = SentinelTask;

	bool m_pause = false;
};
