#version 450

vec3 vertices[3] = vec3[](
	vec3(-0.5, 0.5, 0.0),
	vec3( 0.0, -0.5, 0.0),
	vec3( 0.5, 0.5, 0.0)
);

void main() {
	gl_Position = vec4(vertices[gl_VertexIndex], 1.0);
}