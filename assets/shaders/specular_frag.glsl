#version 330 core

// Ouput data
out vec4 FragColor;

in VertexData {
	vec2 TexCoord;
	vec3 Tangent;
	vec3 Bitangent;
	vec3 worldPosition;
	vec3 normal;
} vofi;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec4 position;
    vec4 diffuse;
    vec4 specular;

    float kl;
    float kq;
    float cutoff;
    float pad0;
}; 
// 4*16 = 64

uniform int numLights;

layout(std140) uniform LightBuffer {
    Light lights[256];
};

uniform vec3 viewPos;
uniform Material material;

vec3 N,E;
const float gamma = 2.2;
const vec3 fogColor = vec3(0.5, 0.5, 1.0);
float depth;

vec3 calcPointLight(Light light)
{
        vec3 L = light.position.xyz - vofi.worldPosition;
  	    float distance = length( L );
        float distance2 = distance*distance;

        L /= distance;

        //vec3 R = reflect(-L, N);  

        // calculate basic attenuation
        // 3.3-ban van ? float denom = fma(light.kq, distance2, fma(light.kl, distance, light.kc));

        float attenuation = 1.0 / (1.0 + light.kl * distance + light.kq * distance2);
        //float attenuation = 1.0 / (1.0 + distance2);

        // scale and bias attenuation such that:
        //   attenuation == 0 at extent of max influence
        //   attenuation == 1 when d == 0

        attenuation = (attenuation - light.cutoff) / (1.0 - light.cutoff);
        attenuation = max(attenuation, 0);
     
        float diff = max(dot(L, N), 0);
        vec3 H = normalize(L + E);
        //float spec = pow(max(dot(E, R), 0), material.shininess);
        float spec = pow(max(dot(N, H), 0), material.shininess);
        vec3 diffuse = material.diffuse * light.diffuse.rgb * diff * attenuation;
        vec3 specular = material.specular * light.specular.rgb * spec * attenuation;

        return diffuse + specular;
        
}

float getFogFactor(float d)
{
    const float FogMax = 30.0;
    const float FogMin = 20.0;

    if (d>=FogMax) return 1;
    if (d<=FogMin) return 0;

    return 1 - (FogMax - d) / (FogMax - FogMin);
}

void main() {

    N = normalize(vofi.normal);
    E = normalize(viewPos - vofi.worldPosition);
    vec3 result = material.ambient;

    //result = pow(result, vec3(1.0/gamma));

    for(int i=0; i<numLights; ++i)
    {
        result += calcPointLight(lights[i]);
    }

    float d = distance(viewPos, vofi.worldPosition);
    float alpha = getFogFactor(d);

    FragColor = vec4(mix(result, fogColor, alpha), 1);

}
