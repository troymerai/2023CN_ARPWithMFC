#include "stdafx.h"
#include "pch.h"
#include "EthernetLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CEthernetLayer::CEthernetLayer(char* pName)
	: CBaseLayer(pName)
{
	ResetHeader();
}

CEthernetLayer::~CEthernetLayer()
{
}

void CEthernetLayer::ResetHeader()
{
	memset(m_sHeader.mac_dstaddr, 0, 6);
	memset(m_sHeader.mac_srcaddr, 0, 6);
	memset(m_sHeader.mac_data, MAC_MAX_DATA_SIZE, 6);
	m_sHeader.mac_type = 0;
}

unsigned char* CEthernetLayer::GetSourceAddress()
{
	return m_sHeader.mac_srcaddr;
}

unsigned char* CEthernetLayer::GetDestinAddress()
{

	// Ethernet ������ �ּ� return
	return m_sHeader.mac_dstaddr;

}

void CEthernetLayer::SetSourceAddress(unsigned char* pAddress)
{

		// �Ѱܹ��� source �ּҸ� Ethernet source�ּҷ� ����
	memcpy(m_sHeader.mac_srcaddr, pAddress, 6);

}

void CEthernetLayer::SetDestinAddress(unsigned char* pAddress)
{
	memcpy(m_sHeader.mac_dstaddr, pAddress, 6);
}

BOOL CEthernetLayer::Send(unsigned char* ppayload, int nlength)
{
	// �� �������� ���� App ������ Frame ���̸�ŭ�� Ethernet������ data�� �ִ´�.
	memcpy(m_sHeader.mac_data, ppayload, nlength);
	// �� �������� ���� type ���� ����� ���Խ�Ų��.
	//memset(m_sHeader.enet_dstaddr, 255, 6);
	m_sHeader.mac_type = MAC_ARP_TYPE;
	BOOL bSuccess = FALSE;

		// Ethernet Data + Ethernet Header�� ����� ���� ũ�⸸ŭ�� Ethernet Frame��
		// File �������� ������.
	bSuccess = this->GetUnderLayer()->Send((unsigned char*)&m_sHeader, MAC_HEADER_SIZE + nlength);
	return bSuccess;
}

BOOL CEthernetLayer::Receive(unsigned char* ppayload)
{
	PETHERNET_HEADER pFrame = (PETHERNET_HEADER)ppayload;

	BOOL bSuccess = FALSE;
	if(memcmp(pFrame->mac_dstaddr, m_sHeader.mac_srcaddr, sizeof(m_sHeader.mac_srcaddr))==0){//�ּ� Ȯ��
			// enet_type�� �������� Ethernet Frame�� data�� �Ѱ��� ���̾ �����Ѵ�.
		if (pFrame->mac_type == MAC_IP_TYPE)
			bSuccess = mp_aUpperLayer[0]->Receive(pFrame->mac_data);
		else if(pFrame->mac_type == MAC_ARP_TYPE)
			bSuccess = mp_aUpperLayer[1]->Receive(pFrame->mac_data);
	}

	return bSuccess;
}
