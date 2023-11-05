
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

///// 
// ARP Layer���� Ȯ���ϴ� �ɷ� ����
// 
// ���� ���̾�(Ethernet Layer)���� payload �޴� �Լ�
//BOOL CIPLayer::Receive(unsigned char* ppayload) {
// return true;
//}

// dlg���� Destination IP �ּ� �޾Ƽ� ������ �����ϴ� �Լ�
void CIPLayer::SetDestIP(unsigned char* IpAddress) {

	// �Ű������� ���޹��� IP �ּҸ� destIP�� ����
	memcpy(destIP, IpAddress, sizeof(destIP));

}

// dlg���� Source IP �ּ� �޾Ƽ� ������ �����ϴ� �Լ�
void CIPLayer::SetSourceIP(unsigned char* IpAddress) {

	// �Ű������� ���޹��� IP �ּҸ� destIP�� ����
	memcpy(sourceIP, IpAddress, sizeof(sourceIP));

}

// Destination IP �ּҸ� ��ȯ�ϴ� �Լ�
unsigned char* CIPLayer::GetDestIP() {

	return destIP;
}


// Source IP �ּҸ� ��ȯ�ϴ� �Լ�
unsigned char* CIPLayer::GetSourceIP() {

	return sourceIP;
}

// �Ű������� ���� �ּҰ� �� ip �ּҿ� ���� �� Ȯ���ϴ� �Լ�
BOOL CIPLayer::IsMyIpAddress(unsigned char* IpAddress) {

	BOOL isMyIP = FALSE;

	if (memcmp(IpAddress, sourceIP, sizeof(sourceIP)) == 0) {
		isMyIP = TRUE;
	}
	
	return isMyIP;
}