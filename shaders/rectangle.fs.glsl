#version 330

in vec2 texcoord;

uniform sampler2D sampler0;
uniform vec3 fcolor;
// Output color
layout(location = 0) out  vec4 color;

void main()
{
    vec4 texColor = texture(sampler0, texcoord);
	color = texColor;
	//color = vec4(fcolor, 1.0) - texture(sampler0, texcoord);
}