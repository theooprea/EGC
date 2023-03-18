#version 330

// Input
in vec2 texcoord;

// Uniform properties
uniform sampler2D texture_uniform;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // TODO(student): Calculate the out_color using the texture2D() function.
    vec4 colour = texture2D(texture_uniform, texcoord);
	
	out_color = colour;
	
	if (out_color.a < .5f) {
		discard;
	}
}
