#version 300 es
// A constance represent the combined model/view/projection
uniform mat4 u_MVPMatrix;

//Per-vertex position information we will pass in.
layout(location = 0) in  vec4 a_Position;

// Per-vertex color information we will pass in.
layout(location = 1) in  vec4 a_Color;

// This will be passed into the fragment shader.
out vec4 v_Color;

// The entry point for our vertex shader.
void main()
{
	// Pass through the color.
	v_Color = a_Color;

	// gl_Position is a special variable used to store the final position.
	// Multiply the vertex by the matrix to get the final point in normalized screen coordinates.
	gl_Position = u_MVPMatrix * a_Position;
}