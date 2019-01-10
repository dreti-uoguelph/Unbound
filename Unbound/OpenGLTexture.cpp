#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_OPENGL
#include"OpenGLTexture.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace OpenGL
		{
			/*Generate a new texture and return its ID*/
			const GLuint createNewTexture()
			{
				//Generate the texture
				GLuint texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);

				//Set texture parameters
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				return texture;
			}

			/*Generate a new texture and load the supplied data into the texture*/
			const GLuint createNewTexture(const uint16_t width, const uint16_t height, const char* bytes)
			{
				GLuint texture = createNewTexture();
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
				glGenerateMipmap(GL_TEXTURE_2D);
				return texture;
			}
		}
	}
}
#endif