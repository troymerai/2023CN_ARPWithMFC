
#include "pch.h"
#include "stdafx.h"
#include "IPLayer.h"


// 생성자
CIPLayer::CIPLayer(char* pName) : CBaseLayer(pName) {
    // 내부 네트워크 IP헤더 초기화
    ResetHeader(INNER);
    // 외부 네트워크 IP헤더 초기화
    ResetHeader(OUTER);
 }

// 소멸자
CIPLayer::~CIPLayer() {

}

// IP 헤더 초기화 함수
// iosel은 내부/외부 선택인자
void CIPLayer::ResetHeader(int iosel) {
    // 출발지 IP 주소 0으로 초기화
    memset(m_sHeader[iosel].ip_srcaddr, 0, IP_ADDR_SIZE);
    // 목적지 IP 주소 0으로 초기화
    memset(m_sHeader[iosel].ip_dstaddr, 0, IP_ADDR_SIZE);
    // 데이터 필드 0으로 초기화 
    memset(m_sHeader[iosel].ip_data, 0, IP_MAX_DATA_SIZE);
}

// IP 헤더 설정 함수
void CIPLayer::SetHeaderFields(unsigned char* ppayload, int iosel){

    // 인자로 받은 payload값을 IP_HEADER 타입으로 캐스팅
    PIP_HEADER pFrame = (PIP_HEADER)ppayload;

    // 버전과 헤더 길이를 가져와 설정
    m_sHeader[iosel].ver_hlegnth = pFrame->ver_hlegnth;
    // 서비스 타입을 가져와 설정
    m_sHeader[iosel].tos = pFrame->tos;
    // 전체 길이를 가져와 설정
    m_sHeader[iosel].tlength = pFrame->tlength;
    // ID를 가져와 설정
    m_sHeader[iosel].id = pFrame->id;
    // TTL(Time To Live)를 가져와 설정
    m_sHeader[iosel].ttl = pFrame->ttl;
    // TTL을 1 감소. 패킷이 네트워크에서 영원히 떠돌지 않도록 하기 위한 장치
    m_sHeader[iosel].ttl = m_sHeader[iosel].ttl - 1;    
    // 프로토콜 타입을 가져와 설정
    m_sHeader[iosel].ptype = pFrame->ptype;
    // 체크섬을 가져와 설정합니다.
    m_sHeader[iosel].checksum = pFrame->checksum;
    // 소스 주소를 가져와 설정
    SetSourceAddress(pFrame->ip_srcaddr, iosel);
    // 목적지 주소를 가져와 설정
    SetDestinAddress(pFrame->ip_dstaddr, iosel);
}

// 출발지 IP 주소 반환 함수
unsigned char* CIPLayer::GetSourceAddress(int iosel) {
    return m_sHeader[iosel].ip_srcaddr;
}

// 목적지 IP 주소 반환 함수
unsigned char* CIPLayer::GetDestinAddress(int iosel) {
    return m_sHeader[iosel].ip_dstaddr;
}

// 출발지 IP 주소 설정 함수
void CIPLayer::SetSourceAddress(unsigned char* pAddress, int iosel) {
    memcpy(m_sHeader[iosel].ip_srcaddr, pAddress, IP_ADDR_SIZE);
}

// 목적지 IP 주소 설정 함수
void CIPLayer::SetDestinAddress(unsigned char* pAddress, int iosel) {
    memcpy(m_sHeader[iosel].ip_dstaddr, pAddress, IP_ADDR_SIZE);
}

//UpperLayer = Application Layer, UnderLayer = ARP Layer
// 
// IP 계층 패킷 전송 함수
BOOL CIPLayer::Send(unsigned char* ppayload, int nlength, int iosel) {
    BOOL bSuccess = FALSE;

    // 전송할 데이터를 헤더의 데이터 필드에 복사
    memcpy(m_sHeader[iosel].ip_data, ppayload, nlength);

    // 전송할 데이터는 m_sHeader[iosel] 
    // 데이터의 길이는 IP_HEADER_SIZE + nlength
    bSuccess = this->GetUnderLayer()->Send((unsigned char*)&m_sHeader[iosel], IP_HEADER_SIZE + nlength, iosel);
    
    // 함수의 결과로 bSuccess를 반환
    return bSuccess;
}

