#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform ProjectionMatrices
{
	mat4 view;
	mat4 projection;
} mat;

layout(push_constant) uniform PushConstants
{
	layout(offset = 0) uint albedoID;
	layout(offset = 16) mat4 world;
} pcs;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec3 inColor;
layout(location = 4) in vec2 inUV;
layout(location = 5) in vec2 inUV2;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec2 fragTexCoord;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	gl_Position = mat.projection * mat.view * pcs.world * vec4(inPosition, 1.0f);
    normal = inNormal;
    fragColor = inColor;
    fragTexCoord = inUV;
}