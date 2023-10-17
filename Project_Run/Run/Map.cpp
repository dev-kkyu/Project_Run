#include "Map.h"
#include <iostream>
#include <fstream>

CMap::CMap(std::string filename)
{
	std::ifstream in{ filename };
	if (!in) {
		std::cerr << filename << " 열기 실패" << std::endl;
	}
	while (in) {
		glm::imat4 mati;
		for (int i = 0; i < 4; ++i) {
			glm::ivec4 line;
			in >> line[0] >> line[1] >> line[2] >> line[3];
			mati[i] = line;
		}
		map_data.push_back(mati);
	}

	Initialize();
}

CMap::~CMap()
{
}

void CMap::Initialize()
{
}

void CMap::Update(float ElapsedTime)
{
	if (isInitialized) {

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

		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
}

void CMap::Release()
{
}
