#pragma once
#include"stdafx.h"
#include"GameObject.h"
#include<glm\gtc\matrix_transform.hpp>

namespace Unbound
{
	/*A class to represent a camera in the game world.*/
	class Camera : public GameObject
	{
	public:

		/*Represents settings for post-processing an image drawn by the camera*/
		struct PostProcessSettings
		{
			GLfloat exposure; //The overall brightness of the scene in f-stops
			GLfloat saturation; //How saturated the image is (grey to colorful)
			GLfloat contrast; //Contrast of the image (push values away from grey)
			GLfloat gamma; //Used to convert colors for showing on display, usually 2.2
			glm::vec3 tint; //color to multiply the output by, used for color grading
			glm::vec3 liftGammaGain;

			PostProcessSettings()
			{
				exposure = 1.0f;
				saturation = 1.0f;
				contrast = 1.0f;
				gamma = 2.2f;
				tint = glm::vec3(1.0f, 1.0f, 1.0f);
				liftGammaGain = glm::vec3(1.0f, 1.0f, 1.0f);
			}
		};

		//Initialize the camera
		virtual void init() override;
		void init(const int renderWidth, const int renderHeight, const float fov);
		//Update the camera properties
		virtual void update() override;
		//Destroy the camera
		virtual void close() override;

		//Get the view matrix associated with this camera
		const glm::mat4& getViewMatrix() const;
		//Get the projection matrix associated with this camera
		const glm::mat4& getProjectionMatrix() const;

		//Move the camera
		void move(const glm::vec3& amount);
		//Move the camera
		void move(const float x, const float y, const float z);

		//Rotate the camera
		void rotate(const glm::vec3& amount);
		//Rotate the camera
		void rotate(const float x, const float y, const float z);

		//Set the camera's position
		void setPosition(const glm::vec3& toSet);
		//Set the camera's position
		void setPosition(const float x, const float y, const float z);

		//Set the camera's rotation
		void setRotation(const glm::vec3& toSet);
		//Set the camera's rotation
		void setRotation(const float x, const float y, const float z);

		//Set the size of the render output (usually same as the window size)
		void setRenderSize(const glm::vec2& toSet);
		//Set the size of the render output
		void setRenderSize(const int x, const int y);

		//Set the camera's field of view
		void setFOV(const float newFieldOfView);

		//Get the camera's position
		const glm::vec3 getPosition() const;
		//Get the camera's rotation
		const glm::vec3 getRotation() const;
		//Get the camera's FOV
		const float getFOV() const;

		//Set whether the input's Y axis is inverted
		void setInvertYAxis(const bool toSet);
		//Get whether the input's Y axis is currently inverted
		const bool getYAxisInverted() const;

		//Set whether camera smoothing is enabled
		void setSmoothCameraEnabled(const bool toSet);
		//Get whether camera smoothing is enabled
		const bool getSmoothCameraEnabled() const;

		//Sets the post processing settings that will be applied to this camera
		void setPostProcessSettings(const PostProcessSettings& toSet);
		//Returns a reference to the current post processing settings applied to the camera
		const PostProcessSettings& getPostProcessSettings();

		Camera() noexcept;
		~Camera();
	private:
		bool isActive; //Indicates whether the camera has been activated

		bool invertYAxis; //Invert movement on the camera's Y axis
		bool enableSmoothCamera; //Enable camera smoothing, which will smooth out camera rotation over multiple frames
		float fieldOfView; //The vertical FOV of the camera
		glm::ivec2 renderSize; //The size of the canvas that is being rendered to
		glm::vec3 position, rotation; //The position and rotation of the camera, in world space
		glm::vec3 forward, up, right; //Vectors for calculating the view matrix
		glm::mat4 view, projection; //The view and projection matrices associated with this camera
		glm::vec2 prevRotationAmounts[6]; //Stores the amount of camera rotation recorded for the past frames, to be used for smoothing
		PostProcessSettings postProcessSettings; //Stores the settings used to apply post-processing effects to the camera

		//Move the camera forward or back
		void moveForwardBack(const float amount);
		//Move the camera left or right
		void moveLeftRight(const float amount);
		//Move the camera up or down
		void moveUpDown(const float amount);
	};
}