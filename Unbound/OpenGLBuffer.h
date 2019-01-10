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
			/*Creates an empty vertex buffer than new vertices can be loaded into*/
			const GLuint createNewVertexBuffer();

			/*Creates a new empty vertex array object for rendering*/
			const GLuint createNewVertexArrayObject();

			/*Creates an empty index buffer that new indices can be loaded into*/
			const GLuint createNewIndexBuffer();
		}
	}
}
#endif