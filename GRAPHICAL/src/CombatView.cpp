#include <Combat/CombatView.hpp>

#include <Easing.hpp>
#include <InputManager.hpp>

#include <Debugger/DebugRenderer.hpp>


using namespace TLOT;

// Helpers =>

static bool PointInRect(float x, float y, glm::vec4 rect)
{
	bool flag = x > rect.x && x < rect.x + rect.z && y > rect.y && y < rect.y + rect.w;
	//if(flag)
	//{
	//	DebugRenderer::Get().CreateRect(rect, {1.0, 0.0, 0.0}, true);
	//}

	return flag;
}

// Public =>

CombatView::CombatView(CombatViewListener * subscriber, TLOT::RenderContext * context, Renderer * renderer, TaskManager & taskManager, TLOT::SceneInspector * inspector)
	: m_subscriber {subscriber}
	, m_context {context}
	, m_renderer {renderer}
	, m_taskManager {taskManager}
	, m_inspector {inspector}
{

}

CardModel * CombatView::RegisterCard(Card * card)
{
	auto it = m_cards.emplace(std::make_unique<CardModel>(m_renderer, card->GetSuit(), card->GetValue()));
	auto actor = it.first->get();

	actor->SetScale({m_cardSize, m_cardSize, 1.0f});
	actor->SetPosition({-1000.0f, 0.0f, 0.0f});
	actor->SetPivot({0.5f, 0.5f, 0.0f});

	actor->SetTooltip(card->GetName(), card->GetDescription());

	return actor;
}

void CombatView::UpdateCard(CardModel * actor, Card * card)
{
	actor->UpdateSymbol(card->GetSuit(), card->GetValue());
	actor->SetTooltip(card->GetName(), card->GetDescription());
}

void CombatView::Init()
{
	float width  = (float)m_context->GetViewport().width;
	float height = (float)m_context->GetViewport().height;

	m_cardSize      = width / 12.8f;
	m_hand.cardSize = m_cardSize;
	m_hand.width    = 10 * m_cardSize;
	m_hand.beginX   = (width - m_hand.width) / 2;

	m_enemyHand.cardSize = m_cardSize / 6.0;
	m_enemyHand.width    = 10 * m_enemyHand.cardSize;
	m_enemyHand.beginX   = (width - m_enemyHand.width) / 2;
	m_enemyHand.beginY   = (height - m_enemyHand.cardSize - 70);


	m_play.beginX  = m_hand.beginX;
	m_play.beginY  = m_cardSize * 2.3;
	m_play.width   = m_hand.width;
	m_play.height  =(height -(m_play.beginY)) * 0.8f;

	m_table.beginY   = m_play.beginY + m_cardSize * 1.0;
	m_table.beginX   = width / 2.0;
	m_table.cardSize = m_cardSize;

	m_playerHealthbar = std::make_unique<HealthbarModel>(m_renderer, 0.0, 0.0);
	m_playerHealthbar->SetPosition(
		{33, (height - m_playerHealthbar->GetWidth()) / 2, 2.}
	); // width instead because we rotate the bar internally

	m_enemyHealthbar = std::make_unique<HealthbarModel>(m_renderer, 0.0, 0.0, false);
	m_enemyHealthbar->SetPosition(
		{
			width - 33 - (m_enemyHealthbar->GetHeight() / 2),
			(height - m_enemyHealthbar->GetWidth()) / 2,
			2.
		}
	); // width instead because we rotate the bar internally

	m_turnDisplay = std::make_unique<TextObject>(
		m_renderer,
		TLOT::AssetManager::Cache("font_hh_light"),
		TLOT::AssetManager::Cache("font_hh_it"),
		TLOT::AssetManager::Cache("font_hh_bold")
	);

	m_turnDisplay->IsVisible(false);

	m_enemy = std::make_unique<EnemyModel>(m_renderer);
	float scaleX = m_enemy->GetScale().x;
	float scaleY = m_enemy->GetScale().y;
	m_enemy->SetPosition(glm::vec3 {width / 2.0f, height - (scaleY / 2.0f) - 30.0f, -2.0f});

	m_inspector->RegisterRenderableObject(m_enemy.get());

	m_inspector->RegisterRenderableObject(m_playerHealthbar.get());
	m_inspector->RegisterRenderableObject(m_enemyHealthbar.get());
}


