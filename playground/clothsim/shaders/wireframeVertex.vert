#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float uDepthBias; // try 1e-4 .. 2e-3

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);
    vec4 clipPos  = projection * view * worldPos;

    // Pull slightly toward the camera in depth:
    clipPos.z -= uDepthBias * clipPos.w;

    gl_Position = clipPos;
}
