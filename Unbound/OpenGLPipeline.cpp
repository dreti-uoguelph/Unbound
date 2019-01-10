#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_OPENGL
#include"OpenGLPipeline.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace OpenGL
		{
			/*Initializes the vertex layout that will be used by opengl for rendering from the layout defined in vertex.h*/
			void setupOpenGLVertexLayout()
			{
				//The position vector of each vertex
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
				//The normal vector of each vertex
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));
				//The tangent vector of each vertex
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(glm::vec3)));
				//The color of each vertex
				glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(glm::vec3)));

				//The first set of UV coordinates for each vertex
				glVertexAttribPointer(4, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)(4 * sizeof(glm::vec3)));
				//The second set of UV coordinates for each vertex
				glVertexAttribPointer(5, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)(4 * sizeof(glm::vec3) + sizeof(glm::vec2)));

				//Enable all vertex attributes
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);
				glEnableVertexAttribArray(2);
				glEnableVertexAttribArray(3);
				glEnableVertexAttribArray(4);
				glEnableVertexAttribArray(5);
			}

			/*Sets up culling to hide back faces, front faces clockwise by default*/
			void setupCulling()
			{
				glEnable(GL_CULL_FACE);
				glFrontFace(GL_CW);
				glCullFace(GL_BACK);
			}

			/*Sets up the post-processing frame buffer*/
			void setupPostProcessingFramebuffer(Buffers& buffers, Images& images, GLuint screenWidth, GLuint screenHeight, short numMultisamplePasses)
			{
				glGenFramebuffers(1, &buffers.postProcessFrameBuffer);
				glBindFramebuffer(GL_FRAMEBUFFER, buffers.postProcessFrameBuffer);

				glGenTextures(1, &images.renderedOutput);
				glBindTexture(GL_TEXTURE_2D, images.renderedOutput);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

				GLuint depthRenderbuffer;
				glGenRenderbuffers(1, &depthRenderbuffer);
				glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

				glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, images.renderedOutput, 0);

				GLenum drawTargets[1] = { GL_COLOR_ATTACHMENT0 };
				glDrawBuffers(1, drawTargets);

				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				{
					Logger::log("Error: could not create framebuffer for rendering. Nothing will show on the screen!");
					return;
				}

				//glBindFramebuffer(GL_FRAMEBUFFER, buffers.postProcessFrameBuffer);
			}

			/*Activates the supplied shader program for future drawing operations*/
			void activateShaderProgram(const ShaderProgram& toActivate)
			{
				glUseProgram(toActivate.program);
			}

			/*Enables depth testing on the current framebuffer, which prevents artifacts*/
			void enableDepthTest()
			{
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LEQUAL);
			}

			/*Switches the direction that culling occurs in, going from clockwise to counter-clockwise and vice versa*/
			void switchCullDirection()
			{

			}

			/*Clears both the color and depth buffer to prepare a scene for drawing*/
			void clearScreen()
			{
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_DEPTH_BUFFER_BIT);
			}

			/*A quick way to bind a set of buffers needed for rendering. Binds all the args at once*/
			void bindBuffers(const GLuint& vertexBuffer, const GLuint& vertexArrayObject, const GLuint& indexBuffer)
			{
				glBindVertexArray(vertexArrayObject);
				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			}

			void bindBuffers(const GLuint& vertexArrayBuffer, const GLuint& indexBuffer)
			{
				glBindVertexArray(vertexArrayBuffer);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			}
		}
	}
}
#endif