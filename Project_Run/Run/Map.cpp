#include "Map.h"
#include "Player.h"
#include <iostream>
#include <fstream>
#include <sstream>

CMap::CMap(std::string filename, int& winWidth, int& winHeight) : w_width{ winWidth }, w_height{ winHeight }
{
	std::ifstream in{ filename };
	if (!in) {
		std::cerr << filename << " ���� ����" << std::endl;
	}
	std::string str;
	while (std::getline(in, str)) {
		std::stringstream ss{ str };
		glm::mat4 mati;
		for (int i = 0; i < 4; ++i) {
			glm::vec4 line;
			ss >> line[0] >> line[1] >> line[2] >> line[3];
			mati[i] = line;
		}
		map_data.push_back(mati);
	}
	if (not map_data.empty()) {
		std::cout << filename << " �� �ε� �Ϸ�" << std::endl;
	}

	Initialize();
}

CMap::~CMap()
{
	Release();
}

void CMap::Initialize()
{
	//for (const auto& a : map_data) {
	//	for (int i = 0; i < 4; ++i)
	//		std::cout << a[i].x << " " << a[i].y << " " << a[i].z << " " << a[i].w << "  ";
	//	std::cout << std::endl;
	//}
	//std::cout << map_data.size() << "��" << std::endl;

	// �׳� �� �� ������ �׸���. �� ���� ���İ� �ٲ㰡�鼭, �����ϰ� �׸��� �����ϰ�, ���� �ε����� translate�� �о �׸���.. ���� 10������.

	GLuint shader = CreateShaderProgram("./Shader/MapShader.vert", "./Shader/MapShader.frag");
	SetShader(shader);

	GLuint vao = InitBuffer();
	SetVao(vao);

	move_z = 0.f;
	map_index = -10;

	isLeft = isRight = false;
	move_x = 0.f;

	m_pplayer = std::make_unique<CPlayer>();
}

void CMap::Update(float ElapsedTime)
{
	// �ʴ� 5��
	if (isInitialized) {
		glUseProgram(m_shader);

		move_z += 3.5f * ElapsedTime;
		if (move_z > 1.f) {
			move_z -= 1.f;
			++map_index;		// map_index�� �ǹ�. ���� 0����ġ�� �簢������ map_data[map_index]�� ������ ����.
		}

		if (isLeft or isRight) {
			move_x += (-isLeft + isRight) * 3.f * ElapsedTime;
		}

		GLint cameraLoc = glGetUniformLocation(m_shader, "cameraMat");
		if (cameraLoc < 0) {
			std::cerr << "cameraLoc ã�� ����" << std::endl;
		}
		GLint projLoc = glGetUniformLocation(m_shader, "projMat");
		if (projLoc < 0) {
			std::cerr << "projLoc ã�� ����" << std::endl;
		}

		if (isOffTile())
			std::cout << "Off" << clock() << std::endl;

		glm::vec3 eye(move_x, -0.9f, 0.9f);
		glm::mat4 camera = glm::lookAt(glm::vec3(eye.x, eye.y, eye.z), glm::vec3(eye.x, eye.y, -50.f), glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(cameraLoc, 1, GL_FALSE, glm::value_ptr(camera));

		glm::mat4 projection = glm::perspective(glm::radians(90.f), (float)w_width / (float)w_height, 0.1f, 100.f);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		if (m_pplayer) {
			m_pplayer->SetCamera(camera);
			m_pplayer->SetProjection(projection);
			m_pplayer->SetMoveX(move_x);
			m_pplayer->Update(ElapsedTime);
		}
	}
}

void CMap::FixedUpdate()
{
}

void CMap::Render()
{
	if (isInitialized) {
		glUseProgram(m_shader);
		glBindVertexArray(m_vao);


		GLint modelLoc = glGetUniformLocation(m_shader, "modelMat");
		if (modelLoc < 0) {
			std::cerr << "modelLoc ã�� ����" << std::endl;
		}
		GLint idxLoc = glGetUniformLocation(m_shader, "Index");
		if (idxLoc < 0) {
			std::cerr << "idxLoc ã�� ����" << std::endl;
		}
		GLint zLoc = glGetUniformLocation(m_shader, "move_z");
		if (zLoc < 0) {
			std::cerr << "zLoc ã�� ����" << std::endl;
		}
		GLint alphaLoc = glGetUniformLocation(m_shader, "alpha_mat");
		if (alphaLoc < 0) {
			std::cerr << "alphaLoc ã�� ����" << std::endl;
		}

		glUniform1f(zLoc, move_z);

		constexpr int MAX_Layer = 30;
		for (int i = 0; i < MAX_Layer; ++i) {
			glm::mat4 alpha(0.f);
			int index = map_index + i;
			if (index >= 0 and index < 100) {
				alpha = map_data[index];
			}
			else {
				for (int a = 0; a < 4; ++a) {
					for (int b = 0; b < 4; ++b) {
						alpha[a][b] = 1.f;
					}
				}
			}
			glUniformMatrix4fv(alphaLoc, 1, GL_FALSE, glm::value_ptr(alpha));

			glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -1.f * i));
			glUniform1f(idxLoc, (float)i);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_QUADS, 0, 64);
		}

		if (m_pplayer)
			m_pplayer->Render();
	}
}

void CMap::Release()
{
}

