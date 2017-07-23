#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform sampler2D samplerPosition;
layout (binding = 1) uniform sampler2D samplerNormal;
layout (binding = 2) uniform sampler2D samplerAlbedo;

layout(location = 0) in vec2 inTextCoord;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(samplerPosition, inTextCoord);
}