void CombatView::Update()
{
    m_delta = m_context->deltaTime;

    //--------------------------------------
    // Input
    //--------------------------------------

    double mouseX, mouseY;
    InputManager::getInstance().getMousePos(mouseX, mouseY);

    mouseY = m_context->GetViewport().height - mouseY;

    bool leftPressed  = InputManager::getInstance().isMouseButtonPressed(GLFW_MOUSE_BUTTON_1);
    bool leftReleased = InputManager::getInstance().isMouseButtonReleased(GLFW_MOUSE_BUTTON_1);

    //--------------------------------------
    // Debug
    //--------------------------------------

    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_D))
        m_subscriber->DebugDrawCard();

	if (InputManager::getInstance().isKeyPressed(GLFW_KEY_P))
        m_subscriber->Pause();

    //--------------------------------------
    // Disable input
    //--------------------------------------

    if (!m_canInput)
    {
        leftPressed  = false;
        leftReleased = false;
    }

    //--------------------------------------
    // Play area detection
    //--------------------------------------

    bool inPlayArea =
        PointInRect(
            mouseX,
            mouseY,
            {
                m_play.beginX,
                m_play.beginY,
                m_play.width,
                m_play.height
            });

    //--------------------------------------
    // Hover detection
    //--------------------------------------

    CardModel* hoveredCard = nullptr;

    if (m_canInput)
        hoveredCard = GetHoveredCard();

    //--------------------------------------
    // Hover transitions
    //--------------------------------------
	HoverType hoverType = HoverType::None;

	if (hoveredCard && !m_cardDraggedCurrent)
	{
		m_hoverMap.Hover(hoveredCard, m_delta);

		hoverType = m_hoverMap.GetHover(hoveredCard);
	}

    if (hoveredCard != m_cardHoveredCurrent)
    {
        if (m_cardHoveredCurrent && !m_cardDraggedCurrent)
        {
            StopHoverCard(m_cardHoveredCurrent);
        }

        if (hoveredCard && !m_cardDraggedCurrent)
        {
			
			//auto hoverResult = m_hoverMap.GetHover(hoveredCard); <-- arrange les correctement
            StartHoverCard(hoveredCard);
        }

        m_cardHoveredCurrent = hoveredCard;
    }

	if (hoveredCard)
		UpdateHover(hoveredCard, hoverType, m_delta);

    //--------------------------------------
    // Drag begin
    //--------------------------------------

    if (leftPressed &&
        hoveredCard &&
        !m_cardDraggedCurrent)
    {
		m_cardDraggedCurrent = hoveredCard;
		
		if (m_hand.GetIndex(m_cardDraggedCurrent) == static_cast<size_t>(-1))
			m_cardDraggedCurrent = nullptr;

        StartDraggingCard(m_cardDraggedCurrent);
    }

    //--------------------------------------
    // Drag update
    //--------------------------------------

    if (m_cardDraggedCurrent)
    {
        DragCard(
            m_cardDraggedCurrent,
            glm::vec2(mouseX, mouseY),
			m_delta);

		//Logger::log(LogLevel::Info, "dragging");
    }

    //--------------------------------------
    // Drag end
    //--------------------------------------

    if (leftReleased &&
        m_cardDraggedCurrent)
    {
        DropCard(
            m_cardDraggedCurrent,
            inPlayArea);

        m_cardDraggedCurrent = nullptr;
    }

    //--------------------------------------
    // Updates
    //--------------------------------------

	//for (auto task : m_modelAnimationStack)
	//{
	//	if (m_taskManager.IsTaskAlive(task))
	//		break;
