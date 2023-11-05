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

	// ���� ���̾�(Ethernet Layer)�� ARP ��Ŷ (request, reply �� ��) ���� �Լ�
	BOOL CARPLayer::Send();

	// ���� ���̾�(Ethernet Layer)���� payload �޴� �Լ�
	BOOL CARPLayer::Receive(unsigned char* ppayload);

	// ARP ��� �ʱ�ȭ
	void CARPLayer::ResetHeader();

	// ARP Header 
	typedef struct _ARP_HEADER {
		unsigned short hard_type; // Hardware type (�̴����̸� 1)
		unsigned short prot_type; // Protocol type (IPv4�� 0x0800)
		unsigned char hard_size;  // Hardware size (�̴����̸� 6)
		unsigned char prot_size;  // Protocol size (IPv4�� 4)
		unsigned short op;        // Operation code (1 for request, 2 for reply)
		unsigned char MAC_srcaddr[6]; // ����� MAC �ּ�
		unsigned char ip_srcaddr[4];       // ����� IP �ּ�
		unsigned char MAC_destaddr[6]; // ������ MAC �ּ� (0 for request)
		unsigned char ip_destaddr[4];       // ������ IP �ּ�
	} ARP_HEADER, * PARP_HEADER;

	ARP_HEADER arp_header;

};