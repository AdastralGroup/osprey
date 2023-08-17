shader_type canvas_item;

uniform sampler2D screen_texture : hint_screen_texture, repeat_disable, filter_nearest;
uniform float radius;

float roundedBoxSDF(vec2 CenterPosition, vec2 Size, float Radius) {
    return length(max(abs(CenterPosition)-Size+Radius,0.0))-Radius;
}
void fragment() {
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
    COLOR = mix(vec4(z, 0.0f), vec4(z, smoothedAlpha), smoothedAlpha);
	COLOR.a = mix(0.0f, smoothedAlpha, smoothedAlpha);
    
}