#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_OPENGL
#include"OpenGLStructs.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace OpenGL
		{
			/*Generate a new texture and return its ID*/
			const GLuint createNewTexture();

			/*Generate a new texture and load the supplied data into the texture*/
			const GLuint createNewTexture(const uint16_t width, const uint16_t height, const char* bytes);
		}
	}
}
#endif