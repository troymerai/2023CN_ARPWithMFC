
#include "pch.h"
#include "stdafx.h"
#include "IPLayer.h"



// ������
CIPLayer::CIPLayer(char* pName) : CBaseLayer(pName){
    
    // ��� ���� �ʱ�ȭ
    ResetHeader();

    ///////////////////////////////////
    ///////////////////////////////////
    // ���� IP ����� �� ������ �ʿ�� ������, ��ü �������� �ڵ带 ¥�� ����
    // ���߿� ������ ���� ���� ������.. 
    ///////////////////////////////////
    ///////////////////////////////////

    // IP ������ ��� ���̸� ���� 
    // ���⼭�� IP ���� 4�� ��� ���� 20����Ʈ�� �ǹ��ϴ� 0x45�� ����
    m_sHeader.ver_hlegnth = 0x45;

    // ���� Ÿ���� ���� 
    // ���⼭�� �⺻���� 0x00�� ����
    m_sHeader.tos = 0x00;

    // ��ü ��Ŷ ���̸� ���� 
    // ���⼭�� �ִ밪�� 0xffff�� ����
    m_sHeader.tlength = 0xffff;

    // ��Ŷ �ĺ��ڸ� ���� 
    // ���⼭�� 0x0000���� ����
    m_sHeader.id = 0x0000;

    // �÷��׿� �����׸�Ʈ �������� ���� 
    // ���⼭�� 0x00���� �����մϴ�.
    m_sHeader.offset = 0x00;

    // ������ ���� �ð��� ���� 
    // ���⼭�� �ִ밪�� 0xff�� ����
    m_sHeader.ttl = 0xff;

    // �������� Ÿ���� ���� 
    // ���⼭�� TCP�� �ǹ��ϴ� 0x06�� ����
    m_sHeader.ptype = 0x06;

    // üũ���� ���� 
    // �ʱⰪ���� 0x0000�� ����
    m_sHeader.checksum = 0x0000;
}

// �Ҹ���
CIPLayer::~CIPLayer(){
}

// IP �ּ� �ʱ�ȭ �Լ�
void CIPLayer::ResetHeader(){

    // ����� IP �ּ� 0���� �ʱ�ȭ
    memset(m_sHeader.ip_srcaddr, 0, IP_ADDR_SIZE);
    // ������ IP �ּ� 0���� �ʱ�ȭ
    memset(m_sHeader.ip_dstaddr, 0, IP_ADDR_SIZE);
    // ������ �ʵ� 0���� �ʱ�ȭ 
    memset(m_sHeader.ip_data, 0, IP_MAX_DATA_SIZE);
}

// ����� IP �ּ� ��ȯ �Լ�
unsigned char* CIPLayer::GetSourceAddress(){
    return m_sHeader.ip_srcaddr;
}

// ������ IP �ּ� ��ȯ �Լ�
unsigned char* CIPLayer::GetDestinAddress() {
    return m_sHeader.ip_dstaddr;
}

// ����� IP �ּ� ���� �Լ�
void CIPLayer::SetSourceAddress(unsigned char* pAddress){
    memcpy(m_sHeader.ip_srcaddr, pAddress, IP_ADDR_SIZE);
}

// ������ IP �ּ� ���� �Լ�
void CIPLayer::SetDestinAddress(unsigned char* pAddress){
    memcpy(m_sHeader.ip_dstaddr, pAddress, IP_ADDR_SIZE);
}


//UpperLayer = AppLayer, UnderLayer = ARPLayer?
// 
// IP ���� ��Ŷ ���� �Լ�
BOOL CIPLayer::Send(unsigned char* ppayload, int nlength){
    
    // argument�� ���� payload�� IP data field�� ����
    memcpy(m_sHeader.ip_data, ppayload, nlength);
    
    // ��Ŷ ���� ���� ���� ���� ���� ����
    BOOL bSuccess = FALSE;

    // ���� ���̾�� ��Ŷ ���� && ����� bSuccess�� ����
    bSuccess = this->GetUnderLayer()->Send((unsigned char*)&m_sHeader, IP_HEADER_SIZE + nlength);
    
    // ��Ŷ ���� ���� ���� ��ȯ
    return bSuccess;
}

// IP ���� ��Ŷ ���� �Լ�
BOOL CIPLayer::Receive(unsigned char* ppayload){

    // ��Ŷ ���� ���� ���� ���� ����
    BOOL bSuccess = FALSE;

    // argument�� ���� payload�� IP��� ����ü�� Ÿ�� ĳ����
    PIP_HEADER pFrame = (PIP_HEADER)ppayload;

    // ������ ��Ŷ�� ������ IP �ּҿ� ���� ���̾��� ����� IP �ּҰ� ���ٸ�
    if(memcmp(pFrame->ip_dstaddr, m_sHeader.ip_srcaddr, sizeof(m_sHeader.ip_srcaddr)) == 0){
        
        // ���� ���̾�� ��Ŷ�� �����͸� ���� && ����� bSuccess�� ����
        bSuccess = mp_aUpperLayer[0]->Receive(pFrame->ip_data);
    }

    // ��Ŷ ���� ���� ���� ��ȯ
    return bSuccess;
}