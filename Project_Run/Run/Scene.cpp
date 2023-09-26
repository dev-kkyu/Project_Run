#include "stdafx.h"
#include "Scene.h"

CScene::CScene(int& width, int& height) : w_width{ width }, w_height{ height }
{
	Initialize();
}

CScene::~CScene()
{
	Release();
}

void CScene::Initialize()
{
}

void CScene::Update(float ElapsedTime)
{
}

void CScene::FixedUpdate()
{
}

void CScene::Render()
{
}

void CScene::Release()
{
}

void CScene::MouseEvent(int button, int state, int x, int y)
{
}

void CScene::KeyboardEvent(int state, unsigned char key)
{
}

void CScene::SpecialKeyEvent(int state, int key)
{
}
