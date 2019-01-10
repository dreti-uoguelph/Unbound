#include "stdafx.h"
#include "Renderer.h"

namespace Unbound
{
	namespace Graphics
	{
		Renderer* Renderer::instance = nullptr;

		Renderer* Renderer::get()
		{
			if (instance == nullptr)
			{
				instance = new Renderer();
			}

			return instance;
		}

		void Renderer::init()
		{
			//Load the main shaders. These are part of every scene
#ifdef UNBOUND_BACKEND_VULKAN
			shaders.push_back(Graphics::Shader("./Assets/Shaders/MainVertexShader.spv"));
			shaders.push_back(Graphics::Shader("./Assets/Shaders/MainFragmentShader.spv"));
#else
			shaders.push_back(Graphics::Shader("./Assets/Shaders/MainVertexShaderGL.vert", ShaderType::SHADER_TYPE_VERTEX));
			shaders.push_back(Graphics::Shader("./Assets/Shaders/MainFragmentShaderGL.frag", ShaderType::SHADER_TYPE_FRAGMENT));
#endif

			renderBackend.models = &models;

			//Initialize the render backend
			renderBackend.init();
		}

		void Renderer::close()
		{
			for (auto i : shaders)
			{
				i.unload();
			}

			renderBackend.close();
			glfwDestroyWindow(targetWindow.window);
			glfwTerminate();
		}

		void Renderer::update()
		{
			sceneCameras[0]->update();
			drawFrame();
			glfwPollEvents();
		}

		void Renderer::newWindow(const Graphics::WindowInfo& newWindowInfo)
		{
			if (targetWindow.window == nullptr)
			{
				glfwInit();
			}

			targetWindow = newWindowInfo;
			initWindow();
		}

		void Renderer::newWindow(const uint32_t width, const uint32_t height, const std::string& name, const bool resizable)
		{
			if (targetWindow.window == nullptr)
			{
				glfwInit();
			}

			targetWindow.displayWidth = width;
			targetWindow.displayHeight = height;
			targetWindow.windowName = name;
			targetWindow.windowResizable = resizable;
			initWindow();
		}

		const Graphics::WindowInfo& Renderer::getTargetWindow() const
		{
			return targetWindow;
		}

		void Renderer::resize(const uint32_t newWidth, const uint32_t newHeight)
		{
		}

		void Renderer::add(const Shader& toAdd)
		{
		}

		void Renderer::add(const Model& toAdd)
		{
			models.push_back(toAdd);
			renderBackend.addGeometry(models.back().mesh);

			//Only add the image data to the render backend if the image is not already loaded
			if (toAdd.material.albedo.getData() != nullptr)
			{
				boundIDS.push_back(renderBackend.getNextFreeImageIndex());
				models.back().material.albedo.setBoundID(renderBackend.getNextFreeImageIndex());
				renderBackend.addImageData(models.back().material.albedo);
			}
			else
			{
				//If the image is already added to the renderer, find it and set its bound ID
				auto bid = AssetManager::get()->imageIndex.find(models.back().material.albedo.getID());
				models.back().material.albedo.setBoundID(boundIDS[std::distance(AssetManager::get()->imageIndex.begin(), bid)]);
				boundIDS.push_back(models.back().material.albedo.getBoundID());
			}

			//Add the image ID to the renderer. If the image was already loaded, the hash will be used to figure out the data
			AssetManager::get()->imageIndex.insert(models.back().material.albedo.getID());
		}

		void Renderer::add(Camera& toAdd)
		{
			sceneCameras.push_back(&toAdd);
			renderBackend.updateProjectionMatrix(sceneCameras[0]->getProjectionMatrix());
		}

		//Add a new terrain to the renderer
		void Renderer::add(const Terrain& toAdd)
		{
			mainTerrain = toAdd;
			renderBackend.addTerrainData(mainTerrain);
		}

		/*Set whether vsync is enabled, which prevents screen tearing at the cost of limiting fps*/
		void Renderer::setVsyncEnabled(const bool toSet)
		{
			vsyncEnabled = toSet;
#ifdef UNBOUND_BACKEND_OPENGL
			glfwSwapInterval(toSet);
#endif
		}

