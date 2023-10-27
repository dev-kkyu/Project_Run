#include "Map.h"
#include "Player.h"
#include <iostream>
#include <fstream>
#include <sstream>

const int CMap::MAX_Layer = 30;
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
	bottom_index = 2;

	isLeft = isRight = isSpace = false;
	move_x = 0.f;
	acc_x = 0.f;

	isBottom = true;
	isDrop = false;
	move_y = 0.f;
	basic_v = 100.f;
	velocity = basic_v;

	m_pplayer = std::make_unique<CPlayer>();

	stop_time = 0.f;

	// ȸ���� ����
	is_Rotating = false;
	now_angle = 0.f;
	bef_mv_x = move_x;
	bef_mv_y = move_y;
}

void CMap::Update(float ElapsedTime)
{
	// �ʴ� 5��
	if (isInitialized) {
		glUseProgram(m_shader);

		if (is_Rotating)
			RotateMap(ElapsedTime);
		else if (stop_time > 0.f) {
			stop_time -= 1.f * ElapsedTime;
			if (stop_time <= 0.f) {
				isBottom = true;
				stop_time = 0.f;
				m_pplayer->SetWalk(true);
			}
		}
		else {
			move_z += 3.5f * ElapsedTime;
			if (move_z > 1.f) {
				move_z -= 1.f;
				++map_index;		// map_index�� �ǹ�. ���� 0����ġ�� �簢������ map_data[map_index]�� ������ ����.
			}

			// ����Ű
			if (isBottom and isSpace) {
				m_pplayer->SetWalk(false);
				isBottom = false;
			}
			// ����Ű
			if (isLeft or isRight)
				acc_x = float(-(int)isLeft + (int)isRight);
			else if (acc_x != 0.f) {
				if (acc_x > 0)
					acc_x -= ElapsedTime * 5.f;
				else
					acc_x += ElapsedTime * 5.f;
				if (abs(acc_x) < 0.2f)
					acc_x = 0.f;
			}
			move_x += acc_x * 3.f * ElapsedTime;
			// �� ����
			if (move_x >= 1.75f or move_x <= -1.75f) {
				move_x = move_x >= 1.75f ? 1.75f : -1.75f;
				if (not isDrop) {
					SetRotate();
					RotateMap(ElapsedTime);
					m_pplayer->SetWalk(false);
				}
			}


			// �߷�
			if (isBottom and isOffTile()) {
				isBottom = false;
				isDrop = true;
				velocity = -1.f;
			}

			if (not isBottom) {
				move_y += pow(velocity, 2) * 9.8f / 12000.f * ElapsedTime * (velocity < 0 ? -1.f : 1.f);
				if (move_y <= 0.f and isOffTile())
					isDrop = true;
				if (move_y <= -5.f or (not isDrop and move_y <= 0.f and not isOffTile())) {
					if (move_y <= -0.5f) {
						MoveBackOnTile();
						m_pplayer->SetWalk(false);
						stop_time = 0.5f;
					}
					else
						m_pplayer->SetWalk(true);
					move_y = 0.f;
					isBottom = true;
					isDrop = false;
					velocity = basic_v;
				}
				else {
					velocity -= 250 * ElapsedTime;
				}
			}
		}

		GLint cameraLoc = glGetUniformLocation(m_shader, "cameraMat");
		if (cameraLoc < 0) {
			std::cerr << "cameraLoc ã�� ����" << std::endl;
		}
		GLint projLoc = glGetUniformLocation(m_shader, "projMat");
		if (projLoc < 0) {
			std::cerr << "projLoc ã�� ����" << std::endl;
		}

		// ������ ī�޶� ã��...
		glm::vec3 eye(move_x, -0.9f + move_y, 0.9f);
		glm::mat4 camera = glm::lookAt(glm::vec3(eye.x, eye.y, eye.z), glm::vec3(eye.x, eye.y, -50.f), glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(cameraLoc, 1, GL_FALSE, glm::value_ptr(camera));

		glm::mat4 projection = glm::perspective(glm::radians(90.f), (float)w_width / (float)w_height, 0.1f, 100.f);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		if (m_pplayer) {
			m_pplayer->SetCamera(camera);
			m_pplayer->SetProjection(projection);
			m_pplayer->SetMoveX(move_x);
			m_pplayer->SetMoveY(move_y);
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

		float angle = 180.f - (bottom_index * 90.f);
		glm::mat4 rotateMat = glm::rotate(glm::mat4(1.f), glm::radians(angle + now_angle), glm::vec3(0.f, 0.f, 1.f));
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

			glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -1.f * i)) * rotateMat;
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
		case ' ':
			isSpace = true;
			break;
		}
		break;
	case GLUT_UP:
		switch (key)
		{
		case ' ':
			isSpace = false;
			break;
		}
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
	int posIdx = round(move_x + 1.5f);
	if (map_index >= -2 and map_index < 99) {
		if (move_z <= 0.5f and map_index >= -1) {
			int lineIdx = map_index + 1;
			if (not map_data[lineIdx][bottom_index][posIdx])
				return true;
		}
		else if (move_z > 0.5f and map_index < 98) {
			int lineIdx = map_index + 2;
			if (not map_data[lineIdx][bottom_index][posIdx])
				return true;
		}
	}
	return false;
}

void CMap::MoveBackOnTile()
{
	int my_index;
	int posIdx;
	for (int i = 1;; ++i) {
		posIdx = round(move_x + 1.5f);
		my_index = map_index - i;
		if (my_index + 1 < 0)
			break;
		if (map_data[my_index + 1][bottom_index][posIdx])
			break;
	}
	map_index = my_index;
	move_x = posIdx - 1.5f;
	move_z = 0.f;
}

void CMap::RotateMap(float ElapsedTime)
{
	static const float time_sec = 0.5f;
	if (bef_mv_x == -1.75f) {
		float finalx = 1.75f - bef_mv_y;
		now_angle += 90.f / time_sec * ElapsedTime;
		move_y -= bef_mv_y / time_sec * ElapsedTime;
		move_x += (finalx - bef_mv_x) / time_sec * ElapsedTime;
		if (now_angle >= 90.f) {
			is_Rotating = false;
			m_pplayer->SetWalk(true);
			bottom_index -= 1;
			now_angle = 0.f;
			move_y = 0.f;
			if (move_x >= 1.75)
				move_x = 1.7499;
			if (bottom_index == -1)
				bottom_index = 3;
		}
	}
	else if (bef_mv_x == 1.75f) {
		float finalx = -1.75f + bef_mv_y;
		now_angle -= 90.f / time_sec * ElapsedTime;
		move_y -= bef_mv_y / time_sec * ElapsedTime;
		move_x -= (bef_mv_x - finalx) / time_sec * ElapsedTime;
		if (now_angle <= -90.f) {
			is_Rotating = false;
			m_pplayer->SetWalk(true);
			bottom_index += 1;
			now_angle = 0.f;
			move_y = 0.f;
			if (move_x <= -1.75)
				move_x = -1.7499;
			if (bottom_index == 4)
				bottom_index = 0;
		}
	}
}

void CMap::SetRotate()
{
	is_Rotating = true;
	now_angle = 0.f;
	bef_mv_x = move_x;
	bef_mv_y = move_y;
}
