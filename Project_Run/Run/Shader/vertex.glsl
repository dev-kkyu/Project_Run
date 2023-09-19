#version 330 core

in vec3 vPos; //--- 응용 프로그램에서 받아온 도형 좌표값
in vec3 vColor;	// 점마다 색상값
out vec3 outColor;

uniform mat4 world;
void main()
{
	gl_Position = world * vec4(vPos, 1.0);
	outColor = vColor;
}
