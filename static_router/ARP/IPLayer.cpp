
#include "pch.h"
#include "stdafx.h"
#include "IPLayer.h"


// ������
CIPLayer::CIPLayer(char* pName) : CBaseLayer(pName) {
    // ���� ��Ʈ��ũ IP��� �ʱ�ȭ
    ResetHeader(INNER);
    // �ܺ� ��Ʈ��ũ IP��� �ʱ�ȭ
    ResetHeader(OUTER);
 }

// �Ҹ���
CIPLayer::~CIPLayer() {

}

// IP ��� �ʱ�ȭ �Լ�
// iosel�� ����/�ܺ� ��������
void CIPLayer::ResetHeader(int iosel) {
    // ����� IP �ּ� 0���� �ʱ�ȭ
    memset(m_sHeader[iosel].ip_srcaddr, 0, IP_ADDR_SIZE);
    // ������ IP �ּ� 0���� �ʱ�ȭ
    memset(m_sHeader[iosel].ip_dstaddr, 0, IP_ADDR_SIZE);
    // ������ �ʵ� 0���� �ʱ�ȭ 
    memset(m_sHeader[iosel].ip_data, 0, IP_MAX_DATA_SIZE);
}

// IP ��� ���� �Լ�
void CIPLayer::SetHeaderFields(unsigned char* ppayload, int iosel){

    // ���ڷ� ���� payload���� IP_HEADER Ÿ������ ĳ����
    PIP_HEADER pFrame = (PIP_HEADER)ppayload;

    // ������ ��� ���̸� ������ ����
    m_sHeader[iosel].ver_hlegnth = pFrame->ver_hlegnth;
    // ���� Ÿ���� ������ ����
    m_sHeader[iosel].tos = pFrame->tos;
    // ��ü ���̸� ������ ����
    m_sHeader[iosel].tlength = pFrame->tlength;
    // ID�� ������ ����
    m_sHeader[iosel].id = pFrame->id;
    // TTL(Time To Live)�� ������ ����
    m_sHeader[iosel].ttl = pFrame->ttl;
    // TTL�� 1 ���� ** ��Ŷ�� ��Ʈ��ũ���� ������ ������ �ʵ��� �ϱ� ���� ��ġ
    m_sHeader[iosel].ttl = m_sHeader[iosel].ttl - 1;    
    // �������� Ÿ���� ������ ����
    m_sHeader[iosel].ptype = pFrame->ptype;
    // üũ���� ������ �����մϴ�.
    m_sHeader[iosel].checksum = pFrame->checksum;
    // �ҽ� �ּҸ� ������ ����
    SetSourceAddress(pFrame->ip_srcaddr, iosel);
    // ������ �ּҸ� ������ ����
    SetDestinAddress(pFrame->ip_dstaddr, iosel);
}

// ����� IP �ּ� ��ȯ �Լ�
unsigned char* CIPLayer::GetSourceAddress(int iosel) {
    return m_sHeader[iosel].ip_srcaddr;
}

// ������ IP �ּ� ��ȯ �Լ�
unsigned char* CIPLayer::GetDestinAddress(int iosel) {
    return m_sHeader[iosel].ip_dstaddr;
}

// ����� IP �ּ� ���� �Լ�
void CIPLayer::SetSourceAddress(unsigned char* pAddress, int iosel) {
    memcpy(m_sHeader[iosel].ip_srcaddr, pAddress, IP_ADDR_SIZE);
}

// ������ IP �ּ� ���� �Լ�
void CIPLayer::SetDestinAddress(unsigned char* pAddress, int iosel) {
    memcpy(m_sHeader[iosel].ip_dstaddr, pAddress, IP_ADDR_SIZE);
}

//UpperLayer = Application Layer, UnderLayer = ARP Layer
// 
// IP ���� ��Ŷ ���� �Լ�
BOOL CIPLayer::Send(unsigned char* ppayload, int nlength, int iosel) {
    BOOL bSuccess = FALSE;

    // ������ �����͸� ����� ������ �ʵ忡 ����
    memcpy(m_sHeader[iosel].ip_data, ppayload, nlength);

    // ������ �����ʹ� m_sHeader[iosel] 
    // �������� ���̴� IP_HEADER_SIZE + nlength 
    // ���� �������� ��Ŷ ���� (���⼭�� ARP layer)
    bSuccess = this->GetUnderLayer()->Send((unsigned char*)&m_sHeader[iosel], IP_HEADER_SIZE + nlength, iosel);
    
    // �Լ��� ����� bSuccess�� ��ȯ
    return bSuccess;
}

// Ư�� ����Ʈ���̷� �����͸� ������ �Լ�
BOOL CIPLayer::RSend(unsigned char* ppayload, int nlength, unsigned char* gatewayIP, int iosel) {
    BOOL bSuccess = FALSE;

    // RSend �Լ��� ȣ���Ͽ� ���� ����(���⼭�� ARP ����)���� ��Ŷ�� ����
    bSuccess = this->GetUnderLayer()->RSend(ppayload, nlength, gatewayIP, iosel);

    // �Լ��� ����� bSuccess�� ��ȯ
    return bSuccess;
}

