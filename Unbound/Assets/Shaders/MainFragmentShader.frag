#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler textureSampler;
layout(binding = 2) uniform texture2D textures[4096];

layout(push_constant) uniform PushConstants
{
	uint albedoID;
	mat4 world;
} pcs;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(sampler2D(textures[pcs.albedoID], textureSampler), fragTexCoord) * vec4(fragColor, 1.0);
}