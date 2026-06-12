
#include <RenderableObjects/CardModel.hpp>

#include <Combat/CombatPresenter.hpp>

using namespace TLOT;

CombatPresenter::CombatPresenter(TLOT::RenderContext * context, TLOT::Renderer * renderer, SceneInspector * inspector)
{
	m_model = std::make_unique<CombatModel> (this);
	m_view  = std::make_unique<CombatView>  (this, context, renderer, m_taskManager, inspector);
}

void CombatPresenter::Init()
{

}

void CombatPresenter::Update()
{
	if (!m_pause && (m_blockingAnimation == SentinelTask || !m_taskManager.IsTaskAlive(m_blockingAnimation)))
	{
		while (!m_animationStack.empty())
		{
			auto animation = m_animationStack.at(0);
			m_blockingAnimation = animation();
			m_animationStack.erase(m_animationStack.begin());

			break;
		}
	}

	m_view->Update();
}

void CombatPresenter::Render()
{
	m_view->Render();
}

void CombatPresenter::Begin(CombatParams params)
{	
	for (auto card : params.playerCards)
	{
		m_model->RegisterPlayerCard(card);
		auto actor = m_view->RegisterCard(card);

		m_cardToActor[card]  = actor;
		m_actorToCard[actor] = card;
	}

	for (auto card : params.enemyCards)
	{
		m_model->RegisterEnemyCard(card);
		auto actor = m_view->RegisterCard(card);

		m_cardToActor[card]  = actor;
		m_actorToCard[actor] = card;
	}
	
	m_view->Init();
	m_model->Init(params.difficulty);
}

void CombatPresenter::OnCardDropInPlayArea(CardModel * actor)
{
	auto card = Convert(actor);
	if (card) m_model->PlayCard(card);
}

void CombatPresenter::DebugDrawCard()
{
	m_model->DrawUntilHandFull();
}

void CombatPresenter::Pause()
{
	m_pause = !m_pause;
}

void CombatPresenter::OnMessage(std::string const message)
{
	Logger::log(LogLevel::Info, "[CombatModel]: {}", message);
}

void CombatPresenter::OnCardsDrawnToHand(std::vector<Card *> cards, bool enemy)
{
	if (!enemy)
	{
		m_animationStack.push_back(m_view->DisableUserInput());
		m_animationStack.push_back(m_view->DrawCardsToHand(Convert(cards), false));
		m_animationStack.push_back(m_view->EnableUserInput());
	}
	else
	{
		m_animationStack.push_back(m_view->DrawCardsToHand(Convert(cards), true));
	}
}

void CombatPresenter::OnCardsDrawnToTable(std::vector<Card *> cards)
{
	m_animationStack.push_back(m_view->DrawCardsToTable(Convert(cards)));
}

void CombatPresenter::OnCardsDiscarded(std::vector<Card *> cards, bool enemy)
{
	m_animationStack.push_back(m_view->DiscardCards(Convert(cards), enemy));
}

void CombatPresenter::OnCardsCaptured(std::vector<Card *> cards, bool enemy)
{
	m_animationStack.push_back(m_view->CaptureCards(Convert(cards), enemy));
}

void CombatPresenter::OnCardUpdate(Card * card)
{
	auto actor = Convert(card);
	if (actor) m_view->UpdateCard(actor, card);
}

void CombatPresenter::OnCardResolving(Card * card, bool enemy)
{
	auto actor = Convert(card);
	if (actor) m_animationStack.push_back(m_view->ResolveCard(actor, enemy));
}

void CombatPresenter::OnCardPlacedOnTable(Card * card, bool enemy)
{
	auto actor = Convert(card);
	if (actor) m_animationStack.push_back(m_view->PlaceCardOnTable(actor, enemy));
}

void CombatPresenter::OnPlayerBeginTurn(int turnCount)
{
	m_animationStack.push_back(m_view->DisplayTurnNumber(turnCount));
	m_animationStack.push_back(m_view->EnableUserInput());
}

void CombatPresenter::OnPlayerHealthChange(int newHP, int newMaxHP)
{
	m_animationStack.push_back(m_view->UpdatePlayerHealth(newHP, newMaxHP));
}

void CombatPresenter::OnPlayerDeath()
{

}

void CombatPresenter::OnPlayerEndTurn()
{
	m_animationStack.push_back(m_view->DisableUserInput());
	m_model->BeginEnemyTurn();
}

void CombatPresenter::OnEnemyBeginTurn()
{
	m_animationStack.push_back(m_view->DisplayEnemyTurn());
}

void CombatPresenter::OnEnemyHealthChange(int newHP, int newMaxHP)
{
	m_animationStack.push_back(m_view->UpdateEnemyHealth(newHP, newMaxHP));
}

void CombatPresenter::OnEnemyDeath()
{

}

void CombatPresenter::OnEnemyEndTurn()
{
	m_model->BeginPlayerTurn();
}

Card * CombatPresenter::Convert(CardModel * actor)
{
	auto it = m_actorToCard.find(actor);
	if (it == m_actorToCard.end())
		return nullptr;

	return it->second;
}

CardModel * CombatPresenter::Convert(Card * card)
{
	auto it = m_cardToActor.find(card);
	if (it == m_cardToActor.end())
		return nullptr;

	return it->second;
}

std::vector<Card *> CombatPresenter::Convert(std::vector<CardModel *> actors)
{
	std::vector<Card *> converted;
	for (auto actor : actors)
	{
		auto card = Convert(actor);
		if (card) converted.emplace_back(card);
	}

	return converted;
}

std::vector<CardModel *> CombatPresenter::Convert(std::vector<Card *> cards)
{
	std::vector<CardModel *> converted;
	for (auto card : cards)
	{
		auto actor = Convert(card);
		if (actor) converted.emplace_back(actor);
	}

	return converted;
}
