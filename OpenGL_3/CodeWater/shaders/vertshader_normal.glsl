#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;
layout (location = 2) in vec2 uv_coordinates;

// Specify the Uniforms of the vertex shader
uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;
uniform mat3 normal_transformation;

// Specify the output of the vertex stage
out vec3 vertNormal;

void main()
{
    // gl_Position is the output (a vec4) of the vertex shader
    gl_Position = projectionTransform * viewTransform * modelTransform * vec4(vertCoordinates_in, 1.0);
    vertNormal = normal_transformation * vertNormal_in;
}
