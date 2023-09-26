/*
ReadMe
�ش� �ڵ�� Kyu's Study ���� �ۼ��� �ڵ��̸�
�������� ��� �� ������ �� ������
�ܺη� ������ ���� �ʱ⸦ �ٶ��ϴ�.
*/

#include "stdafx.h"
#include "Timer.h"
#include "Scene.h"
#include <iostream>
#include <fstream>
#include <string>


// �ݹ��Լ�
GLvoid Display(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid KeyboardUp(unsigned char key, int x, int y);
GLvoid SpecialKeyboard(int key, int x, int y);
GLvoid SpecialKeyboardUp(int key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLvoid PassiveMotion(int x, int y);
GLvoid IdleScene(GLvoid);
GLvoid TimerFunction(int value);

// ���̴� ���� �Լ���
std::string ReadFile(std::string fileName);
GLuint CreateShaderProgram(std::string vertexFile, std::string fragmentFile);
GLuint CompileShader(std::string fileName, GLenum shaderType);
GLuint InitBuffer();


// --- ���� ����
int winWidth = 800;
int winHeight = 800;

CTimer g_gameTimer;
GLuint g_shader;
GLuint g_VAO;

CScene* g_pscene = nullptr;

void main(int argc, char** argv)								//--- ������ ����ϰ� �ݹ��Լ� ���� 
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv);										// glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);	// ���÷��� ��� ����
	glutInitWindowPosition(100, 100);							// �������� ��ġ ����
	glutInitWindowSize(winWidth, winHeight);								// �������� ũ�� ����
	glutCreateWindow("Run");									// ������ ����(������ �̸�)

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {								// glew �ʱ�ȭ
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "GLEW Initialized\n";
	}

	if (glewIsSupported("GL_VERSION_3_0")) {
		std::cout << "GLEW Version is 3.0\n";
	}
	else {
		std::cout << "GLEW 3.0 not supported\n";
	}

	g_pscene = new CScene{ winWidth, winHeight };

	//	�ݹ��Լ� ����
	glutDisplayFunc(Display);									// ��� �Լ��� ����
	glutReshapeFunc(Reshape);									// �ٽ� �׸��� �Լ� ����
	glutKeyboardFunc(Keyboard);									// Ű���� �Է�(������ ��) �ݹ��Լ� ����
	glutKeyboardUpFunc(KeyboardUp);								// Ű���� �Է�(���� ��) �ݹ��Լ� ����
	glutSpecialFunc(SpecialKeyboard);							// F1~F9 ���
	glutSpecialUpFunc(SpecialKeyboardUp);
	glutMouseFunc(Mouse);										// Ŭ������ ��
	glutMotionFunc(Motion);										// ������ �̵��� ��
	glutPassiveMotionFunc(PassiveMotion);						// ������ �ʰ� �̵��� ��
	glutIdleFunc(IdleScene);									// �ƹ� �̺�Ʈ�� ���� ��
	//glutTimerFunc(0, TimerFunction, 0);						// Ư�� �ð����� �� �� ����


	g_shader = CreateShaderProgram("./Shader/vertex.glsl", "./Shader/fragment.glsl");	// ���̴� ���α׷� ����
	glUseProgram(g_shader);											// ������ ���̴� ���α׷� ���
	g_VAO = InitBuffer();

	glEnable(GL_DEPTH_TEST);	// ���̰˻� Ȱ��ȭ
	glEnable(GL_CULL_FACE);		// �ø�	(�޸� ����)
	//glFrontFace(GL_CCW);		// �ø��� �ո� ���� (GL_CW - �ð�, GL_CCW - �ݽð�)
	//glCullFace(GL_BACK);		// � ���� �������� ����
	//glPolygonMode(GL_FRONT_AND_BACK, GL_POLYGON);	// �������� � ���� �׸�������(GL_POINT, GL_LINE, GL_POLYGON)

	// �Ʒ��� �Ⱦ��� �ɼ�
	////glEnable(GL_DITHER);		// ǥ���� �Ų�����
	////glEnable(GL_LINE_SMOOTH);	// ��Ƽ �ٸ����
	////glEnable(GL_POLYGON_SMOOTH);// ��Ƽ �ٸ����
	////glShadeModel(GL_SMOOTH);	// �ε巯�� ������ �����մϴ�.

	glEnable(GL_BLEND);			// ���� ����� Ȱ��ȭ�Ѵ�.	// Alpha Ȱ��ȭ
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	//������ ���� �÷� ���� Csource�� ���İ����� ����

	glutMainLoop();												// �̺�Ʈ ó�� ����
	glutLeaveMainLoop();										// ���α׷� ����
}

