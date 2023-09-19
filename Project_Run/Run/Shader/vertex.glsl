#version 330 core

in vec3 vPos; //--- ���� ���α׷����� �޾ƿ� ���� ��ǥ��
in vec3 vColor;	// ������ ����
out vec3 outColor;

uniform mat4 world;
void main()
{
	gl_Position = world * vec4(vPos, 1.0);
	outColor = vColor;
}
