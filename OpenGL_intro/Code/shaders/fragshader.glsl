#version 330 core
in vec3 interpolatedColor;
out vec4 value;

void main()
{
    value = vec4(interpolatedColor,1);

}
