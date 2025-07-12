#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    // For now, we'll just use the positions directly.
    // Later, we'll multiply by a model-view-projection matrix.
    gl_Position = vec4(inPosition, 1.0);
    fragColor = inColor;
}