// 특정 게이트웨이로 데이터를 보내는 함수
BOOL CIPLayer::RSend(unsigned char* ppayload, int nlength, unsigned char* gatewayIP, int iosel) {
    BOOL bSuccess = FALSE;

    // RSend 함수를 호출하여 하위 계층(여기서는 ARP 계층)으로 패킷을 전송
    bSuccess = this->GetUnderLayer()->RSend(ppayload, nlength, gatewayIP, iosel);

    // 함수의 결과로 bSuccess를 반환
    return bSuccess;
}

// 20231201 변경됨
// IP 계층 패킷 수신 함수
BOOL CIPLayer::Receive(unsigned char* ppayload, int iosel) {

    // 패킷 수신 여부 저장 변수 선언
    BOOL bSuccess = FALSE;
    // 수신된 데이터를 IP_HEADER 타입으로 캐스팅
    PIP_HEADER pFrame = (PIP_HEADER)ppayload;
    
    // 수신된 데이터를 헤더의 데이터 필드에 복사
    memcpy(m_sHeader[iosel].ip_data, pFrame->ip_data, IP_MAX_DATA_SIZE);
    // 수신된 데이터로부터 헤더 필드를 설정
    SetHeaderFields(ppayload, iosel);
    
    // 목적지 주소를 기반으로 라우팅을 수행
    Routing(pFrame->ip_dstaddr, ppayload, iosel);

    // 함수의 결과로 bSuccess를 반환
    return bSuccess;
}

// 라우팅 테이블에 루트를 추가하는 함수
void CIPLayer::AddRouteTable(unsigned char* _destination_ip, unsigned char* _netmask, unsigned char* _gateway, unsigned char _flag, unsigned char interFace) {
    
    // 라우팅 테이블 노드를 선언하고 초기화
    ROUTING_TABLE_NODE rt;

    // 목적지 IP 복사
    memcpy(rt.destination_ip, _destination_ip, IP_ADDR_SIZE);
    // 넷마스크 복사
    memcpy(rt.netmask, _netmask, IP_ADDR_SIZE);
    // 게이트웨이 복사
    memcpy(rt.gateway, _gateway, IP_ADDR_SIZE);
    // 플래그 설정
    rt.flag = _flag;
    // 인터페이스 설정
    rt._interface = interFace;

    ///// route_table list에서 새로운 열을 추가할 위치 찾기
    // 라우팅 테이블의 시작점을 가리키는 반복자를 선언
    std::list<ROUTING_TABLE_NODE>::iterator add_point = route_table.begin();

    //// 새로운 루트를 추가할 위치 찾기
    // 라우팅 테이블의 끝에 도달하거나 새로운 루트의 게이트웨이가 
    // 현재 루트의 게이트웨이보다 긴 헤더를 가질 때까지 반복
    while (add_point != route_table.end() && LongestPrefix(add_point->gateway, rt.gateway)) {
        add_point++;
    }

    // 찾은 위치에 새로운 라우트를 추가
    route_table.insert(add_point, rt);
}

// 라우팅 테이블에서 특정 루트를 제거하는 함수
void CIPLayer::DelRouteTable(unsigned char index) {

    // 제거할 라우트를 찾기 위해 라우팅 테이블의 시작점을 가리키는 반복자를 선언
    std::list<ROUTING_TABLE_NODE>::iterator del_point = route_table.begin();

    // 제거할 라우트의 위치를 찾는다. **인덱스가 1부터 시작하므로 인덱스 - 1번 만큼 반복
    for (int i = 0; i < int(index) - 1; i++) {
        del_point++;
    }

    // 찾은 위치의 라우트를 제거
    route_table.erase(del_point);
}

// Default Gateway를 설정하는 함수
void CIPLayer::SetDefaultGateway(unsigned char* _gateway, unsigned char _flag, unsigned char interFace) {

    // 비교를 위한 '0.0.0.0' IP 주소를 저장하는 배열을 선언
    unsigned char zero[IP_ADDR_SIZE];
    // Destination_IP: '0.0.0.0' 확인용 값
    // zero 배열을 0으로 초기화
    memset(zero, 0, IP_ADDR_SIZE);                 
    
    // default gateway가 있는 지 확인
    // 라우팅 테이블의 마지막 노드의 목적지 IP가 '0.0.0.0'인지 확인 
    if (!memcmp(route_table.back().destination_ip, zero, IP_ADDR_SIZE)) {   
        // 기본 게이트웨이가 이미 있는 경우, 새로운 기본 게이트웨이로 설정
        memcpy(&route_table.back().gateway, _gateway, IP_ADDR_SIZE);
        // 플래그 설정
        route_table.back().flag = _flag;
        // 인터페이스 설정
        route_table.back()._interface = interFace;
    }
    // default gateway가 없는 경우
    else {
        // 새로운 라우팅 테이블 노드를 생성
        ROUTING_TABLE_NODE* tmp = new ROUTING_TABLE_NODE;
        // 새로운 노드의 목적지 IP와 넷마스크를 '0.0.0.0'로 설정
        memset(&tmp->destination_ip, 0, IP_ADDR_SIZE);
        memset(&tmp->netmask, 0, IP_ADDR_SIZE);
        // 새로운 노드의 게이트웨이를 새로운 기본 게이트웨이로 설정
        memcpy(&tmp->gateway, _gateway, IP_ADDR_SIZE);
        // 플래그 설정
        tmp->flag = _flag;
        // 인터페이스 설정
        tmp->_interface = interFace;
    
        // 라우팅 테이블에 새로운 노드를 추가
        route_table.push_back(*tmp);
    }
 }

