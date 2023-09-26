#include "stdafx.h"
#include "Scene.h"
#include <iostream>

CScene::CScene(int& width, int& height) : w_width{ width }, w_height{ height }
{
	Initialize();
}

CScene::~CScene()
{
	Release();
}

void CScene::Initialize()
{
	angle = 0.f;

	m_shader = CreateShaderProgram("./Shader/vertex.glsl", "./Shader/fragment.glsl");	// ���̴� ���α׷� ����
	m_vao = InitBuffer();

}

void CScene::Update(float ElapsedTime)
{
	float changeangle = 90.f * ElapsedTime;						// �ʴ� 90�� ȸ��
	angle += changeangle;
	if (angle > 360.f)
		angle -= 360.f;
}

void CScene::FixedUpdate()
{
}

void CScene::Render()
{
	glUseProgram(m_shader);

	glm::mat4 trans = glm::rotate(glm::mat4(1.f), glm::radians(angle), glm::vec3(0.f, 1.f, 0.f));
	GLint worldLoc = glGetUniformLocation(m_shader, "world");
	glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(trans));

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);	// �ﰢ���� �׸���. 0�� �ε������� 3���� ��
}

void CScene::Release()
{
}

void CScene::MouseEvent(int button, int state, int x, int y)
{
}

void CScene::KeyboardEvent(int state, unsigned char key)
{
}

void CScene::SpecialKeyEvent(int state, int key)
{
}

GLuint CScene::InitBuffer()
{
	GLuint VAO, VBO;					// ���� �����͸� GPU�� �Ѱ��� VAO, VBO ����
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	// VBO�� �������۷� ���� �� ���ε�

	float vertex[18]{
		0.0, 0.5, 0, 1, 0, 0,		// x, y, z, r, g, b
		-0.5, -0.5, 0, 0, 1, 0,
		0.5, -0.5, 0, 0, 0, 1
	};

	float vertexb[18 * 2 * 6]{// 6��, ��� 2��, ��� ��3, ���� 6��
		//Front
		-0.5, -0.5, 0.5,1, 0, 0,
		0.5, -0.5, 0.5,1, 0, 0,
		0.5, 0.5, 0.5,1, 0, 0,
		0.5, 0.5, 0.5,1, 0, 0,
		-0.5, 0.5, 0.5,1, 0, 0,
		-0.5, -0.5, 0.5,1, 0, 0,
		//Back
		-0.5, -0.5, -0.5,0,1,0,
		-0.5, 0.5, -0.5,0,1,0,
		0.5, 0.5, -0.5,0,1,0,
		0.5, 0.5, -0.5,0,1,0,
		0.5, -0.5, -0.5,0,1,0,
		-0.5, -0.5, -0.5,0,1,0,
		//Left
		-0.5, -0.5, 0.5,0,0,1,
		-0.5, 0.5, 0.5,0,0,1,
		-0.5, 0.5, -0.5,0,0,1,
		-0.5, 0.5, -0.5,0,0,1,
		-0.5, -0.5, -0.5,0,0,1,
		-0.5, -0.5, 0.5,0,0,1,
		//Right
		0.5, -0.5, 0.5,1,1,0,
		0.5, -0.5, -0.5,1,1,0,
		0.5, 0.5, -0.5,1,1,0,
		0.5, 0.5, -0.5,1,1,0,
		0.5, 0.5, 0.5,1,1,0,
		0.5, -0.5, 0.5,1,1,0,
		//Top
		-0.5, 0.5, 0.5,1,0,1,
		0.5, 0.5, 0.5,1,0,1,
		0.5, 0.5, -0.5,1,0,1,
		0.5, 0.5, -0.5,1,0,1,
		-0.5, 0.5, -0.5,1,0,1,
		-0.5, 0.5, 0.5,1,0,1,
		//Bottom
		-0.5, -0.5, 0.5,0,1,1,
		-0.5, -0.5, -0.5,0,1,1,
		0.5, -0.5, -0.5,0,1,1,
		0.5, -0.5, -0.5,0,1,1,
		0.5, -0.5, 0.5,0,1,1,
		-0.5, -0.5, 0.5,0,1,1
	};

	glBufferData(GL_ARRAY_BUFFER, 18 * 2 * 6 * sizeof(float), vertexb, GL_STATIC_DRAW);	// VBO(GPU)�� ���� ������ ����

	GLint AttribPosLoc = glGetAttribLocation(m_shader, "vPos");						// ���̴����� vPos�� ��ġ �����´�.
	GLint AttribColorLoc = glGetAttribLocation(m_shader, "vColor");					// ���̴����� vColor�� ��ġ �����´�.
	if (AttribPosLoc < 0 or AttribColorLoc < 0) {
		std::cerr << "AttribLoc ã�� ����" << std::endl;
	}
	glVertexAttribPointer(AttribPosLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));	// ���� VBO�� �ִ� ������ �Ӽ� ����
	glVertexAttribPointer(AttribColorLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	// glVertexAttribPointer(attrib ��ġ, vertex ��� ��������, gl_float, gl_false, stride(����), ���� ��ġ(������));
	glEnableVertexAttribArray(AttribPosLoc);										// Attribute Ȱ��ȭ
	glEnableVertexAttribArray(AttribColorLoc);										// Attribute Ȱ��ȭ

	return VAO;
}