GLvoid Display(GLvoid)
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);						// ������ ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			// ������ ������ ��ü�� ĥ�ϱ�

	float elapsedTime = g_gameTimer.Tick(0);					// ���� �ð� ��� �� Ÿ�̸� �۵� (value�� 0���� ������ ���������� X)

	// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡ ���Եȴ�.

	static float angle = 0.f;
	float changeangle = 90.f * elapsedTime;						// �ʴ� 90�� ȸ��
	angle += changeangle;
	glm::mat4 trans = glm::rotate(glm::mat4(1.f), glm::radians(angle), glm::vec3(0.f, 1.f, 0.f));
	GLint worldLoc = glGetUniformLocation(g_shader, "world");
	glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(trans));

	glBindVertexArray(g_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);	// �ﰢ���� �׸���. 0�� �ε������� 3���� ��


	glutSwapBuffers();											// ȭ�鿡 ����ϱ�
}

GLvoid Reshape(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	if (g_pscene)
		g_pscene->KeyboardEvent(GLUT_DOWN, key);

	switch (key) {
	case 'a':
		break;
	case 27:			// ESC Key
		glutLeaveMainLoop();
	default:
		break;
	}
}

GLvoid KeyboardUp(unsigned char key, int x, int y)
{
	if (g_pscene)
		g_pscene->KeyboardEvent(GLUT_UP, key);

	switch (key) {
	case 'a':
		break;
	default:
		break;
	}
}

GLvoid SpecialKeyboard(int key, int x, int y)
{
	if (g_pscene)
		g_pscene->SpecialKeyEvent(GLUT_UP, key);

	switch (key) {
	case GLUT_KEY_F9:
		break;
	}
}

GLvoid SpecialKeyboardUp(int key, int x, int y)
{
	if (g_pscene)
		g_pscene->SpecialKeyEvent(GLUT_DOWN, key);

	switch (key) {
	case GLUT_KEY_F9:
		glutFullScreenToggle();
		break;
	}
}

GLvoid Mouse(int button, int state, int x, int y)
{
	if (g_pscene)
		g_pscene->MouseEvent(button, state, x, y);

	static const int WHEEL_UP = 3, WHEEL_DOWN = 4;
	switch (state) {
	case GLUT_DOWN:
		switch (button) {
		case GLUT_LEFT_BUTTON:
			break;
		case GLUT_RIGHT_BUTTON:
			break;
		case GLUT_MIDDLE_BUTTON:
			break;
		case WHEEL_DOWN:
			break;
		case WHEEL_UP:
			break;
		}
		break;
	case GLUT_UP:
		switch (button) {
		case GLUT_LEFT_BUTTON:
			break;
		case GLUT_RIGHT_BUTTON:
			break;
		case GLUT_MIDDLE_BUTTON:
			break;
		case WHEEL_DOWN:
			break;
		case WHEEL_UP:
			break;
		}
		break;
	default:
		break;
	}
}

GLvoid Motion(int x, int y)
{
	return GLvoid();
}

GLvoid PassiveMotion(int x, int y)
{
	return GLvoid();
}

GLvoid IdleScene(GLvoid)
{
	glutPostRedisplay();						// glutDisplayFunc �ݹ� ȣ��
}