//
	//	m_modelAnimationStack.erase(m_modelAnimationStack.begin());
	//	m_taskManager.StartTask(m_modelAnimationStack[0]);
	//	break;
	//}

    m_hoverMap.Update();
    m_taskManager.Update(m_context->deltaTime);
}

void CombatView::Render()
{
	for (auto & actor : m_cards)
	{
		actor->Render();
	}

	m_playerHealthbar->Render();
	m_enemyHealthbar->Render();

	m_turnDisplay->Render();

	m_enemy->Render();
}

// Warning: right now, it can't be spammed, otherwise it will have a ugly effect
std::function<TaskID()> CombatView::DrawCardsToHand(std::vector<CardModel *> cards, bool enemy)
{
	if(cards.empty())
		return [] () { return SentinelTask; };

	if (!enemy)
	{
		return [this, cards]()
		{
			std::map<CardModel *, size_t> handCopy; 
			handCopy.insert(m_hand.begin(), m_hand.end());
			
			for(auto actor : cards)
			{
				m_hand.AddCard(actor);
			}
			
			// Rearange hand
			for (auto & [actor, index] : handCopy)
			{
				auto task = GenerateMoveCardTask(actor, m_hand.GetCardPos(actor), m_hand.GetCardSize(actor), 1.0f, easeInOutCirc);
				m_taskQueue.PushCancel(reinterpret_cast<uint64_t>(actor), task);
			}
			
			int index = 0;
			double timeOffset = handCopy.empty() ? 0.05 : 0.0;
			TaskID task;
			for (auto & actor : cards)
			{
				actor->SetPosition({ -1000.0f, (m_context->GetViewport().height + m_cardSize) / 2.0f, 0.0f });
				task = GenerateMoveCardTask(actor, m_hand.GetCardPos(actor), m_hand.GetCardSize(actor), 1.0f, easeInOutCirc);
				m_taskQueue.PushCancel(reinterpret_cast<uint64_t>(actor), task, timeOffset + index * 0.25);
				++index;
			}
			
			return task;
		};
	}
	else
	{
		return [this, cards]()
		{
			std::map<CardModel *, size_t> handCopy; 
			handCopy.insert(m_enemyHand.begin(), m_enemyHand.end());
			
			for(auto actor : cards)
			{
				m_enemyHand.AddCard(actor);
			}
			
			// Rearange hand
			for (auto & [actor, index] : handCopy)
			{
				auto task = GenerateMoveCardTask(actor, m_enemyHand.GetCardPos(actor), m_enemyHand.GetCardSize(actor), 1.0f, easeInOutCirc);
				m_taskQueue.PushCancel(reinterpret_cast<uint64_t>(actor), task);
			}
			
			int index = 0;
			double timeOffset = handCopy.empty() ? 0.05 : 0.0;
			TaskID task;
			for (auto & actor : cards)
			{
				actor->SetPosition({ -1000.0f, (m_context->GetViewport().height + m_cardSize) / 2.0f, 0.0f });
				task = GenerateMoveCardTask(actor, m_enemyHand.GetCardPos(actor), m_enemyHand.GetCardSize(actor), 1.0f, easeInOutCirc);
				m_taskQueue.PushCancel(reinterpret_cast<uint64_t>(actor), task, timeOffset + index * 0.25);
				++index;
			}
			
			return task;
		};
	}
}

std::function<TaskID()> CombatView::DrawCardsToTable(std::vector<CardModel *> cards)
{
	if(cards.empty())
		return [] () { return SentinelTask; };


	return [this, cards] ()
	{
		for (auto actor : cards)
		{
			m_table.AddCard(actor);
		}
	
		return RearangeTableCards();
	};
}

std::function<TaskID()> CombatView::DiscardCards(std::vector<CardModel *> cards, bool enemy)
{
	if(cards.empty())
		return [] () { return SentinelTask; };

	// TODO : implement discard

	return [] () { return SentinelTask; };
}

