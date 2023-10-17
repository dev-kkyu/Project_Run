#include "Scene.h"
#include "Map.h"
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
	m_map = std::make_unique<CMap>();

}

void CScene::Update(float ElapsedTime)
{
	if (m_map) {
		m_map->Update(ElapsedTime);
	}
}

void CScene::FixedUpdate()
{
}

void CScene::Render()
{
	if (m_map) {
		m_map->Render();
	}
}

void CScene::Release()
{
}

void CScene::MouseEvent(int button, int state, int x, int y)
{
	static const int WHEEL_UP = 3, WHEEL_DOWN = 4;
	switch (state) {
	case GLUT_DOWN:
		switch (button) {
		case GLUT_LEFT_BUTTON:
			break;
		case GLUT_RIGHT_BUTTON:
			break;
		case GLUT_MIDDLE_BUTTON:
			break;
		case WHEEL_DOWN:
			break;
		case WHEEL_UP:
			break;
		}
		break;
	case GLUT_UP:
		break;
	default:
		break;
	}
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

