#version 330 core

layout(location = 0) in vec3 va_Position;

uniform mat4 MVP;

void main()
{
  gl_Position = MVP * vec4(va_Position, 1.0);
}
