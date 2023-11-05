#pragma once
#include "pch.h"
#include "BaseLayer.h"

class CARPLayer : public CBaseLayer {

	// 여기에 변수 선언


public:
	CARPLayer(char* pName);
	~CARPLayer(); // 소멸자


	// ARP request 패킷 작성 함수
	unsigned char* CARPLayer::make_request();
	// ARP reply 패킷 작성 함수
	unsigned char* CARPLayer::make_reply();


};