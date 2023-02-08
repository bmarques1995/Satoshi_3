#version 410

layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 col;

layout (location = 0) out vec4 p_Col;

void main()
{
    gl_Position = vec4(pos, 1.0f);
    p_Col = col;
}