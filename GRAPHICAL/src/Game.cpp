#include <Game.hpp>

#include <AssetLoader.hpp>
#include <AssetManager.hpp>

using namespace TLOT;


Game::Game(
		TLOT::RenderContext * context,
		TLOT::Renderer * renderer,
		TLOT::SceneInspector * inspector,
		std::shared_ptr<TLOT::VertexTemplate> vertexTemplate, 
		FT_Library library
	)
	: m_context{context}
	, m_renderer{renderer}
	, m_inspector{inspector}
	, m_vertexTemplate{vertexTemplate}
	, m_combat{m_context, renderer, m_inspector}
	, m_ftlibrary{library}
{
	Setup();

	InitPlayerDeck();
	InitEnemyDeck();

	auto param = GenerateCombatParameters();

	m_combat.Begin(param);
}

void Game::Update(double deltaTime)
{
	m_combat.Update();
}

void Game::Render()
{
	m_combat.Render();
}

void Game::InitPlayerDeck()
{
	m_playerDeck.BuildScopaDeck();
}

void Game::InitEnemyDeck()
{
	m_enemyDeck.BuildScopaDeck(false);
}

CombatParams Game::GenerateCombatParameters()
{
	CombatParams combatParams;
	combatParams.playerHP    = m_playerHP;
	combatParams.playerMaxHP = m_playerMaxHP;
	combatParams.difficulty  = Difficulty::Easy;

	for (auto & card : m_playerDeck.GetCards())
	{
		combatParams.playerCards.emplace_back(card.get());
	}

	for (auto & card : m_enemyDeck.GetCards())
	{
		combatParams.enemyCards.push_back(card.get());
	}

	combatParams.playerItems = {};

	return combatParams;
}

















void Game::Setup()
{
	SetupGeometry();
	SetupTextures();
	SetupShaders();
	SetupMaterials();
	SetupTechniques();
	SetupFonts();

	m_renderer->Sync();
}

void Game::SetupGeometry()
{
	std::vector<float> const cardVertices = {
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f,   0.0f, 1.0f
	};
	
	std::vector<uint32_t> const cardIndices = {0, 1, 2, 3, 4, 5};

	AssetManager::Cache("geometry_card",
		AssetManager::CreateGeometry(
			cardVertices, cardIndices, m_vertexTemplate
		)
	);

	std::vector<float> const glyphVertices = {
		 0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
		 1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
		 1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
		 0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
		 1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
		 0.0f, 1.0f, 0.0f,   0.0f, 1.0f
	};
	
	std::vector<uint32_t> const glyphIndices = {0, 1, 2, 3, 4, 5};

	AssetManager::Cache("geometry_quad",
		AssetManager::CreateGeometry(
			glyphVertices, glyphIndices, m_vertexTemplate
		)
	);
}

void Game::SetupTextures()
{
	// scene
	LoadTexture ("texture_table", "data/assets/textures/table.png");

	// enemies
	LoadTexture ("texture_enemy_badalisc", "data/assets/textures/badalisc_placeholder.png");

	// cards
	LoadTexture ("texture_card_back", "data/assets/textures/card_back_normal.png");
	for (auto const & suit : { Suit::Bastoni, Suit::Coppe, Suit::Denari, Suit::Spada })
	{
		for (auto const & value : { 
			CardValue::Asso,    CardValue::Due,     CardValue::Tre,
			CardValue::Quattro, CardValue::Cinque,  CardValue::Sei,
			CardValue::Sette,   CardValue::Cavallo, CardValue::Fante,
			CardValue::Re
		})
		{
			std::string key = Card::GetKey (suit, value);
			LoadTexture ("texture_card_" + key, "data/assets/textures/card_" + key + ".png");
		}
	}

	//ui
	LoadTexture("texture_healthbar_full",  "data/assets/textures/ui_health_bar_health_full.png");
	LoadTexture("texture_healthbar_empty", "data/assets/textures/ui_health_bar_health_empty.png");
	LoadTexture("texture_poisonbar_full",  "data/assets/textures/ui_health_bar_poison_full.png");
	LoadTexture("texture_poisonbar_empty", "data/assets/textures/ui_health_bar_poison_empty.png");

	LoadTexture("texture_box_corner", "data/assets/textures/ui_box_corner.png");
	LoadTexture("texture_box_side"  , "data/assets/textures/ui_box_side.png");
	LoadTexture("texture_box_center", "data/assets/textures/ui_box_center.png");

	LoadTexture("texture_status_poison", "data/assets/textures/status_poison.png");
	LoadTexture("texture_status_burn", "data/assets/textures/status_burn.png");
	LoadTexture("texture_status_bleed", "data/assets/textures/status_bleed.png");

	
}

void Game::SetupShaders()
{
	LoadShaderSource ("shader_source_vertex_common"     , "data/assets/shaders/common.vertex");
	LoadShaderSource ("shader_source_fragment_card"     , "data/assets/shaders/card.fragment");
	LoadShaderSource ("shader_source_fragment_healthbar", "data/assets/shaders/healthBar.fragment");
	LoadShaderSource ("shader_source_fragment_glyph"    , "data/assets/shaders/glyph.fragment");
	LoadShaderSource ("shader_source_fragment_box9patch", "data/assets/shaders/box9patch.fragment");
	LoadShaderSource ("shader_source_fragment_enemy"    , "data/assets/shaders/enemy.fragment");
	LoadShaderSource ("shader_source_fragment_icon"     , "data/assets/shaders/icon.fragment");
}

