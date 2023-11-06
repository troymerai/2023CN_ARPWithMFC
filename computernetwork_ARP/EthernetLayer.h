// EthernetLayer.h: interface for the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////
#pragma once


#include "pch.h"
#include "BaseLayer.h"


class CEthernetLayer :public CBaseLayer {
	// 여기에 변수 선언

public: 
	unsigned char sourceMAC[6];
	unsigned char destMAC[6];


public:
	CEthernetLayer(char* pName);
	~CEthernetLayer(); // 소멸자

	BOOL CEthernetLayer::Send(unsigned char* ppayload);
	BOOL CEthernetLayer::Receive(unsigned char* pPacket);





};