#pragma once
#include "GameObject.h"
#include <vector>

class CMap : public CGameObject
{
private:
	int& w_width;
	int& w_height;
	std::vector<glm::mat4> map_data;

	float move_z;
	int map_index;

public:
	CMap(std::string filename, int& winWidth, int& winHeight);
	virtual ~CMap();

	virtual void Initialize() override;					//생성될 때 할 일
	virtual void Update(float ElapsedTime) override;	//타이머에서 할 일
	virtual void FixedUpdate() override;				//충돌처리 등
	virtual void Render() override;						//드로우
	virtual void Release() override;					//소멸될 때 할 일

	GLuint InitBuffer();
};

