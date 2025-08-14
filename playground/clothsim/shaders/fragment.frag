#version 330 core

uniform vec3 colour;
uniform vec2 u_resolution;

out vec4 FragColor;

void main()
{
    vec2 fragPos = gl_FragCoord.xy / u_resolution;  // normalize 0..1
    float dist = distance(fragPos, vec2(0.5, 0.5));

    float intensity = 1.0 - smoothstep(0.0, 0.1, dist); // radius ~0.1 normalized

    vec3 highlight = vec3(0.0, 0.0, 1.0); 
    vec3 finalColor = mix(colour, highlight, intensity);

    FragColor = vec4(finalColor, 1.0);
}
