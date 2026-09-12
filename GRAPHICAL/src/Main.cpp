#include <memory>

#include <Renderer/Renderer.hpp>
#include <RenderContext.hpp>
#include <AssetLoader.hpp>

#include <RenderableObjects/CardModel.hpp>
#include <RenderableObjects/HealthbarModel.hpp>

#include <Debugger/DebugImGuI.hpp>
#include <Debugger/SceneInspector.hpp>
#include <Debugger/DebugRenderer.hpp>

#include <InputManager.hpp>

#include <chrono>

#include <Game.hpp>

using namespace TLOT;

int main()
{
	size_t width  = 1920;
	size_t height = 1080;
	//size_t width  = 1200;
	//size_t height = 900;


	RenderContext ctx;
	ctx.Init (width, height, "Scopalatro");
	// ctx and renderer will probably become singleton
	// TODO: see what can be done about multithreading
	// maybe put the atlas creation in the asset manager ?
	// then the renderer won't have the responsability to provide quads anymore ?

	// cache it in assetManager
	std::shared_ptr<VertexTemplate> vertexTemplate = std::make_shared<VertexTemplate>(std::vector<VertexAtom::Type> {
		VertexAtom::Vec3,
		VertexAtom::Vec2
	});

	SceneInspector inspector;
	Renderer renderer {vertexTemplate, width, height}; // renderer should be in the context class (probabky not constructed in place tho)
	// + vertexTemplate should not be a 'dependency' for renderer, there should probably be multiple templates allowed
	
	inspector.SetRenderer(&renderer);

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return -1;
	}

	Game game {&ctx, &renderer, &inspector, vertexTemplate, ft};

	int updateCount = 0;
	int renderCount = 0;

	enum CountMode
	{
		None   = 0,
		Update = 1,
		Render = 2
	};

	CountMode currentMode = None;

	SetupImGuI (ctx.window);
	while (ctx.ShouldRenderFrame() || !ctx.ShouldExit())
	{
		ctx.Update ();
		updateCount++;
		
		auto begin = std::chrono::steady_clock::now();
		game.Update(ctx.deltaTime);
		auto end   = std::chrono::steady_clock::now();

		++updateCount;
		
		if (updateCount > 1000 && currentMode == Update)
		{
			auto delta = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
			Logger::log(LogLevel::Info, "[Update] Time difference = {} [µs]", delta);
			updateCount = 0;
		}

		if (InputManager::getInstance().isKeyPressed(GLFW_KEY_UP))
		{
			currentMode = static_cast<CountMode>((currentMode + 1) % 3);
		}

		if (!ctx.ShouldRenderFrame ()) continue;
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // will be part of the technique class responsabilty in a future update

		renderCount++;

		game.Render();
		
		begin = std::chrono::steady_clock::now();
		renderer.Render();
		end   = std::chrono::steady_clock::now();


		if (renderCount > 100 && currentMode == Render)
		{
			auto delta = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
			Logger::log(LogLevel::Info, "[Render] Time difference = {} [µs]", delta);
			renderCount = 0;
		}


		DebugRenderer::Get().Render();

		StartFrameImGuI();
		inspector.OpenMenu();
		EndFrameImGuI();

		ctx.Render ();

	}

	return 0;
}