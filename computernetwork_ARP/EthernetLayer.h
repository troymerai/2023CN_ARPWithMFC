// EthernetLayer.h: interface for the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////
#pragma once


#include "pch.h"
#include "BaseLayer.h"


class CEthernetLayer :public CBaseLayer {
	// ���⿡ ���� ����

public: 
	unsigned char sourceMAC[6];
	unsigned char destMAC[6];


public:
	CEthernetLayer(char* pName);
	~CEthernetLayer(); // �Ҹ���

	BOOL CEthernetLayer::Send(unsigned char* ppayload);
	BOOL CEthernetLayer::Receive(unsigned char* pPacket);





};