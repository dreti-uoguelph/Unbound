#pragma once
#include"stdafx.h"
#include<functional>
#include<bitset>

namespace Unbound
{
	namespace IO
	{
		enum InputEvent { KEY_DOWN, KEY_UP, KEY_PRESSED };
		typedef uint32_t Key;

		/*This struct represents the keyboard. Currently all keys are bound to their GLFW equilavelents but this can also be changed*/
		struct Keyboard
		{
			static const uint32_t NONE = 0;
			static const uint32_t ZERO = GLFW_KEY_0;
			static const uint32_t ONE = GLFW_KEY_1;
			static const uint32_t TWO = GLFW_KEY_2;
			static const uint32_t THREE = GLFW_KEY_3;
			static const uint32_t FOUR = GLFW_KEY_4;
			static const uint32_t FIVE = GLFW_KEY_5;
			static const uint32_t SIX = GLFW_KEY_6;
			static const uint32_t SEVEN = GLFW_KEY_7;
			static const uint32_t EIGHT = GLFW_KEY_8;
			static const uint32_t NINE = GLFW_KEY_9;
			static const uint32_t A = GLFW_KEY_A;
			static const uint32_t B = GLFW_KEY_B;
			static const uint32_t C = GLFW_KEY_C;
			static const uint32_t D = GLFW_KEY_D;
			static const uint32_t E = GLFW_KEY_E;
			static const uint32_t F = GLFW_KEY_F;
			static const uint32_t G = GLFW_KEY_G;
			static const uint32_t H = GLFW_KEY_H;
			static const uint32_t I = GLFW_KEY_I;
			static const uint32_t J = GLFW_KEY_J;
			static const uint32_t K = GLFW_KEY_K;
			static const uint32_t L = GLFW_KEY_L;
			static const uint32_t M = GLFW_KEY_M;
			static const uint32_t N = GLFW_KEY_N;
			static const uint32_t O = GLFW_KEY_O;
			static const uint32_t P = GLFW_KEY_P;
			static const uint32_t Q = GLFW_KEY_Q;
			static const uint32_t R = GLFW_KEY_R;
			static const uint32_t S = GLFW_KEY_S;
			static const uint32_t T = GLFW_KEY_T;
			static const uint32_t U = GLFW_KEY_U;
			static const uint32_t V = GLFW_KEY_V;
			static const uint32_t W = GLFW_KEY_W;
			static const uint32_t X = GLFW_KEY_X;
			static const uint32_t Y = GLFW_KEY_Y;
			static const uint32_t Z = GLFW_KEY_Z;
			static const uint32_t SHIFT = GLFW_KEY_LEFT_SHIFT;
			static const uint32_t ALT = GLFW_KEY_LEFT_ALT;
			static const uint32_t SUPER = GLFW_KEY_LEFT_SUPER;
			static const uint32_t TAB = GLFW_KEY_TAB;
			static const uint32_t ESCAPE = GLFW_KEY_ESCAPE;
		};

		/*A class for storing the states of various input devices*/
		class InputHandler
		{
		public:
			struct KeyboardCallback
			{
				std::function<void()> callbackFunction;
				int keyCode;
			};

			//Get the current instance of the InputHandler class
			static InputHandler* get();

			//Initialize the input handler
			void init();
			void init(GLFWwindow* windowToSet);

			//Update the input handler, to be called each frame
			void update();

			//Set the target window to receive input from
			void setTargetWindow(GLFWwindow* toSet);

			//Add a callback for a key, which is a function that will run when a certain action is performed on a key.
			//The keycode can be given as a char literal and it will be converted. The modifier can be shift, alt, etc.
			//The action can be UNBOUND_INPUT_KEY_PRESSED etc. The callback is the function that will be called
			void addKeyCallback(int keyCode, InputEvent action, std::function<void()> callback, Key modifier = Keyboard::NONE);
			//Returns a bool indicaticating weather a key is being pressed down. The keycode can be given as a char literal.
			//The modifier is none by default, but can be set to keys like UNBOUND_INPUT_KEYBOARD_SHIFT for key combinations
			bool getKeyDown(int keyCode, Key modifier = Keyboard::NONE);

			//Send a key press without the keyboard physically being pressed.
			void pressKey(int keyCode, Key modifier = Keyboard::NONE);
			//Send a key release event without the key actually being physically released
			void releaseKey(int keyCode, Key modifier = Keyboard::NONE);

			//Get the current mouse position
			glm::ivec2 getMousePosition();
			int getMousePositionX();
			int getMousePositionY();

			//Get the change in mouse position over the last frame
			glm::dvec2 getMousePositionDelta();
			double getMousePositionXDelta();
			double getMousePositionYDelta();

			//Set the mouse position without the mouse actually moving. Used internally
			void setMousePosition(glm::dvec2 newPosition);

			//Set the mouse sensitivity
			void setMouseSensitivity(float newSensitivity);
			//Get the mouse sensitivity multiplyer
			float getMouseSensitivity();

			//Set the mouse acceleration exponent
			void setMouseAcceleration(float newAccelerationValue);
			//Get the current mouse acceleration exponent
			float getMouseAcceleration();

			//Ignore the last mouse move event. Used to prevent the camera from moving unpredictably
			void flushMouseMovements();

			//Enable or disable the mouse
			void enableMouseMovement();
			void disableMouseMovement();

			//Get the vector of all the input callbacks currently set
			const std::vector<KeyboardCallback>& getKeyboardCallbacks();

			InputHandler() noexcept;
			~InputHandler();
		private:
			GLFWwindow * targetWindow; //The current target window that input is being taken from
			std::bitset<1024> keysDown; //The keys currently being held down. Each element in the bitset represents one key. Because of space optimizations, this whole strucure is actually quite small
			std::vector<KeyboardCallback> keyboardCallbacks; //A vector of all the keyboard callbacks that are currently set

			bool mouseMovementEnabled = false; //Whether mouse movement is enabled. If not, no mouse movement events will be recorded
			float mouseSensitivity; //A multiplyer for the amount of movement recorded for the mouse.
			float mouseAcceleration; //Determines the effect of mouse acceleration
			glm::dvec2 mousePosition, mousePositionOld, mousePositionDelta; //The current, old and change in mouse position

			static InputHandler* instance;
		};
	}
}