    #version 430

    layout(location = 0) in vec3 aPosition;
    layout(location = 1) in vec2 aUV;

    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProjection;

	uniform vec3 aColor;
	out vec3 color;

    void main()
    {
        gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
		color = aColor;
    }