#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;
layout (location = 2) in vec2 uv_coord;

// Specify the Uniforms of the vertex shader
uniform mat4 modelViewTransform;
uniform mat4 projectionTransform;
uniform mat3 normal_transformation;

// Specify the output of the vertex stage
out vec3 vertNormal;
out vec2 uv_coordinates;

void main()
{
    // gl_Position is the output (a vec4) of the vertex shader
    vec3 vertex_coordinates = vertCoordinates_in;
    float frequency = 5.0;
    float amplitude = 0.5;
    float phase = M_PI;
    vertex_coordinates.z += amplitude * sin(2 * M_PI * frequency * uv_coord.x + phase);
    gl_Position = projectionTransform * modelViewTransform * vec4(vertex_coordinates, 1.0);
    vertNormal = vertNormal_in;
    uv_coordinates = uv_coord;
}
