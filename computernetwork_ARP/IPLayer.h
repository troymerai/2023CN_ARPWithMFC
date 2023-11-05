#pragma once
#include "pch.h" // �ٸ� include���ϵ麸�� ���� ���ԵǾ�� ��
#include "BaseLayer.h"

class CIPLayer : public CBaseLayer {

	// ���⿡ ���� ����


public:
	CIPLayer(char* pName);
	~CIPLayer(); // �Ҹ���

	// ���⿡ �Լ� ����

	// IP�ּ� ���� ���� �ʱ�ȭ �Լ�
	void ResetAddr();

	
	// ARP Layer���� Ȯ���ϴ� �ɷ� ����
	// 
	// /////
	// ���� ���̾�(Ethernet Layer)���� payload �޴� �Լ�
	//BOOL CIPLayer::Receive(unsigned char* ppayload);

	// dlg���� Destination IP �ּ� �޾Ƽ� ������ ����, ARP Layer�� �����ϴ� �Լ�
	unsigned char* CIPLayer::GetDestIP(unsigned char* IpAddress);
	// dlg���� Source IP �ּ� �޾Ƽ� ������ ����, ARP Layer�� �����ϴ� �Լ�
	unsigned char* CIPLayer::GetSourceIP(unsigned char* IpAddress);





};