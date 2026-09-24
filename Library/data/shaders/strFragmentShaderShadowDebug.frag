#version 430
in vec2 vUV;
out vec3 outputColor;

uniform sampler2D uDepthMap;

void main()
{
    float depth = texture(uDepthMap, vUV).r;
    outputColor = vec3(pow(depth, 200.0));
}