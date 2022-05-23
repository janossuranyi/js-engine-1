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
    vec3 position;
    vec3 diffuse;
    vec3 specular;

    float kc;
    float kl;
    float kq;
};

uniform Light light;
uniform vec3 viewPos;
uniform Material material;

const float cutoff = 0.0005;

void main() {

    vec3 N = normalize(vofi.normal);
    vec3 L = light.position - vofi.worldPosition;
    vec3 E = normalize(viewPos - vofi.worldPosition);
  	float distance = length( L );
    L /= distance;

    //vec3 R = reflect(-L, N);  


    // calculate basic attenuation
    float denom = light.kc + light.kl * distance + light.kq * distance * distance;
    float attenuation = 1.0 / denom;

    // scale and bias attenuation such that:
    //   attenuation == 0 at extent of max influence
    //   attenuation == 1 when d == 0
    attenuation = (attenuation - cutoff) / (1.0 - cutoff);
    attenuation = max(attenuation, 0.0);
     
    vec3 ambient = material.diffuse * 0.01;
    float diff = max(dot(L, N), 0);
    vec3 H = normalize(L + E);
    //float spec = pow(max(dot(E, R), 0), material.shininess);
    float spec = pow(max(dot(N, H), 0), material.shininess);
    //float spec = 0.0;  
    vec3 diffuse = max(ambient, material.diffuse * light.diffuse * diff * attenuation);
    vec3 specular = material.specular * light.specular * spec * attenuation;


    vec3 result = material.ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);

}
