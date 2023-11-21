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
	// �̴��� ��� �ʱ�ȭ
	ResetHeader();
}

// �Ҹ���
CEthernetLayer::~CEthernetLayer()
{
}


// �̴��� ��� �ʱ�ȭ �Լ�
void CEthernetLayer::ResetHeader()
{
	memset(m_sHeader.enet_dstaddr, 0, 6);
	memset(m_sHeader.enet_srcaddr, 0, 6);
	memset(m_sHeader.enet_data, ETHER_MAX_DATA_SIZE, 6);
	m_sHeader.enet_type = 0;
}

// ����� MAC �ּ� ��ȯ �Լ�
unsigned char* CEthernetLayer::GetSourceAddress()
{
	return m_sHeader.enet_srcaddr;
}

// ������ MAC �ּ� ��ȯ �Լ�
unsigned char* CEthernetLayer::GetDestinAddress()
{

	return m_sHeader.enet_dstaddr;

}

// ����� MAC �ּ� ���� �Լ�
void CEthernetLayer::SetSourceAddress(unsigned char* pAddress)
{
	// �Ű������� �Ѱܹ��� source �ּҸ� Ethernet source�ּҷ� ����
	memcpy(m_sHeader.enet_srcaddr, pAddress, 6);

}

// ������ MAC �ּ� ���� �Լ�
void CEthernetLayer::SetDestinAddress(unsigned char* pAddress)
{
	// �Ű������� �Ѱܹ��� destination �ּҸ� Ethernet dest�ּҷ� ����
	memcpy(m_sHeader.enet_dstaddr, pAddress, 6);
}

// �̴��� ���� ��Ŷ ���� �Լ�
BOOL CEthernetLayer::Send(unsigned char* ppayload, int nlength)
{
	// �� �������� ���� Frame ���̸�ŭ�� Ethernet������ data�� ����
	memcpy(m_sHeader.enet_data, ppayload, nlength);

	//memset(m_sHeader.enet_dstaddr, 255, 6);
	m_sHeader.enet_type = ETHER_ARP_TYPE;

	BOOL bSuccess = FALSE;

	// �̴��� �����Ϳ� �̴��� ����� ����� ���� ũ�⸸ŭ�� �̴��� �������� ���� �������� ����
	bSuccess = this->GetUnderLayer()->Send((unsigned char*)&m_sHeader, ETHER_HEADER_SIZE + nlength);
	
	return bSuccess;
}

// �̴��� ���� ��Ŷ ���� �Լ�
BOOL CEthernetLayer::Receive(unsigned char* ppayload)
{
	// �̴��� ����� �޾Ƽ� �̴��� ��� ����ü�� Ÿ�� ĳ����
	PETHERNET_HEADER pFrame = (PETHERNET_HEADER)ppayload;

	unsigned char broad[6] = { 255, 255, 255, 255, 255, 255 };

	BOOL bSuccess = FALSE;

	// ������ �ּҸ� Ȯ��
	if(memcmp(pFrame->enet_dstaddr, m_sHeader.enet_srcaddr, sizeof(m_sHeader.enet_srcaddr))==0){//�ּ� Ȯ��
		
		// enet_type�� �������� �̴��� �������� �����͸� �Ѱ��� ���̾� ����

		// IP Ÿ���� ���, ���� ����(IP Layer)���� ������ ����
		if (pFrame->enet_type == ETHER_IP_TYPE)
			bSuccess = mp_aUpperLayer[0]->Receive(pFrame->enet_data);
		// ARP Ÿ���� ���, ���� ����(ARP Layer)���� ������ ����
		else if(pFrame->enet_type == ETHER_ARP_TYPE)
			bSuccess = mp_aUpperLayer[1]->Receive(pFrame->enet_data);
	}

	// GARP�� ���, ���� ����(ARP Layer)���� ������ ����
	else if (memcmp(pFrame->enet_dstaddr, broad, ENET_ADDR_SIZE) == 0) {
		if (pFrame->enet_type == ETHER_ARP_TYPE)
			bSuccess = mp_aUpperLayer[1]->Receive(pFrame->enet_data);
	}

	return bSuccess;
}
