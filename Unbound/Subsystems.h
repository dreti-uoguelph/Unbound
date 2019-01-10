#pragma once
#include"Config.h"
#include"Renderer.h"
#include"Input.h"
#include"Util.h"
#include"logger.h"
#include"Time.h"
#include"TaskScheduler.h"
#include"MeshLoader.h"
#include"ImageLoader.h"
#include"MemoryManager.h"
#include"Terrain.h"
#include"AudioManager.h"

namespace Unbound
{
	void initSubsystems();
	void updateSubsystems();
	void closeSubsystems();
}