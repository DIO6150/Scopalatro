
//#define TEST_RENDERING
//#define TEST_SCOPA

#define SCOPALATRO


#ifdef SCOPALATRO



#include <opengl/RenderContext.hpp>

#include <modules/camera.hpp>
#include <modules/DebugRenderer.hpp>
#include <modules/RenderableManager.hpp>

#include <Combat/CombatPresenter.hpp>

#include <core/InputManager.hpp>

#include <EnemyName.hpp>

using namespace std;
using namespace TLOT;

#elif TEST_RENDERING

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <modules/AssetManager.hpp>

#include <opengl/RenderContext.hpp>

#include <modules/camera.hpp>
#include <modules/Renderer.hpp>
#include <modules/Renderable.hpp>

#include <renderables/CardSprite.hpp>

#include <cstdlib>

using namespace std;
using namespace TLOT;

#elif TEST_SCOPA

#include "Game.hpp"

#endif


int main ()
{
	#ifdef SCOPALATRO

	// Initialization
	RenderContext rctx;

	size_t width = 1280;
	size_t height = 800;
	rctx.Init (width, height, "Combat Test", false);

	AssetManager assetManager;
	Renderer sceneRenderer {width, height, ProjectionMode::Perspective};
	Renderer uiRenderer    {width, height, ProjectionMode::Orthogonal};
	DebugRenderer::Get().Init (width, height);


	// Resources
	RenderableManager sceneManager {assetManager, sceneRenderer};
	RenderableManager uiManager    {assetManager, uiRenderer};
	

	// ui
	assetManager.LoadTexture ("ui_button_corner_top_left",     "data/assets/textures/button_top_left_corner.png");
	assetManager.LoadTexture ("ui_button_corner_top_right",    "data/assets/textures/button_top_right_corner.png");
	assetManager.LoadTexture ("ui_button_corner_bottom_left",  "data/assets/textures/button_bottom_left_corner.png");
	assetManager.LoadTexture ("ui_button_corner_bottom_right", "data/assets/textures/button_bottom_right_corner.png");
	assetManager.LoadTexture ("ui_button_center",              "data/assets/textures/button_center.png");

	// scene
	assetManager.LoadTexture ("scene_table", "data/assets/textures/table.png");

	// enemies
	assetManager.LoadTexture ("enemy_badalisc_idle_0", "data/assets/textures/badalisc_placeholder.png");

	// cards
	assetManager.LoadTexture ("card_back", "data/assets/textures/card_back_normal.png");
	// { Suit::Bastoni, Suit::Coppe, Suit::Denari, Suit::Spada }
	// { Suit::Denari }
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
			assetManager.LoadTexture (key, "data/assets/textures/card_" + key + ".png");
		}
	}
	ResourceHandle shaderHandle = assetManager.LoadShader ("default", "data/assets/shaders/default.vertex", "data/assets/shaders/default.fragment");
	Shader const & shader = assetManager.GetShader (shaderHandle); // def change that asap, thats ugly as hell

	ResourceHandle uiShaderHandle = assetManager.LoadShader ("ui", "data/assets/shaders/ui.vertex", "data/assets/shaders/default.fragment");
	Shader const & uiShader = assetManager.GetShader (uiShaderHandle); // def change that asap, thats ugly as hell
	

	// Card Creation

	Camera camera;

	std::unique_ptr<Card> dummy = std::make_unique<Card> ("Dummy Card", "If you see this, there is a bug.");
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
			std::string name = "[" + suitToString (suit) + " - " + valueToString (value) +  "]"; 
			deck.push_back (std::move (std::make_unique<Card> (name, "Basic Card", suit, value)));
		}
	}

	// Combat Initialization

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

	CombatPresenter combat {rctx, camera, sceneManager, uiManager, combatParams};
	CombatView * view = combat.GetView ();

	camera.rotation.x = 90.0;
	camera.rotation.y = 0.0;

	combat.Begin ();


	// game loop stuff
	static float const SENSITIVITY = 0.25f;

	bool DEBUG_MODE = false;
	bool _3D_MODE = false;
	rctx.HideMouse (false);

	double ready = 2.0;

	// logic setup
	rctx.SetUpdateLoop ([&](RenderContext::Context const & ctx) {
		if (ready < 0.0)
		{
			view->Update (ctx, camera);
		}

		ready -= ctx.deltaTime;

		double deltaX = ctx.mouseX - ctx.lastMouseX;
		double deltaY = ctx.mouseY - ctx.lastMouseY;

		if (InputManager::getInstance ().isKeyPressed (GLFW_KEY_O)) {
			DEBUG_MODE = !DEBUG_MODE;
			DebugRenderer::Get ().SetActive (DEBUG_MODE);
			//ctx.HideMouse (DEBUG_MODE);
			//deltaX = 0.0;
			//deltaY = 0.0;
		}

		if (InputManager::getInstance ().isKeyPressed (GLFW_KEY_M)) {
			_3D_MODE = !_3D_MODE;
			ctx.HideMouse (_3D_MODE);
			deltaX = 0.0;
			deltaY = 0.0;
		}

		if (InputManager::getInstance ().isKeyPressed (GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose (ctx.window, true);
		}

		if (!_3D_MODE) return;
		camera.rotate ({deltaX * SENSITIVITY, deltaY * -SENSITIVITY, 0.0});
		if (glfwGetKey (ctx.window, GLFW_KEY_W) == GLFW_PRESS) {
			camera.move_forward (ctx.deltaTime);
		}
		if (glfwGetKey (ctx.window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.move_backward (ctx.deltaTime);
		}
		if (glfwGetKey (ctx.window, GLFW_KEY_A) == GLFW_PRESS) {
			camera.move_left (ctx.deltaTime);
		}
		if (glfwGetKey (ctx.window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.move_right (ctx.deltaTime);
		}
		if (glfwGetKey (ctx.window, GLFW_KEY_E) == GLFW_PRESS) {
			camera.move_up (ctx.deltaTime);
		}
		if (glfwGetKey (ctx.window, GLFW_KEY_Q) == GLFW_PRESS) {
			camera.move_down (ctx.deltaTime);
		}
		if (glfwGetKey (ctx.window, GLFW_KEY_P) == GLFW_PRESS) {
			camera.rotation.x = 90.0;
			camera.rotation.y = 10.0;
		}
	});


	rctx.SetRenderLoop ([&] (TLOT::RenderContext::Context const & ctx) {
		// TODO: a renderable should have a shader to pass to the renderer
		// TODO: a shader should not be an asset
		// The assetManager should load "ShaderSource"
		// and the renderer should create the "Shader"
		// by combining multiple "ShaderSource"s
		glClear  (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		sceneRenderer.Render (shader, camera);
		uiRenderer.Render (uiShader, camera);

		DebugRenderer::Get ().Render ();

		// somehow the Syntax will kinda look like that
		//frame = transparentObjectRenderer.RenderTo ();
		//baseObjectRenderer.RenderFromTo (frame, compositor);
		//compositor.ApplyAllPostProcess ();
	});

	rctx.Loop ();

	#elif TEST_RENDERING

	RenderContext ctx {800, 800, "Scopalatro"};

	AssetManager assetManager;
	Renderer renderer;

	RenderableManager rManager {assetManager, renderer};
	camera camera;
	
	CardManager cardManager {rManager};

	static const int debugSize = 10000;

	std::vector<CardSprite> cardSprites;
	cardSprites.reserve (debugSize);

	for (int i = 0; i < debugSize; ++i)
	{
		auto index = i % 10;
		cardSprites.push_back (std::move (cardManager.Create (CardColor::Deny, CardSymbol {index})));

		cardSprites.at (i).Translate ({rand () % 4, rand () % 4, rand () % 4});
	}

	//cardManager.Create (CardColor::Deny, CardSymbol::Seven);

	ResourceHandle shaderHandle = assetManager.LoadShader ("default", "data/assets/shaders/default.vertex", "data/assets/shaders/default.fragment");
	Shader const & shader = assetManager.GetShader (shaderHandle);


	static float const SENSITIVITY = 0.25f;

	ctx.SetUpdateLoop ([&](RenderContext::Context const & ctx) {
		double deltaX = ctx.mouseX - ctx.lastMouseX;
		double deltaY = ctx.mouseY - ctx.lastMouseY;

		camera.rotate ({deltaX * SENSITIVITY, deltaY * -SENSITIVITY, 0.0});

		if (glfwGetKey (ctx.window, GLFW_KEY_W) == GLFW_PRESS) {
			camera.move_forward (ctx.deltaTime);
		}

		if (glfwGetKey (ctx.window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.move_backward (ctx.deltaTime);
		}

		if (glfwGetKey (ctx.window, GLFW_KEY_A) == GLFW_PRESS) {
			camera.move_left (ctx.deltaTime);
		}

		if (glfwGetKey (ctx.window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.move_right (ctx.deltaTime);
		}

		if (glfwGetKey (ctx.window, GLFW_KEY_E) == GLFW_PRESS) {
			camera.move_up (ctx.deltaTime);
		}

		if (glfwGetKey (ctx.window, GLFW_KEY_Q) == GLFW_PRESS) {
			camera.move_down (ctx.deltaTime);
		}

		if (glfwGetKey (ctx.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose (ctx.window, true);
		}

		if (glfwGetKey (ctx.window, GLFW_KEY_P) == GLFW_PRESS) {
			camera.rotation.x = 90.0;
			camera.rotation.y = 10.0;
		}
	});

	ctx.SetRenderLoop ([&](RenderContext::Context const & ctx) {
		renderer.Render (shader, camera);
	});

	ctx.Loop ();

	return 0;

	#elif TEST_SCOPA

	Game game;
	game.run();

	#endif
}
