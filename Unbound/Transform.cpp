#include"stdafx.h"
#include"Transform.h"

namespace Unbound
{
	/*Return the entire transformation matrix with all transformations applied*/
	glm::mat4x4 Transform::getTransformMatrix()
	{
		return transform;
	}

	//Set the world space position of this transform
	void Transform::setPosition(glm::vec3 toSet)
	{
		transformPosition = toSet;
		recalculateTransformMatrix();
	}

	//Move this transform by amount units
	void Transform::translate(glm::vec3 amount)
	{
		transformPosition += amount;
		recalculateTransformMatrix();
	}

	//Get the current position of this transformation
	glm::vec3 Transform::getPosition()
	{
		return transformPosition;
	}

	//Set the current rotation, in degrees, of this transformation
	void Transform::setRotation(glm::vec3 toSet)
	{
		transformRotation = toSet;
		recalculateTransformMatrix();
	}

	//Rotate this transformation by amount degrees
	void Transform::rotate(glm::vec3 amount)
	{
		transformRotation += amount;
		recalculateTransformMatrix();
	}

	//Get the current rotation of this transformation in degrees
	glm::vec3 Transform::getRotation()
	{
		return transformRotation;
	}

	//Set the scale of this transformation, with 1.0 being default
	void Transform::setScale(glm::vec3 toSet)
	{
		transformScale = toSet;
		recalculateTransformMatrix();
	}

	//Change the scale of this transformation. Amount is added to the current scale
	void Transform::scale(glm::vec3 amount)
	{
		transformScale += amount;
		recalculateTransformMatrix();
	}

	//Get the current scale of this transformation
	glm::vec3 Transform::getScale()
	{
		return transformScale;
	}

	//Set the parent transform that this is relative to
	void Transform::setParent(Transform* toSet)
	{
		parent = toSet;
		recalculateTransformMatrix();
	}

	//Get the current parent transform of this transformation
	Transform* Transform::getParent()
	{
		return parent;
	}

	Transform::Transform() noexcept
	{
		transformPosition = glm::vec3(0.0f, 0.0f, 0.0f);
		transformRotation = glm::vec3(0.0f, 0.0f, 0.0f);
		transformScale = glm::vec3(1.0f, 1.0f, 1.0f);
		parent = nullptr;
		recalculateTransformMatrix();
	}

	Transform::~Transform()
	{
	}

	//Recalculate the transform matrix with new supplied values
	void Transform::recalculateTransformMatrix()
	{
		glm::mat4x4 trans, rot, rotx, roty, rotz, scl;

		trans = glm::translate(trans, transformPosition);
		rotx = glm::rotate(rotx, glm::radians(transformRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		roty = glm::rotate(roty, glm::radians(transformRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		rotz = glm::rotate(rotz, glm::radians(transformRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		rot = roty * rotx * rotz;
		scl = glm::scale(scl, transformScale);

		transform = rot * scl * trans;
	}
}