#version 430 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec3 inColor;
layout(location = 4) in vec2 inUV;
layout(location = 5) in vec2 inUV2;

layout(location = 0) out vec2 vertexUV1;
layout(location = 1) out vec2 vertexUV2;

void main()
{
	gl_Position = vec4(inPosition, 1.0);
	vertexUV1 = inUV;
	vertexUV2 = inUV2;
}