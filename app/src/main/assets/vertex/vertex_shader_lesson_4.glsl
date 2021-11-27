#version 300 es
// A constant representing the combined model/view/projection matrix.
uniform mat4 u_MVPMatrix;
// A constant representing the combined model/view matrix.
uniform mat4 u_MVMatrix;

// Per-vertex position information we will pass in.
layout(location = 0) in vec4 a_Position;
// Per-vertex color information we will pass in.
layout(location = 1) in vec4 a_Color;
// Per-vertex normal information we will pass in.
layout(location = 2) in vec3 a_Normal;
// Per-vertex texture coordinate information we will pass in.
layout(location = 3) in vec2 a_TexCoordinate;

// This will be passed into the fragment shader.
out vec3 v_Position;
out vec4 v_Color;
out vec3 v_Normal;
out vec2 v_TexCoordinate;
		  
// The entry point for our vertex shader.  
void main()                                                 	
{                                                         
	// Transform the vertex into eye space. 	
	v_Position = vec3(u_MVMatrix * a_Position);            
		
	// Pass through the color.
	v_Color = a_Color;
	
	// Pass through the texture coordinate.
	v_TexCoordinate = a_TexCoordinate;                                      
	
	// Transform the normal's orientation into eye space.
    v_Normal = vec3(u_MVMatrix * vec4(a_Normal, 0.0));
          
	// gl_Position is a special variable used to store the final position.
	// Multiply the vertex by the matrix to get the final point in normalized screen coordinates.
	gl_Position = u_MVPMatrix * a_Position;                       		  
}                                                          