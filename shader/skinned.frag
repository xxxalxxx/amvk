#version 450
#extension GL_ARB_separate_shader_objects : enable
#define SAMPLER_LIST_SIZE 16
layout(set = 1, binding = 0) uniform sampler2D texSampler[SAMPLER_LIST_SIZE];

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) flat in uvec4 inSamplerIndices;
layout(location = 2) in vec3 inPosition;
layout(location = 3) in vec3 inNormal;

layout (location = 0) out float outPosition;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec4 outAlbedo;

void main() {
	outPosition = gl_FragCoord.z;
	outNormal = vec4(inNormal, 0.0);
	outAlbedo = texture(texSampler[inSamplerIndices.x], inTexCoord);
}
