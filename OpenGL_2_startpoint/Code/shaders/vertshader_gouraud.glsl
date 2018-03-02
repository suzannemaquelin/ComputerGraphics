#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;

// Specify the Uniforms of the vertex shader
 uniform mat4 modelViewTransform;
 uniform mat4 projectionTransform;
 uniform mat3 normal_transformation;

 uniform vec3 light_position;
 uniform vec3 light_intensity;
 uniform vec3 material_Ia;
 uniform vec4 material_k;

// Specify the output of the vertex stage
out vec3 vertNormal;

void main()
{
    // gl_Position is the output (a vec4) of the vertex shader
    vec4 pos = modelViewTransform * vec4(vertCoordinates_in, 1.0);
    gl_Position = projectionTransform * pos;
    vec4 light_pos = modelViewTransform * vec4(light_position, 1.0);
    vec3 normal = normal_transformation * vertNormal_in;
    vec3 v = normalize( -pos.xyz);
    vec3 lightdir = normalize(light_pos.xyz - pos.xyz);
    vec3 half_2 = normalize(v + lightdir);

    vec3 n = normalize(normal.xyz);
    vec3 h = normalize(half_2);
    vec3 l = normalize(lightdir);

    vec3 intensity = material_k[0] * material_Ia
            + material_k[1] * light_intensity * max( 0.0, dot(n, l) )
            + material_k[2] * light_intensity
            * pow( max( 0.0, dot(n, h) ), material_k[3]);
    vertNormal = material_Ia;
}
