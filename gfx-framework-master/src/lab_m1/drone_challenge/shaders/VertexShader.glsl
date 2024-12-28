#version 330

// Input
layout(location = 0) in vec3 position;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float seed;

// Output
out float noise_value;

float random (in vec2 pos) {
    return fract(sin(dot(pos.xy, vec2(12.9898f, 78.233f))) * 43758.5453123f);
}

float noise (in vec2 pos) {
    vec2 i = floor(pos);
    vec2 j = fract(pos);

    float a = random(i + seed);
    float b = random(i + vec2(1.0f, 0.0f) + seed);
    float c = random(i + vec2(0.0f, 1.0f) + seed);
    float d = random(i + vec2(1.0f, 1.0f) + seed);

    vec2 u = j * j * (3.0f - 2.0f * j);
    
    return mix(a, b, u.x) +
            (c - a) * u.y * (1.0f - u.x) +
            (d - b) * u.x * u.y;
}

void main()
{
	const float frequency = 5.0f;
	noise_value = noise(position.xz * frequency);

	vec3 new_pos = position;
    new_pos.y += noise_value; 

	gl_Position = Projection * View * Model * vec4(new_pos, 1.0f);
}