GLuint CMap::InitBuffer()
{
	glUseProgram(m_shader);
	GLuint VAO, VBO;					// ���� �����͸� GPU�� �Ѱ��� VAO, VBO ����
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	// VBO�� �������۷� ���� �� ���ε�

	// y -2.f ~ 2.f
	// x -2.f ~ 2.f
	// z -0.5f, 0.5f
	float zSize = 0.5f;
	float vertexRects[]{	// �� �� �� ��	// �ݽð�
		// ���� �װ� �簢��
		1.f, 2.f, zSize,
		1.f, 2.f, -zSize,
		2.f, 2.f, -zSize,
		2.f, 2.f, zSize,

		0.f, 2.f, zSize,
		0.f, 2.f, -zSize,
		1.f, 2.f, -zSize,
		1.f, 2.f, zSize,

		-1.f, 2.f, zSize,
		-1.f, 2.f, -zSize,
		0.f, 2.f, -zSize,
		0.f, 2.f, zSize,

		-2.f, 2.f, zSize,
		-2.f, 2.f, -zSize,
		-1.f, 2.f, -zSize,
		-1.f, 2.f, zSize,

		// ���� �װ� �簢��
		-2.f, 2.f, zSize,
		-2.f, 1.f, zSize,
		-2.f, 1.f, -zSize,
		-2.f, 2.f, -zSize,

		-2.f, 1.f, zSize,
		-2.f, 0.f, zSize,
		-2.f, 0.f, -zSize,
		-2.f, 1.f, -zSize,

		-2.f, 0.f, zSize,
		-2.f, -1.f, zSize,
		-2.f, -1.f, -zSize,
		-2.f, 0.f, -zSize,

		-2.f, -1.f, zSize,
		-2.f, -2.f, zSize,
		-2.f, -2.f, -zSize,
		-2.f, -1.f, -zSize,

		// �Ʒ��� �װ� �簢��
		-2.f, -2.f, -zSize,
		-2.f, -2.f, zSize,
		-1.f, -2.f, zSize,
		-1.f, -2.f, -zSize,

		-1.f, -2.f, -zSize,
		-1.f, -2.f, zSize,
		0.f, -2.f, zSize,
		0.f, -2.f, -zSize,

		0.f, -2.f, -zSize,
		0.f, -2.f, zSize,
		1.f, -2.f, zSize,
		1.f, -2.f, -zSize,

		1.f, -2.f, -zSize,
		1.f, -2.f, zSize,
		2.f, -2.f, zSize,
		2.f, -2.f, -zSize,

		// ������ �װ� �簢��
		2.f, -1.f, -zSize,
		2.f, -2.f, -zSize,
		2.f, -2.f, zSize,
		2.f, -1.f, zSize,

		2.f, 0.f, -zSize,
		2.f, -1.f, -zSize,
		2.f, -1.f, zSize,
		2.f, 0.f, zSize,

		2.f, 1.f, -zSize,
		2.f, 0.f, -zSize,
		2.f, 0.f, zSize,
		2.f, 1.f, zSize,

		2.f, 2.f, -zSize,
		2.f, 1.f, -zSize,
		2.f, 1.f, zSize,
		2.f, 2.f, zSize

	};

	glBufferData(GL_ARRAY_BUFFER, 192 * sizeof(float), vertexRects, GL_STATIC_DRAW);	// VBO(GPU)�� ���� ������ ����

	GLint AttribPosLoc = glGetAttribLocation(m_shader, "vPos");						// ���̴����� vPos�� ��ġ �����´�.
	if (AttribPosLoc < 0) {
		std::cerr << "AttribLoc ã�� ����" << std::endl;
	}
	glVertexAttribPointer(AttribPosLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));	// ���� VBO�� �ִ� ������ �Ӽ� ����
	// glVertexAttribPointer(attrib ��ġ, vertex ��� ��������, gl_float, gl_false, stride(����), ���� ��ġ(������));
	glEnableVertexAttribArray(AttribPosLoc);										// Attribute Ȱ��ȭ

	return VAO;
}

void CMap::KeyboardEvent(int state, unsigned char key)
{
	switch (state) {
	case GLUT_DOWN:
		switch (key) {
		case 32:
			std::cout << (int)key << " space" << std::endl;
			break;
		}
		break;
	case GLUT_UP:
		break;
	}
}

void CMap::SpecialKeyEvent(int state, int key)
{
	switch (state) {
	case GLUT_DOWN:
		switch (key) {
		case GLUT_KEY_LEFT:
			isLeft = true;
			break;
		case GLUT_KEY_RIGHT:
			isRight = true;
			break;
		}
		break;
	case GLUT_UP:
		switch (key) {
		case GLUT_KEY_LEFT:
			isLeft = false;
			break;
		case GLUT_KEY_RIGHT:
			isRight = false;
			break;
		}
		break;
	}
}

bool CMap::isOffTile()
{
	int bottom = 2;

	int posIdx = round(move_x + 1.5f);
	if (map_index >= -2 and map_index < 99) {
		if (move_z <= 0.5f and map_index >= -1) {
			int lineIdx = map_index + 1;
			if (not map_data[lineIdx][bottom][posIdx])
				return true;
		}
		else if (move_z > 0.5f and map_index < 98){
			int lineIdx = map_index + 2;
			if (not map_data[lineIdx][bottom][posIdx])
				return true;
		}
	}
	return false;
}
