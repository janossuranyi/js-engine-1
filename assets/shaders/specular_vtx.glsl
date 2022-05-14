#version 330 core


// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 va_Position;
layout(location = 1) in vec3 va_Normal;
layout(location = 2) in vec3 va_Tangent;
layout(location = 3) in vec3 va_Bitangent;
layout(location = 4) in vec2 va_TexCoord;


out VertexData {
	vec2 TexCoord;
	vec3 Tangent;
	vec3 Bitangent;
	vec3 worldPosition;
	vec3 normal;
} vofi;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 NM;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position = MVP * vec4(va_Position, 1.0);

	vofi.TexCoord = va_TexCoord;
	vofi.Tangent = va_Tangent;
	vofi.Bitangent = va_Bitangent;
	vofi.worldPosition = vec3(M * vec4(va_Position, 1.0));
	vofi.normal = (NM * vec4(va_Normal, 0.0)).xyz;
}
