#version 450 core

layout (location = 0) out vec4 fColour;

in vec3 col;
in vec3 nor;
in vec3 FragPosWorldSpace;
in vec4 FragPosProjectedLightSpace;

uniform sampler2D shadowMap;

uniform vec3 lightDirection;
uniform vec3 lightColour;
uniform vec3 lightPos;
uniform vec3 camPos;

float shadowOnFragment(vec4 FragPosProjectedLightSpace)
{
	vec3 ndc = FragPosProjectedLightSpace.xyz / FragPosProjectedLightSpace.w;
	vec3 ss = (ndc + 1) * 0.5f;

	float fragDepth = ss.z;
	if (fragDepth > 1.0f)
	{
		return 0.0f;
	}

	vec3 Nnor = normalize(nor);
	vec3 Nto_light = normalize(-lightDirection);
	float bias = max(0.05f * (1.0f - dot(Nnor, Nto_light)), 0.005f);

	float shadow = 0.0f;
	
	//3x3 Percentage-Closer Filtering
	vec2 texelSize = 1.0f / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, ss.xy + vec2(x, y) * texelSize).r; 
            if (fragDepth > (pcfDepth + bias))
            {
                shadow += 1.0f; 
            }        
        }    
    }
	shadow /= 9.0f; // Average the shadow factor over the 3x3 kernel

	//Normal Shadow Mapping
	/*float litDepth = texture(shadowMap, ss.xy).r;
	if (fragDepth > (litDepth + bias))
	{
		shadow = 1.0f; //Shadow
	}*/
	return shadow;
}

float CalculateDirectionalIllumination()
{
	float ambient = 0.1f;

	vec3 Nnor = normalize(nor);
	vec3 Nto_light = normalize(-lightDirection);
	
	float diffuse = max(dot(Nnor, Nto_light), 0.0f);

	vec3 Nfrom_light = normalize(lightDirection);
	vec3 NrefLight = reflect(Nfrom_light, Nnor);

	vec3 camDirection = camPos - FragPosWorldSpace;
	vec3 NcamDirection = normalize(camDirection);

	float specular = pow(max(dot(NcamDirection, NrefLight), 0.0f), 128.0f);

	float shadow = shadowOnFragment(FragPosProjectedLightSpace);
	float phong = ambient + ((1.0f - shadow) * (diffuse + specular));
	return phong;
}

void main()
{
	float phong = CalculateDirectionalIllumination();
	fColour = vec4(phong * col * lightColour, 1.f);
}

float CalculatePositionalIllumination()
{
	float ambient = 0.1f;

	vec3 Nnor = normalize(nor);
	vec3 Nto_light = normalize(lightPos - FragPosWorldSpace);
	
	float diffuse = max(dot(Nnor, Nto_light), 0.0f);

	vec3 Nfrom_light = normalize(-Nto_light);
	vec3 NrefLight = reflect(Nfrom_light, Nnor);

	vec3 camDirection = camPos - FragPosWorldSpace;
	vec3 NcamDirection = normalize(camDirection);

	float specular = pow(max(dot(NcamDirection, NrefLight), 0.0f), 128.0f);

	float att_c = 1.f;
	float att_l = 0.05f;
	float att_q = 0.002f;
	float d = length(lightPos - FragPosWorldSpace);

	float attenuation = 1 / (att_c + ((att_l * d) + (att_q * sqrt(d))));

	float phong = (ambient + diffuse + specular) * attenuation;
	return phong;
}

float CalculateSpotIllumination()
{
	float ambient = 0.1f;

	vec3 Nnor = normalize(nor);
	vec3 Nto_light = normalize(lightPos - FragPosWorldSpace);
	
	float diffuse = max(dot(Nnor, Nto_light), 0.0f);

	vec3 Nfrom_light = normalize(-Nto_light);
	vec3 NrefLight = reflect(Nfrom_light, Nnor);

	vec3 camDirection = camPos - FragPosWorldSpace;
	vec3 NcamDirection = normalize(camDirection);

	float specular = pow(max(dot(NcamDirection, NrefLight), 0.0f), 128.0f);

	float att_c = 1.f;
	float att_l = 0.05f;
	float att_q = 0.002f;
	float d = length(lightPos - FragPosWorldSpace);

	float attenuation = 1 / (att_c + ((att_l * d) + (att_q * sqrt(d))));

	float cut_off = 15.0;
	float phi = cos(radians(cut_off));

	vec3 NSpotDir = normalize(lightDirection);

	float theta = dot(Nfrom_light, NSpotDir);

	float phong;
	if (theta > phi)
	{
		phong = (ambient + diffuse + specular) * attenuation;
	}
	else 
	{
		phong = ambient * attenuation;
	}

	return phong;
}