std::function<TaskID()> CombatView::CaptureCards(std::vector<CardModel *> cards, bool enemy)
{
	if(cards.empty())
		return [] () { return SentinelTask; };

	auto vp = m_context->GetViewport();
	glm::vec3 targetPosition {3000.0, vp.height / 2, 0.0};

	if (!enemy)
	{
		return [this, cards, targetPosition]()
		{
			TaskID lastTaskID;
			size_t index = 0; // ideally we sort cards by distance to target so we get more of a fan effect
			for(auto actor : cards)
			{
				lastTaskID = GenerateMoveCardTask(actor, targetPosition, glm::vec3{m_cardSize, m_cardSize, 1.0}, 3.0f, easeInOutCirc);
				m_taskQueue.PushCancel(reinterpret_cast<uint64_t>(actor), lastTaskID, 0.5 * index);
				index++;
				
				m_table.RemoveCard(actor); // remove the capture cards
				m_hand.RemoveCard(actor);  // remove the capturing card
			}
			
			return RearangeTableCards();
		};
	}
	else
	{
		return [this, cards, targetPosition]()
		{
			TaskID lastTaskID;
			size_t index = 0; // ideally we sort cards by distance to target so we get more of a fan effect
			for(auto actor : cards)
			{
				lastTaskID = GenerateMoveCardTask(actor, targetPosition, glm::vec3{m_cardSize, m_cardSize, 1.0}, 3.0f, easeInOutCirc);
				m_taskQueue.PushCancel(reinterpret_cast<uint64_t>(actor), lastTaskID, 0.5 * index);
				index++;
				
				m_table.RemoveCard(actor); // remove the capture cards
				m_enemyHand.RemoveCard(actor);  // remove the capturing card
			}
			
			return RearangeTableCards();
		};
	}
}

std::function<TaskID()> CombatView::ExhaustCards(std::vector<CardModel *> cards)
{
	if(cards.empty())
		return [] () { return SentinelTask; };

	// TODO : implement exhaust

	return [] () { return SentinelTask; };
}

std::function<TaskID()> CombatView::PlaceCardOnTable(CardModel * actor, bool enemy)
{
	if(actor == nullptr)
		return [] () { return SentinelTask; };

	if (!enemy)
	{
		return [this, actor]()
		{
			m_hand.RemoveCard(actor);
			m_table.AddCard(actor);
		
			return RearangeTableCards();
		};
	}
	else
	{
		return [this, actor]()
		{
			m_enemyHand.RemoveCard(actor);
			m_table.AddCard(actor);

			RearangeHandCards(true);
		
			return RearangeTableCards();
		};
	}
	
}

std::function<TaskID()> CombatView::UpdatePlayerHealth(int targetHP, int targetMaxHP)
{
	return [this, targetHP, targetMaxHP]()
	{			
		auto oldHealth = m_playerHealthbar->GetHealth();
		
		float startHP    = oldHealth.first;
		float startMaxHP = oldHealth.second;
		
		auto task = m_taskManager.RegisterTask(
			[=, this] (TaskID ID, double progress, double deltaTime) mutable -> TaskResult
			{
				progress *= 1.5;
				
				float curve = easeInOutCirc(std::min(progress, 1.0));
				
				float currentHealth    = glm::mix(startHP ,   static_cast<float>(targetHP)   , curve);
				float currentMaxHealth = glm::mix(startMaxHP, static_cast<float>(targetMaxHP), curve);
				
				m_playerHealthbar->SetHealth(currentHealth, static_cast<float>(targetMaxHP));
				
				if (progress >= 1.0)
				return TaskResult::Return;
				
				return TaskResult::Yield;
			}
		);
		
		m_taskManager.StartTask(task);
		return task;
	};
}

