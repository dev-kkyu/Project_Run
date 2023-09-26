#pragma once
class CScene
{
private:
	int& w_width;
	int& w_height;

	float angle;
	GLuint m_shader;
	GLuint m_vao;

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

	GLuint InitBuffer();
};

