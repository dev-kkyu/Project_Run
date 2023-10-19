#pragma once
#include "GameObject.h"
#include <vector>

class CMap : public CGameObject
{
private:
	int& w_width;
	int& w_height;
	std::vector<glm::mat4> map_data;

	float move_z;
	int map_index;

public:
	CMap(std::string filename, int& winWidth, int& winHeight);
	virtual ~CMap();

	virtual void Initialize() override;					//������ �� �� ��
	virtual void Update(float ElapsedTime) override;	//Ÿ�̸ӿ��� �� ��
	virtual void FixedUpdate() override;				//�浹ó�� ��
	virtual void Render() override;						//��ο�
	virtual void Release() override;					//�Ҹ�� �� �� ��

	GLuint InitBuffer();
};

