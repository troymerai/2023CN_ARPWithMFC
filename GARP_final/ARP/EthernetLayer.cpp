#include "stdafx.h"
#include "pch.h"
#include "EthernetLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


// 생성자
CEthernetLayer::CEthernetLayer(char* pName)
	: CBaseLayer(pName)
{
	// 이더넷 헤더 초기화
	ResetHeader();
}

// 소멸자
CEthernetLayer::~CEthernetLayer()
{
}


// 이더넷 헤더 초기화 함수
void CEthernetLayer::ResetHeader()
{
	memset(m_sHeader.enet_dstaddr, 0, 6);
	memset(m_sHeader.enet_srcaddr, 0, 6);
	memset(m_sHeader.enet_data, ETHER_MAX_DATA_SIZE, 6);
	m_sHeader.enet_type = 0;
}

// 출발지 MAC 주소 반환 함수
unsigned char* CEthernetLayer::GetSourceAddress()
{
	return m_sHeader.enet_srcaddr;
}

// 목적지 MAC 주소 반환 함수
unsigned char* CEthernetLayer::GetDestinAddress()
{

	return m_sHeader.enet_dstaddr;

}

// 출발지 MAC 주소 설정 함수
void CEthernetLayer::SetSourceAddress(unsigned char* pAddress)
{
	// 매개변수로 넘겨받은 source 주소를 Ethernet source주소로 지정
	memcpy(m_sHeader.enet_srcaddr, pAddress, 6);

}

// 목적지 MAC 주소 설정 함수
void CEthernetLayer::SetDestinAddress(unsigned char* pAddress)
{
	// 매개변수로 넘겨받은 destination 주소를 Ethernet dest주소로 지정
	memcpy(m_sHeader.enet_dstaddr, pAddress, 6);
}

// 이더넷 계층 패킷 전송 함수
BOOL CEthernetLayer::Send(unsigned char* ppayload, int nlength)
{
	// 윗 계층에서 받은 Frame 길이만큼을 Ethernet계층의 data로 넣음
	memcpy(m_sHeader.enet_data, ppayload, nlength);

	//memset(m_sHeader.enet_dstaddr, 255, 6);
	m_sHeader.enet_type = ETHER_ARP_TYPE;

	BOOL bSuccess = FALSE;

	// 이더넷 데이터와 이더넷 헤더의 사이즈를 합한 크기만큼의 이더넷 프레임을 하위 계층으로 전송
	bSuccess = this->GetUnderLayer()->Send((unsigned char*)&m_sHeader, ETHER_HEADER_SIZE + nlength);
	
	return bSuccess;
}

// 이더넷 계층 패킷 수신 함수
BOOL CEthernetLayer::Receive(unsigned char* ppayload)
{
	// 이더넷 헤더를 받아서 이더넷 헤더 구조체로 타입 캐스팅
	PETHERNET_HEADER pFrame = (PETHERNET_HEADER)ppayload;

	unsigned char broad[6] = { 255, 255, 255, 255, 255, 255 };

	BOOL bSuccess = FALSE;

	// 목적지 주소를 확인
	if(memcmp(pFrame->enet_dstaddr, m_sHeader.enet_srcaddr, sizeof(m_sHeader.enet_srcaddr))==0){//주소 확인
		
		// enet_type을 기준으로 이더넷 프레임의 데이터를 넘겨줄 레이어 지정

		// IP 타입의 경우, 상위 계층(IP Layer)으로 데이터 전달
		if (pFrame->enet_type == ETHER_IP_TYPE)
			bSuccess = mp_aUpperLayer[0]->Receive(pFrame->enet_data);
		// ARP 타입의 경우, 상위 계층(ARP Layer)으로 데이터 전달
		else if(pFrame->enet_type == ETHER_ARP_TYPE)
			bSuccess = mp_aUpperLayer[1]->Receive(pFrame->enet_data);
	}

	// GARP인 경우, 상위 계층(ARP Layer)으로 데이터 전달
	else if (memcmp(pFrame->enet_dstaddr, broad, ENET_ADDR_SIZE) == 0) {
		if (pFrame->enet_type == ETHER_ARP_TYPE)
			bSuccess = mp_aUpperLayer[1]->Receive(pFrame->enet_data);
	}

	return bSuccess;
}
