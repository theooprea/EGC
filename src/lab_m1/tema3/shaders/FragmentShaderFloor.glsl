#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_directions[4];
uniform vec3 light_positions[4];
uniform vec3 light_colours[4];

uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

// TODO(student): Declare any other uniforms

uniform vec3 object_color;
uniform float cutoff_angle;

uniform sampler2D texture_uniform;
uniform vec3 disco_ball_position;
uniform int disco_ball;

uniform float time;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // TODO(student): Define ambient, diffuse and specular light components
    vec3 light_intensity_final = vec3(0);

    for (int i = 0; i < 4; i++) {
        vec3 light_position = light_positions[i];
        vec3 light_direction = light_directions[i];
        vec3 light_colour = light_colours[i];
    
        vec3 L = normalize(light_position - world_position);
	    vec3 V = normalize(eye_position - world_position);
	    vec3 H = normalize(L + V);

        vec3 R = normalize(reflect(L, world_normal));

        vec3 ambient_light = vec3(0.075);
        vec3 diffuse_light = light_colour * max(dot(normalize(world_normal), L), 0.f);;
        vec3 specular_light = vec3(0);

        // It's important to distinguish between "reflection model" and
        // "shading method". In this shader, we are experimenting with the Phong
        // (1975) and Blinn-Phong (1977) reflection models, and we are using the
        // Phong (1975) shading method. Don't mix them up!
        if (dot(L, world_normal) > 0) {
            specular_light = light_colour * pow(max(dot(V, R), 0), material_shininess);
        }

        vec3 light_intensity;

        float spot_light = dot(-L, light_direction);

        if (spot_light > cos(cutoff_angle)) {
	        float linear_att = (spot_light - cos(cutoff_angle)) / (1.0f - cos(cutoff_angle));
            float attenuation_factor = pow(linear_att, 2);

            light_intensity = ambient_light + attenuation_factor * (diffuse_light + specular_light);
        }
        else {
            light_intensity = ambient_light;
        }

        light_intensity_final += light_intensity;
    }


    // TODO(student): Compute the total light. You can just add the components
    // together, but if you're feeling extra fancy, you can add individual
    // colors to the light components. To do that, pick some vec3 colors that
    // you like, and multiply them with the respective light components.
    vec3 colour = object_color * light_intensity_final;

    if (disco_ball == 1) {
        vec3 light_dir = world_position - disco_ball_position;
 
        // texcoord este coordonata de textura utilizata pentru esantionare
        vec2 texcoord;
        texcoord.x = (1.0 / (2 * 3.14159)) * atan (light_dir.x, light_dir.z);
        texcoord.y = (1.0 / 3.14159) * acos (light_dir.y / length (light_dir));
 
        if (time >= 0)
	    {
		    texcoord = vec2(texcoord.x - time / 10.f, texcoord.y);
	    }

        // color este culoarea corespunzatoare pozitiei world_position
        vec3 color_texture = texture(texture_uniform, texcoord).xyz;

        vec4 colour_vec4 = vec4(colour, 1);
        vec4 colour_texture_vec4 = vec4(color_texture, 1);

        out_color = mix(colour_vec4, colour_texture_vec4, 0.1);
    }
    else {
        out_color = vec4(colour, 1);
    }

}
