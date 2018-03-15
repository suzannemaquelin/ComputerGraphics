#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;
layout (location = 2) in vec2 texCoordinates_in;

// Specify the Uniforms of the vertex shader
 uniform mat4 modelTransform;
 uniform mat4 viewTransform;
 uniform mat4 projectionTransform;
 uniform mat3 normal_transformation;
 uniform sampler2D textureSampler;

 uniform vec3 light_position;
 uniform vec3 light_intensity;
 uniform vec3 material_Ia;
 uniform vec3 material_kd;
 uniform vec3 material_ka;
 uniform vec3 material_ks;
 uniform float phongExp;

// Specify the output of the vertex stage
out vec4 vertNormal;

void main()
{
    // gl_Position is the output (a vec4) of the vertex shader
    vec4 pos = viewTransform * modelTransform * vec4(vertCoordinates_in, 1.0);
    gl_Position = projectionTransform * pos;

    vec4 light_pos = viewTransform * vec4(light_position, 1.0);
    vec3 normal = normal_transformation * vertNormal_in;
    vec3 v = normalize(-pos.xyz);
    vec3 lightdir = normalize(light_pos.xyz - pos.xyz);
    vec3 half_2 = normalize(v + lightdir);
    vec3 n = normalize(normal);

    vec3 intensity = material_ka * material_Ia
            + material_kd * light_intensity * max( 0.0, dot(n, lightdir) )
            + material_ks * light_intensity
            * pow( max( 0.0, dot(n, half_2) ), phongExp);

    vec4 t = texture(textureSampler, texCoordinates_in);
    vec4 i = vec4(normalize(intensity), 1.0);
    vertNormal = t * i;
}
