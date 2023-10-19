#version 330 core

in vec3 vPos; //--- 응용 프로그램에서 받아온 도형 좌표값
out float outIdx;

uniform float idx;
uniform mat4 modelMat;
uniform mat4 cameraMat;
uniform mat4 projMat;

void main()
{
	gl_Position = projMat * cameraMat * modelMat * vec4(vPos, 1.0);
	gl_Position.y += 0.2f;
	outIdx = idx;
}
