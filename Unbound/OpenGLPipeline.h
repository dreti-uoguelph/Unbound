#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_OPENGL
#include"OpenGLStructs.h"
#include"Vertex.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace OpenGL
		{
			/*Initializes the vertex layout that will be used by opengl for rendering from the layout defined in vertex.h*/
			void setupOpenGLVertexLayout();

			/*Sets up culling to hide back faces, front faces counter-clockwise by default*/
			void setupCulling();

			/*Sets up the post-processing frame buffer*/
			void setupPostProcessingFramebuffer(Buffers& buffers, Images& images, GLuint screenWidth, GLuint screenHeight, short numMultisamplePasses);

			/*Activates the supplied shader program for future drawing operations*/
			void activateShaderProgram(const ShaderProgram& toActivate);

			/*Enables depth testing on the current framebuffer, which prevents artifacts*/
			void enableDepthTest();

			/*Switches the direction that culling occurs in, going from clockwise to counter-clockwise and vice versa*/
			void switchCullDirection();

			/*Clears both the color and depth buffer to prepare a scene for drawing*/
			void clearScreen();

			/*A quick way to bind a set of buffers needed for rendering. Binds all the args at once*/
			void bindBuffers(const GLuint& vertexBuffer, const GLuint& vertexArrayObject, const GLuint& indexBuffer);
			void bindBuffers(const GLuint& vertexArrayBuffer, const GLuint& indexBuffer);
		}
	}
}
#endif