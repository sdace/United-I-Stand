/*
	This is a texture shader kept separate from Jose's textured.xx.glsl in case changes need to be made
	Currently, the player uses this
	Enemies can use this too, or we can make new ones
*/

#version 330 

// Input attributes
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;

void main()
{
	texcoord = in_texcoord;
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}