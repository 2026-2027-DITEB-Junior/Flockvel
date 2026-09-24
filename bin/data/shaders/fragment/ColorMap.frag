#version 330

in vec2 UV;						// texture coord (interpolated per fragment)
in vec4 vtxCol;					// vertex color	 (interpolated per fragment)

uniform vec4 drawColor;

out vec4 color;					// final fragment color (output of that shader)

void main(){
	color += drawColor;
	return;
}