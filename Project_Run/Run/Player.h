#pragma once
#include "GameObject.h"

class CPlayer : public CGameObject
{
private:
	glm::mat4 cameraMat;
	glm::mat4 projMat;
	glm::mat4 modelMat;

	GLuint Textures[4];
	int TexFrame;

	bool isWalk;

	float move_x;
	float move_y;

public:
	CPlayer();
	virtual ~CPlayer();

	virtual void Initialize() override;					//������ �� �� ��
	virtual void Update(float ElapsedTime) override;	//Ÿ�̸ӿ��� �� ��
	virtual void FixedUpdate() override;				//�浹ó�� ��
	virtual void Render() override;						//��ο�
	virtual void Release() override;					//�Ҹ�� �� �� ��

	void SetCamera(glm::mat4 cameraMat);
	void SetProjection(glm::mat4 projMat);

	GLuint InitBuffer();

	void SetWalk(bool isWalk);

	void SetMoveX(float move_x);
	void SetMoveY(float move_y);
};