// 두 주소의 최장 공통 헤더 길이를 비교하는 함수
// 라우팅 테이블에서 라우트를 추가하거나 찾을 때 사용
bool CIPLayer::LongestPrefix(unsigned char* a, unsigned char* b) {

    // 두 주소의 각 바이트를 비교
    for (int i = 0; i < IP_ADDR_SIZE; i++) {

        // a의 i번째 바이트가 b의 i번째 바이트보다 작은 경우, false를 반환
        // a가 b보다 작은 주소를 가지고 있음을 의미
        if (a[i] < b[i])return false;
    }

    return true;
}

// 라우팅 처리 함수
void CIPLayer::Routing(unsigned char* dest_ip, unsigned char* ppayload, int iosel) {

    // 패킷 데이터를 IP_HEADER 타입으로 캐스팅
    PIP_HEADER pFrame = (PIP_HEADER)ppayload;

    // 넷마스크와의 AND 연산 결과를 저장할 배열을 선언하고 초기화
    unsigned char masked[IP_ADDR_SIZE] = { 0, };

    // 라우팅 테이블의 각 라우트에 대해 반복
    for each (ROUTING_TABLE_NODE row in route_table) {       //table에서 한 열씩 뽑아...

        // 목적지 IP와 넷마스크와의 AND 연산을 수행하여 마스킹 결과를 계산
        for (int i = 0; i < IP_ADDR_SIZE; i++)masked[i] = dest_ip[i] & row.netmask[i];  
             
        // 마스킹 결과와 라우트의 목적지 IP를 비교하여 일치하는 경우 
        // 해당 라우트로 패킷을 전송
        if (memcmp(masked, row.destination_ip, IP_ADDR_SIZE)==0) {      
            // 라우트가 활성화되어 있고, 대상이 호스트인 경우 
            // 패킷을 직접 전송
            if ((row.flag & IP_ROUTE_UP) && (row.flag & IP_ROUTE_HOST)) {    
                mp_UnderLayer->RSend(ppayload, pFrame->tlength, dest_ip, row._interface);
            }

            // 라우트가 활성화되어 있고, 대상이 게이트웨이인 경우 
            // 패킷을 게이트웨이에 전송
            else if ((row.flag & IP_ROUTE_UP)&& (row.flag & IP_ROUTE_GATEWAY)) {
                mp_UnderLayer->RSend(ppayload, pFrame->tlength, row.gateway, row._interface); //Gateway IP의 MAC address를 얻기 위해...
            }

            // 패킷 전송 후 함수 종료
            return;
        }
    }
}

// ROUTING_TABLE_NODE 구조체의 기본 생성자
CIPLayer::_ROUTING_TABLE_NODE::_ROUTING_TABLE_NODE()
{
    // 모든 필드를 초기화
    memset(destination_ip, 0, IP_ADDR_SIZE);
    memset(gateway, 0, IP_ADDR_SIZE);
    memset(netmask, 0, IP_ADDR_SIZE);
    flag = 0;
    _interface = 1;
}

// ROUTING_TABLE_NODE 구조체의 복사 생성자
CIPLayer::_ROUTING_TABLE_NODE::_ROUTING_TABLE_NODE(const _ROUTING_TABLE_NODE& other)
{

    // 다른 ROUTING_TABLE_NODE 객체의 모든 필드를 복사
    memcpy(destination_ip, other.destination_ip, IP_ADDR_SIZE);
    memcpy(gateway, other.gateway, IP_ADDR_SIZE);
    memcpy(netmask, other.netmask, IP_ADDR_SIZE);
    flag = other.flag;
    _interface = other._interface;
}

