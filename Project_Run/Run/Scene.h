#pragma once
#include "stdafx.h"
#include <memory>

class CMap;
class CScene
{
private:
	int& w_width;
	int& w_height;

	std::unique_ptr<CMap> m_map;

public:
	CScene(int& width, int& height);
	~CScene();

	void Initialize();				//������ �� �� ��
	void Update(float ElapsedTime);	//Ÿ�̸ӿ��� �� ��
	void FixedUpdate();				//�浹ó�� ��
	void Render();					//��ο�
	void Release();					//�Ҹ�� �� �� ��

	void MouseEvent(int button, int state, int x, int y);
	void KeyboardEvent(int state, unsigned char key);
	void SpecialKeyEvent(int state, int key);
};

