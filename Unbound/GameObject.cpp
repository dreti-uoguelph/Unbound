#include "stdafx.h"
#include "GameObject.h"

namespace Unbound
{
	//Set the interval, in milliseconds that the object should be updated at
	void GameObject::setUpdateInterval(time_t interval)
	{
		updateInterval = interval;
	}

	//Get the update interval, as a float
	time_t GameObject::getUpdateInterval()
	{
		return updateInterval;
	}
}