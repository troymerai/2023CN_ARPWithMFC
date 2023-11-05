
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

///// 
// ARP Layer에서 확인하는 걸로 변경
// 
// 하위 레이어(Ethernet Layer)에서 payload 받는 함수
//BOOL CIPLayer::Receive(unsigned char* ppayload) {
// return true;
//}

// dlg에서 Destination IP 주소 받아서 변수에 저장하는 함수
void CIPLayer::SetDestIP(unsigned char* IpAddress) {

	// 매개변수로 전달받은 IP 주소를 destIP에 복사
	memcpy(destIP, IpAddress, sizeof(destIP));

}

// dlg에서 Source IP 주소 받아서 변수에 저장하는 함수
void CIPLayer::SetSourceIP(unsigned char* IpAddress) {

	// 매개변수로 전달받은 IP 주소를 destIP에 복사
	memcpy(sourceIP, IpAddress, sizeof(sourceIP));

}

// Destination IP 주소를 반환하는 함수
unsigned char* CIPLayer::GetDestIP() {

	return destIP;
}


// Source IP 주소를 반환하는 함수
unsigned char* CIPLayer::GetSourceIP() {

	return sourceIP;
}

// 매개변수로 받은 주소가 내 ip 주소와 같은 지 확인하는 함수
BOOL CIPLayer::IsMyIpAddress(unsigned char* IpAddress) {

	BOOL isMyIP = FALSE;

	if (memcmp(IpAddress, sourceIP, sizeof(sourceIP)) == 0) {
		isMyIP = TRUE;
	}
	
	return isMyIP;
}