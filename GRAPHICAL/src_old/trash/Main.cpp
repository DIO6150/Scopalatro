
#include <memory>

#include <RenderContext.hpp>
#include <AssetManager.hpp>

#include <Renderer/InstanceRenderer/InstanceRenderer.hpp>
#include <Renderer/Renderables/ModelFactory.hpp>

#include <Debugger/DebugImGuI.hpp>
#include <Debugger/SceneEditor.hpp>
#include <Debugger/DebugRenderer.hpp>

#include <AssetLoader.hpp>

#include <Card.hpp>

#include <Combat/CombatPresenter.hpp>

#include <EnemyName.hpp>

using namespace TLOT;

int main (void)
{
	size_t width = 1280;
	size_t height = 800;

	RenderContext ctx;
	ctx.Init (width, height, "TETO ENGINE NEW TEST");

	LoadAll ();
	
	InstanceRenderer sceneRenderer { width, height, ProjectionMode::Perspective };
	InstanceRenderer uiRenderer  { width, height, ProjectionMode::Orthogonal };
	Camera camera;
	

	DebugRenderer::Get ().Init (width, height);

	DebugRenderer::Get ().SetActive (true);



	// Game Setup

	std::vector<std::unique_ptr<Card>> deck;

	for (auto const & suit : { Suit::Bastoni, Suit::Coppe, Suit::Denari, Suit::Spada })
	{
		for (auto const & value : { 
			CardValue::Asso,    CardValue::Due,     CardValue::Tre,
			CardValue::Quattro, CardValue::Cinque,  CardValue::Sei,
			CardValue::Sette,   CardValue::Cavallo, CardValue::Fante,
			CardValue::Re
		})
		{
			deck.push_back (std::move (std::make_unique<Card> (Card::GetKey (suit, value), "BasicCard", suit, value)));
		}
	}


	auto badalisc = Enemy {BADALISC, 0};

	CombatParams combatParams;
	combatParams.enemies = {&badalisc};
	combatParams.playerHP = 50.0;
	combatParams.playerMaxHP = 50.0;
	combatParams.playerItems = 0; // TODO: placeholder

	// deck creation
	for (auto const & card : deck)
	{
		combatParams.playerCards.emplace (card.get ());
	}

	CombatPresenter combat {ctx, camera, sceneRenderer, uiRenderer, combatParams};
	CombatView * view = combat.GetView ();


	// Game Rendering & debug stuff

	SetupImGuI (ctx.window);

	SceneEditor editor;
	editor.RegisterRenderer (uiRenderer);
	
	while (!ctx.ShouldExit ())
	{
		ctx.Update ();
		view->Update (ctx, camera);


		if (!ctx.ShouldRenderFrame ()) continue;
		glClear  (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		StartFrameImGuI ();
		sceneRenderer.Render (AssetManager::Cache ("shader_program_default"), camera);
		uiRenderer.Render (AssetManager::Cache ("shader_program_card"), camera);

		DebugRenderer::Get ().Render ();
		
		editor.OpenMenu ();

		EndFrameImGuI ();

		ctx.Render ();
	}

	ShutdownImGuI ();
}