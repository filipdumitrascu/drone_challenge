#version 330

// Input
in float noise_value;

// Output
layout(location = 0) out vec4 out_color;

void main()
{
    vec3 brown = vec3(0.65f, 0.32f, 0.17f);
    vec3 dark_green = vec3(0.0f, 0.5f, 0.0f);

	vec3 frag_color = mix(brown, dark_green, noise_value);

    out_color = vec4(frag_color, 1);
}
