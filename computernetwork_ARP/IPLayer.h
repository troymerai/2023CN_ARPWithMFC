#pragma once
#include "pch.h" // 다른 include파일들보다 먼저 포함되어야 함
#include "BaseLayer.h"

class CIPLayer : public CBaseLayer {

	// 여기에 변수 선언


public:
	CIPLayer(char* pName);
	~CIPLayer(); // 소멸자

	// 여기에 함수 선언

	// 주석 달아야 함
	void ResetHeader(int iosel);


};