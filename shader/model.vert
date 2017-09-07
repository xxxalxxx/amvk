#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec3 inBitangent;
layout(location = 4) in vec2 inTexCoord;


layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out vec3 outPosition;
layout(location = 2) out vec3 outNormal;

vec2 packNormal(vec3 n) 
{
	return normalize(n.xy) * sqrt(n.z * 0.5 + 0.5);
}

void main() { 
	mat4 transform = ubo.view * ubo.model;
	mat3 normalTransform = transpose(inverse(mat3(transform)));
    vec4 position = transform * vec4(inPosition, 1.0);

    outTexCoord = inTexCoord;
	outPosition = vec3(position);
	
	vec3 normal = normalTransform * inNormal;
	
	outNormal = normal;

	gl_Position = ubo.proj * position;
}
