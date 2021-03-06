// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
#pragma once
#define UNBOUND_VERSION_MAJOR 1
#define UNBOUND_VERSION_MINOR 0
#define UNBOUND_VERSION_PATCH 0
//#define UNBOUND_BACKEND_VULKAN
#define UNBOUND_BACKEND_OPENGL
#define UNBOUND_EDITOR

#include "targetver.h"
#include <iostream>
#include <string>
#include <vector>
#include <mutex>

#define GLM_FORCE_RADIANS
#ifdef UNBOUND_BACKEND_VULKAN
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLFW_INCLUDE_VULKAN
#endif
#ifdef UNBOUND_BACKEND_OPENGL
#include <glad\glad.h>
#endif

#if defined UNBOUND_BACKEND_OPENGL || defined UNBOUND_BACKEND_VULKAN
#include <GLFW\glfw3.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm\glm.hpp>
#endif

#include"Logger.h"
#include"FileSystem.h"