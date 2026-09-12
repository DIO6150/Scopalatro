#pragma once

#include <map>
#include <string>

#include <Core/ResourceHandle.hpp>
#include <Renderer/Renderer.hpp>
#include <RenderContext.hpp>
#include <RenderableObject.hpp>
#include <Debugger/SceneInspector.hpp>
#include <Resources/Font.hpp>

#include <Combat/CombatPresenter.hpp>
#include <Deck.hpp>

class Game
{
public:

	Game(
		TLOT::RenderContext * context,
		TLOT::Renderer * renderer,
		TLOT::SceneInspector * inspector,
		std::shared_ptr<TLOT::VertexTemplate> vertexTemplate, 
		FT_Library library);

	void Setup();

	void Update(double deltaTime);
	void Render();

private:
	void InitPlayerDeck();
	void InitEnemyDeck();
	CombatParams GenerateCombatParameters();


	void SetupGeometry();
	void SetupTextures();
	void SetupShaders();
	void SetupMaterials();
	void SetupTechniques();
	void SetupFonts();


	TLOT::RenderContext * m_context;
	TLOT::Renderer * m_renderer;
	TLOT::SceneInspector * m_inspector;
	FT_Library m_ftlibrary;

	std::shared_ptr<TLOT::VertexTemplate> m_vertexTemplate;


	//std::vector<TLOT::IObject *> m_objects;

	// Player Persistent Data
	float m_playerHP;
	float m_playerMaxHP;
	Deck m_playerDeck;
	Deck m_enemyDeck;



	CombatPresenter m_combat;
};
