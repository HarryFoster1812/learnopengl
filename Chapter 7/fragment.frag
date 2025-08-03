#version 330 core

uniform vec2 mouse_pos;    // in pixels, Y already flipped
uniform vec2 window_res;   // in pixels

in vec2 TexCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;

out vec4 FragColor;

void main()
{

    FragColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), 0.5);
}