void Game::SetupMaterials()
{
	AssetManager::Cache("material_healthbar",
		AssetManager::CreateMaterialTemplate({
			{"health", Float},
			{"poison", Float},
			{"fullHealthBar" , TexQ},
			{"emptyHealthBar", TexQ},
			{"fullPoisonBar" , TexQ},
			{"emptyPoisonBar", TexQ},
		})
	);

	AssetManager::Cache("material_card",
		AssetManager::CreateMaterialTemplate({
			{"symbol" ,   TexQ},
			{"backgroud", TexQ},
			{"back" ,     TexQ}
		})
	);

	AssetManager::Cache("material_glyph",
		AssetManager::CreateMaterialTemplate({
			{"glyph" ,TexQ},
			{"color", Vec4}
		})
	);

	AssetManager::Cache("material_box9patch",
		AssetManager::CreateMaterialTemplate({
			{"containerSize" , Vec2},
			{"borderSize"    , Float},
			{"corner"    , TexQ},
			{"side"      , TexQ},
			{"center"    , TexQ},
			{"color", Vec3}
		})
	);

	AssetManager::Cache("material_enemy",
		AssetManager::CreateMaterialTemplate({
			{"texture", TexQ},
		})
	);

	AssetManager::Cache("material_icon",
		AssetManager::CreateMaterialTemplate({
			{"texture", TexQ}
		})
	);
}

void Game::SetupTechniques()
{
	ID64_t cardProgram = m_renderer->CreateProgram(
		AssetManager::Cache("shader_source_vertex_common"),
		AssetManager::Cache("shader_source_fragment_card")
	);

	ID64_t healthbarProgram = m_renderer->CreateProgram(
		AssetManager::Cache("shader_source_vertex_common"),
		AssetManager::Cache("shader_source_fragment_healthbar")
	);

	// will probably have a dedicated text renderer in renderer class
	ID64_t glyphProgram = m_renderer->CreateProgram(
		AssetManager::Cache("shader_source_vertex_common"),
		AssetManager::Cache("shader_source_fragment_glyph")
	);

	ID64_t box9patchProgram = m_renderer->CreateProgram(
		AssetManager::Cache("shader_source_vertex_common"),
		AssetManager::Cache("shader_source_fragment_box9patch")
	);

	ID64_t enemyProgram = m_renderer->CreateProgram(
		AssetManager::Cache("shader_source_vertex_common"),
		AssetManager::Cache("shader_source_fragment_enemy")
	);

	ID64_t iconProgram = m_renderer->CreateProgram(
		AssetManager::Cache("shader_source_vertex_common"),
		AssetManager::Cache("shader_source_fragment_icon")
	);

	Technique cardTechnique;
	cardTechnique.mode     = ProjectionMode::Orthographic;
	cardTechnique.program  = cardProgram;
	cardTechnique.material = AssetManager::Cache("material_card");
	m_renderer->RegisterTechnique(cardTechnique, 50.0f);

	Technique healthBarTechnique;
	healthBarTechnique.mode     = ProjectionMode::Orthographic;
	healthBarTechnique.program  = healthbarProgram;
	healthBarTechnique.material = AssetManager::Cache("material_healthbar");
	m_renderer->RegisterTechnique(healthBarTechnique, 99.0f);

	Technique glyphTechnique;
	glyphTechnique.mode     = ProjectionMode::Orthographic;
	glyphTechnique.program  = glyphProgram;
	glyphTechnique.material = AssetManager::Cache("material_glyph");
	glyphTechnique.useFontAtlas = true;
	m_renderer->RegisterTechnique(glyphTechnique, 100.0f);

	Technique tooltipTechnique;
	tooltipTechnique.mode     = ProjectionMode::Orthographic;
	tooltipTechnique.program  = box9patchProgram;
	tooltipTechnique.material = AssetManager::Cache("material_box9patch");
	m_renderer->RegisterTechnique(tooltipTechnique, 98.0f);

	Technique enemyTechnique;
	enemyTechnique.mode     = ProjectionMode::Orthographic;
	enemyTechnique.program  = enemyProgram;
	enemyTechnique.material = AssetManager::Cache("material_enemy");
	m_renderer->RegisterTechnique(enemyTechnique, 25.0f);

	Technique iconTechnique;
	iconTechnique.mode     = ProjectionMode::Orthographic;
	iconTechnique.program  = iconProgram;
	iconTechnique.material = AssetManager::Cache("material_icon");
	m_renderer->RegisterTechnique(iconTechnique, 99.0f);
}

void Game::SetupFonts()
{
	AssetManager::Cache("font_hh_light", AssetManager::LoadFont("data/assets/fonts/cooper-hewitt-fixed-for-windows-master/CooperHewitt-Light.ttf", m_ftlibrary));
	AssetManager::Cache("font_hh_bold",  AssetManager::LoadFont("data/assets/fonts/cooper-hewitt-fixed-for-windows-master/CooperHewitt-Bold.ttf", m_ftlibrary));
	AssetManager::Cache("font_hh_it",    AssetManager::LoadFont("data/assets/fonts/cooper-hewitt-fixed-for-windows-master/CooperHewitt-BoldItalic.ttf", m_ftlibrary));

	m_renderer->RegisterFont(AssetManager::Cache("font_hh_light"));
	m_renderer->RegisterFont(AssetManager::Cache("font_hh_bold"));
	m_renderer->RegisterFont(AssetManager::Cache("font_hh_it"));
}
