#pragma once
#include "GameObject.h"
#include <vector>
#include <memory>

class CPlayer;
class CMap : public CGameObject
{
private:
	static const int MAX_Layer;

	int& w_width;
	int& w_height;
	std::vector<glm::mat4> map_data;

	float move_z;
	int map_index;
	int bottom_index;

	std::unique_ptr<CPlayer> m_pplayer;

	bool isLeft;
	bool isRight;
	float move_x;
	float acc_x;

	float move_y;
	bool isBottom;
	bool isDrop;
	float basic_v;
	float velocity;

	float stop_time;

	// ȸ���� ����
	bool is_rotating;
	float now_angle;
	int bef_bot_idx;
	float bef_mv_x;
	float bef_mv_y;

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

	bool isOffTile();
	void MoveBackOnTile();

	void SetRotate();
	void RotateMap(float ElapsedTime);
};

