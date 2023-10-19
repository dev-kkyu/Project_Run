#pragma once
#include "GameObject.h"
#include <vector>
#include <memory>

class CPlayer;
class CMap : public CGameObject
{
private:
	int& w_width;
	int& w_height;
	std::vector<glm::mat4> map_data;

	float move_z;
	int map_index;

	std::unique_ptr<CPlayer> m_pplayer;

	bool isLeft;
	bool isRight;
	float move_x;

public:
	CMap(std::string filename, int& winWidth, int& winHeight);
	virtual ~CMap();

	virtual void Initialize() override;					//������ �� �� ��
	virtual void Update(float ElapsedTime) override;	//Ÿ�̸ӿ��� �� ��
	virtual void FixedUpdate() override;				//�浹ó�� ��
	virtual void Render() override;						//��ο�
	virtual void Release() override;					//�Ҹ�� �� �� ��

	GLuint InitBuffer();

	void KeyboardEvent(int state, unsigned char key);
	void SpecialKeyEvent(int state, int key);
};

