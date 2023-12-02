#include "stdafx.h"
#include "pch.h"
#include "EthernetLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// ������
CEthernetLayer::CEthernetLayer(char* pName)
	: CBaseLayer(pName)
{
	// ����, �ܺ� ��Ʈ��ũ �̴��� ��� �ʱ�ȭ
	ResetHeader(INNER);
	ResetHeader(OUTER);
}

// �Ҹ���
CEthernetLayer::~CEthernetLayer()
{
}

// �̴��� ��� �ʱ�ȭ �Լ�
void CEthernetLayer::ResetHeader(int iosel)
{
	memset(m_sHeader[iosel].enet_dstaddr, 0, 6);
	memset(m_sHeader[iosel].enet_srcaddr, 0, 6);
	memset(m_sHeader[iosel].enet_data, 0, ETHER_MAX_DATA_SIZE);
	m_sHeader[iosel].enet_type = 0;
}

// ����� MAC �ּ� ��ȯ �Լ�
unsigned char* CEthernetLayer::GetSourceAddress(int iosel)
{
	return m_sHeader[iosel].enet_srcaddr;
}

// ������ MAC �ּ� ��ȯ �Լ�
unsigned char* CEthernetLayer::GetDestinAddress(int iosel)
{
	return m_sHeader[iosel].enet_dstaddr;
}

// ����� MAC �ּ� ���� �Լ�
void CEthernetLayer::SetSourceAddress(unsigned char* pAddress, int iosel)
{
	// ���ڷ� �Ѱܹ��� source �ּҸ� Ethernet source�ּҷ� ����
	memcpy(m_sHeader[iosel].enet_srcaddr, pAddress, 6);
}

// ������ MAC �ּ� ���� �Լ�
void CEthernetLayer::SetDestinAddress(unsigned char* pAddress, int iosel)
{
	// ���ڷ� �Ѱܹ��� destination �ּҸ� Ethernet dest�ּҷ� ����
	memcpy(m_sHeader[iosel].enet_dstaddr, pAddress, 6);
}

// �̴��� ���� ��Ŷ ���� �Լ�
BOOL CEthernetLayer::Send(unsigned char* ppayload, int nlength, int iosel)
{
	// �� �������� ���� Frame ���̸�ŭ�� Ethernet������ data�� ����
	memcpy(m_sHeader[iosel].enet_data, ppayload, nlength);
	BOOL bSuccess = FALSE;

	// ���� �������� ��Ŷ�� ���� 
	// �����ϴ� �������� ũ��� Ethernet ����� ũ��� ���� �������� ũ�� �� ���� ������ ���� -> ���� �����Ͱ� �� ũ�� ������ �ս� �߻�?
	bSuccess = mp_UnderLayer->Send((unsigned char*)&m_sHeader[iosel], ETHER_HEADER_SIZE + nlength > ETHER_MAX_SIZE ? ETHER_MAX_SIZE: ETHER_HEADER_SIZE+nlength, iosel);
	
	return bSuccess;
}

// Ethernet Ÿ�� ���� �Լ�
void CEthernetLayer::SetType(unsigned short type, int iosel)
{
	m_sHeader[iosel].enet_type = type;
}

// �̴��� ���� ��Ŷ ���� �Լ�
BOOL CEthernetLayer::Receive(unsigned char* ppayload, int iosel)
{
	// �̴��� ����� �޾Ƽ� �̴��� ��� ����ü�� Ÿ�� ĳ����
	PETHERNET_HEADER pFrame = (PETHERNET_HEADER)ppayload;

	unsigned char broad[6] = { 255, 255, 255, 255, 255, 255 };

	// �ڽſ��Լ� ����� GARP request�� ����
	// �۽����� �ּҰ� �ڽ��� �ּҿ� ���ٸ� ��Ŷ�� ����
	if (memcmp(pFrame->enet_srcaddr, m_sHeader[iosel].enet_srcaddr, ENET_ADDR_SIZE) == 0) {
		return FALSE;
	}

	BOOL bSuccess = FALSE;

	// �������� �ּҰ� �ڽ��� �ּҿ� ���ٸ� ��Ŷ�� ����
	if(memcmp(pFrame->enet_dstaddr, m_sHeader[iosel].enet_srcaddr, sizeof(m_sHeader[iosel].enet_srcaddr)) == 0) {
		
		// enet_type�� �������� Ethernet Frame�� data�� �Ѱ��� ���̾ ����

		// IP Ÿ���� ���, ���� ����(IP Layer)���� ������ ����
		if (pFrame->enet_type == ETHER_IP_TYPE)
			bSuccess = mp_aUpperLayer[0]->Receive(pFrame->enet_data, iosel);
		// ARP Ÿ���� ���, ���� ����(ARP Layer)���� ������ ����
		else if(pFrame->enet_type == ETHER_ARP_TYPE)
			bSuccess = mp_aUpperLayer[1]->Receive(pFrame->enet_data, iosel);
	}

	// �������� �ּҰ� ��ε�ĳ��Ʈ �ּҿ� ���ٸ� ��Ŷ�� ����
	// GARP�� �´µ� PARP��?
	else if (memcmp(pFrame->enet_dstaddr, broad, ENET_ADDR_SIZE) == 0) {
		if (pFrame->enet_type == ETHER_ARP_TYPE)
			bSuccess = mp_aUpperLayer[1]->Receive(pFrame->enet_data, iosel);
		else if (pFrame->enet_type == ETHER_IP_TYPE)
			bSuccess = mp_aUpperLayer[0]->Receive(pFrame->enet_data, iosel);
	}

	return bSuccess;
}
