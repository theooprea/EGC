#version 330

// Input
in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_texture;
in vec3 frag_color;

// Output
layout(location = 0) out vec4 out_color;

void main()
{
    // TODO(student): Write pixel out color
	out_color	= vec4(abs(frag_normal), 1);
}
