shader_type canvas_item;
uniform sampler2D target_texture;
uniform float weight: hint_range(0, 1);
uniform float alpha: hint_range(0,1);

void fragment()
{
    vec4 color_a = texture(TEXTURE, UV);
    vec4 color_b = texture(target_texture, UV);
    vec4 color = mix(color_a, color_b, weight);
	COLOR = vec4(color.r,color.g,color.b,color.a*alpha);
}
