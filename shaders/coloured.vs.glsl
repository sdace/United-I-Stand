#version 330
 
// !!! Simple shader for colouring basic meshes

// Input attributes
in vec3 in_position;
out vec3 loc;

// Application data
uniform mat3 transform;
uniform mat3 projection;

void main()
{
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	loc = pos;
	gl_Position = vec4(pos.x, pos.y, in_position.z, 1.0);
}
