    #version 430

	uniform vec3 uColor;
    out vec4 outputColor;

	void main(void)
	{
        outputColor = vec4(uColor, 1.0);
	}