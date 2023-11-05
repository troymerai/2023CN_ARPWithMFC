
#include "pch.h"
#include "ARPLayer.h"


CARPLayer::CARPLayer(char* pName) : CBaseLayer(pName) {
	// 생성자
}

CARPLayer:: ~CARPLayer() {

}



//
// ARP 테이블 조회 함수 (Source용) -> 나누는 게 맞는 건지는 모르겠음 근데 헷갈릴 것 같아서 나눔
// 
// TODO
// 
// source ip address가 있는 지 확인
// 없으면 IP Layer에 Source ip address 요청해서 ARP cache table 업데이트
// 
// source MAC address가 있는 지 확인
// 없으면 Ethernet Layer에 Source MAC address 요청해서 ARP cache table 업데이트
// 
// 

//
// ARP request 패킷 작성 함수
unsigned char* CARPLayer::make_request() {
	unsigned char* pPacket = nullptr;

	// 패킷의 크기에 맞게 메모리 할당
	unsigned int packetSize = 42;
	pPacket = new unsigned char[packetSize];

	// 패킷 내용 초기화
	memset(pPacket, 0, packetSize);


	// 이더넷 헤더 작성 함수

	// ARP 헤더 작성 함수

	return pPacket;
}

//
// ARP reply 패킷 작성 함수
unsigned char* CARPLayer::make_reply() {
	unsigned char* pPacket = nullptr;

	// 패킷의 크기에 맞게 메모리 할당
	unsigned int packetSize = 42;
	pPacket = new unsigned char[packetSize];

	// 패킷 내용 초기화
	memset(pPacket, 0, packetSize);

	// 이더넷 헤더에 자기 MAC 넣는 함수

	//  ARP 헤더에 자기 MAC 넣는 함수

	return pPacket;
}



//
// 하위 레이어(Ethernet Layer)로 ARP 패킷 (request, reply 둘 다) 전송 함수





//
// 상위 레이어(IP Layer)에서 payload 받는 함수
//
//

//
// ARP 테이블 조회 함수 (Destination용)
//
// TODO
// 
// Destination MAC address가 ARP cache table에 있는 지 확인
// 없으면 Ethernet Layer에 Destination MAC address 요청 (Ethernet Layer의 MAC 주소 추출 함수 그대로 사용)
// 있다면 ARP reply 패킷 작성
//

//
// 하위 레이어(Ethernet Layer)에서 payload 받는 함수
// 
// TODO
// 
// ARP cache table에 Destination MAC 주소 업데이트
// Dlg에 업데이트하는 함수
// 타이머 stop함수
// status를 complete로 바꿔주는 함수
// 
//

//
// IP Layer에서 Destination IP 주소 받아오는 함수
// 
// TODO
// 
// 받아와서 ARP cache table 업데이트
//
//
