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
	// 내부, 외부 네트워크 이더넷 헤더 초기화
	ResetHeader(INNER);
	ResetHeader(OUTER);
}

// 소멸자
CEthernetLayer::~CEthernetLayer()
{
}

// 이더넷 헤더 초기화 함수
void CEthernetLayer::ResetHeader(int iosel)
{
	memset(m_sHeader[iosel].enet_dstaddr, 0, 6);
	memset(m_sHeader[iosel].enet_srcaddr, 0, 6);
	memset(m_sHeader[iosel].enet_data, 0, ETHER_MAX_DATA_SIZE);
	m_sHeader[iosel].enet_type = 0;
}

// 출발지 MAC 주소 반환 함수
unsigned char* CEthernetLayer::GetSourceAddress(int iosel)
{
	return m_sHeader[iosel].enet_srcaddr;
}

// 목적지 MAC 주소 반환 함수
unsigned char* CEthernetLayer::GetDestinAddress(int iosel)
{
	return m_sHeader[iosel].enet_dstaddr;
}

// 출발지 MAC 주소 설정 함수
void CEthernetLayer::SetSourceAddress(unsigned char* pAddress, int iosel)
{
	// 인자로 넘겨받은 source 주소를 Ethernet source주소로 지정
	memcpy(m_sHeader[iosel].enet_srcaddr, pAddress, 6);
}

// 목적지 MAC 주소 설정 함수
void CEthernetLayer::SetDestinAddress(unsigned char* pAddress, int iosel)
{
	// 인자로 넘겨받은 destination 주소를 Ethernet dest주소로 지정
	memcpy(m_sHeader[iosel].enet_dstaddr, pAddress, 6);
}

// 이더넷 계층 패킷 전송 함수
BOOL CEthernetLayer::Send(unsigned char* ppayload, int nlength, int iosel)
{
	// 윗 계층에서 받은 Frame 길이만큼을 Ethernet계층의 data로 넣음
	memcpy(m_sHeader[iosel].enet_data, ppayload, nlength);
	BOOL bSuccess = FALSE;

	// 하위 계층으로 패킷을 전송 
	// 전송하는 데이터의 크기는 Ethernet 헤더의 크기와 전송 데이터의 크기 중 작은 것으로 정함 -> 전송 데이터가 더 크면 데이터 손실 발생?
	bSuccess = mp_UnderLayer->Send((unsigned char*)&m_sHeader[iosel], ETHER_HEADER_SIZE + nlength > ETHER_MAX_SIZE ? ETHER_MAX_SIZE: ETHER_HEADER_SIZE+nlength, iosel);
	
	return bSuccess;
}

// Ethernet 타입 설정 함수
void CEthernetLayer::SetType(unsigned short type, int iosel)
{
	m_sHeader[iosel].enet_type = type;
}

// 이더넷 계층 패킷 수신 함수
BOOL CEthernetLayer::Receive(unsigned char* ppayload, int iosel)
{
	// 이더넷 헤더를 받아서 이더넷 헤더 구조체로 타입 캐스팅
	PETHERNET_HEADER pFrame = (PETHERNET_HEADER)ppayload;

	unsigned char broad[6] = { 255, 255, 255, 255, 255, 255 };

	// 자신에게서 출발한 GARP request는 무시
	// 송신자의 주소가 자신의 주소와 같다면 패킷을 무시
	if (memcmp(pFrame->enet_srcaddr, m_sHeader[iosel].enet_srcaddr, ENET_ADDR_SIZE) == 0) {
		return FALSE;
	}

	BOOL bSuccess = FALSE;

	// 목적지의 주소가 자신의 주소와 같다면 패킷을 수신
	if(memcmp(pFrame->enet_dstaddr, m_sHeader[iosel].enet_srcaddr, sizeof(m_sHeader[iosel].enet_srcaddr)) == 0) {
		
		// enet_type을 기준으로 Ethernet Frame의 data를 넘겨줄 레이어를 지정

		// IP 타입의 경우, 상위 계층(IP Layer)으로 데이터 전달
		if (pFrame->enet_type == ETHER_IP_TYPE)
			bSuccess = mp_aUpperLayer[0]->Receive(pFrame->enet_data, iosel);
		// ARP 타입의 경우, 상위 계층(ARP Layer)으로 데이터 전달
		else if(pFrame->enet_type == ETHER_ARP_TYPE)
			bSuccess = mp_aUpperLayer[1]->Receive(pFrame->enet_data, iosel);
	}

	// GARP
	// 목적지의 주소가 브로드캐스트 주소와 같다면 패킷을 수신
	else if (memcmp(pFrame->enet_dstaddr, broad, ENET_ADDR_SIZE) == 0) {
		if (pFrame->enet_type == ETHER_ARP_TYPE)
			bSuccess = mp_aUpperLayer[1]->Receive(pFrame->enet_data, iosel);
		else if (pFrame->enet_type == ETHER_IP_TYPE)
			bSuccess = mp_aUpperLayer[0]->Receive(pFrame->enet_data, iosel);
	}

	return bSuccess;
}
