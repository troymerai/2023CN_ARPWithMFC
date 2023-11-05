

#include "pch.h"
#include "stdafx.h"
#include "EthernetLayer.h"


// 출발지 MAC 주소
//unsigned char sourceMAC[6];
// 목적지 MAC 주소
//unsigned char destMAC[6];

//CEthernetLayer::CEthernetLayer(char* pName) :CBaseLayer(pName) {

	// 생성자
//}

//CEthernetLayer:: ~CEthernetLayer() {

	// 소멸자
//}


//
// source MAC 주소 따오는 함수
// 
// TODO
// NI Layer 참조해서 source MAC 주소 따오기
//
//

//
// 
//
//



// 상위 레이어(ARP Layer)로 패킷 전송하는 함수
//BOOL CEthernetLayer::Send(unsigned char* ppayload) {
	// 
	// TODO
	// 
	// 상위 레이어로 전송 코드
	// 
//}

// 하위 레이어(NI Layer)에서 패킷 받아서 MAC 주소 확인하는 함수
//BOOL CEthernetLayer::Receive(unsigned char* pPacket) {
	// 
	// TODO
	// 
	// 패킷 받아서 이더넷 헤더 타입으로 캐스팅
	// 이더넷 헤더의 목적지 MAC 주소가 broadcast 또는 source MAC 주소인지 확인
	// broadcast, source MAC 주소면 packet을 상위 레이어(ARP Layer)로 이더넷 헤더 떼고 ARP 헤더만(ppayload) 전송 (전송 함수 따로 만들기)
	
//}