std::function<TaskID()> CombatView::UpdateEnemyHealth(int targetHP, int targetMaxHP)
{
	return [this, targetHP, targetMaxHP]()
	{			
		auto oldHealth = m_enemyHealthbar->GetHealth();
		
		float startHP    = oldHealth.first;
		float startMaxHP = oldHealth.second;
		
		auto task = m_taskManager.RegisterTask(
			[=, this] (TaskID ID, double progress, double deltaTime) mutable -> TaskResult
			{
				progress *= 1.5;
				
				float curve = easeInOutCirc(std::min(progress, 1.0));
				
				float currentHealth    = glm::mix(startHP ,   static_cast<float>(targetHP)   , curve);
				float currentMaxHealth = glm::mix(startMaxHP, static_cast<float>(targetMaxHP), curve);
				
				m_enemyHealthbar->SetHealth(currentHealth, static_cast<float>(targetMaxHP));
				
				if (progress >= 1.0)
				return TaskResult::Return;
				
				return TaskResult::Yield;
			}
		);
		
		m_taskManager.StartTask(task);
		return task;
	};
}

std::function<TaskID()> CombatView::DisplayTurnNumber(int turnCount)
{
	return [this, turnCount]()
	{
		m_turnDisplay->SetText("{S:BOLD}Beginning Turn " + std::to_string(turnCount), 44);

		float x = (m_context->GetViewport().width - m_turnDisplay->GetWidth()) / 2.0f;
		float y = (m_context->GetViewport().height - m_turnDisplay->GetHeight()) / 2.0f;

		glm::vec3 startingPosition {x, 2000.0f, 3.01f};
		glm::vec3 intermediatePosition {x, y, 3.01f};
		glm::vec3 targetPosition {x, -2000.0f, 3.01f};

		m_turnDisplay->SetPosition(startingPosition);

		auto taskID = m_taskManager.RegisterTask([=, this](TaskID task, double progress, double deltaTime) -> TaskResult
		{
			progress *= 0.25;

			if (progress < 0.45)
			{
				m_turnDisplay->IsVisible(true);
				float localProgress = static_cast<float>(progress / 0.45);
				float curve = easeInOutCirc(localProgress);

				glm::vec3 currentPosition =
					glm::mix(startingPosition, intermediatePosition, curve);

				m_turnDisplay->SetPosition(currentPosition);
			}
			else if (progress < 0.55)
			{
				m_turnDisplay->SetPosition(intermediatePosition);
			}
			else if (progress < 1.0)
			{
				float localProgress =
					static_cast<float>((progress - 0.55) / (1.0 - 0.55));

				float curve = easeInOutCirc(localProgress);

				glm::vec3 currentPosition =
					glm::mix(intermediatePosition, targetPosition, curve);

				m_turnDisplay->SetPosition(currentPosition);
			}
			else
			{
				m_turnDisplay->IsVisible(false);
				m_turnDisplay->SetPosition(targetPosition);
				return TaskResult::Return;
			}

			return TaskResult::Yield;
		});

		m_taskManager.StartTask(taskID, 0.2f);

		return taskID;
	};
}

