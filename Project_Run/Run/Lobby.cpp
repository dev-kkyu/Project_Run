#include "Lobby.h"
#include "Image.h"
#include <iostream>

CLobby::CLobby()
{
	GLuint shader = CreateShaderProgram("./Shader/LobbyShader.vert", "./Shader/LobbyShader.frag");
	m_shader = shader;

	GLuint vao = InitBuffer();
	m_vao = vao;

	isLobby = true;
}

CLobby::~CLobby()
{
}

void CLobby::SetIsLobby(bool isLobby)
{
	this->isLobby = isLobby;
}

void CLobby::Render()
{
	glUseProgram(m_shader);
	glBindVertexArray(m_vao);
	glBindTexture(GL_TEXTURE_2D, m_tex[int(!isLobby)]);
	glDrawArrays(GL_QUADS, 0, 4);
	if (not isLobby)
		glClear(GL_DEPTH_BUFFER_BIT);
}

GLuint CLobby::InitBuffer()
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
	glGenTextures(2, m_tex);
	for (int i = 0; i < 2; ++i) {
		glBindTexture(GL_TEXTURE_2D, m_tex[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		int ImageWidth, ImageHeight, numberOfChannel;
		GLubyte* data;
		if (0 == i)
			data = CImage::LoadImg("./Resources/Lobby.png", &ImageWidth, &ImageHeight, &numberOfChannel, 0);
		else
			data = CImage::LoadImg("./Resources/BackGround.png", &ImageWidth, &ImageHeight, &numberOfChannel, 0);
		if (!data)
			std::cerr << i << ": image load Error" << std::endl;
		int texLevel = numberOfChannel == 4 ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, numberOfChannel, ImageWidth, ImageHeight, 0, texLevel, GL_UNSIGNED_BYTE, data);
		CImage::FreeImg(data);
	}

	return VAO;
}
