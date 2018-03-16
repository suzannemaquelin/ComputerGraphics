#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 vertNormal;
in vec2 uv_coordinates;
in vec3 normal;
in vec3 light_direction;
in vec3 reflection;
in vec3 view;
in vec3 vertex_coordinates;
in float absolute;

// Specify the Uniforms of the fragment shaders
// uniform vec3 lightPosition; // for example
uniform vec3 material_Ia;
uniform vec3 material_ka;
uniform vec3 material_kd;
uniform vec3 material_ks;
uniform float phong_exp;

uniform vec3 light_position;
uniform vec3 light_intensity;

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fColor;

void main()
{
    vec3 n = normalize(normal);

    float step = smoothstep(-absolute, absolute, vertex_coordinates.z);
    vec3 color = mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 1.0, 1.0), step);

//    material_kd = texture(textureSampler, texCoord).rgb;
    vec3 intensity = material_ka * color
            + material_kd * light_intensity * max( 0.0, dot(n, light_direction) )
            + material_ks * light_intensity
            * pow( max(0.0, dot(reflection, view)), phong_exp);
    vec4 i = vec4(normalize(intensity), 1.0);

    fColor = i;

    //fColor = vec4(normalize(vertNormal), 1.0);
    //fColor = vec4(uv_coordinates, 0.0, 1.0);
}
