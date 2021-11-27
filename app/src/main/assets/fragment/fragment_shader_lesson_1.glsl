#version 300 es                
precision mediump float;            // Set the default precision to medium. We don't need as high of a
in vec4 v_Color;                    // This is the color from the vertex shader interpolated across the
out vec4 o_fragColor;          
void main()                         // The entry point for our fragment shader.
{                              
   o_fragColor = v_Color;           // Pass the color directly through the pipeline.
}