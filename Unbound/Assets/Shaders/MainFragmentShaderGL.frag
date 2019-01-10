#version 430 core

layout(location = 0) out vec4 outColor;

in vec3 vertexColor;
in vec2 vertexUV1;
in vec2 vertexUV2;

uniform	sampler2D albedo;

void main()
{
	vec4 color = texture(albedo, vertexUV1);
	color.x = pow(color.x, 1.0 / 2.2);
	color.y = pow(color.y, 1.0 / 2.2);
	color.z = pow(color.z, 1.0 / 2.2);
    outColor = color;
}