#pragma once
#include"stdafx.h"

namespace Unbound
{
	class GameObject
	{
	public:
		//Called when the GameObject is created or added to the scene
		virtual void init() = 0;
		//Called when the GameObject is updated
		virtual void update() = 0;
		//Called when the GameObject is removed from the scene or when the game closes
		virtual void close() = 0;

		//Set the interval, in milliseconds that the object should be updated at
		//A value of 0 means the object is updated every frame, and a negative value stops the object from being updated
		void setUpdateInterval(time_t interval);

		//Get the update interval, as a time_t
		time_t getUpdateInterval();

	protected:
		time_t updateInterval = static_cast<time_t>(0.0); //The interval the object is updated at
	};
}