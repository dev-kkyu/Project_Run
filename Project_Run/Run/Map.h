#pragma once
#include "GameObject.h"

class CMap : public CGameObject
{
private:


public:
	CMap();
	virtual ~CMap();

	virtual void Initialize() override;					//������ �� �� ��
	virtual void Update(float ElapsedTime) override;	//Ÿ�̸ӿ��� �� ��
	virtual void FixedUpdate() override;				//�浹ó�� ��
	virtual void Render() override;						//��ο�
	virtual void Release() override;					//�Ҹ�� �� �� ��
};

