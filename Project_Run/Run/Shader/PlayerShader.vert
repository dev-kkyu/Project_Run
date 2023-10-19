#version 330 core

in vec3 vPos; //--- ���� ���α׷����� �޾ƿ� ���� ��ǥ��

uniform mat4 modelMat;
uniform mat4 cameraMat;
uniform mat4 projMat;

void main()
{
	gl_Position = projMat * cameraMat * modelMat * vec4(vPos, 1.0);
	gl_Position.y += 0.2f;		// ��ü������ ī�޶� �����ֱ�
}
