#pragma once
#include "GameObject.h"

class CMap : public CGameObject
{
private:


public:
	CMap();
	virtual ~CMap();

	virtual void Initialize() override;					//생성될 때 할 일
	virtual void Update(float ElapsedTime) override;	//타이머에서 할 일
	virtual void FixedUpdate() override;				//충돌처리 등
	virtual void Render() override;						//드로우
	virtual void Release() override;					//소멸될 때 할 일
};

