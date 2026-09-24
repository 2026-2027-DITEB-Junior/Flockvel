#version 330

in vec2 UV;						// texture coord (interpolated per fragment)
in vec4 vtxCol;					// vertex color	 (interpolated per fragment)

uniform sampler2D texSampler;
uniform vec4 tintColor;
uniform float tintIntensity = 0.f;

out vec4 color;					// final fragment color (output of that shader)

void main(){
	//color = vec4(0.2f, 0.2f, 0.2f, 1);
	//color = vec4(UV.x, UV.y, 0, 1);
	color = texture(texSampler, UV) * (1.f - tintIntensity) + tintColor * tintIntensity;
	return;
}