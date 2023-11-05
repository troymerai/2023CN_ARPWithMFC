
#include "pch.h"
#include "ARPLayer.h"


CARPLayer::CARPLayer(char* pName) : CBaseLayer(pName) {
	// 생성자
	ResetHeader();

}

CARPLayer:: ~CARPLayer() {
	// 소멸자
}

// ARP 헤더 초기화
void CARPLayer::ResetHeader() {
	// 하드웨어 타입 설정 (이더넷은 1)
	arp_header.hard_type = htons(1);  
	// 프로토콜 타입 설정 (IPv4는 0x0800)
	arp_header.prot_type = htons(0x0800);
	// 하드웨어 크기 설정 (이더넷은 6)
	arp_header.hard_size = 6;
	// 프로토콜 크기 설정 (IPv4는 4)
	arp_header.prot_size = 4;
	// 작업 코드 초기화 (0으로 설정)
	arp_header.op = 0;
	// MAC 주소들과 IP 주소들 초기화 (0으로 설정)
	memset(arp_header.MAC_srcaddr, 0, sizeof(arp_header.MAC_srcaddr));
	memset(arp_header.ip_srcaddr, 0, sizeof(arp_header.ip_srcaddr));
	memset(arp_header.MAC_destaddr, 0, sizeof(arp_header.MAC_destaddr));
	memset(arp_header.ip_destaddr, 0, sizeof(arp_header.ip_destaddr));
}


//
// ARP 테이블 조회 함수 (Source용) -> 나누는 게 맞는 건지는 모르겠음 근데 헷갈릴 것 같아서 나눔
// 
// TODO
// 
// source ip address가 있는 지 확인
// 없으면 IP Layer에 Source ip address 요청해서(함수 만듦)  ARP cache table 업데이트
// 
// source MAC address가 있는 지 확인
// 없으면 Ethernet Layer에 Source MAC address 요청해서(함수 만들기) ARP cache table 업데이트
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

	// 주소 넣어주는 거 ARP table 조회쪽으로 추상화할 수 있을 것 같은데 일단 냅두기..

	// 이더넷 헤더 작성 함수
	//
	// TODO
	//
	// source MAC 주소 있는 지 확인하고 없으면 이더넷 레이어에 요청해서 멤버 변수에 받아와서 저장하고 넣어주기
	// 나머지는 고정값 넣어주기
	//

	// ARP 헤더 작성 함수
	// 
	// TODO
	// 
	// source MAC는 위에서 저장한 변수에서 받아와서 넣어주고
	// source IP 주소 없는 지 확인하고 없으면 IP 레이어에 요청해서 멤버 변수에 받아와서 저장하고 넣어주기
	// destination IP 주소 없는 지 확인하고 없으면 IP 레이어에 요청해서 멤버 변수에 받아와서 저장하고 넣어주기
	// 나머지는 고정값 넣어주기
	//



	return pPacket;
}

//
// ARP reply 패킷 작성 함수
unsigned char* CARPLayer::make_reply(unsigned char* ppayload, unsigned char* MAC_target) {

	unsigned char* pPacket = nullptr;

	// 패킷의 크기에 맞게 메모리 할당
	unsigned int packetSize = 42;
	pPacket = new unsigned char[packetSize];

	// 패킷 내용 초기화
	memset(pPacket, 0, packetSize);

	// 
	// 매개변수로 받은 ppayload를 ARP 헤더 구조체로 캐스팅하기
	// 
	// 
	// 이더넷 헤더 작성
	//
	// TODO
	// 
	// Source MAC에 매개변수로 받은 MAC_target값 넣어주기
	// destination MAC으로 캐스팅한 값 중에 source MAC 넣어주기
	// 나머지는 고정값
	// 
	// 
	// ARP 헤더 작성
	// 
	// TODO
	// 
	// SOURCE MAC에 매개변수로 받은 MAC_target값 넣어주기
	// destination MAC으로 캐스팅한 값 중에 source MAC 넣어주기
	// source IP에 캐스팅한 값 중에 destination IP 넣어주기
	// destination IP에 캐스팅한 값 중에 source IP 넣어주기
	//

	// 다 합쳐서 pPacket 완성

	return pPacket;
}



//
// 하위 레이어(Ethernet Layer)로 ARP 패킷 (request, reply 둘 다) 전송 함수
BOOL CARPLayer::Send(unsigned char* pPacket) {


	BOOL bSuccess = FALSE;

	//
	// TODO
	// 
	// 하위 레이어(ethernet layer)로 매개변수로 받은 패킷 전송
	//
	//

	return bSuccess;
}


//
// ARP 테이블 조회 함수 (Destination용)
//
// TODO
// 
// Destination MAC address가 ARP cache table에 있는 지 확인
// 없으면 Ethernet Layer에 Destination MAC address 요청 (Ethernet Layer에 MAC 주소 반환 함수 참조해서 사용)
// 있다면 ARP reply 패킷 작성
//


// 하위 레이어(Ethernet Layer)에서 payload 받는 함수 (목적지에서 사용하는 것)
BOOL CARPLayer::Receive_Request(unsigned char* ppayload) {

	// 받은 데이터를 ARP header 구조체로 캐스팅
	PARP_HEADER pFrame = (PARP_HEADER)ppayload;

	BOOL bSuccess = FALSE;

	// ARP header의 목적지 ip 주소가 내 ip와 같은 지 IP Layer에 요청
	
	// 상위 계층 (IP Layer)의 참조를 얻음
	CIPLayer* pIPLayer = dynamic_cast<CIPLayer*>(this->GetUpperLayer(0));

	// 목적지 IP 주소가 내 IP와 같은 경우
	if (pIPLayer != nullptr && pIPLayer->IsMyIpAddress(pFrame->ip_destaddr)) {
		
		// TODO
		// 
		// 이더넷 레이어에 source MAC 달라고 요청 보내서 멤버 변수에 저장
		// ARP cache table에 저장된 멤버 변수를 매개변수로 넣기 
		// ARP reply 패킷 작성 요청 
		
		
		
	}


	return bSuccess;
}

// 하위 레이어(Ethernet Layer)에서 payload 받는 함수 (출발지에서 사용하는 것)
BOOL CARPLayer::Receive_Reply(unsigned char* ppayload) {

	// 받은 데이터를 ARP header 구조체로 캐스팅
	PARP_HEADER pFrame = (PARP_HEADER)ppayload;

	BOOL bSuccess = FALSE;
	
	// TODO
	//
	// 캐스팅한 값 중에 source MAC을 멤버 변수에 저장
	// 멤버 변수 값으로 ARP table 업데이트
	// timer stop 코드
	// dlg에 ARP table 업데이트 코드


	return bSuccess;
}


//
// IP Layer에서 Destination IP 주소 받아오는 함수
// 
// TODO
// 
// IP Layer에 요청해서 멤버 변수에 저장 (참조 형식)
// 
// 멤버 변수 이용해서 ARP cache table 업데이트
//
//
