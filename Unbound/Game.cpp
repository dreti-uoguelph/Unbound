#include"stdafx.h"
#include"Game.h"

namespace Unbound
{
	bool Game::running = true; //Whether the game is currently running (used in update loop)
	Camera camera; //The main camera that the player will control in the scene

	void Game::init()
	{
		initSubsystems(); //Initialize all the engine subsystems

		TaskScheduler::get()->triggerEvent(TaskScheduler::TASK_EVENT_LEVEL_LOAD);

		camera.init(1280, 720, 48); //Set the initial image width, height and field of view of the camera
		camera.setPosition(0.0f, 0.0f, -5.0f); //Set the camera's initial position
		Graphics::Renderer::get()->add(camera); //Add the camera to the renderer

		TaskScheduler::get()->add([]()
		{
			Terrain mainTerrain; //Create a new terrain
			mainTerrain.setHeightmap("./Assets/Textures/heightmap.png"); //Load the terrain's elevation heightmap
			mainTerrain.setMaterial(Graphics::TerrainMaterial()); //Create a new material and assign it to the terrain
			mainTerrain.setNumChunks(1, 1); //Set the number of chunks or sections that the terrain has
			mainTerrain.setChunkSize(glm::vec2(16, 16)); //Set the size of each chunk in world units
			mainTerrain.setVerticesPerChunk(64); //Set how detailed each chunk should be 
			mainTerrain.init(false); //Initialize the terrain, without generating a cache

			Graphics::Renderer::get()->add(mainTerrain); //Add the terrain to the level
		}, false, TaskScheduler::TASK_PRIORITY_HIGH);

		TaskScheduler::get()->add([]()
		{
			Model chaletModel;
			chaletModel.transform.setRotation(glm::vec3(0.0, 90.0f, 0.0f));
			chaletModel.material.albedo = IO::ImageLoader::loadFromFile("./Assets/Textures/chalet.jpg");
			chaletModel.mesh = IO::MeshLoader::loadFromFile("./Assets/Models/chalet.m");
			Graphics::Renderer::get()->add(chaletModel);

			TaskScheduler::get()->triggerEvent(TaskScheduler::TASK_EVENT_LEVEL_LOAD_FINISHED);
		}, false, TaskScheduler::TASK_PRIORITY_HIGH);

		TaskScheduler::get()->add([]()
		{
			AudioSource testAudioSource;
			testAudioSource.load("./Assets/Audio/ondine.ogg");
			testAudioSource.play();
		});
	}

	/*The update function for the game. This is called every frame and manages all the scripts and game behaviour*/
	void Game::update()
	{
		updateSubsystems();
		TaskScheduler::get()->triggerEvent(TaskScheduler::TASK_EVENT_FRAME);

		if (IO::InputHandler::get()->getKeyDown(IO::Keyboard::W))
		{
			camera.move(glm::vec3(0.0f, 0.0f, 5 * Util::Time::get()->getDeltaTime().asSeconds()));
		}

		if (IO::InputHandler::get()->getKeyDown(IO::Keyboard::S))
		{
			camera.move(glm::vec3(0.0f, 0.0f, -5 * Util::Time::get()->getDeltaTime().asSeconds()));
		}

		if (IO::InputHandler::get()->getKeyDown(IO::Keyboard::A))
		{
			camera.move(glm::vec3(5 * Util::Time::get()->getDeltaTime().asSeconds(), 0.0f, 0.0f));
		}

		if (IO::InputHandler::get()->getKeyDown(IO::Keyboard::D))
		{
			camera.move(glm::vec3(-5 * Util::Time::get()->getDeltaTime().asSeconds(), 0.0f, 0.0f));
		}

		camera.rotate(glm::vec3(IO::InputHandler::get()->getMousePositionYDelta() * Util::Time::get()->getDeltaTime().asSeconds() * 0.1, IO::InputHandler::get()->getMousePositionXDelta() * Util::Time::get()->getDeltaTime().asSeconds() * 0.1, 0.0f));
	}

	int Game::close()
	{
		TaskScheduler::get()->triggerEvent(TaskScheduler::TASK_EVENT_LEVEL_UNLOAD);
		closeSubsystems();
		return 0;
	}
}