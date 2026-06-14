#version 450 core

layout (location = 0) out vec4 fColour;

in vec3 col;
in vec3 nor;
in vec3 FragPosWorldSpace;

uniform vec3 lightDirection;
uniform vec3 lightColour;
uniform vec3 lightPos;
uniform vec3 camPos;

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

	float phong = ambient + diffuse + specular;
	return phong;
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

void main()
{
	float phong = CalculatePositionalIllumination();
	fColour = vec4(phong * col * lightColour, 1.f);
}