GLvoid TimerFunction(int value)
{
	glutPostRedisplay();						// ȭ�� �� ���
	glutTimerFunc(0, TimerFunction, 0);			// Ÿ�̸��Լ� �� ����
}

std::string ReadFile(std::string fileName)
{
	std::ifstream in{ fileName };
	if (!in) {
		std::cerr << "���� �б� ����" << std::endl;
		return std::string{};
	}
	std::string str{ std::istreambuf_iterator<char>{ in }, {} };
	return str;
}

GLuint CreateShaderProgram(std::string vertexFile, std::string fragmentFile)
{
	GLuint ShaderProgram = glCreateProgram(); //�� ���̴� ���α׷� ����

	if (ShaderProgram == 0) { //���̴� ���α׷��� ����������� Ȯ��
		std::cerr << "Error: shader program ���� ����" << std::endl;
		return 0;
	}

	GLuint vertexShader = CompileShader(vertexFile, GL_VERTEX_SHADER);
	GLuint fragmentShader = CompileShader(fragmentFile, GL_FRAGMENT_SHADER);

	glAttachShader(ShaderProgram, vertexShader);	//--- ���̴� ���α׷��� ���ؽ� ���̴� ���̱�
	glAttachShader(ShaderProgram, fragmentShader);	//--- ���̴� ���α׷��� �����׸�Ʈ ���̴� ���̱�
	glLinkProgram(ShaderProgram);					//--- ���̴� ���α׷� ��ũ�ϱ�
	glDeleteShader(vertexShader);					//--- ���̴� ��ü�� ���̴� ���α׷��� ��ũ��������, ���̴� ��ü ��ü�� ���� ����
	glDeleteShader(fragmentShader);

	GLint result;
	//��ũ�� �����ߴ��� Ȯ��
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &result);
	if (!result) {
		GLchar InfoLog[1024];

		// shader program �α׸� �޾ƿ�
		glGetProgramInfoLog(ShaderProgram, 1024, nullptr, InfoLog);
		std::cerr << "Error: shader program ���� ����\n" << InfoLog << std::endl;
	}
	else {
		std::cout << vertexFile << ", " << fragmentFile << " shader ������ ����." << std::endl;
	}

	glUseProgram(ShaderProgram);		//--- ������� ���̴� ���α׷� ����ϱ�
	//--- ���� ���� ���̴����α׷� ���� �� �ְ�, �� �� �Ѱ��� ���α׷��� ����Ϸ���
	//--- glUseProgram �Լ��� ȣ���Ͽ� ��� �� Ư�� ���α׷��� �����Ѵ�.
	//--- ����ϱ� ������ ȣ���� �� �ִ�.
	return ShaderProgram;
}

GLuint CompileShader(std::string fileName, GLenum shaderType)
{
	std::string typeName = shaderType == GL_VERTEX_SHADER ? "vertex shader" : shaderType == GL_FRAGMENT_SHADER ? "fragment shader" : "Error Type";

	//���̴� ������Ʈ ����
	GLuint ShaderObj = glCreateShader(shaderType);

	if (ShaderObj == 0) {
		std::cerr << "Error: shader ���� ����: type - " << typeName << std::endl;
	}

	// ������ �д´�.
	std::string shaderStrs = ReadFile(fileName);
	const char* shaderTexts[1]{ shaderStrs.c_str() };

	//���̴� �ڵ带 ���̴� ������Ʈ�� �Ҵ�
	glShaderSource(ShaderObj, 1, shaderTexts, nullptr);

	//�Ҵ�� ���̴� �ڵ带 ������
	glCompileShader(ShaderObj);

	GLint result;
	// ShaderObj �� ���������� ������ �Ǿ����� Ȯ��
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &result);
	if (!result) {
		GLchar InfoLog[1024];

		//OpenGL �� shader log �����͸� ������
		glGetShaderInfoLog(ShaderObj, 1024, nullptr, InfoLog);
		std::cerr << "Error: shader ������ ����: type - " << typeName << std::endl;
		std::cerr << InfoLog << std::endl;
		std::cout << shaderTexts[0] << std::endl;
	}

	return ShaderObj;
}

