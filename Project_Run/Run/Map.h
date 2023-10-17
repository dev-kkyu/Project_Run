#pragma once
#include "GameObject.h"
#include <vector>

class CMap : public CGameObject
{
private:
	std::vector<glm::imat4> map_data;

public:
	CMap(std::string filename);
	virtual ~CMap();

	virtual void Initialize() override;					//������ �� �� ��
	virtual void Update(float ElapsedTime) override;	//Ÿ�̸ӿ��� �� ��
	virtual void FixedUpdate() override;				//�浹ó�� ��
	virtual void Render() override;						//��ο�
	virtual void Release() override;					//�Ҹ�� �� �� ��
};

