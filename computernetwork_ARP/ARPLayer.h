#pragma once
#include "pch.h"
#include "BaseLayer.h"

class CARPLayer : public CBaseLayer {

	// 여기에 변수 선언


public:
	CARPLayer(char* pName);
	~CARPLayer(); // 소멸자


	// ARP request 패킷 작성 함수
	unsigned char* CARPLayer::make_request();
	// ARP reply 패킷 작성 함수
	unsigned char* CARPLayer::make_reply();

	// 하위 레이어(Ethernet Layer)로 ARP 패킷 (request, reply 둘 다) 전송 함수
	BOOL CARPLayer::Send();

	// 하위 레이어(Ethernet Layer)에서 payload 받는 함수
	BOOL CARPLayer::Receive(unsigned char* ppayload);

	// ARP Header 
	typedef struct _ARP_HEADER {
		unsigned short hard_type; // Hardware type (이더넷이면 1)
		unsigned short prot_type; // Protocol type (IPv4면 0x0800)
		unsigned char hard_size;  // Hardware size (이더넷이면 6)
		unsigned char prot_size;  // Protocol size (IPv4면 4)
		unsigned short op;        // Operation code (1 for request, 2 for reply)
		unsigned char MAC_srcaddr[6]; // 출발지 MAC 주소
		unsigned char IP_srcaddr[4];       // 출발지 IP 주소
		unsigned char MAC_dstaddr[6]; // 목적지 MAC 주소 (0 for request)
		unsigned char IP_dstaddr[4];       // 목적지 IP 주소
	} ARP_HEADER, * PARP_HEADER;

	ARP_HEADER arp_header;

};