#pragma once
#include "pch.h"
#include "BaseLayer.h"

class CARPLayer : public CBaseLayer {

	// ���⿡ ���� ����


public:
	CARPLayer(char* pName);
	~CARPLayer(); // �Ҹ���


	// ARP request ��Ŷ �ۼ� �Լ�
	unsigned char* CARPLayer::make_request();
	// ARP reply ��Ŷ �ۼ� �Լ�
	unsigned char* CARPLayer::make_reply();


};