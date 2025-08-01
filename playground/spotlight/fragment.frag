#version 330 core

uniform vec2 mouse_pos;    // in pixels, Y already flipped
uniform vec2 window_res;   // in pixels

out vec4 FragColor;

void main()
{
    vec2 fragPos = gl_FragCoord.xy;

    float dist = distance(fragPos, mouse_pos);

    // Adjust radius and softness here (in pixels)
    float intensity = 1.0 - smoothstep(20.0, 40.0, dist);

    FragColor = vec4(vec3(intensity), 1.0);
}
