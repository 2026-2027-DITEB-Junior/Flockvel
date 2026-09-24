#version 430

	#define MAX_LIGHTS 8
	
	const int POINT_LIGHT = 1;
	const int DIRECTIONAL_LIGHT = 2;
	const int SPOT_LIGHT = 3;

	const float ambient = 0.1;

	in vec2 vUV;
	in vec3 vFragPos;
	in vec3 vNormal;
	in vec3 vTangent;
	in vec3 vBitangent;
	in vec4 vFragPosLightSpace;

	uniform float uShininess;

	out vec3 outputColor;

	uniform vec3 uCamPos;
	
	uniform sampler2D uTexture;
	uniform sampler2D uNormalMap; 
	uniform sampler2D uShadowMap;
	uniform float uShadowBias;
	uniform int uPCFSamples;

	uniform int uDrawMode;

	uniform  int uLightNum;
	
	in vec3 normal_camera;
	in vec3 tangent_camera;
	in vec3 bitangent_camera;

	layout (std140, binding = 0) uniform aLightInfo{
		
		vec4 color[MAX_LIGHTS];
		vec4 position[MAX_LIGHTS];
		vec4 direction[MAX_LIGHTS];
		vec4 attenuation[MAX_LIGHTS];
		vec4 params[MAX_LIGHTS];
	};

	float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; 
    projCoords = projCoords * 0.5 + 0.5; 

    if (projCoords.z > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;

    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
    float shadow = 0.0;
    int samples = 0;

    for (int x = -uPCFSamples; x <= uPCFSamples; ++x)
    {
        for (int y = -uPCFSamples; y <= uPCFSamples; ++y)
        {
            float D = texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (currentDepth - uShadowBias > D) ? 1.0 : 0.0;
            samples++;
        }
    }
    return shadow / float(samples);
}

	void calculate_light(inout vec3 ambientL, inout vec3 diffuseL, inout vec3 specularL){
		
	    vec3 T = normalize(vTangent);
		vec3 B = normalize(vBitangent);
		vec3 gN = normalize(vNormal);
		mat3 TBN = mat3(T, B, gN);
		    
		vec3 local_normal = texture(uNormalMap, vUV).rgb;
		local_normal = normalize(local_normal * 2.0 - 1.0);   
		
		vec3 N = (uDrawMode == 1) ? normalize(TBN * local_normal) : gN;
		
		vec3 L;
		vec3 V = normalize(uCamPos - vFragPos);
		vec3 R;
		
		for(int i = 0; i < uLightNum; ++i){
			
			//vecrtors ----------------------------------
			vec3 pos_to_light = position[i].xyz - vFragPos;
			int  lightType    = int(params[i].x);
			
			if(lightType == DIRECTIONAL_LIGHT)
			    L = normalize(-direction[i].xyz);
			else
			    L = normalize(pos_to_light);

			R = reflect(-L, N);
			
			float dotN_L = max(dot(N, L), 0.0);

			//attenuation ------------------------------------
			float atte = 1.0;
			if(lightType != DIRECTIONAL_LIGHT)
			{
			    float d = length(pos_to_light);
			    atte = min(1.0 / (attenuation[i].x + attenuation[i].y * d + attenuation[i].z * d * d), 1.0);
			}
			
			//spotlight ----------------------------------------
			float spotFactor = 1.0;
			if(lightType == SPOT_LIGHT)
			{
			    float cos_alpha   = dot(L, normalize(-direction[i].xyz));
			    float inner_outer = params[i].y - params[i].z;
			    float intensity = (cos_alpha - params[i].z) / inner_outer;
				intensity = clamp(intensity, 0.0, 1.0);

			    spotFactor = pow(intensity, params[i].w);
			}
			
			//final ------------------------------------------------
			vec3 Iambient  = color[i].xyz * max(color[i].w, ambient); //if I don't do this, everithing is so dark
			vec3 Idiffuse  = color[i].xyz * dotN_L * spotFactor;
			vec3 Ispecular = vec3(0.0);

			if(dotN_L > 0.0){ 
				Ispecular = vec3(1.0) * pow(max(dot(R, V), 0.0), uShininess) * spotFactor;
			}
			
			ambientL  += Iambient  * atte;
			diffuseL  += Idiffuse  * atte;
			specularL += Ispecular * atte;
		}
	}

    void main()
    {
		if(uDrawMode == 2)
		{
		    outputColor = normalize(normal_camera);
		    return;
		}
		else if(uDrawMode == 3)
		{
		    outputColor = normalize(tangent_camera);
		    return;
		}
		else if(uDrawMode == 4)
		{
		    outputColor = normalize(bitangent_camera);
		    return;
		}
		
		vec3 texColor = texture(uTexture, vUV).rgb;

		vec3 ambientL = vec3(0.0);
		vec3 diffuseL = vec3(0.0);
		vec3 specularL = vec3(0.0);

		vec3 Ka = texColor;
		vec3 Kd = texColor;
		vec3 Ks = vec3(1.0);

		calculate_light(ambientL, diffuseL, specularL);

		float shadow = ShadowCalculation(vFragPosLightSpace);

		outputColor = ambientL * Ka + (1.0 - shadow) * (diffuseL * Kd + specularL * Ks); 
		outputColor = clamp(outputColor, 0.0, 1.0);
    }