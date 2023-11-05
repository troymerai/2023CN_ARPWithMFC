
#include "pch.h" // 다른 include파일들보다 먼저 포함되어야 함
#include "stdafx.h"
#include "BaseLayer.h"
#include "IPLayer.h"


// 변수 목록
//
// Source IP address 
// Destination IP address
//


CIPLayer::CIPLayer(char* pName) : CBaseLayer(pName) {
	// 생성자
	// 패킷 초기화?


}

CIPLayer:: ~CIPLayer() {
	// 소멸자
}



//
// 하위 레이어(Ethernet Layer)에서 payload 받는 함수
// 
// TODO
// 
// 목적지 IP 주소가 자신의 IP 주소와 같은지 확인
// 다르면 discard
// 같으면 ARP Layer로 payload 전송
//

//
// dlg에서 Destination IP 주소 받아서 변수에 저장, ARP Layer에 전달하는 함수
//
//

//
// dlg에서 Source IP 주소 받아서 변수에 저장, ARP Layer에 전달하는 함수
//
//
