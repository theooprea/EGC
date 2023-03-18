#version 330

// Input
in vec3 world_position;
in vec3 world_normal;
in vec2 texcoord;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;
uniform sampler2D textura;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

// TODO(student): Declare any other uniforms

uniform vec3 object_color;
uniform int light_type;
uniform float cutoff_angle;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // TODO(student): Define ambient, diffuse and specular light components

    vec3 L = normalize(light_position - world_position);
	vec3 V = normalize(eye_position - world_position);
	vec3 H = normalize(L + V);

    vec3 R = normalize(reflect(L, world_normal));

    float ambient_light = 0.25;
    float diffuse_light = material_kd * max(dot(normalize(world_normal), L), 0.f);;
    float specular_light = 0;

    // It's important to distinguish between "reflection model" and
    // "shading method". In this shader, we are experimenting with the Phong
    // (1975) and Blinn-Phong (1977) reflection models, and we are using the
    // Phong (1975) shading method. Don't mix them up!
    if (diffuse_light > 0) {
        specular_light = material_ks * pow(max(dot(V, R), 0), material_shininess);
    }

    float light_intensity = 0;

    // TODO(student): If (and only if) the light is a spotlight, we need to do
    // some additional things.
    if (light_type == 1) {
        float spot_light = dot(-L, light_direction);
        if (spot_light > cos(cutoff_angle)) {
	        float linear_att = (spot_light - cos(cutoff_angle)) / (1.0f - cos(cutoff_angle));
            float light_att_factor = pow(linear_att, 2);
            light_intensity = ambient_light + light_att_factor * (diffuse_light + specular_light);
        }
        else {
            light_intensity = ambient_light;
        }
    }
    else {
        float d	= distance(light_position, world_position);
	    float attenuation_factor = 1 / (1 + 0.1 * d + 0.05 * d * d);
	    light_intensity = ambient_light + attenuation_factor * (diffuse_light + specular_light);
    }


    // TODO(student): Compute the total light. You can just add the components
    // together, but if you're feeling extra fancy, you can add individual
    // colors to the light components. To do that, pick some vec3 colors that
    // you like, and multiply them with the respective light components.
    vec3 colour = object_color * light_intensity;

    // TODO(student): Write pixel out color
    out_color = vec4(colour, 1);

    vec4 colour_texture = texture2D(textura, texcoord);

    out_color += colour_texture;

    if (colour_texture.a < 0.5) {
        discard;
    }


}
