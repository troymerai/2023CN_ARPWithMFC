
#include "pch.h" // 다른 include파일들보다 먼저 포함되어야 함
#include "stdafx.h"
#include "BaseLayer.h"
#include "IPLayer.h"

CIPLayer::CIPLayer(char* pName) : CBaseLayer(pName) {
	// 생성자
	// 패킷 초기화?


}

CIPLayer:: ~CIPLayer() {
	// 소멸자
}


// 
// ARP request 패킷 받아서 ARP Layer로 전송
//

