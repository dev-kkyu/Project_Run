#include "Player.h"
#define STB_IMAGE_IMPLEMENTATION
#include "OpenGL/stb_image.h"
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

	TexFrame = 0;

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

		static float time_sec;
		time_sec += 5.f * ElapsedTime;
		if (time_sec >= 6.f)
			time_sec -= 6.f;
		TexFrame = int(time_sec);
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
		if (TexFrame > 3)
			glBindTexture(GL_TEXTURE_2D, Textures[6 - TexFrame]);
		else
			glBindTexture(GL_TEXTURE_2D, Textures[TexFrame]);
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
	GLuint VAO, VBO, textureVBO;					// ���� �����͸� GPU�� �Ѱ��� VAO, VBO ����
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	// VBO�� �������۷� ���� �� ���ε�

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

	glGenBuffers(1, &textureVBO);
	glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
	float texCoord[] = {
		0.f, 0.f,
		0.f, 1.f,
		1.f, 1.f,
		1.f, 0.f
	};
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), texCoord, GL_STATIC_DRAW);
	GLint TexLoc = glGetAttribLocation(m_shader, "vTexCoord");						// ���̴����� vPos�� ��ġ �����´�.
	if (TexLoc < 0) {
		std::cerr << "TexLoc ã�� ����" << std::endl;
	}
	glVertexAttribPointer(TexLoc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(0)); //--- �ؽ�ó ��ǥ �Ӽ�
	glEnableVertexAttribArray(TexLoc);

	// �ؽ��� �ε�
	glGenTextures(4, Textures);
	for (int i = 0; i < 4; ++i) {
		glBindTexture(GL_TEXTURE_2D, Textures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		int ImageWidth, ImageHeight, numberOfChannel;
		//stbi_set_flip_vertically_on_load(true); //--- �̹����� �Ųٷ� �����ٸ� �߰�
		std::string name = "./Resources/" + std::to_string(i + 1) + ".png";
		GLubyte* data = stbi_load(name.c_str(), &ImageWidth, &ImageHeight, &numberOfChannel, 0);
		if (!data)
			std::cerr << "image load Error" << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 0, 4, ImageWidth, ImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

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
