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

	// ���� ���̾�(Ethernet Layer)���� payload �޴� �Լ�
	BOOL CIPLayer::Receive(unsigned char* ppayload);

	// dlg���� Destination IP �ּ� �޾Ƽ� ������ ����, ARP Layer�� �����ϴ� �Լ�
	void CIPLayer::SetDestIP(unsigned char* IpAddress);
	// dlg���� Source IP �ּ� �޾Ƽ� ������ ����, ARP Layer�� �����ϴ� �Լ�
	void CIPLayer::SetSourceIP(unsigned char* IpAddress);





};