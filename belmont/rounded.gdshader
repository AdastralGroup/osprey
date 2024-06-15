shader_type canvas_item;

uniform sampler2D screen_texture : hint_screen_texture, repeat_disable, filter_nearest;
uniform float radius;
uniform sampler2D target_texture;
uniform float weight: hint_range(0, 1);

float adjust_weight(float input, vec2 uv)
{
    return input;
}

float roundedBoxSDF(vec2 CenterPosition, vec2 Size, float Radius) {
    return length(max(abs(CenterPosition)-Size+Radius,0.0))-Radius;
}
void fragment() {
	vec4 color_a = texture(TEXTURE, UV);
    vec4 color_b = texture(target_texture, UV);
    float adjusted_weight = adjust_weight(weight, UV);
	vec3 color = mix(color_a, color_b, adjusted_weight).rgb;
    // The pixel space scale of the rectangle.
    vec2 size = 1.0f / TEXTURE_PIXEL_SIZE;
    vec2 xy = UV / TEXTURE_PIXEL_SIZE;
    // the pixel space location of the rectangle.
    vec2 location = vec2(0.0f,0.0f);

    // How soft the edges should be (in pixels). Higher values could be used to simulate a drop shadow.
    float edgeSoftness  = 1.0f;
    
    
    // Calculate distance to edge.   
    float distance = roundedBoxSDF(xy- location - (size/2.0f), size / 2.0f, radius);
    
    // Smooth the result (free antialiasing).
    float smoothedAlpha =  1.0f-smoothstep(0.0f, edgeSoftness * 2.0f,distance);
    //Return the resultant shape.
	vec3 z = texture(TEXTURE,UV).rgb;
    COLOR = mix(vec4(color, 0.0f), vec4(color, smoothedAlpha), smoothedAlpha);
	COLOR.a = mix(0.0f, smoothedAlpha, smoothedAlpha);
    
}