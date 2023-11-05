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

	// dlg���� Destination IP �ּ� �޾Ƽ� ������ �����ϴ� �Լ�
	void CIPLayer::SetDestIP(unsigned char* IpAddress);
	// dlg���� Source IP �ּ� �޾Ƽ� ������ �����ϴ� �Լ�
	void CIPLayer::SetSourceIP(unsigned char* IpAddress);

	// Destination IP �ּҸ� ��ȯ�ϴ� �Լ�
	unsigned char* CIPLayer::GetDestIP();
	// Source IP �ּҸ� ��ȯ�ϴ� �Լ�
	unsigned char* CIPLayer::GetSourceIP();

	// �Ű������� ���� �ּҰ� �� ip �ּҿ� ���� �� Ȯ���ϴ� �Լ�
	BOOL CIPLayer::IsMyIpAddress(unsigned char* IpAddress);

};