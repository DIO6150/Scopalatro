#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include <RenderContext.hpp>
#include <Debugger/SceneInspector.hpp>
#include <Renderer/Camera.hpp>
#include <Renderer/Renderer.hpp>

#include <Combat/CombatHelper.hpp>
#include <Combat/CombatViewHelper.hpp>
#include <Combat/CombatViewListener.hpp>
#include <Combat/HandArea.hpp>
#include <Combat/CombatTable.hpp>
#include <Combat/PlayArea.hpp>
#include <TaskManager.hpp>
#include <TaskQueue.hpp>
#include <Transform.hpp>
#include <HoverMap.hpp>

#include <RenderableObjects/CardModel.hpp>
#include <RenderableObjects/HealthbarModel.hpp>
#include <RenderableObjects/ToolTipModel.hpp>
#include <RenderableObjects/EnemyModel.hpp>


class Card;

struct DrawArea
{
	TLOT::Transform cardBase;
	std::map<ObjectID, size_t> m_drawPile;
};

class CombatView
{
public:
	CombatView(
		CombatViewListener * subscriber,
		TLOT::RenderContext * context,
		TLOT::Renderer * renderer,
		TaskManager & manager,
		TLOT::SceneInspector * inspector
	);
	CardModel * RegisterCard(Card * card);
	void UpdateCard(CardModel * actor, Card * card);

	void Init();
	void Update();
	void Render();

	std::function<TaskID()> DrawCardsToHand(std::vector<CardModel *> cards, bool enemy);
	std::function<TaskID()> DrawCardsToTable(std::vector<CardModel *> cards);
	std::function<TaskID()> DiscardCards(std::vector<CardModel *> cards, bool enemy);
	std::function<TaskID()> CaptureCards(std::vector<CardModel *> cards, bool enemy);
	std::function<TaskID()> ExhaustCards(std::vector<CardModel *> cards);
	std::function<TaskID()> PlaceCardOnTable(CardModel * card, bool enemy);
	std::function<TaskID()> UpdatePlayerHealth(int hp, int maxhp);
	std::function<TaskID()> UpdateEnemyHealth(int hp, int maxhp);
	std::function<TaskID()> DisplayTurnNumber(int turnCount);
	std::function<TaskID()> DisplayEnemyTurn();
	std::function<TaskID()> EnableUserInput();
	std::function<TaskID()> DisableUserInput();
	std::function<TaskID()> ResolveCard(CardModel * actor, bool enemy);



private:
	CombatViewListener * m_subscriber;


	std::vector<CardModel *> GetHoveredCards();
	CardModel * GetHoveredCard();

	TaskID GenerateMoveCardTask(CardModel * card, glm::vec3 targetPosition, glm::vec3 targetScale, float speed, float(*easingFunction)(float));

	void StartHoverCard(CardModel * actor);
	void UpdateHover(CardModel * actor, HoverType hoverType, double delta);
	void StopHoverCard(CardModel * actor);

	void StartDraggingCard(CardModel * actor);
	void DragCard(CardModel * actor, glm::vec2 mouse, double deltaTime);
	void DropCard(CardModel * actor, bool shouldPlay);

	TaskID RearangeTableCards();
	TaskID RearangeHandCards(bool enemy);

	// Pointers

	TLOT::RenderContext  * m_context;
	TLOT::Renderer       * m_renderer;
	TLOT::SceneInspector * m_inspector;

	// States
	bool m_canInput = false;
	double m_delta;

	HoverMap m_hoverMap;

	CardModel * m_cardHoveredCurrent = nullptr;
	CardModel * m_cardDraggedCurrent = nullptr;
	CardModel * m_cardHoveredLast = nullptr;
	CardModel * m_cardDraggedLast = nullptr;

	// Positions and sizes
	float m_cardSize = 200.0f;
	float m_handBeginX = 200.0f;

	HandArea  m_hand;
	TableArea m_table;
	PlayArea  m_play;

	HandArea  m_enemyHand;
	
	// Models
	std::set<std::unique_ptr<CardModel>> m_cards;

	std::unique_ptr<HealthbarModel> m_playerHealthbar;
	std::unique_ptr<HealthbarModel> m_enemyHealthbar;
	
	std::unique_ptr<TLOT::TextObject> m_turnDisplay;

	std::unique_ptr<EnemyModel> m_enemy;

	// Events
	TaskManager & m_taskManager;
	TaskQueue   m_taskQueue {m_taskManager};

	TaskID      m_dragTask;
};
