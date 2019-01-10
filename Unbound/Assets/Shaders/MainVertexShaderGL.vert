#version 430 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec3 inColor;
layout(location = 4) in vec2 inUV;
layout(location = 5) in vec2 inUV2;

layout (binding = 0, std140) uniform worldViewProjection
{
	mat4 world;
	mat4 view;
	mat4 projection;
} wvp;

out vec3 vertexColor;
out vec2 vertexUV1;
out vec2 vertexUV2;

void main()
{
	gl_Position = wvp.projection * wvp.view * wvp.world * vec4(inPosition, 1.0);
	vertexColor = inColor;
	vertexUV1 = inUV;
	vertexUV2 = inUV2;
}