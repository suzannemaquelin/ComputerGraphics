#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 vertNormal;
in vec2 uv_coordinates;
in vec3 normal;

// Specify the Uniforms of the fragment shaders
// uniform vec3 lightPosition; // for example

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fColor;

void main()
{
    //fColor = vec4(normalize(vertNormal), 1.0);
    //fColor = vec4(uv_coordinates, 0.0, 1.0);
    fColor = vec4(normal, 1.0);
}