// 20231201 �����
// IP ���� ��Ŷ ���� �Լ�
BOOL CIPLayer::Receive(unsigned char* ppayload, int iosel) {

    // ��Ŷ ���� ���� ���� ���� ����
    BOOL bSuccess = FALSE;
    // ���ŵ� �����͸� IP_HEADER Ÿ������ ĳ����
    PIP_HEADER pFrame = (PIP_HEADER)ppayload;
    
    // ���ŵ� �����͸� ����� ������ �ʵ忡 ����
    memcpy(m_sHeader[iosel].ip_data, pFrame->ip_data, IP_MAX_DATA_SIZE);
    // ���ŵ� �����ͷκ��� ��� �ʵ带 ����
    SetHeaderFields(ppayload, iosel);
    
    // ������ �ּҸ� ������� ������� ����
    Routing(pFrame->ip_dstaddr, ppayload, iosel);

    // �Լ��� ����� bSuccess�� ��ȯ
    return bSuccess;
}

// ����� ���̺� ��Ʈ�� �߰��ϴ� �Լ�
void CIPLayer::AddRouteTable(unsigned char* _destination_ip, unsigned char* _netmask, unsigned char* _gateway, unsigned char _flag, unsigned char interFace) {
    
    // ����� ���̺� ��带 �����ϰ� �ʱ�ȭ
    ROUTING_TABLE_NODE rt;

    // ������ IP ����
    memcpy(rt.destination_ip, _destination_ip, IP_ADDR_SIZE);
    // �ݸ���ũ ����
    memcpy(rt.netmask, _netmask, IP_ADDR_SIZE);
    // ����Ʈ���� ����
    memcpy(rt.gateway, _gateway, IP_ADDR_SIZE);
    // �÷��� ����
    rt.flag = _flag;
    // �������̽� ����
    rt._interface = interFace;

    ///// route_table list���� ���ο� ���� �߰��� ��ġ ã��
    // ����� ���̺��� �������� ����Ű�� �ݺ��ڸ� ����
    std::list<ROUTING_TABLE_NODE>::iterator add_point = route_table.begin();

    //// ���ο� ��Ʈ�� �߰��� ��ġ ã�� (Longest Match �˰��� ���)
    // ����� ���̺��� ���� �����ϰų� 
    // ���ο� ��Ʈ�� ����Ʈ���̰� ���� ��Ʈ�� ����Ʈ���̺��� �� prefix(subnet mask)�� ���� ������ �տ������� Ȯ��
    while (add_point != route_table.end() && LongestPrefix(add_point->gateway, rt.gateway)) {
        add_point++;
    }

    // ã�� ��ġ�� ���ο� ���Ʈ�� �߰�
    route_table.insert(add_point, rt);
}

// ����� ���̺��� Ư�� ��Ʈ�� �����ϴ� �Լ�
void CIPLayer::DelRouteTable(unsigned char index) {

    // ������ ���Ʈ�� ã�� ���� ����� ���̺��� �������� ����Ű�� �ݺ��ڸ� ����
    std::list<ROUTING_TABLE_NODE>::iterator del_point = route_table.begin();

    // ������ ���Ʈ�� ��ġ�� ã�´�. **�ε����� 1���� �����ϹǷ� �ε��� - 1�� ��ŭ �ݺ�
    for (int i = 0; i < int(index) - 1; i++) {
        del_point++;
    }

    // ã�� ��ġ�� ���Ʈ�� ����
    route_table.erase(del_point);
}

// Default Gateway�� �����ϴ� �Լ�
void CIPLayer::SetDefaultGateway(unsigned char* _gateway, unsigned char _flag, unsigned char interFace) {

    // �񱳸� ���� '0.0.0.0' IP �ּҸ� �����ϴ� �迭�� ����
    unsigned char zero[IP_ADDR_SIZE];
    // Destination_IP: '0.0.0.0' Ȯ�ο� ��
    // zero �迭�� 0���� �ʱ�ȭ
    memset(zero, 0, IP_ADDR_SIZE);                 
    
    // default gateway�� �ִ� �� Ȯ��
    // ����� ���̺��� ������ ����� ������ IP�� '0.0.0.0'���� Ȯ�� 
    if (!memcmp(route_table.back().destination_ip, zero, IP_ADDR_SIZE)) {   
        // default gateway�� �̹� �ִ� ���, ���ο� default gateway�� ����
        memcpy(&route_table.back().gateway, _gateway, IP_ADDR_SIZE);
        // �÷��� ����
        route_table.back().flag = _flag;
        // �������̽� ����
        route_table.back()._interface = interFace;
    }
    // default gateway�� ���� ���
    else {
        // ���ο� ����� ���̺� ��带 ����
        ROUTING_TABLE_NODE* tmp = new ROUTING_TABLE_NODE;
        // ���ο� ����� ������ IP�� �ݸ���ũ�� '0.0.0.0'�� ����
        memset(&tmp->destination_ip, 0, IP_ADDR_SIZE);
        memset(&tmp->netmask, 0, IP_ADDR_SIZE);
        // ���ο� ����� ����Ʈ���̸� ���ο� default gateway�� ����
        memcpy(&tmp->gateway, _gateway, IP_ADDR_SIZE);
        // �÷��� ����
        tmp->flag = _flag;
        // �������̽� ����
        tmp->_interface = interFace;
    
        // ����� ���̺� ���ο� ��带 �߰�
        route_table.push_back(*tmp);
    }
 }

