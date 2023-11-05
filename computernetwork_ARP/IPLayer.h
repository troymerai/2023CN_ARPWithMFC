#pragma once
#include "pch.h" // 다른 include파일들보다 먼저 포함되어야 함
#include "BaseLayer.h"

class CIPLayer : public CBaseLayer {

	// 여기에 변수 선언


public:
	CIPLayer(char* pName);
	~CIPLayer(); // 소멸자

	// 여기에 함수 선언

	// IP주소 저장 변수 초기화 함수
	void ResetAddr();

	
	// ARP Layer에서 확인하는 걸로 변경
	// 
	// /////
	// 하위 레이어(Ethernet Layer)에서 payload 받는 함수
	//BOOL CIPLayer::Receive(unsigned char* ppayload);

	// dlg에서 Destination IP 주소 받아서 변수에 저장, ARP Layer에 전달하는 함수
	unsigned char* CIPLayer::GetDestIP(unsigned char* IpAddress);
	// dlg에서 Source IP 주소 받아서 변수에 저장, ARP Layer에 전달하는 함수
	unsigned char* CIPLayer::GetSourceIP(unsigned char* IpAddress);





};