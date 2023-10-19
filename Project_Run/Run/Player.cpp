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
		std::cerr << "cameraLoc 찾지 못함" << std::endl;
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
			std::cerr << "cameraLoc 찾지 못함" << std::endl;
		}
		GLint projLoc = glGetUniformLocation(m_shader, "projMat");
		if (projLoc < 0) {
			std::cerr << "projLoc 찾지 못함" << std::endl;
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
			std::cerr << "modelLoc 찾지 못함" << std::endl;
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
	GLuint VAO, VBO;					// 정점 데이터를 GPU에 넘겨줄 VAO, VBO 생성
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	// VBO를 정점버퍼로 설정 및 바인딩

	float zSize = 0.5f;
	float vertex[]{
		-1.f, 1.f, 0.f,
		-1.f, -1.f, 0.f,
		1.f, -1.f, 0.f,
		1.f, 1.f, 0.f
	};

	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), vertex, GL_STATIC_DRAW);	// VBO(GPU)로 정점 데이터 복사

	GLint AttribPosLoc = glGetAttribLocation(m_shader, "vPos");						// 셰이더에서 vPos의 위치 가져온다.
	if (AttribPosLoc < 0) {
		std::cerr << "AttribLoc 찾기 실패" << std::endl;
	}
	glVertexAttribPointer(AttribPosLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));	// 현재 VBO에 있는 데이터 속성 정의
	// glVertexAttribPointer(attrib 위치, vertex 몇개씩 읽을건지, gl_float, gl_false, stride(간격), 시작 위치(포인터));
	glEnableVertexAttribArray(AttribPosLoc);										// Attribute 활성화

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
