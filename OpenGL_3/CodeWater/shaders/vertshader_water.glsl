#version 330 core

// Define constants
const float M_PI = 3.141593;

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;
layout (location = 2) in vec2 uv_coord;

// Specify the Uniforms of the vertex shader
uniform mat4 modelViewTransform;
uniform mat4 projectionTransform;
uniform mat3 normal_transformation;
uniform int numberOfWaves;
uniform float amplitude[8];
uniform float frequency[8];
uniform float phase[8];
uniform float time;

uniform vec3 light_position;
uniform vec3 light_intensity;

// Specify the output of the vertex stage
out vec3 vertNormal;
out vec3 normal;
out vec2 uv_coordinates;
out vec3 light_direction;
out vec3 half_2;

float waveHeight(int waveIdx, float uvalue){
    return amplitude[waveIdx] * sin(2 * M_PI * (frequency[waveIdx] * uvalue + time) + phase[waveIdx]);

}

float waveDU(int waveIdx, float uvalue){
    return amplitude[waveIdx] * 2 * M_PI * frequency[waveIdx] * cos(2 * M_PI * (frequency[waveIdx] * uvalue + time) + phase[waveIdx]);

}

void main()
{
    // gl_Position is the output (a vec4) of the vertex shader
    vec4 pos = modelViewTransform * vec4(vertCoordinates_in, 1.0);
    gl_Position = projectionTransform * pos;

    vec4 light_pos = vec4(light_position, 1.0);
    normal = normal_transformation * vertNormal_in;
    vec3 v = normalize(-pos.xyz);
    light_direction = normalize(light_pos.xyz - pos.xyz);
    half_2 = normalize(v + light_direction);


    // gl_Position is the output (a vec4) of the vertex shader
    vec3 vertex_coordinates = vertCoordinates_in;

    for (int w = 0; w < numberOfWaves; w++) {
        vertex_coordinates.z += waveHeight(w, uv_coord.x);
    }

    float dU = 0.0;
    for (int w = 0; w < numberOfWaves; w++) {
        dU += waveDU(w, uv_coord.x);
    }
    float dV = 0.0;
    vertNormal = normalize(vec3(-dU, -dV, 1.0));

    uv_coordinates = uv_coord;
}
