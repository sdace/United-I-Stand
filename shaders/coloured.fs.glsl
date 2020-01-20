#version 330

uniform vec3 fcolor;
in vec3 loc;

// Output color
layout(location = 0) out vec4 out_color;

void main()
{
	out_color = vec4(sin(loc.x), sin(loc.y), sin(loc.z), 1.0f);
}
