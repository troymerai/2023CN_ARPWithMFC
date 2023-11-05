
#include "pch.h"
#include "ARPLayer.h"


CARPLayer::CARPLayer(char* pName) : CBaseLayer(pName) {
	// ������
	ResetHeader();

}

CARPLayer:: ~CARPLayer() {
	// �Ҹ���
}

// ARP ��� �ʱ�ȭ
void CARPLayer::ResetHeader() {
	// �ϵ���� Ÿ�� ���� (�̴����� 1)
	arp_header.hard_type = htons(1);  
	// �������� Ÿ�� ���� (IPv4�� 0x0800)
	arp_header.prot_type = htons(0x0800);
	// �ϵ���� ũ�� ���� (�̴����� 6)
	arp_header.hard_size = 6;
	// �������� ũ�� ���� (IPv4�� 4)
	arp_header.prot_size = 4;
	// �۾� �ڵ� �ʱ�ȭ (0���� ����)
	arp_header.op = 0;
	// MAC �ּҵ�� IP �ּҵ� �ʱ�ȭ (0���� ����)
	memset(arp_header.MAC_srcaddr, 0, sizeof(arp_header.MAC_srcaddr));
	memset(arp_header.ip_srcaddr, 0, sizeof(arp_header.ip_srcaddr));
	memset(arp_header.MAC_destaddr, 0, sizeof(arp_header.MAC_destaddr));
	memset(arp_header.ip_destaddr, 0, sizeof(arp_header.ip_destaddr));
}


//
// ARP ���̺� ��ȸ �Լ� (Source��) -> ������ �� �´� ������ �𸣰��� �ٵ� �򰥸� �� ���Ƽ� ����
// 
// TODO
// 
// source ip address�� �ִ� �� Ȯ��
// ������ IP Layer�� Source ip address ��û�ؼ�(�Լ� ����)  ARP cache table ������Ʈ
// 
// source MAC address�� �ִ� �� Ȯ��
// ������ Ethernet Layer�� Source MAC address ��û�ؼ�(�Լ� �����) ARP cache table ������Ʈ
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

	// �ּ� �־��ִ� �� ARP table ��ȸ������ �߻�ȭ�� �� ���� �� ������ �ϴ� ���α�..

	// �̴��� ��� �ۼ� �Լ�
	//
	// TODO
	//
	// source MAC �ּ� �ִ� �� Ȯ���ϰ� ������ �̴��� ���̾ ��û�ؼ� ��� ������ �޾ƿͼ� �����ϰ� �־��ֱ�
	// �������� ������ �־��ֱ�
	//

	// ARP ��� �ۼ� �Լ�
	// 
	// TODO
	// 
	// source MAC�� ������ ������ �������� �޾ƿͼ� �־��ְ�
	// source IP �ּ� ���� �� Ȯ���ϰ� ������ IP ���̾ ��û�ؼ� ��� ������ �޾ƿͼ� �����ϰ� �־��ֱ�
	// destination IP �ּ� ���� �� Ȯ���ϰ� ������ IP ���̾ ��û�ؼ� ��� ������ �޾ƿͼ� �����ϰ� �־��ֱ�
	// �������� ������ �־��ֱ�
	//



	return pPacket;
}

//
// ARP reply ��Ŷ �ۼ� �Լ�
unsigned char* CARPLayer::make_reply(unsigned char* ppayload, unsigned char* MAC_target) {

	unsigned char* pPacket = nullptr;

	// ��Ŷ�� ũ�⿡ �°� �޸� �Ҵ�
	unsigned int packetSize = 42;
	pPacket = new unsigned char[packetSize];

	// ��Ŷ ���� �ʱ�ȭ
	memset(pPacket, 0, packetSize);

	// 
	// �Ű������� ���� ppayload�� ARP ��� ����ü�� ĳ�����ϱ�
	// 
	// 
	// �̴��� ��� �ۼ�
	//
	// TODO
	// 
	// Source MAC�� �Ű������� ���� MAC_target�� �־��ֱ�
	// destination MAC���� ĳ������ �� �߿� source MAC �־��ֱ�
	// �������� ������
	// 
	// 
	// ARP ��� �ۼ�
	// 
	// TODO
	// 
	// SOURCE MAC�� �Ű������� ���� MAC_target�� �־��ֱ�
	// destination MAC���� ĳ������ �� �߿� source MAC �־��ֱ�
	// source IP�� ĳ������ �� �߿� destination IP �־��ֱ�
	// destination IP�� ĳ������ �� �߿� source IP �־��ֱ�
	//

	// �� ���ļ� pPacket �ϼ�

	return pPacket;
}



//
// ���� ���̾�(Ethernet Layer)�� ARP ��Ŷ (request, reply �� ��) ���� �Լ�
BOOL CARPLayer::Send(unsigned char* pPacket) {


	BOOL bSuccess = FALSE;

	//
	// TODO
	// 
	// ���� ���̾�(ethernet layer)�� �Ű������� ���� ��Ŷ ����
	//
	//

	return bSuccess;
}


//
// ARP ���̺� ��ȸ �Լ� (Destination��)
//
// TODO
// 
// Destination MAC address�� ARP cache table�� �ִ� �� Ȯ��
// ������ Ethernet Layer�� Destination MAC address ��û (Ethernet Layer�� MAC �ּ� ��ȯ �Լ� �����ؼ� ���)
// �ִٸ� ARP reply ��Ŷ �ۼ�
//


// ���� ���̾�(Ethernet Layer)���� payload �޴� �Լ� (���������� ����ϴ� ��)
BOOL CARPLayer::Receive_Request(unsigned char* ppayload) {

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
		// �̴��� ���̾ source MAC �޶�� ��û ������ ��� ������ ����
		// ARP cache table�� ����� ��� ������ �Ű������� �ֱ� 
		// ARP reply ��Ŷ �ۼ� ��û 
		
		
		
	}


	return bSuccess;
}

// ���� ���̾�(Ethernet Layer)���� payload �޴� �Լ� (��������� ����ϴ� ��)
BOOL CARPLayer::Receive_Reply(unsigned char* ppayload) {

	// ���� �����͸� ARP header ����ü�� ĳ����
	PARP_HEADER pFrame = (PARP_HEADER)ppayload;

	BOOL bSuccess = FALSE;
	
	// TODO
	//
	// ĳ������ �� �߿� source MAC�� ��� ������ ����
	// ��� ���� ������ ARP table ������Ʈ
	// timer stop �ڵ�
	// dlg�� ARP table ������Ʈ �ڵ�


	return bSuccess;
}


//
// IP Layer���� Destination IP �ּ� �޾ƿ��� �Լ�
// 
// TODO
// 
// IP Layer�� ��û�ؼ� ��� ������ ���� (���� ����)
// 
// ��� ���� �̿��ؼ� ARP cache table ������Ʈ
//
//
