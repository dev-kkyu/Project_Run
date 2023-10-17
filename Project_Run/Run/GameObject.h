#pragma once
#include "stdafx.h"

class CGameObject
{
protected:
	GLuint m_shader;
	GLuint m_vao;

	bool isInitialized;

public:
	CGameObject();
	virtual ~CGameObject();

	virtual void Initialize() = 0;				//������ �� �� ��
	virtual void Update(float ElapsedTime) = 0;	//Ÿ�̸ӿ��� �� ��
	virtual void FixedUpdate() = 0;					//�浹ó�� ��
	virtual void Render() = 0;					//��ο�
	virtual void Release() = 0;					//�Ҹ�� �� �� ��

	void SetShader(GLuint shader);
	void SetVao(GLuint vao);
};

