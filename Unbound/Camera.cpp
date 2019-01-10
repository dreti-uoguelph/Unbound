#include "stdafx.h"
#include "Camera.h"

namespace Unbound
{
	/*Initialize the camera*/
	void Camera::init()
	{
		//Build the projection matrix, which will use the coordinate system corrosponding to the correct render backend
		projection = glm::perspectiveFovRH(glm::radians(fieldOfView), static_cast<float>(renderSize.x), static_cast<float>(renderSize.y), 0.01f, 100000.0f);

		//Set the initial internal vectors used by the camera
		forward = glm::vec3(0.0f, 0.0f, 1.0f);
		up = glm::vec3(0.0f, 1.0f, 0.0f);
		right = glm::vec3(1.0f, 0.0f, 0.0f);

		//Set up initial post processing settings
		postProcessSettings = PostProcessSettings();
	}

	void Camera::init(int renderWidth, int renderHeight, float fov)
	{
		renderSize.x = renderWidth;
		renderSize.y = renderHeight;
		fieldOfView = fov;
		init();
	}

	/*Update the camera properties*/
	void Camera::update()
	{
		//Update the view matrix
		forward.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
		forward.y = sin(glm::radians(rotation.x));
		forward.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
		forward = glm::normalize(forward);

#ifdef UNBOUND_BACKEND_VULKAN
		float factor = -1.0f;
#else
		float factor = -1.0f;
#endif

		right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, factor, 0.0f)));

		glm::mat4 translationMatrix = glm::translate(glm::mat4(), position);
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(), glm::radians(rotation.x), glm::vec3(1.0f, 0.0, 0.0));
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0.0, 1.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		view = glm::lookAtRH(position, position + forward, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	/*Destroy the camera*/
	void Camera::close()
	{
	}

	/*Get the view matrix associated with this camera*/
	const glm::mat4& Camera::getViewMatrix() const
	{
		return view;
	}

	/*Get the projection matrix associated with this camera*/
	const glm::mat4& Camera::getProjectionMatrix() const
	{
		return projection;
	}

	//Move the camera
	void Camera::move(const glm::vec3& amount)
	{
		moveForwardBack(amount.z);
		moveLeftRight(amount.x);
		moveUpDown(amount.y);
	}

	//Move the camera
	void Camera::move(const float x, const float y, const float z)
	{
		move(glm::vec3(x, y, z));
	}

	//Rotate the camera
	void Camera::rotate(const glm::vec3& amount)
	{
#ifdef UNBOUND_BACKEND_VULKAN
		rotation.x += amount.x;
#else
		rotation.x -= amount.x;
#endif
		rotation.y += amount.y;
		rotation.z += amount.z;
	}

	//Rotate the camera
	void Camera::rotate(const float x, const float y, const float z)
	{
		if (!invertYAxis)
		{
			rotate(glm::vec3(x, y, z));
		}
		else
		{
			rotate(glm::vec3(-x, y, z));
		}
	}

	//Set the camera's position
	void Camera::setPosition(const glm::vec3& toSet)
	{
		position = toSet;
	}

	//Set the camera's position
	void Camera::setPosition(const float x, const float y, const float z)
	{
		setPosition(glm::vec3(x, y, z));
	}

	//Set the camera's rotation
	void Camera::setRotation(const glm::vec3& toSet)
	{
		rotation = toSet;
	}

	//Set the camera's rotation
	void Camera::setRotation(float x, float y, float z)
	{
		setRotation(glm::vec3(x, y, z));
	}

	//Set the size of the render output (usually same as the window size)
	void Camera::setRenderSize(const glm::vec2& toSet)
	{
		renderSize = toSet;
	}

	//Set the size of the render output
	void Camera::setRenderSize(const int x, const int y)
	{
		renderSize = glm::ivec2(x, y);
	}

	//Get the camera's field of view
	void Camera::setFOV(const float newFieldOfView)
	{
		fieldOfView = newFieldOfView;
	}

	//Returns the camera's position in world space
	const glm::vec3 Camera::getPosition() const
	{
		return position;
	}

	//Returns the camera's rotation in world space
	const glm::vec3 Camera::getRotation() const
	{
		return rotation;
	}

	//Get the camera's field of view
	const float Camera::getFOV() const
	{
		return fieldOfView;
	}

	void Camera::setInvertYAxis(const bool toSet)
	{
		invertYAxis = toSet;
	}

	const bool Camera::getYAxisInverted() const
	{
		return invertYAxis;
	}

	//Set whether camera smoothing is enabled
	void Camera::setSmoothCameraEnabled(const bool toSet)
	{
		enableSmoothCamera = toSet;
	}

	//Get whether camera smoothing is enabled
	const bool Camera::getSmoothCameraEnabled() const
	{
		return enableSmoothCamera;
	}

	//Sets the post processing settings that will be applied to this camera
	void Camera::setPostProcessSettings(const PostProcessSettings& toSet)
	{
		postProcessSettings = PostProcessSettings(toSet);
	}

	//Returns a reference to the current post processing settings applied to the camera
	const Camera::PostProcessSettings& Camera::getPostProcessSettings()
	{
		return postProcessSettings;
	}

	Camera::Camera() noexcept
	{
		invertYAxis = false;
		isActive = false;
		enableSmoothCamera = true;
		fieldOfView = 48;
	}

	Camera::~Camera()
	{
	}

	//Move the camera forward or backward
	void Camera::moveForwardBack(const float amount)
	{
		position += forward * amount;
	}

	//Move the camera left or right
	void Camera::moveLeftRight(const float amount)
	{
		position += right * amount;
	}

	//Move the camera up or down
	void Camera::moveUpDown(const float amount)
	{
		position += up * amount;
	}
}