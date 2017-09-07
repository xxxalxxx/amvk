#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in vec3 inPosition;
layout(location = 2) in vec3 inNormal;

layout (location = 0) out float outPosition;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec4 outAlbedo;

void main() {
	outPosition = gl_FragCoord.z;
	outNormal = vec4(inNormal, 0.0);
	outAlbedo = texture(texSampler, inTexCoord);
}
