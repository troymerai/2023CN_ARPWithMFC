
#include "pch.h" // �ٸ� include���ϵ麸�� ���� ���ԵǾ�� ��
#include "stdafx.h"
#include "BaseLayer.h"
#include "IPLayer.h"




// ����� IP �ּ�
unsigned char sourceIP[4];
// ������ IP �ּ�
unsigned char destIP[4];

CIPLayer::CIPLayer(char* pName) : CBaseLayer(pName) {
	// ������
	// IP ���� ���� �ʱ�ȭ
	ResetAddr();


}

CIPLayer:: ~CIPLayer() {
	// �Ҹ���
}


// IP�ּ� ���� ���� �ʱ�ȭ �Լ�
void CIPLayer::ResetAddr() {
	memset(sourceIP, 0, sizeof(sourceIP));
	memset(destIP, 0, sizeof(destIP));
}


// ���� ���̾�(Ethernet Layer)���� payload �޴� �Լ�
BOOL CIPLayer::Receive(unsigned char* ppayload) {

	

	return true;
}

// dlg���� Destination IP �ּ� �޾Ƽ� ������ ����, ARP Layer�� �����ϴ� �Լ�
void CIPLayer::SetDestIP(unsigned char* IpAddress) {

}


// dlg���� Source IP �ּ� �޾Ƽ� ������ ����, ARP Layer�� �����ϴ� �Լ�
void CIPLayer::SetSourceIP(unsigned char* IpAddress) {

}
