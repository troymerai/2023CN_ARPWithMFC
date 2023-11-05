
#include "pch.h"
#include "ARPLayer.h"


CARPLayer::CARPLayer(char* pName) : CBaseLayer(pName) {
	// ������
}

CARPLayer:: ~CARPLayer() {

}



//
// ARP ���̺� ��ȸ �Լ� (Source��) -> ������ �� �´� ������ �𸣰��� �ٵ� �򰥸� �� ���Ƽ� ����
// 
// TODO
// 
// source ip address�� �ִ� �� Ȯ��
// ������ IP Layer�� Source ip address ��û�ؼ� ARP cache table ������Ʈ
// 
// source MAC address�� �ִ� �� Ȯ��
// ������ Ethernet Layer�� Source MAC address ��û�ؼ� ARP cache table ������Ʈ
// 
// 

//
// ARP request ��Ŷ �ۼ� �Լ�
unsigned char* CARPLayer::make_request() {
	unsigned char* pPacket = nullptr;

	// ��Ŷ�� ũ�⿡ �°� �޸� �Ҵ�
	unsigned int packetSize = 42;
	pPacket = new unsigned char[packetSize];

	// ��Ŷ ���� �ʱ�ȭ
	memset(pPacket, 0, packetSize);


	// �̴��� ��� �ۼ� �Լ�

	// ARP ��� �ۼ� �Լ�

	return pPacket;
}

//
// ARP reply ��Ŷ �ۼ� �Լ�
unsigned char* CARPLayer::make_reply() {
	unsigned char* pPacket = nullptr;

	// ��Ŷ�� ũ�⿡ �°� �޸� �Ҵ�
	unsigned int packetSize = 42;
	pPacket = new unsigned char[packetSize];

	// ��Ŷ ���� �ʱ�ȭ
	memset(pPacket, 0, packetSize);

	// �̴��� ����� �ڱ� MAC �ִ� �Լ�

	//  ARP ����� �ڱ� MAC �ִ� �Լ�

	return pPacket;
}



//
// ���� ���̾�(Ethernet Layer)�� ARP ��Ŷ (request, reply �� ��) ���� �Լ�
BOOL CARPLayer::Send() {


	BOOL bSuccess = FALSE;

	return bSuccess;
}


//
// ARP ���̺� ��ȸ �Լ� (Destination��)
//
// TODO
// 
// Destination MAC address�� ARP cache table�� �ִ� �� Ȯ��
// ������ Ethernet Layer�� Destination MAC address ��û (Ethernet Layer�� MAC �ּ� ���� �Լ� �״�� ���)
// �ִٸ� ARP reply ��Ŷ �ۼ�
//


// ���� ���̾�(Ethernet Layer)���� payload �޴� �Լ�
BOOL CARPLayer::Receive(unsigned char* ppayload) {

	// ���� �����͸� ARP header ����ü�� ĳ����
	PARP_HEADER pFrame = (PARP_HEADER)ppayload;

	BOOL bSuccess = FALSE;

	// ARP header�� ������ ip �ּҰ� �� ip�� ���� �� IP Layer�� ��û
	
	// ���� ���� (IP Layer)�� ������ ����
	CIPLayer* pIPLayer = dynamic_cast<CIPLayer*>(this->GetUpperLayer(0));

	// ������ IP �ּҰ� �� IP�� ���� ���
	if (pIPLayer != nullptr && pIPLayer->IsMyIpAddress(pFrame->ip_destaddr)) {
		
		// TODO
		// 
		// ARP cache table�� Destination MAC �ּ� ������Ʈ
		// Dlg�� ������Ʈ�ϴ� �Լ�
		// Ÿ�̸� stop�Լ�
		// status�� complete�� �ٲ��ִ� �Լ�
		
		
	}



	


	return bSuccess;
}
// 
// TODO
// 
// ARP cache table�� Destination MAC �ּ� ������Ʈ
// Dlg�� ������Ʈ�ϴ� �Լ�
// Ÿ�̸� stop�Լ�
// status�� complete�� �ٲ��ִ� �Լ�
// 
//

//
// IP Layer���� Destination IP �ּ� �޾ƿ��� �Լ�
// 
// TODO
// 
// �޾ƿͼ� ARP cache table ������Ʈ
//
//
