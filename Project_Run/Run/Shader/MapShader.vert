#version 330 core

in vec3 vPos; //--- ���� ���α׷����� �޾ƿ� ���� ��ǥ��

out float zVal;
out float alpha_val;

uniform mat4 modelMat;
uniform mat4 cameraMat;
uniform mat4 projMat;

uniform float move_z;
uniform mat4 alpha_mat;

int getquo(float a, float b) {
    return int(floor(float(a) / float(b)));
}

int getrem(float a, float b){
	return int(mod(int(a), int(b)));
}

void main()
{
	vec4 newPos = vec4(vPos, 1.0);
	newPos.z += move_z;
	gl_Position = projMat * cameraMat * modelMat * newPos;
	gl_Position.y += 0.2f;		// ��ü������ ī�޶� �����ֱ�
	zVal = move_z;				// move_z�� 0~1�� �ݺ�, ���� ��ȭ�� ���� �Ѱ���(�ִϸ��̼�)

	float val = gl_VertexID / 4.f;
	val = floor(val);	// 0~15
	
	alpha_val = alpha_mat[getquo(val, 4)][getrem(val, 4)];
}
