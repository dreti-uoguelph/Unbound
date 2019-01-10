#pragma once
#include"stdafx.h"

namespace Unbound
{
	namespace Graphics
	{
		/*Struct respresenting information about the window that is being rendered*/
		struct WindowInfo
		{
			std::string windowName; //The name of the current output window
			unsigned int displayWidth; //Number of pixels being rendered, horizontally (screen width)
			unsigned int displayHeight; //Height of the screen
			bool windowResizable; //Whether or not the window is resizable when the application is running
			bool windowFullscreen; //Whether the window should run in fullscreen mode
			bool showCursor; //Whether to show the mouse cursor on top of the render ouput
			GLFWwindow* window; //Pointer to the current GLFW window handle
			std::string iconName; //Path to the image file to set the window icon to. If this is unspecified, the window will not have an icon

			WindowInfo()
			{
				window = nullptr;
			}
		};
	}
}