#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_OPENGL
#include"OpenGLDebug.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace OpenGL
		{
			/*A function to log any errors that are sent from opengl. If the error is severe enough, the program might exit*/
			void GraphicsErrorHandler(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
			{
				if (severity == GL_DEBUG_SEVERITY_HIGH)
				{
					Logger::log("Error: " + std::string(message));
					Logger::logError("Fatal error encountered, exiting program");
					exit(0);
				}

				Logger::log("Warning: " + std::string(message));
			}

			/*Sets up the debug callback that will handle error messages from opengl*/
			void setupErrorCallback()
			{
				glDebugMessageCallback(GraphicsErrorHandler, NULL);
			}
		}
	}
}
#endif