		/*Returns whether vsync is currently enabled*/
		const bool Renderer::getVsyncEnabled() const
		{
			return vsyncEnabled;
		}

		/*Sets the output resolution of the renderer, which can be different from the window size*/
		void Renderer::setOutputResolution(const uint16_t width, const uint16_t height)
		{
			outputResolution = glm::ivec2(width, height);
		}

		/*Sets the output resolution of the renderer, which can be different from the window size*/
		void Renderer::setOutputResolution(glm::ivec2 toSet)
		{
			outputResolution = toSet;
		}

		/*Sets the height of the current render output*/
		void Renderer::setOutputWidth(const uint16_t toSet)
		{
			outputResolution.x = toSet;
		}

		/*Sets the height of the current render output*/
		void Renderer::setOutputHeight(const uint16_t toSet)
		{
			outputResolution.y = toSet;
		}

		/*Returns the current output resolution of the renderer*/
		const glm::ivec2& Renderer::getOutputResolution() const
		{
			return outputResolution;
		}

		/*Returns the width of the current render output*/
		const uint16_t Renderer::getOutputWidth() const
		{
			return outputResolution.x;
		}

		/*Returns the height of the current render output*/
		const uint16_t Renderer::getOutputHeight() const
		{
			return outputResolution.y;
		}

		/*Sets the multisample level of the output render buffer*/
		void Renderer::setMultisampleLevel(const uint8_t toSet)
		{
			multisampleLevel = toSet;
#ifdef UNBOUND_BACKEND_OPENGL
			renderBackend.setMultisampleLevel(multisampleLevel);
#endif
		}

		/*Returns the current multisample level of the final framebuffer of the renderer*/
		const uint8_t Renderer::getMultisampleLevel() const
		{
			return multisampleLevel;
		}

		Renderer::Renderer() noexcept : renderBackend(targetWindow, shaders)
		{
			models.reserve(256);
			shaders.reserve(24);
			boundIDS.reserve(4096);
			vsyncEnabled = true;
			AssetManager::get()->imageIndex.insert(0);
			boundIDS.push_back(0);
		}

		Renderer::~Renderer()
		{
		}

		void Renderer::initWindow()
		{
#ifdef UNBOUND_BACKEND_VULKAN
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, targetWindow.windowResizable);
#endif
#ifdef UNBOUND_BACKEND_OPENGL
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

			GLFWimage iconImage = {};
			Graphics::Image windowIcon = IO::ImageLoader::loadFromFile(targetWindow.iconName);
			iconImage.width = windowIcon.getWidth();
			iconImage.height = windowIcon.getHeight();
			iconImage.pixels = reinterpret_cast<unsigned char*>(const_cast<char*>(windowIcon.getData()));

			GLFWmonitor* displayMonitor = nullptr;

			if (targetWindow.windowFullscreen)
			{
				displayMonitor = glfwGetPrimaryMonitor();
			}

			targetWindow.window = glfwCreateWindow(targetWindow.displayWidth, targetWindow.displayHeight, targetWindow.windowName.c_str(), displayMonitor, nullptr);
			glfwSetWindowIcon(targetWindow.window, 1, &iconImage);

			if (targetWindow.window == nullptr)
			{
				Logger::log("Error: could not create GLFW window on this platform");
				throw std::runtime_error("Could not create a GLFW window on this platform");
			}

#ifdef UNBOUND_BACKEND_OPENGL
			glfwMakeContextCurrent(targetWindow.window);

			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				Logger::log("Error: could not load opengl extensions for rendering");
				exit(0);
			}

			glViewport(0, 0, targetWindow.displayWidth, targetWindow.displayHeight);
#endif

			if (!targetWindow.showCursor)
			{
				glfwSetInputMode(targetWindow.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
		}

		void Renderer::drawFrame()
		{
			//Update the uniforms in the render backend
			renderBackend.updateViewMatrix(sceneCameras[0]->getViewMatrix());
			renderBackend.drawFrame();
		}
	}
}