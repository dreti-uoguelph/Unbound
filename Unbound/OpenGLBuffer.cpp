#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_OPENGL
#include"OpenGLBuffer.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace OpenGL
		{
			/*Creates an empty vertex buffer than new vertices can be loaded into*/
			const GLuint createNewVertexBuffer()
			{
				GLuint vbo;
				glGenBuffers(1, &vbo);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				return vbo;
			}

			/*Creates a new empty vertex array object for rendering*/
			const GLuint createNewVertexArrayObject()
			{
				GLuint vao;
				glGenVertexArrays(1, &vao);
				glBindVertexArray(vao);
				return vao;
			}

			/*Creates an empty index buffer that new indices can be loaded into*/
			const GLuint createNewIndexBuffer()
			{
				GLuint ibo;

				//Generate and bind the buffer
				glGenBuffers(1, &ibo);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
				return ibo;
			}
		}
	}
}
#endif