GLuint InitBuffer()
{
	GLuint VAO, VBO;					// ���� �����͸� GPU�� �Ѱ��� VAO, VBO ����
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	// VBO�� �������۷� ���� �� ���ε�

	float vertex[18]{
		0.0, 0.5, 0, 1, 0, 0,		// x, y, z, r, g, b
		-0.5, -0.5, 0, 0, 1, 0,
		0.5, -0.5, 0, 0, 0, 1
	};

	float vertexb[18 * 2 * 6]{// 6��, ��� 2��, ��� ��3, ���� 6��
		//Front
		-0.5, -0.5, 0.5,1, 0, 0,
		0.5, -0.5, 0.5,1, 0, 0,
		0.5, 0.5, 0.5,1, 0, 0,
		0.5, 0.5, 0.5,1, 0, 0,
		-0.5, 0.5, 0.5,1, 0, 0,
		-0.5, -0.5, 0.5,1, 0, 0,
		//Back
		-0.5, -0.5, -0.5,0,1,0,
		-0.5, 0.5, -0.5,0,1,0,
		0.5, 0.5, -0.5,0,1,0,
		0.5, 0.5, -0.5,0,1,0,
		0.5, -0.5, -0.5,0,1,0,
		-0.5, -0.5, -0.5,0,1,0,
		//Left
		-0.5, -0.5, 0.5,0,0,1,
		-0.5, 0.5, 0.5,0,0,1,
		-0.5, 0.5, -0.5,0,0,1,
		-0.5, 0.5, -0.5,0,0,1,
		-0.5, -0.5, -0.5,0,0,1,
		-0.5, -0.5, 0.5,0,0,1,
		//Right
		0.5, -0.5, 0.5,1,1,0,
		0.5, -0.5, -0.5,1,1,0,
		0.5, 0.5, -0.5,1,1,0,
		0.5, 0.5, -0.5,1,1,0,
		0.5, 0.5, 0.5,1,1,0,
		0.5, -0.5, 0.5,1,1,0,
		//Top
		-0.5, 0.5, 0.5,1,0,1,
		0.5, 0.5, 0.5,1,0,1,
		0.5, 0.5, -0.5,1,0,1,
		0.5, 0.5, -0.5,1,0,1,
		-0.5, 0.5, -0.5,1,0,1,
		-0.5, 0.5, 0.5,1,0,1,
		//Bottom
		-0.5, -0.5, 0.5,0,1,1,
		-0.5, -0.5, -0.5,0,1,1,
		0.5, -0.5, -0.5,0,1,1,
		0.5, -0.5, -0.5,0,1,1,
		0.5, -0.5, 0.5,0,1,1,
		-0.5, -0.5, 0.5,0,1,1
	};

	glBufferData(GL_ARRAY_BUFFER, 18 * 2 * 6 * sizeof(float), vertexb, GL_STATIC_DRAW);	// VBO(GPU)�� ���� ������ ����

	GLint AttribPosLoc = glGetAttribLocation(g_shader, "vPos");						// ���̴����� vPos�� ��ġ �����´�.
	GLint AttribColorLoc = glGetAttribLocation(g_shader, "vColor");					// ���̴����� vColor�� ��ġ �����´�.
	if (AttribPosLoc < 0 or AttribColorLoc < 0) {
		std::cerr << "AttribLoc ã�� ����" << std::endl;
	}
	glVertexAttribPointer(AttribPosLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));	// ���� VBO�� �ִ� ������ �Ӽ� ����
	glVertexAttribPointer(AttribColorLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	// glVertexAttribPointer(attrib ��ġ, vertex ��� ��������, gl_float, gl_false, stride(����), ���� ��ġ(������));
	glEnableVertexAttribArray(AttribPosLoc);										// Attribute Ȱ��ȭ
	glEnableVertexAttribArray(AttribColorLoc);										// Attribute Ȱ��ȭ

	return VAO;
}
