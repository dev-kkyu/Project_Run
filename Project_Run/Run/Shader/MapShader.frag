#version 330 core

out vec4 FragColor;

in float outIdx;

void main()
{
	vec4 newColor = vec4(0.f, 1.f, 0.f, 1.0);

	newColor.g = (9.f - outIdx + 1.f) / 10.f / 2.f + 0.5f;
	FragColor = newColor;
}
