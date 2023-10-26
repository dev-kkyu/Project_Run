#version 330 core

in vec3 vPos; //--- ���� ���α׷����� �޾ƿ� ���� ��ǥ��
in vec2 vTexCoord;	// �ؽ�ó UV ��ǥ

out vec2 TexCoord;	// Fragment Shader�� �ѱ�� ��ǥ

void main()
{
	gl_Position = vec4(vPos, 1.0);
	TexCoord = vTexCoord;
}
