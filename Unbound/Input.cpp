#include "stdafx.h"
#include "Input.h"

namespace Unbound
{
	namespace IO
	{
		InputHandler* InputHandler::instance = nullptr;

		InputHandler* InputHandler::get()
		{
			if (instance == nullptr)
			{
				instance = new InputHandler();
			}

			return instance;
		}

		void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			//Go through all the callbacks and and if any of them are bound to this key, run them
			auto callbacks = InputHandler::get()->getKeyboardCallbacks();

			for (auto i : callbacks)
			{
				if (i.keyCode == key)
				{
					i.callbackFunction();
				}
			}

			if (action == GLFW_PRESS)
			{
				//Set the key corrosponding to the key to be down
				InputHandler::get()->pressKey(key);
			}
			else if (action == GLFW_RELEASE)
			{
				//Release the key relating to the key
				InputHandler::get()->releaseKey(key);
			}
		}

		void InputHandler::init()
		{
			if (targetWindow != nullptr)
			{
				glfwSetKeyCallback(targetWindow, keyCallback);
			}
		}

		void InputHandler::init(GLFWwindow* windowToSet)
		{
			targetWindow = windowToSet;
			init();
		}

		void InputHandler::update()
		{
			glm::dvec2 newPosition;
			glfwGetCursorPos(targetWindow, &newPosition.x, &newPosition.y);

			mousePositionOld = mousePosition;
			mousePosition = newPosition;

			if (mouseMovementEnabled)
			{
				mousePositionDelta = mousePosition - mousePositionOld;
			}
		}

		//Set the target window to receive input from
		void InputHandler::setTargetWindow(GLFWwindow* toSet)
		{
			targetWindow = toSet;
		}

		//Add a callback for a key, which is a function that will run when a certain action is performed on a key.
		//The keycode can be given as a char literal and it will be converted. The modifier can be shift, alt, etc.
		//The action can be UNBOUND_INPUT_KEY_PRESSED etc. The callback is the function that will be called
		void InputHandler::addKeyCallback(int keyCode, InputEvent action, std::function<void()> callback, Key modifier)
		{
			keyboardCallbacks.push_back({ callback, keyCode });
		}

		//Returns a bool indicaticating weather a key is being pressed down. The keycode can be given as a char literal.
		//The modifier is none by default, but can be set to keys like UNBOUND_INPUT_KEYBOARD_SHIFT for key combinations
		bool InputHandler::getKeyDown(int keyCode, Key modifier)
		{
			if (keyCode <= 1024)
			{
				if (keysDown[keyCode])
				{
					return true;
				}
			}

			return false;
		}

		void InputHandler::pressKey(int keyCode, Key modifier)
		{
			if (keyCode <= 1024)
			{
				keysDown[keyCode] = true;
			}
		}

		void InputHandler::releaseKey(int keyCode, Key modifier)
		{
			if (keyCode <= 1024)
			{
				keysDown[keyCode] = false;
			}
		}

		//Get the current mouse position
		glm::ivec2 InputHandler::getMousePosition()
		{
			return mousePosition;
		}

		int InputHandler::getMousePositionX()
		{
			return static_cast<int>(mousePosition.x);
		}

		int InputHandler::getMousePositionY()
		{
			return static_cast<int>(mousePosition.y);
		}

		//Get the change in mouse position from the last frame
		glm::dvec2 InputHandler::getMousePositionDelta()
		{
			return mousePositionDelta * glm::dvec2(mouseSensitivity, mouseSensitivity);
		}

		double InputHandler::getMousePositionXDelta()
		{
			return mousePositionDelta.x * mouseSensitivity;
		}

		double InputHandler::getMousePositionYDelta()
		{
			return mousePositionDelta.y * mouseSensitivity;
		}

		//Set the mouse position without the mouse actually moving. Used internally
		void InputHandler::setMousePosition(glm::dvec2 newPosition)
		{
			mousePositionOld = mousePosition;
			mousePosition = newPosition;

			if (mouseMovementEnabled)
			{
				mousePositionDelta = mousePosition - mousePositionOld;
			}
		}

		//Set the mouse sensitivity multiplyer
		void InputHandler::setMouseSensitivity(float newSensitivity)
		{
			mouseSensitivity = newSensitivity;
		}

		//Get the mouse sensitivity multiplyer
		float InputHandler::getMouseSensitivity()
		{
			return mouseSensitivity;
		}

		//Set the mouse acceleration exponent
		void InputHandler::setMouseAcceleration(float newAccelerationValue)
		{
			mouseAcceleration = newAccelerationValue;
		}

		//Returns the mouse acceleration exponent
		float InputHandler::getMouseAcceleration()
		{
			return mouseAcceleration;
		}

		//Ignore the last mouse move event. Used to prevent the camera from moving unpredictably on game load
		void InputHandler::flushMouseMovements()
		{
			mousePosition = mousePositionOld;
			mousePositionDelta = glm::dvec2(0.0, 0.0);
		}

		//Enable mouse movement
		void InputHandler::enableMouseMovement()
		{
			mouseMovementEnabled = true;
		}

		//Disable mouse movement
		void InputHandler::disableMouseMovement()
		{
			mouseMovementEnabled = false;
		}

		//Get a vector of all the keyboard callbacks
		const std::vector<InputHandler::KeyboardCallback>& InputHandler::getKeyboardCallbacks()
		{
			return keyboardCallbacks;
		}

		InputHandler::InputHandler() noexcept
		{
			targetWindow = nullptr;
		}

		InputHandler::~InputHandler()
		{
		}
	}
}