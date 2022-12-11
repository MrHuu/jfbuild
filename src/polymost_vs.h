/*#glbuild(ES2) #version 100
#glbuild(2)   #version 110
#glbuild(3)   #version 140

#ifdef GL_ES
#elif __VERSION__ < 140
#  define mediump
#else
#  define attribute in
#  define varying out
#endif

attribute vec3 a_vertex;
attribute mediump vec2 a_texcoord;
varying mediump vec2 v_texcoord;

uniform mat4 u_modelview;
uniform mat4 u_projection;

void main(void)
{
    v_texcoord = a_texcoord;
    gl_Position = u_projection * u_modelview * vec4(a_vertex, 1.0);
}*/

const char *default_polymost_vs_glsl = 
R"(
void main(
	float3 a_vertex,
	float2 a_texcoord,
	uniform float4x4 u_modelview,
	uniform float4x4 u_projection,
	float2 out v_texcoord : TEXCOORD0,
	float4 out v_pos : POSITION)
{
	v_texcoord = a_texcoord;
	v_pos = mul(mul(float4(a_vertex, 1.0f), u_modelview), u_projection);
}
)";