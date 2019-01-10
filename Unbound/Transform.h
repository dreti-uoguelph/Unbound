#pragma once
#include"stdafx.h"
#include"glm\gtc\matrix_transform.hpp"

namespace Unbound
{
	/*This class keeps track of a transformation in 3d space*/
	class Transform
	{
	public:
		/*Return the entire transformation matrix with all transformations applied*/
		glm::mat4x4 getTransformMatrix();

		//Set the world space position of this transform
		void setPosition(glm::vec3 toSet);
		//Move this transform by amount units
		void translate(glm::vec3 amount);
		//Get the current position of this transformation
		glm::vec3 getPosition();

		//Set the current rotation, in degrees, of this transformation
		void setRotation(glm::vec3 toSet);
		//Rotate this transformation by amount degrees
		void rotate(glm::vec3 amount);
		//Get the current rotation of this transformation in degrees
		glm::vec3 getRotation();

		//Set the scale of this transformation, with 1.0 being default
		void setScale(glm::vec3 toSet);
		//Change the scale of this transformation. Amount is added to the current scale
		void scale(glm::vec3 amount);
		//Get the current scale of this transformation
		glm::vec3 getScale();

		//Set the parent transform that this is relative to
		void setParent(Transform* toSet);
		//Get the current parent transform of this transformation
		Transform* getParent();

		Transform() noexcept;
		~Transform();
	private:
		Transform * parent;
		glm::mat4x4 transform;
		glm::vec3 transformPosition, transformRotation, transformScale;

		//Recalculate the transform matrix with new values
		void recalculateTransformMatrix();
	};
}