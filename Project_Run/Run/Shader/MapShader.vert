#version 330 core

in vec3 vPos; //--- 응용 프로그램에서 받아온 도형 좌표값
out float zVal;

uniform mat4 modelMat;
uniform mat4 cameraMat;
uniform mat4 projMat;
uniform float move_z;

void main()
{
	vec4 newPos = vec4(vPos, 1.0);
	newPos.z += move_z;
	gl_Position = projMat * cameraMat * modelMat * newPos;
	gl_Position.y += 0.2f;
	zVal = move_z;
}
