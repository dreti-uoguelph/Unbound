#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_OPENGL

namespace Unbound
{
	namespace Graphics
	{
		namespace OpenGL
		{
			/*A function to log any errors that are sent from opengl. If the error is severe enough, the program might exit*/
			void GraphicsErrorHandler(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

			/*Sets up the debug callback that will handle error messages from opengl*/
			void setupErrorCallback();
		}
	}
}
#endif