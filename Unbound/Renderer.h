#pragma once
#include"stdafx.h"
#include"RenderBackendVulkan.h"
#include"RenderBackendOpenGL.h"
#include"ImageLoader.h"
#include"Camera.h"
#include"Model.h"
#include"MemoryManager.h"
#include"Terrain.h"

namespace Unbound
{
	namespace Graphics
	{
		/*The main class responsible for handling render windows and drawing things to the screen*/
		class Renderer
		{
		public:
			/*Initialize all renderer features*/
			void init();
			/*Close the renderer and clean up any used memory*/
			void close();
			/*Updates the renderer and draws a frame*/
			void update();
			/*Get the current renderer instance*/
			static Renderer* get();

			/*Open a new GLFW window to render to*/
			void newWindow(const Graphics::WindowInfo& newWindowInfo);
			/*Open a new GLFW window to render to*/
			void newWindow(const uint32_t width = 1280, const uint32_t height = 720, const std::string& name = "Unbound", const bool resizable = false);
			/*Get a pointer to the window currently used for rendering*/
			const Graphics::WindowInfo& getTargetWindow() const;
			/*Resize the output window currently being rendered to*/
			void resize(const uint32_t newWidth, const uint32_t newHeight);

			/*Add a new shader to the renderer*/
			void add(const Shader& toAdd);
			/*Add a new Mesh to the renderer*/
			void add(const Model& toAdd);
			/*Add a new Camera to the renderer*/
			void add(Camera& toAdd);
			/*Add a new terrain to the renderer*/
			void add(const Terrain& toAdd);

			/*Set whether vsync is enabled, which prevents screen tearing at the cost of limiting fps*/
			void setVsyncEnabled(const bool toSet);
			/*Returns whether vsync is currently enabled*/
			const bool getVsyncEnabled() const;
			/*Sets the output resolution of the renderer, which can be different from the window size*/
			void setOutputResolution(const uint16_t width, const uint16_t height);
			/*Sets the output resolution of the renderer, which can be different from the window size*/
			void setOutputResolution(glm::ivec2 toSet);
			/*Sets the width of the current render output*/
			void setOutputWidth(const uint16_t toSet);
			/*Sets the height of the current render output*/
			void setOutputHeight(const uint16_t toSet);
			/*Returns the current output resolution of the renderer*/
			const glm::ivec2& getOutputResolution() const;
			/*Returns the width of the current render output*/
			const uint16_t getOutputWidth() const;
			/*Returns the height of the current render output*/
			const uint16_t getOutputHeight() const;
			/*Sets the multisample level of the output render buffer*/
			void setMultisampleLevel(const uint8_t toSet);
			/*Returns the current multisample level of the final framebuffer of the renderer*/
			const uint8_t getMultisampleLevel() const;

			std::vector<uint32_t> boundIDS; //The ID of each loaded image within the renderer

			Renderer() noexcept;
			~Renderer();
		private:
			static Renderer* instance;
			Graphics::WindowInfo targetWindow; //Information about the target window being rendered to. In the future, this could become a vector to support multiple windows
			Terrain mainTerrain; //The main terrain that will be rendered. It is given priority over secondary terrains in terms of detail
			std::vector<Camera*> sceneCameras; //The cameras currently in the scene. The first camera has top priority
			std::vector<Graphics::Shader> shaders; //All the shaders that are currently part of the renderer
			std::vector<Model> models; //Keeps track of all the models that are currently on the renderer
			bool vsyncEnabled; //Whether vsync is enabled, which prevents screen tearing at the cost of fps
			glm::ivec2 outputResolution; //The resolution of the render output
			uint8_t multisampleLevel; //The level of multisampling used for the final framebuffer

#ifdef UNBOUND_BACKEND_VULKAN
			Graphics::RenderBackendVulkan renderBackend; //The backend this renderer uses. Uses vulkan by default
#endif
#ifdef UNBOUND_BACKEND_OPENGL
			Graphics::RenderBackendOpenGL renderBackend; //The render backend for this renderer. Can opengl if vulkan is unsupported
#endif

			/*Initialize the current output window*/
			void initWindow();
			/*Draws a frame to the screen*/
			void drawFrame();
		};
	}
}