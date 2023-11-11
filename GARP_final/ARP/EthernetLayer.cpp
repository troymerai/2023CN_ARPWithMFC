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
	memset(m_sHeader.mac_dstaddr, 0, 6);
	memset(m_sHeader.mac_srcaddr, 0, 6);
	memset(m_sHeader.mac_data, MAC_MAX_DATA_SIZE, 6);
	m_sHeader.mac_type = 0;
}

// ����� MAC �ּ� ��ȯ �Լ�
unsigned char* CEthernetLayer::GetSourceAddress()
{
	return m_sHeader.mac_srcaddr;
}

// ������ MAC �ּ� ��ȯ �Լ�
unsigned char* CEthernetLayer::GetDestinAddress()
{

	return m_sHeader.mac_dstaddr;

}

// ����� MAC �ּ� ���� �Լ�
void CEthernetLayer::SetSourceAddress(unsigned char* pAddress)
{

	// �Ű������� �Ѱܹ��� source �ּҸ� Ethernet source�ּҷ� ����
	memcpy(m_sHeader.mac_srcaddr, pAddress, 6);

}

// ������ MAC �ּ� ���� �Լ�
void CEthernetLayer::SetDestinAddress(unsigned char* pAddress)
{
	// �Ű������� �Ѱܹ��� destination �ּҸ� Ethernet dest�ּҷ� ����
	memcpy(m_sHeader.mac_dstaddr, pAddress, 6);
}

// �̴��� ���� ��Ŷ ���� �Լ�
BOOL CEthernetLayer::Send(unsigned char* ppayload, int nlength)
{
	// �� �������� ���� Frame ���̸�ŭ�� Ethernet������ data�� ����
	memcpy(m_sHeader.mac_data, ppayload, nlength);
	// �� �������� ���� type ���� ����� ����
	m_sHeader.mac_type = MAC_ARP_TYPE;

	BOOL bSuccess = FALSE;

	// �̴��� �����Ϳ� �̴��� ����� ����� ���� ũ�⸸ŭ�� �̴��� �������� ���� �������� ����
	bSuccess = this->GetUnderLayer()->Send((unsigned char*)&m_sHeader, MAC_HEADER_SIZE + nlength);
	
	return bSuccess;
}

// �̴��� ���� ��Ŷ ���� �Լ�
BOOL CEthernetLayer::Receive(unsigned char* ppayload)
{
	// �̴��� ����� �޾Ƽ� �̴��� ��� ����ü�� Ÿ�� ĳ����
	PETHERNET_HEADER pFrame = (PETHERNET_HEADER)ppayload;

	BOOL bSuccess = FALSE;

	// ������ �ּҸ� Ȯ��
	if(memcmp(pFrame->mac_dstaddr, m_sHeader.mac_srcaddr, sizeof(m_sHeader.mac_srcaddr))==0){
		
		// enet_type�� �������� �̴��� �������� �����͸� �Ѱ��� ���̾� ����

		// IP Ÿ���� ���, ���� ����(IP Layer)���� ������ ����
		if (pFrame->mac_type == MAC_IP_TYPE)
			bSuccess = mp_aUpperLayer[0]->Receive(pFrame->mac_data);
		// ARP Ÿ���� ���, ���� ����(ARP Layer)���� ������ ����
		else if(pFrame->mac_type == MAC_ARP_TYPE)
			bSuccess = mp_aUpperLayer[1]->Receive(pFrame->mac_data);
	}

	return bSuccess;
}
