
#include "pch.h"
#include "stdafx.h"
#include "IPLayer.h"



// 생성자
CIPLayer::CIPLayer(char* pName) : CBaseLayer(pName){
    
    // 헤더 정보 초기화
    ResetHeader();

    ///////////////////////////////////
    ///////////////////////////////////
    // 굳이 IP 헤더를 다 선언할 필요는 없지만, 객체 지향으로 코드를 짜기 위함
    // 나중에 참조할 일이 있지 않을까.. 
    ///////////////////////////////////
    ///////////////////////////////////

    // IP 버전과 헤더 길이를 설정 
    // 여기서는 IP 버전 4와 헤더 길이 20바이트를 의미하는 0x45를 설정
    m_sHeader.ver_hlegnth = 0x45;

    // 서비스 타입을 설정 
    // 여기서는 기본값인 0x00을 설정
    m_sHeader.tos = 0x00;

    // 전체 패킷 길이를 설정 
    // 여기서는 최대값인 0xffff를 설정
    m_sHeader.tlength = 0xffff;

    // 패킷 식별자를 설정 
    // 여기서는 0x0000으로 설정
    m_sHeader.id = 0x0000;

    // 플래그와 프래그먼트 오프셋을 설정 
    // 여기서는 0x00으로 설정합니다.
    m_sHeader.offset = 0x00;

    // 프레임 생존 시간을 설정 
    // 여기서는 최대값인 0xff를 설정
    m_sHeader.ttl = 0xff;

    // 프로토콜 타입을 설정 
    // 여기서는 TCP를 의미하는 0x06을 설정
    m_sHeader.ptype = 0x06;

    // 체크섬을 설정 
    // 초기값으로 0x0000을 설정
    m_sHeader.checksum = 0x0000;
}

// 소멸자
CIPLayer::~CIPLayer(){
}

// IP 주소 초기화 함수
void CIPLayer::ResetHeader(){

    // 출발지 IP 주소 0으로 초기화
    memset(m_sHeader.ip_srcaddr, 0, IP_ADDR_SIZE);
    // 목적지 IP 주소 0으로 초기화
    memset(m_sHeader.ip_dstaddr, 0, IP_ADDR_SIZE);
    // 데이터 필드 0으로 초기화 
    memset(m_sHeader.ip_data, 0, IP_MAX_DATA_SIZE);
}

// 출발지 IP 주소 반환 함수
unsigned char* CIPLayer::GetSourceAddress(){
    return m_sHeader.ip_srcaddr;
}

// 목적지 IP 주소 반환 함수
unsigned char* CIPLayer::GetDestinAddress() {
    return m_sHeader.ip_dstaddr;
}

// 출발지 IP 주소 설정 함수
void CIPLayer::SetSourceAddress(unsigned char* pAddress){
    memcpy(m_sHeader.ip_srcaddr, pAddress, IP_ADDR_SIZE);
}

// 목적지 IP 주소 설정 함수
void CIPLayer::SetDestinAddress(unsigned char* pAddress){
    memcpy(m_sHeader.ip_dstaddr, pAddress, IP_ADDR_SIZE);
}


//UpperLayer = AppLayer, UnderLayer = ARPLayer?
// 
// IP 계층 패킷 전송 함수
BOOL CIPLayer::Send(unsigned char* ppayload, int nlength){
    
    // argument로 받은 payload를 IP data field에 복사
    memcpy(m_sHeader.ip_data, ppayload, nlength);
    
    // 패킷 전송 성공 여부 저장 변수 선언
    BOOL bSuccess = FALSE;

    // 하위 레이어로 패킷 전송 && 결과는 bSuccess에 저장
    bSuccess = this->GetUnderLayer()->Send((unsigned char*)&m_sHeader, IP_HEADER_SIZE + nlength);
    
    // 패킷 전송 성공 여부 반환
    return bSuccess;
}

// IP 계층 패킷 수신 함수
BOOL CIPLayer::Receive(unsigned char* ppayload){

    // 패킷 수신 여부 저장 변수 선언
    BOOL bSuccess = FALSE;

    // argument로 받은 payload를 IP헤더 구조체로 타입 캐스팅
    PIP_HEADER pFrame = (PIP_HEADER)ppayload;

    // 수신한 패킷의 목적지 IP 주소와 현재 레이어의 출발지 IP 주소가 같다면
    if(memcmp(pFrame->ip_dstaddr, m_sHeader.ip_srcaddr, sizeof(m_sHeader.ip_srcaddr)) == 0){
        
        // 상위 레이어로 패킷의 데이터를 전달 && 결과를 bSuccess에 저장
        bSuccess = mp_aUpperLayer[0]->Receive(pFrame->ip_data);
    }

    // 패킷 수신 성공 여부 반환
    return bSuccess;
}