#version 410

layout (location = 0) in vec4 p_Col;

layout (location = 0) out vec4 col;

void main()
{
    col = p_Col;
}