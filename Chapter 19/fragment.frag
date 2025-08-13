#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_diffuse4;

uniform float diffuseWeights[4];  // blend weights for each texture
uniform int diffuseCount;         // how many textures to blend (<=4)

void main() {
    vec4 blendedColor = vec4(0.0);
    float totalWeight = 0.0;

    if (diffuseCount >= 1) {
        vec4 c = texture(texture_diffuse1, TexCoords);
        if (c.a < 0.1) discard;
        blendedColor += c * diffuseWeights[0];
        totalWeight += diffuseWeights[0];
    }
    if (diffuseCount >= 2) {
        vec4 c = texture(texture_diffuse2, TexCoords);
        if (c.a < 0.1) discard;
        blendedColor += c * diffuseWeights[1];
        totalWeight += diffuseWeights[1];
    }
    if (diffuseCount >= 3) {
        vec4 c = texture(texture_diffuse3, TexCoords);
        if (c.a < 0.1) discard;
        blendedColor += c * diffuseWeights[2];
        totalWeight += diffuseWeights[2];
    }
    if (diffuseCount >= 4) {
        vec4 c = texture(texture_diffuse4, TexCoords);
        if (c.a < 0.1) discard;
        blendedColor += c * diffuseWeights[3];
        totalWeight += diffuseWeights[3];
    }

    if (totalWeight > 0.0)
        blendedColor /= totalWeight;
    else
        discard; // no visible fragments

    FragColor = blendedColor;
}
