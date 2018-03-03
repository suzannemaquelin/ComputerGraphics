#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 normal;
in vec3 half_2;
in vec3 lightdir;
in vec2 texCoord;

// Specify the Uniforms of the fragment shaders
// uniform vec3 lightPosition; // for example
uniform vec3 light_position;
uniform vec3 light_intensity;
uniform vec3 material_Ia;
uniform vec3 material_kd;
uniform vec3 material_ka;
uniform vec3 material_ks;
uniform float phongExp;
uniform sampler2D textureSampler;

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fColor;

void main()
{
    vec3 n = normalize(normal);
    vec3 h = half_2;
    vec3 l = lightdir;

    //material_kd = texture2D(textureSampler, texCoord).rgb;
    vec3 intensity = material_ka * material_Ia
            + material_kd * light_intensity * max( 0.0, dot(n, l) )
            + material_ks * light_intensity
            * pow( max( 0.0, dot(n, h) ), phongExp);
    //textureColor = texture2D(textureSampler, texCoord);
    //fColor = vec4(normalize(intensity), 1.0);
    //fColor = texture(textureSampler, texCoord);
    fColor = vec4(texCoord, 0.0, 1.0);
}
