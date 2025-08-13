#version 330 core

uniform vec2 mouse_pos;    // in pixels, Y already flipped
uniform vec2 window_res;   // in pixels

uniform vec3 lightColour;

out vec4 FragColor;

void main()
{

    FragColor = vec4(lightColour, 1.0);
}
