#version 430 core

layout(location = 0) out vec4 outColor;

in vec3 vertexColor;
in vec2 vertexUV1;
in vec2 vertexUV2;

layout (std140) uniform cameraSettings
{
	vec3 tint; //A color to multiply the final output by, helps color grading
	vec3 liftGammaGain;
	float exposure; //The overall brightness of the scene in f-stops
	float saturation; //Represents how saturated the image is (grey to colorfull)
	float contrast; //Represents the contrast of the image (push values away from grey)
	float gamma; //The gamma that is used to convert colors for showing on the display, usually 2.2
} pInput;

uniform sampler2D rawImage;

void main()
{
	vec4 startColor = texture(rawImage, vertexUV1);
	startColor = vec4(startColor.xyz * tint, 1.0);

	startColor.x = pow(startColor.x, 2.2);
	startColor.y = pow(startColor.y, 2.2);
	startColor.z = pow(startColor.z, 2.2);
	outColor = startColor;
}