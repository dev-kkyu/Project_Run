#include "stdafx.h"
#include <iostream>
#include <fstream>

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

