#version 430

	#define MAX_LIGHTS 8

    layout(location = 0) in vec3 aPosition;
    layout(location = 1) in vec2 aUV;
    layout(location = 2) in vec3 aNormal;
    layout(location = 3) in vec3 tangent;
    layout(location = 4) in vec3 bitangent;

    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProjection;
    uniform mat4 uLightSpaceMatrix;

    out vec2 vUV;
	out vec3 vFragPos;
	out vec3 vNormal;
    out vec3 vTangent;
    out vec3 vBitangent;
    out vec4 vFragPosLightSpace;
    
	out vec3 normal_camera;
	out vec3 tangent_camera;
	out vec3 bitangent_camera;

    void main()
    {
        gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
        vUV = aUV;
    
        mat3 inverse_tras = inverse(transpose(mat3(uView * uModel)));

        normal_camera = inverse_tras * aNormal;
        tangent_camera = inverse_tras * tangent;
        bitangent_camera = inverse_tras * bitangent;
	
        vec4 worldPos = uModel * vec4(aPosition, 1.0);
        vFragPos = worldPos.xyz;
        vFragPosLightSpace = uLightSpaceMatrix * worldPos;

        vec3 N = normalize(mat3(transpose(inverse(uModel))) * aNormal);
        vNormal = N;

        vec3 T = normalize(mat3(uModel) * tangent);
        T = normalize(T - dot(T, N) * N);
        vTangent = T;

        vec3 B = cross(N, T);

        if (dot(cross(N, T), mat3(uModel) * bitangent) < 0.0) {
            B = -B;
        }
        vBitangent = B;
    }