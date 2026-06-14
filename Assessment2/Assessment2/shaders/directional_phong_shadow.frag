#version 450 core

layout (location = 0) out vec4 fColour;

in vec2 tex;
in vec3 nor;
in vec3 FragPosWorldSpace;
in vec4 FragPosProjectedLightSpace;
in mat3 TBN;

uniform vec3 lightDirection;
uniform vec3 lightColour;
uniform vec3 lightPos;
uniform vec3 camPos;

uniform sampler2D shadowMap;
uniform sampler2D textureMap;
uniform sampler2D normalMap;

vec2 poissonDisk[4] = vec2[](
  vec2(-0.94201624, -0.39906216),
  vec2(0.94558609, -0.76890725),
  vec2(-0.094184101, -0.92938870),
  vec2(0.34495938, 0.29387760)
);

float shadowOnFragment(vec4 FragPosProjectedLightSpace) {
	vec3 ndc = FragPosProjectedLightSpace.xyz / FragPosProjectedLightSpace.w;
	vec3 ss = (ndc + 1) * 0.5f;

	float fragDepth = ss.z;
	if (fragDepth > 1.0f) {
		return 0.0f;
	}

	vec3 Nnor = normalize(nor);
	vec3 Nto_light = normalize(-lightDirection);
	float bias = max(0.05f * (1.0f - dot(Nnor, Nto_light)), 0.005f);

	float shadow = 0.0f;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

	//Poisson Disk Sampling
	float spread = 2.5f;
	for (int i = 0; i < 4; i++) {
		float pcfDepth = texture(shadowMap, ss.xy + (poissonDisk[i] * texelSize * spread)).r;
		if (fragDepth > (pcfDepth + bias)) {
			shadow += 1.0f;
		}
	}
	return shadow / 4.0;
}

float CalculateDirectionalIllumination() {
	//Ambient
	float ambient = 0.1f;

	//Diffuse
	vec3 tangentNormal = texture(normalMap, tex).rgb * 2.0 - 1.0;
	vec3 worldNormal  = normalize(TBN * tangentNormal);

	vec3 Nto_light = normalize(-lightDirection);
	float diffuse = max(dot(worldNormal, Nto_light), 0.0);

	//Specular
	vec3 Nfrom_light = normalize(lightDirection);
	vec3 NrefLight = reflect(Nfrom_light, worldNormal);

	vec3 camDirection = camPos - FragPosWorldSpace;
	vec3 NcamDirection = normalize(camDirection);
	float specular = pow(max(dot(NcamDirection, NrefLight), 0.0f), 128.0f);

	//Shadow
	float shadow = shadowOnFragment(FragPosProjectedLightSpace);

	//Phong
	float phong = ambient + ((1.0f - shadow) * (diffuse + specular));
	return phong;
}

void main() {
	float phong = CalculateDirectionalIllumination();

	vec3 col = texture(textureMap, tex).rgb;
	fColour = vec4(phong * col * lightColour, 1.0);
}
