#version 330

in vec2 UV;						// texture coord (interpolated per fragment)

uniform sampler2D scene;

out vec4 color;					// final fragment color (output of that shader)

void main(){
	color = texture(scene, UV);
	return;
}