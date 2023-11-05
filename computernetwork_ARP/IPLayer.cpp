
#include "pch.h" // 다른 include파일들보다 먼저 포함되어야 함
#include "stdafx.h"
#include "BaseLayer.h"
#include "IPLayer.h"




// 출발지 IP 주소
unsigned char sourceIP[4];
// 목적지 IP 주소
unsigned char destIP[4];

CIPLayer::CIPLayer(char* pName) : CBaseLayer(pName) {
	// 생성자
	// IP 저장 변수 초기화
	ResetAddr();


}

CIPLayer:: ~CIPLayer() {
	// 소멸자
}


// IP주소 저장 변수 초기화 함수
void CIPLayer::ResetAddr() {
	memset(sourceIP, 0, sizeof(sourceIP));
	memset(destIP, 0, sizeof(destIP));
}


// 하위 레이어(Ethernet Layer)에서 payload 받는 함수
BOOL CIPLayer::Receive(unsigned char* ppayload) {

	

	return true;
}

// dlg에서 Destination IP 주소 받아서 변수에 저장, ARP Layer에 전달하는 함수
void CIPLayer::SetDestIP(unsigned char* IpAddress) {

}


// dlg에서 Source IP 주소 받아서 변수에 저장, ARP Layer에 전달하는 함수
void CIPLayer::SetSourceIP(unsigned char* IpAddress) {

}
