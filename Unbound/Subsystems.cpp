#include "stdafx.h"
#include "Subsystems.h"

namespace Unbound
{
	void initSubsystems()
	{
		Util::Time::get()->init();

#ifndef _DEBUG
		Logger::Logger();
#endif
#ifdef UNBOUND_BACKEND_VULKAN
		Logger::log("Starting Engine with Vulkan render backend");
#else
		Logger::log("Starting Engine with OpenGL render backend");
#endif

		Logger::log("Loading Settings...");

		//Load user defined settings
		IO::SettingsManager::get()->loadFile("settings.ini");

		//Open a new window
		Graphics::WindowInfo windowInfo = {};
		windowInfo.displayWidth = IO::SettingsManager::get()->getSetting<int>("iWindowWidth", 1280, "WINDOW");
		windowInfo.displayHeight = IO::SettingsManager::get()->getSetting<int>("iWindowHeight", 720, "WINDOW");
		windowInfo.windowName = IO::SettingsManager::get()->getSetting<std::string>("sWindowName", "Unbound", "WINDOW");
		windowInfo.windowResizable = IO::SettingsManager::get()->getSetting<bool>("bWindowResizable", false, "WINDOW");
		windowInfo.showCursor = IO::SettingsManager::get()->getSetting<bool>("bShowMouseCursor", false, "WINDOW");
		windowInfo.iconName = IO::SettingsManager::get()->getSetting<std::string>("sWindowIcon", "icon.png", "WINDOW");
		windowInfo.windowFullscreen = IO::SettingsManager::get()->getSetting<bool>("bWindowFullscreen", false, "WINDOW");

		//Allocate memory
		Logger::log("Creating memory pools...");
		uint64_t vertexBufferSize = IO::SettingsManager::get()->getSetting<int>("iMeshBufferSizeMB", 64, "RENDERER");
		uint64_t vertexStagingBufferSize = IO::SettingsManager::get()->getSetting<int>("iMeshStagingBufferSizeMB", 16, "RENDERER");
		uint64_t textureBufferSize = IO::SettingsManager::get()->getSetting<int>("iTextureBufferSizeMB", 256, "RENDERER");
		uint64_t textureStagingBufferSize = IO::SettingsManager::get()->getSetting<int>("iTextureStagingBufferSizeMB", 32, "RENDERER");
		uint64_t indexStagingBufferSize = IO::SettingsManager::get()->getSetting<int>("iIndexStagingBufferSizeMB", 8, "RENDERER");
		uint64_t indexBufferSize = IO::SettingsManager::get()->getSetting<int>("iIndexBufferSizeMB", 16, "RENDERER");

		//Initialize the vertex and texture memory pools to allocate memory
		MemoryManager::get()->vertexDataPool.init(vertexBufferSize * 1000 * 1000);
		MemoryManager::get()->textureDataPool.init(textureBufferSize * 1000 * 1000);
		MemoryManager::get()->indexDataPool.init(indexBufferSize * 1000 * 1000);
		MemoryManager::get()->vertexStagingPool.setMaxSize(vertexStagingBufferSize * 1000 * 1000);
		MemoryManager::get()->indexStagingPool.setMaxSize(indexStagingBufferSize * 1000 * 1000);
		MemoryManager::get()->textureStagingPool.setMaxSize(textureStagingBufferSize * 1000 * 1000);

		IO::MeshLoader::get()->init();
		IO::ImageLoader::get()->init();

		//Initialize the renderer
		Logger::log("Initializing renderer...");
		Graphics::Renderer::get()->newWindow(windowInfo);
		Graphics::Renderer::get()->init();
		Graphics::Renderer::get()->setVsyncEnabled(IO::SettingsManager::get()->getSetting<bool>("bVsyncEnabled", true, "DISPLAY"));
		Graphics::Renderer::get()->setMultisampleLevel(IO::SettingsManager::get()->getSetting<int>("iMultisampleLevel", 1, "RENDERER"));

		//Initialize the input handler
		IO::InputHandler::get()->init(Graphics::Renderer::get()->getTargetWindow().window);
		IO::InputHandler::get()->addKeyCallback(IO::Keyboard::ESCAPE, Unbound::IO::InputEvent::KEY_DOWN, [&]() { exit(0); });
		IO::InputHandler::get()->setMouseSensitivity(IO::SettingsManager::get()->getSetting<float>("fMouseSensitivity", 1.0f, "INPUT"));
		IO::InputHandler::get()->setMouseAcceleration(IO::SettingsManager::get()->getSetting<float>("fMouseAcceleration", 1.0f, "INPUT"));

		//Initialize the audio manager
		Logger::log("Initializing audio manager...");
		Audio::AudioManager::get()->init();

		//Set task to enable mouse movement once everything is loaded
		TaskScheduler::get()->add([]() {IO::InputHandler::get()->enableMouseMovement(); }, false, TaskScheduler::TASK_PRIORITY_NORMAL, { TaskScheduler::TASK_EVENT_LEVEL_LOAD_FINISHED });

		//Set task to update the graphics and input handlers
		TaskScheduler::get()->add([]() {Graphics::Renderer::get()->update(); }, true, TaskScheduler::TASK_PRIORITY_HIGH);
		TaskScheduler::get()->add([]() {IO::InputHandler::get()->update(); }, true, TaskScheduler::TASK_PRIORITY_HIGH);

		//Trigger an event to indicate initialization is complete
		TaskScheduler::get()->triggerEvent(TaskScheduler::TASK_EVENT_PROGRAM_START);

		Logger::log("Initilized all subsystems, engine started with " + std::to_string(Logger::getNumErrors()) + std::string(" error(s)"));
		Logger::newLine();
	}

	void updateSubsystems()
	{
		Util::Time::get()->frame();
		TaskScheduler::get()->update();
	}

	void closeSubsystems()
	{
		TaskScheduler::get()->triggerEvent(TaskScheduler::TASK_EVENT_PROGRAM_CLOSE);
		Graphics::Renderer::get()->close();
	}
}