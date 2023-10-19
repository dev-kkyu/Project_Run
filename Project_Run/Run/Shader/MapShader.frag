#version 330 core

out vec4 FragColor;

in float outIdx;

void main()
{
	vec4 newColor = vec4(0.f, 0.f, 0.f, 1.0);

	if (outIdx < 15) {
		newColor.g = (19.f - outIdx + 1.f) / 20.f;
		newColor.b = (19.f - outIdx + 1.f) / 20.f;
	}
	else {
		newColor.g = ((19.f - 15.f + 1.f) / 20.f) * (30.f - outIdx) / 15.f;
		newColor.b = ((19.f - 15.f + 1.f) / 20.f) * (30.f - outIdx) / 15.f;
	}
//	if(outIdx == 0.f){
//		newColor.r = 1.f;
//		newColor.gb = vec2(0.f);
//	}
//	else if(outIdx > 0.99f && outIdx < 1.01f){
//		newColor.rb = vec2(0.f);
//	}
	FragColor = newColor;
}