// Longest Match ���
// ������ �ּҿ� ���� ���� ��ġ�ϴ� ��θ� �����ϱ� ����
// �� �ּ��� ���� prefix(subnet mask�� ���� ���� ��Ʈ) ���̸� ���ϴ� �Լ�
// ����� ���̺��� ���Ʈ�� �߰��ϰų� ã�� �� ���
bool CIPLayer::LongestPrefix(unsigned char* a, unsigned char* b) {

    // �� �ּ��� �� ����Ʈ�� ��
    for (int i = 0; i < IP_ADDR_SIZE; i++) {

        // a�� i��° ����Ʈ�� b�� i��° ����Ʈ���� ���� ���, false�� ��ȯ
        // a�� b���� �� ��ġ�ϴ� �ּҸ� ������ ������ �ǹ�
        if (a[i] < b[i])return false;
    }

    return true;
}

// ����� ó�� �Լ�
void CIPLayer::Routing(unsigned char* dest_ip, unsigned char* ppayload, int iosel) {

    // ��Ŷ �����͸� IP_HEADER Ÿ������ ĳ����
    PIP_HEADER pFrame = (PIP_HEADER)ppayload;

    // �ݸ���ũ���� AND ���� ����� ������ �迭�� �����ϰ� �ʱ�ȭ
    unsigned char masked[IP_ADDR_SIZE] = { 0, };

    // ����� ���̺��� �� ���Ʈ�� ���� �ݺ�
    for each (ROUTING_TABLE_NODE row in route_table) {       //table���� �� ���� �̾�...

        // 20231212
        // ���߿� �ڵ� �����丵
        // �ݸ���ũ ���� ����� ���� ���� ���ص� �� 
        // �ٷ� ���ؼ� �б��ϴ� ������� �����ϱ�
      
        // ������ IP�� �ݸ���ũ���� AND ������ �����Ͽ� ����ŷ ����� ���
        for (int i = 0; i < IP_ADDR_SIZE; i++)masked[i] = dest_ip[i] & row.netmask[i];  
             
        // ����ŷ ����� ���Ʈ�� ������ IP�� ���Ͽ� ��ġ�ϴ� ��� 
        // �ش� ���Ʈ�� ��Ŷ�� ����
        if (memcmp(masked, row.destination_ip, IP_ADDR_SIZE)==0) {    

            // ���Ʈ�� Ȱ��ȭ�Ǿ� �ְ�, ����� ȣ��Ʈ�� ��� 
            // ��Ŷ�� ���� ����
            if ((row.flag & IP_ROUTE_UP) && (row.flag & IP_ROUTE_HOST)) {    
                mp_UnderLayer->RSend(ppayload, pFrame->tlength, dest_ip, row._interface);
            }

            // ���Ʈ�� Ȱ��ȭ�Ǿ� �ְ�, ����� gateway�� ��� 
            // ��Ŷ�� gateway�� ����
            else if ((row.flag & IP_ROUTE_UP)&& (row.flag & IP_ROUTE_GATEWAY)) {
                mp_UnderLayer->RSend(ppayload, pFrame->tlength, row.gateway, row._interface); //Gateway IP�� MAC address�� ��� ����...
            }

            // ��Ŷ ���� �� �Լ� ����
            return;
        }
        
        // test�� �ڵ� ���� �� ������ �� (���� ����� ����� ���ص� �˱� ���ؼ�..)
        AfxMessageBox(_T("ȣ��Ʈ �Ǵ� default gateway�� IP�ּҰ� �߰ߵ��� �ʾ���!"));
    }
}

// ROUTING_TABLE_NODE ����ü�� �⺻ ������
CIPLayer::_ROUTING_TABLE_NODE::_ROUTING_TABLE_NODE()
{
    // ��� �ʵ带 �ʱ�ȭ
    memset(destination_ip, 0, IP_ADDR_SIZE);
    memset(gateway, 0, IP_ADDR_SIZE);
    memset(netmask, 0, IP_ADDR_SIZE);
    flag = 0;
    _interface = 1;
}

// ROUTING_TABLE_NODE ����ü�� ���� ������
CIPLayer::_ROUTING_TABLE_NODE::_ROUTING_TABLE_NODE(const _ROUTING_TABLE_NODE& other)
{

    // �ٸ� ROUTING_TABLE_NODE ��ü�� ��� �ʵ带 ����
    memcpy(destination_ip, other.destination_ip, IP_ADDR_SIZE);
    memcpy(gateway, other.gateway, IP_ADDR_SIZE);
    memcpy(netmask, other.netmask, IP_ADDR_SIZE);
    flag = other.flag;
    _interface = other._interface;
}

