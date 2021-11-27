// Set the default precision to medium. We don't need as high of a
// precision in the fragment shader.
precision mediump float;
// This is the color from the vertex shader interpolated across the
// triangle per fragment.
varying vec4 v_Color;
// The entry point for our fragment shader.
void main()
{
// Pass the color directly through the pipeline.
   gl_FragColor = v_Color;
}