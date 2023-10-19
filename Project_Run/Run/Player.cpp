#include "Player.h"
#include <iostream>

CPlayer::CPlayer()
{
	Initialize();
}

CPlayer::~CPlayer()
{
	Release();
}

void CPlayer::Initialize()
{
	GLuint shader = CreateShaderProgram("./Shader/PlayerShader.vert", "./Shader/PlayerShader.frag");
	SetShader(shader);

	GLuint vao = InitBuffer();
	SetVao(vao);

	GLint cameraLoc = glGetUniformLocation(m_shader, "cameraMat");
	if (cameraLoc < 0) {
		std::cerr << "cameraLoc ã�� ����" << std::endl;
	}
	glUniformMatrix4fv(cameraLoc, 1, GL_FALSE, glm::value_ptr(cameraMat));

	move_x = 0.f;
	move_y = 0.f;
}

void CPlayer::Update(float ElapsedTime)
{
	if (isInitialized) {
		glUseProgram(m_shader);
		GLint cameraLoc = glGetUniformLocation(m_shader, "cameraMat");
		if (cameraLoc < 0) {
			std::cerr << "cameraLoc ã�� ����" << std::endl;
		}
		GLint projLoc = glGetUniformLocation(m_shader, "projMat");
		if (projLoc < 0) {
			std::cerr << "projLoc ã�� ����" << std::endl;
		}

		glUniformMatrix4fv(cameraLoc, 1, GL_FALSE, glm::value_ptr(cameraMat));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMat));

		glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(0.25f, 0.25f, 1.f));
		modelMat = glm::translate(glm::mat4(1.f), glm::vec3(move_x, -1.75f + move_y, -1.f)) * scale;
	}
}

void CPlayer::FixedUpdate()
{
}

void CPlayer::Render()
{
	if (isInitialized) {
		glUseProgram(m_shader);
		glBindVertexArray(m_vao);

		GLint modelLoc = glGetUniformLocation(m_shader, "modelMat");
		if (modelLoc < 0) {
			std::cerr << "modelLoc ã�� ����" << std::endl;
		}

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
		glDrawArrays(GL_QUADS, 0, 4);
	}
}

void CPlayer::Release()
{
}

void CPlayer::SetCamera(glm::mat4 cameraMat)
{
	this->cameraMat = cameraMat;
}

void CPlayer::SetProjection(glm::mat4 projMat)
{
	this->projMat = projMat;
}

GLuint CPlayer::InitBuffer()
{
	glUseProgram(m_shader);
	GLuint VAO, VBO;					// ���� �����͸� GPU�� �Ѱ��� VAO, VBO ����
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	// VBO�� �������۷� ���� �� ���ε�

	float zSize = 0.5f;
	float vertex[]{
		-1.f, 1.f, 0.f,
		-1.f, -1.f, 0.f,
		1.f, -1.f, 0.f,
		1.f, 1.f, 0.f
	};

	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), vertex, GL_STATIC_DRAW);	// VBO(GPU)�� ���� ������ ����

	GLint AttribPosLoc = glGetAttribLocation(m_shader, "vPos");						// ���̴����� vPos�� ��ġ �����´�.
	if (AttribPosLoc < 0) {
		std::cerr << "AttribLoc ã�� ����" << std::endl;
	}
	glVertexAttribPointer(AttribPosLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));	// ���� VBO�� �ִ� ������ �Ӽ� ����
	// glVertexAttribPointer(attrib ��ġ, vertex ��� ��������, gl_float, gl_false, stride(����), ���� ��ġ(������));
	glEnableVertexAttribArray(AttribPosLoc);										// Attribute Ȱ��ȭ

	return VAO;
}

void CPlayer::SetMoveX(float move_x)
{
	this->move_x = move_x;
}

void CPlayer::SetMoveY(float move_y)
{
	this->move_y = move_y;
}
