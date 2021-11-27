#version 300 es                                
uniform mat4 u_MVPMatrix;                                       // A constant representing the combined model/view/projection matrix.
layout(location = 0) in vec4 a_Position;     	                // Per-vertex position information we will pass in.
layout(location = 1) in vec4 a_Color;        	                // Per-vertex color information we will pass in.
out vec4 v_Color;          					                    // This will be passed into the fragment shader.
void main()                    				                    // The entry point for our vertex shader.
{                              				
   v_Color = a_Color;          				                    // Pass the color through to the fragment shader.
   gl_Position = u_MVPMatrix * a_Position; 	     			    // gl_Position is a special variable used to store the final position.
}                              				                    // normalized screen coordinates.