#pragma once
#include"stdafx.h"
#include"Subsystems.h"
#include"Scene.h"
#include"Model.h"

namespace Unbound
{
	class Game
	{
	public:
		static bool running;

		static void init();
		static void update();
		static int close();
	};
}