std::function<TaskID()> CombatView::DisplayEnemyTurn()
{
	return [this]()
	{
		m_turnDisplay->SetText("{S:BOLD}Enemy Turn ", 44);

		float x = (m_context->GetViewport().width - m_turnDisplay->GetWidth()) / 2.0f;
		float y = (m_context->GetViewport().height - m_turnDisplay->GetHeight()) / 2.0f;

		glm::vec3 startingPosition {x, 2000.0f, 3.01f};
		glm::vec3 intermediatePosition {x, y, 3.01f};
		glm::vec3 targetPosition {x, -2000.0f, 3.01f};

		m_turnDisplay->SetPosition(startingPosition);

		auto taskID = m_taskManager.RegisterTask([=, this](TaskID task, double progress, double deltaTime) -> TaskResult
		{
			progress *= 0.25;

			if (progress < 0.45)
			{
				m_turnDisplay->IsVisible(true);
				float localProgress = static_cast<float>(progress / 0.45);
				float curve = easeInOutCirc(localProgress);

				glm::vec3 currentPosition =
					glm::mix(startingPosition, intermediatePosition, curve);

				m_turnDisplay->SetPosition(currentPosition);
			}
			else if (progress < 0.55)
			{
				m_turnDisplay->SetPosition(intermediatePosition);
			}
			else if (progress < 1.0)
			{
				float localProgress =
					static_cast<float>((progress - 0.55) / (1.0 - 0.55));

				float curve = easeInOutCirc(localProgress);

				glm::vec3 currentPosition =
					glm::mix(intermediatePosition, targetPosition, curve);

				m_turnDisplay->SetPosition(currentPosition);
			}
			else
			{
				m_turnDisplay->IsVisible(false);
				m_turnDisplay->SetPosition(targetPosition);
				return TaskResult::Return;
			}

			return TaskResult::Yield;
		});

		m_taskManager.StartTask(taskID, 0.2f);

		return taskID;
	};
}

std::function<TaskID()> CombatView::EnableUserInput()
{
	return [this]()
	{
		m_canInput = true;
		return SentinelTask;
	};
}

std::function<TaskID()> CombatView::DisableUserInput()
{
	return [this]()
	{
		m_canInput = false;
		return SentinelTask;
	};
}

std::function<TaskID()> CombatView::ResolveCard(CardModel * actor, bool enemy)
{
	return [this, actor, enemy]()
	{
		if (enemy) m_enemyHand.RemoveCard(actor);
		else m_hand.RemoveCard(actor);

		auto vp = m_context->GetViewport();
		glm::vec3 targetPosition {(vp.width - m_cardSize) / 2.0, (vp.height - m_cardSize) / 2.0f, 1.999f};
		glm::vec3 targetScale {m_cardSize * 1.5f, m_cardSize * 1.5f, 1.0};

		glm::vec3 startingPosition = actor->GetPosition();
		glm::vec3 startingScale    = actor->GetScale();

		if (enemy)
		{
			targetPosition.y += m_cardSize;
		}

		if (enemy)
		{
			RearangeHandCards(true);
		}

		auto task = m_taskManager.RegisterTask(
			[this, startingPosition, startingScale, targetPosition, targetScale, actor] (TaskID task, double progress, double deltaTime) -> TaskResult
			{
				progress *= 0.75;

				if (progress < 0.45)
				{
					float localProgress = static_cast<float>(progress / 0.45);
					float curve = easeInOutCirc(localProgress);

					glm::vec3 currentPosition = glm::mix(startingPosition, targetPosition, curve);
					glm::vec3 currentScale = glm::mix(startingScale, targetScale, curve);
					actor->SetPosition(currentPosition);
					actor->SetScale(currentScale);
				}
				else if (progress < 1.0)
				{
					actor->SetPosition(targetPosition);
					actor->SetScale(targetScale);
				}
				else
				{
					return TaskResult::Return;
				}

				return TaskResult::Yield;
			}
		);

		m_taskManager.StartTask(task); //.PushCancel(reinterpret_cast<uint64_t>(actor)
		return task;
	};
}

// Private =>

std::vector<CardModel *> CombatView::GetHoveredCards()
{
	auto height = m_context->GetViewport().height;
	
	double mouseX, mouseY;
	InputManager::getInstance().getMousePos(mouseX, mouseY);

	std::vector<CardModel *> hovered;
	for(auto & actor : m_cards)
	{
		if (actor->GetPosition().z < -10.0 || actor->GetPosition().z > 10.0)
			continue;

		glm::mat4 model = glm::scale(actor->GetTransformMatrix(), glm::vec3 {0.65, 1.0, 1.0});
		glm::mat4 invModel = glm::inverse(model);
		glm::vec4 localMouse = invModel * glm::vec4(mouseX, height - mouseY, 0.0f, 1.0f);
		
		if(localMouse.x >= -1.0f && localMouse.x <= 1.0f && 
			localMouse.y >= -1.0f && localMouse.y <= 1.0f)
		{
			hovered.emplace_back(actor.get());
		}
	}
	
	return hovered;
}

