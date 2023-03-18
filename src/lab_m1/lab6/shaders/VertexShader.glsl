#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_texture;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float Time;

// Output
out vec3 frag_position;
out vec3 frag_normal;
out vec3 frag_texture;
out vec3 frag_color;

void main()
{
    frag_position = v_position;
    frag_normal = v_normal + vec3(sin(Time));
    frag_texture = v_texture;
    frag_color = v_color;
    
    gl_Position = Projection * View * (Model * vec4(v_position, 1.0) + vec4(cos(Time), sin(Time), 0.f, 0.f));
}
