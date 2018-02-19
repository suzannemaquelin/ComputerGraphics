#version 330 core

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertColor_in;


// Specify the Uniforms of the vertex shader
// uniform mat4 modelTransform; for example
uniform mat4 translation;
uniform mat4 perspective;
uniform mat4 rotation;
uniform mat4 scale;

// Specify the output of the vertex stage
out vec3 vertColor;

void main()
{
    // gl_Position is the output (a vec4) of the vertex shader
    gl_Position = perspective * translation * scale * rotation * vec4(vertCoordinates_in, 1.0);
    vertColor = vertColor_in;
}
