#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 2, binding = 0) uniform LightUniformBufferObject {
	vec3 color;
	vec3 position;
	float specPower;
	float quadratic, linear, constant; 
} light;

layout(location = 0) in vec4 inPosition;

layout(location = 0) out vec4 outColor;

void main() {
	vec3 ndc = inPosition.xyz / inPosition.w;
	if (ndc.x < 0.0 || ndc.y < 0.0) {
		discard;
	}
    outColor = vec4(light.color, 1.0);
}
