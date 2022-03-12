#version 330
 
uniform sampler2D texture_1;
uniform sampler2D texture_2;
uniform bool is_quad;
uniform bool is_coconut;
uniform float time_elapsed;
 
in vec2 texcoord;

layout(location = 0) out vec4 out_color;

void main()
{
	// TODO : calculate the out_color using the texture2D() function
	vec2 finaltexcoord = texcoord;
	if (is_coconut) {
	finaltexcoord.x += time_elapsed;
	}
	vec4 color1 = texture2D(texture_1, finaltexcoord);
	vec4 color2 = texture2D(texture_2, texcoord);
	if (is_quad) {
		out_color = mix(color1, color2, 0.5f);
	} else {
		out_color = color1;
	
	}
	if (out_color.a <= 0.5f) {
		discard;
	}
}