CardModel * CombatView::GetHoveredCard()
{
	auto height = m_context->GetViewport().height;

	double mouseX, mouseY;
	InputManager::getInstance().getMousePos(mouseX, mouseY);

	CardModel* bestCard = nullptr;
	float bestZ = -std::numeric_limits<float>::infinity();

	for (auto& actor : m_cards)
	{
		float z = actor->GetPosition().z;

		if (z < -10.0f || z > 10.0f)
			continue;

		glm::mat4 model = glm::scale(actor->GetTransformMatrix(), glm::vec3{0.65f, 1.0f, 1.0f});
		glm::mat4 invModel = glm::inverse(model);

		glm::vec4 localMouse =
			invModel * glm::vec4(mouseX, height - mouseY, 0.0f, 1.0f);

		if (localMouse.x >= -1.0f && localMouse.x <= 1.0f &&
			localMouse.y >= -1.0f && localMouse.y <= 1.0f)
		{
			if (z > bestZ)
			{
				bestZ = z;
				bestCard = actor.get();
			}
		}
	}

	return bestCard;
}

TaskID CombatView::GenerateMoveCardTask(CardModel * actor, glm::vec3 targetPosition, glm::vec3 targetScale, float speed, float(*easingFunction)(float))
{
	glm::vec3 startingPosition = actor->GetPosition();
	glm::vec3 startingScale    = actor->GetScale();

	return m_taskManager.RegisterTask([
		this,
		actor,
    	startingPosition,
    	startingScale,
    	targetPosition,
    	targetScale,
    	speed,
    	easingFunction
	](TaskID ID, double progress, double deltaTime) -> TaskResult
	{
		progress *= speed;

		float curve = easingFunction(std::min(progress, 1.0));
		glm::vec3 currentPosition = glm::mix(startingPosition, targetPosition, curve);
		glm::vec3 currentScale = glm::mix(startingScale, targetScale, curve);
		actor->SetPosition(currentPosition);
		actor->SetScale(currentScale);

		if(progress >= 1.0)
		{
			actor->SetPosition(targetPosition);
			actor->SetScale(targetScale);
			return TaskResult::Return;
		}

		return TaskResult::Yield;
	});
}


/*
std::string description =
	"{S:ITALIC, C:RED   }If "
	"{S:BOLD  , C:ORANGE}you "
	"{S:NORMAL, C:YELLOW}see "
	"{S:BOLD  , C:GREEN }this, "
	"{S:ITALIC, C:BLUE  }there "
	"{S:NORMAL, C:RED   }is "
	"{S:ITALIC, C:BLUE  }a "
	"{S:NORMAL, C:YELLOW}bug.";

	std::string name = "{C:BLUE, S:ITALIC}Placeholder";
*/

void CombatView::StartHoverCard(CardModel * actor)
{
	if (m_hand.GetIndex(actor) == static_cast<size_t>(-1))
		return;

	m_hand.SetHover(actor);
	
	for(auto & [actor, index] : m_hand)
	{
		auto task = GenerateMoveCardTask(actor, m_hand.GetCardPos(actor), m_hand.GetCardSize(actor), 5.0f, easeInOutCirc);
		m_taskQueue.PushCancel(reinterpret_cast<uint64_t>(actor), task);
	}
}

void CombatView::UpdateHover(CardModel * actor, HoverType hoverType, double delta)
{
	// ugly as fuck but hey
	if (hoverType == HoverType::Short)
	{

	}
	else if (hoverType == HoverType::Medium)
	{
		actor->OnHoverStart();
	}
	else if (hoverType == HoverType::Long)
	{

	}
}

