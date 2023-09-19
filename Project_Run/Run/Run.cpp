/*
ReadMe
해당 코드는 Kyu's Study 에서 작성된 코드이며
언제든지 사용 및 수정할 수 있지만
외부로 유출은 하지 않기를 바랍니다.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include "OpenGL/glew.h"
#include "OpenGL/freeglut.h"
#include "OpenGL/glm/ext.hpp"

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freeglut.lib")


// 콜백함수
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

// 셰이더 관련 함수들
std::string ReadFile(std::string fileName);
GLuint CreateShaderProgram(std::string vertexFile, std::string fragmentFile);
GLuint CompileShader(std::string fileName, GLenum shaderType);
GLuint InitBuffer();

class Timer {
	std::chrono::steady_clock::time_point last_time;		// 마지막으로 호출된 시간
	std::chrono::nanoseconds accm_time;
	int fps;
	int frame;

public:
	Timer() : last_time{ std::chrono::steady_clock::now() }, accm_time{ std::chrono::seconds{ 0 } }, fps{ 0 }, frame{ 0 } {}

	float Tick(int fps_value)								// 경과 시간 리턴, 1 Frame에 단 한번 호출되어야 함
	{
		if (fps_value > 0 and accm_time.count() / 1'000'000'000. * fps_value < frame)		// 프레임 제한
			std::this_thread::sleep_until(last_time + std::chrono::nanoseconds{ static_cast<int>(1. / fps_value * 1'000'000'000.) });

		++frame;
		auto now_time = std::chrono::steady_clock::now();
		auto elapsed_time = now_time - last_time;
		accm_time += elapsed_time;
		last_time = now_time;
		if (accm_time >= std::chrono::seconds{ 1 }) {			// 초당 한번씩 FPS 표시
			float over_time = accm_time.count() / 1'000'000'000.;
			fps = static_cast<int>(round(frame * (1.f / over_time)));
			frame -= fps;
			accm_time -= std::chrono::seconds{ 1 };

			std::stringstream title;
			title << "Run - (" << fps << "FPS)";
			glutSetWindowTitle(title.str().c_str());
		}

		return elapsed_time.count() / 1'000'000'000.;		// 나노초를 초로 바꿔준다.
	}
};

// --- 전역 변수
Timer g_gameTimer;
GLuint g_shader;
GLuint g_VAO;

void main(int argc, char** argv)								//--- 윈도우 출력하고 콜백함수 설정 
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);										// glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);	// 디스플레이 모드 설정
	glutInitWindowPosition(100, 100);							// 윈도우의 위치 지정
	glutInitWindowSize(800, 800);								// 윈도우의 크기 지정
	glutCreateWindow("Run");									// 윈도우 생성(윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {								// glew 초기화
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

	//	콜백함수 설정
	glutDisplayFunc(Display);									// 출력 함수의 지정
	glutReshapeFunc(Reshape);									// 다시 그리기 함수 지정
	glutKeyboardFunc(Keyboard);									// 키보드 입력(눌렀을 때) 콜백함수 지정
	glutKeyboardUpFunc(KeyboardUp);								// 키보드 입력(뗐을 때) 콜백함수 지정
	glutSpecialFunc(SpecialKeyboard);							// F1~F9 등등
	glutSpecialUpFunc(SpecialKeyboardUp);
	glutMouseFunc(Mouse);										// 클릭했을 때
	glutMotionFunc(Motion);										// 누르고 이동할 때
	glutPassiveMotionFunc(PassiveMotion);						// 누르지 않고 이동할 때
	glutIdleFunc(IdleScene);									// 아무 이벤트가 없을 때
	//glutTimerFunc(0, TimerFunction, 0);						// 특정 시간마다 할 일 설정


	g_shader = CreateShaderProgram("./Shader/vertex.glsl", "./Shader/fragment.glsl");	// 셰이더 프로그램 생성
	glUseProgram(g_shader);											// 생성한 셰이더 프로그램 사용
	g_VAO = InitBuffer();

	glEnable(GL_DEPTH_TEST);	// 깊이검사 활성화
	glEnable(GL_CULL_FACE);		// 컬링	(뒷면 제거)
	//glFrontFace(GL_CCW);		// 컬링의 앞면 설정 (GL_CW - 시계, GL_CCW - 반시계)
	//glCullFace(GL_BACK);		// 어떤 면을 제거할지 설정
	//glPolygonMode(GL_FRONT_AND_BACK, GL_POLYGON);	// 폴리곤을 어떤 모드로 그릴것인지(GL_POINT, GL_LINE, GL_POLYGON)

	// 아래는 안쓰는 옵션
	////glEnable(GL_DITHER);		// 표면을 매끄럽게
	////glEnable(GL_LINE_SMOOTH);	// 안티 앨리어싱
	////glEnable(GL_POLYGON_SMOOTH);// 안티 앨리어싱
	////glShadeModel(GL_SMOOTH);	// 부드러운 음영을 수행합니다.

	glEnable(GL_BLEND);			// 블렌딩 기능을 활성화한다.	// Alpha 활성화
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	//지수를 원본 컬러 벡터 Csource의 알파값으로 설정

	glutMainLoop();												// 이벤트 처리 시작
	glutLeaveMainLoop();										// 프로그램 종료
}

GLvoid Display(GLvoid)
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);						// 바탕색 지정
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			// 설정된 색으로 전체를 칠하기

	float elapsedTime = g_gameTimer.Tick(0);					// 지난 시간 얻기 및 타이머 작동 (value를 0으로 넣으면 프레임제한 X)

	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다.

	static float angle = 0.f;
	float changeangle = 90.f * elapsedTime;						// 초당 90도 회전
	angle += changeangle;
	glm::mat4 trans = glm::rotate(glm::mat4(1.f), glm::radians(angle), glm::vec3(0.f, 1.f, 0.f));
	GLint worldLoc = glGetUniformLocation(g_shader, "world");
	glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(trans));

	glBindVertexArray(g_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);	// 삼각형을 그린다. 0번 인덱스부터 3개의 점


	glutSwapBuffers();											// 화면에 출력하기
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
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
	switch (key) {
	case 'a':
		break;
	default:
		break;
	}
}

GLvoid SpecialKeyboard(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_F9:
		break;
	}
}

GLvoid SpecialKeyboardUp(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_F9:
		glutFullScreenToggle();
		break;
	}
}

GLvoid Mouse(int button, int state, int x, int y)
{
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
	glutPostRedisplay();						// glutDisplayFunc 콜백 호출
}

GLvoid TimerFunction(int value)
{
	glutPostRedisplay();						// 화면 재 출력
	glutTimerFunc(0, TimerFunction, 0);			// 타이머함수 재 설정
}

std::string ReadFile(std::string fileName)
{
	std::ifstream in{ fileName };
	if (!in) {
		std::cerr << "파일 읽기 실패" << std::endl;
		return std::string{};
	}
	std::string str{ std::istreambuf_iterator<char>{ in }, {} };
	return str;
}

GLuint CreateShaderProgram(std::string vertexFile, std::string fragmentFile)
{
	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (ShaderProgram == 0) { //쉐이더 프로그램이 만들어졌는지 확인
		std::cerr << "Error: shader program 생성 실패" << std::endl;
		return 0;
	}

	GLuint vertexShader = CompileShader(vertexFile, GL_VERTEX_SHADER);
	GLuint fragmentShader = CompileShader(fragmentFile, GL_FRAGMENT_SHADER);

	glAttachShader(ShaderProgram, vertexShader);	//--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(ShaderProgram, fragmentShader);	//--- 세이더 프로그램에 프래그먼트 세이더 붙이기
	glLinkProgram(ShaderProgram);					//--- 세이더 프로그램 링크하기
	glDeleteShader(vertexShader);					//--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);

	GLint result;
	//링크가 성공했는지 확인
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &result);
	if (!result) {
		GLchar InfoLog[1024];

		// shader program 로그를 받아옴
		glGetProgramInfoLog(ShaderProgram, 1024, nullptr, InfoLog);
		std::cerr << "Error: shader program 연결 실패\n" << InfoLog << std::endl;
	}
	else {
		std::cout << vertexFile << ", " << fragmentFile << " shader 컴파일 성공." << std::endl;
	}

	glUseProgram(ShaderProgram);		//--- 만들어진 세이더 프로그램 사용하기
	//--- 여러 개의 세이더프로그램 만들 수 있고, 그 중 한개의 프로그램을 사용하려면
	//--- glUseProgram 함수를 호출하여 사용 할 특정 프로그램을 지정한다.
	//--- 사용하기 직전에 호출할 수 있다.
	return ShaderProgram;
}

GLuint CompileShader(std::string fileName, GLenum shaderType)
{
	std::string typeName = shaderType == GL_VERTEX_SHADER ? "vertex shader" : shaderType == GL_FRAGMENT_SHADER ? "fragment shader" : "Error Type";

	//쉐이더 오브젝트 생성
	GLuint ShaderObj = glCreateShader(shaderType);

	if (ShaderObj == 0) {
		std::cerr << "Error: shader 생성 실패: type - " << typeName << std::endl;
	}

	// 파일을 읽는다.
	std::string shaderStrs = ReadFile(fileName);
	const char* shaderTexts[1]{ shaderStrs.c_str() };

	//쉐이더 코드를 쉐이더 오브젝트에 할당
	glShaderSource(ShaderObj, 1, shaderTexts, nullptr);

	//할당된 쉐이더 코드를 컴파일
	glCompileShader(ShaderObj);

	GLint result;
	// ShaderObj 가 성공적으로 컴파일 되었는지 확인
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &result);
	if (!result) {
		GLchar InfoLog[1024];

		//OpenGL 의 shader log 데이터를 가져옴
		glGetShaderInfoLog(ShaderObj, 1024, nullptr, InfoLog);
		std::cerr << "Error: shader 컴파일 실패: type - " << typeName << std::endl;
		std::cerr << InfoLog << std::endl;
		std::cout << shaderTexts[0] << std::endl;
	}

	return ShaderObj;
}

GLuint InitBuffer()
{
	GLuint VAO, VBO;					// 정점 데이터를 GPU에 넘겨줄 VAO, VBO 생성
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	// VBO를 정점버퍼로 설정 및 바인딩

	float vertex[18]{
		0.0, 0.5, 0, 1, 0, 0,		// x, y, z, r, g, b
		-0.5, -0.5, 0, 0, 1, 0,
		0.5, -0.5, 0, 0, 0, 1
	};

	float vertexb[18 * 2 * 6]{// 6면, 면당 2삼, 삼당 점3, 점당 6개
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

	glBufferData(GL_ARRAY_BUFFER, 18 * 2 * 6 * sizeof(float), vertexb, GL_STATIC_DRAW);	// VBO(GPU)로 정점 데이터 복사

	GLint AttribPosLoc = glGetAttribLocation(g_shader, "vPos");						// 셰이더에서 vPos의 위치 가져온다.
	GLint AttribColorLoc = glGetAttribLocation(g_shader, "vColor");					// 셰이더에서 vColor의 위치 가져온다.
	if (AttribPosLoc < 0 or AttribColorLoc < 0) {
		std::cerr << "AttribLoc 찾기 실패" << std::endl;
	}
	glVertexAttribPointer(AttribPosLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));	// 현재 VBO에 있는 데이터 속성 정의
	glVertexAttribPointer(AttribColorLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	// glVertexAttribPointer(attrib 위치, vertex 몇개씩 읽을건지, gl_float, gl_false, stride(간격), 시작 위치(포인터));
	glEnableVertexAttribArray(AttribPosLoc);										// Attribute 활성화
	glEnableVertexAttribArray(AttribColorLoc);										// Attribute 활성화

	return VAO;
}