void CombatView::StopHoverCard(CardModel * actor)
{
	m_hand.SetHover(nullptr);
	actor->OnHoverStop();
	
	for(auto & [actor, index] : m_hand)
	{
		auto task = GenerateMoveCardTask(actor, m_hand.GetCardPos(actor), m_hand.GetCardSize(actor), 5.0f, easeInOutCirc);
		m_taskQueue.PushCancel(reinterpret_cast<uint64_t>(actor), task);
	}
}

void CombatView::StartDraggingCard(CardModel * actor)
{
	if (m_hand.GetIndex(actor) == static_cast<size_t>(-1))
		return;

	m_hand.SetDrag(m_cardDraggedCurrent);
	
	for(auto & [actorB, index] : m_hand)
	{
		if(actorB == actor) continue;

		auto task = GenerateMoveCardTask(actorB, m_hand.GetCardPos(actorB), m_hand.GetCardSize(actorB), 2.0f, easeInOutCirc);
		m_taskQueue.PushCancel(reinterpret_cast<uint64_t>(actorB), task);
	}
}

void CombatView::DragCard(CardModel * actor, glm::vec2 mouse, double deltaTime)
{
	glm::vec2 cardPos = actor->GetPosition();
	glm::vec2 mousePos {mouse.x +(m_cardSize / 2), mouse.y +(m_cardSize / 2)};

	float deltaX = std::abs(cardPos.x - mousePos.x);
	float deltaY = std::abs(cardPos.y - mousePos.y);
	float distance = std::sqrt(deltaX * deltaX + deltaY * deltaY);
	
	float const radius = 0.001f;
	float const speed  = 2.0f;

	if (distance > radius)
	{
		float const trackingSpeed = 12.0f; 
		float factor = 1.0f - std::exp(-trackingSpeed * static_cast<float>(deltaTime));
		
		glm::vec3 targetPosition{mousePos.x, mousePos.y, 1.0f};
		glm::vec3 currentPosition = glm::mix(actor->GetPosition(), targetPosition, factor);
		
		actor->SetPosition(currentPosition);
	}
	else
	{
		actor->SetPosition({mousePos.x, mousePos.y, 1.0f});
	}	
}

void CombatView::DropCard(CardModel * actor, bool inPlayArea)
{
	m_hand.SetDrag(nullptr);

	if (!inPlayArea)
	{
		for(auto & [actor, index] : m_hand)
		{
			auto task = GenerateMoveCardTask(actor, m_hand.GetCardPos(actor), m_hand.GetCardSize(actor), 3.0f, easeInOutCirc);
			m_taskQueue.PushCancel(reinterpret_cast<uint64_t>(actor), task);
		}
	}
	else
	{
		m_subscriber->OnCardDropInPlayArea(actor);
	}
}

TaskID CombatView::RearangeTableCards()
{
	TaskID task;
	for (auto & [actor, index] : m_table)
	{
		task = GenerateMoveCardTask(actor, m_table.GetPos(actor), m_table.GetScale(), 1.0, easeInOutCirc);
		m_taskQueue.PushCancel(reinterpret_cast<uint64_t>(actor), task);
	}

	return task;
}

TaskID CombatView::RearangeHandCards(bool enemy)
{
	TaskID task;

	if (!enemy)
	{
		for (auto [actor, index] : m_hand)
		{
			task = GenerateMoveCardTask(actor, m_hand.GetCardPos(actor), m_hand.GetCardSize(actor), 5.0, easeInOutCirc);
			m_taskQueue.PushCancel(reinterpret_cast<uint64_t>(actor), task);
		}
	}
	else
	{
		for (auto [actor, index] : m_enemyHand)
		{
			task = GenerateMoveCardTask(actor, m_enemyHand.GetCardPos(actor), m_enemyHand.GetCardSize(actor), 5.0, easeInOutCirc);
			m_taskQueue.PushCancel(reinterpret_cast<uint64_t>(actor), task);
		}
	}

	return task;
}