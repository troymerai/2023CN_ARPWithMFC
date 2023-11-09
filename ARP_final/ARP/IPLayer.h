#pragma once
#include "BaseLayer.h"
#include "pch.h"

class CIPLayer : public CBaseLayer{

private:
    // IP 주소 초기화 함수
    void ResetHeader();

public:

    // IP 계층 패킷 수신 함수
    BOOL			Receive(unsigned char* ppayload);
    // IP 계층 패킷 전송 함수
    BOOL			Send(unsigned char* ppayload, int nlength);
    // 목적지 IP 주소 설정 함수
    void			SetDestinAddress(unsigned char* pAddress);
    // 출발지 IP 주소 설정 함수
    void			SetSourceAddress(unsigned char* pAddress);
    // 목적지 IP 주소 반환 함수
    unsigned char*  GetDestinAddress();
    // 출발지 IP 주소 반환 함수
    unsigned char*  GetSourceAddress();

    CIPLayer(char* pName);
	virtual ~CIPLayer();

    // IP 헤더 구조체
    typedef struct _IP_HEADER {
        unsigned char   ver_hlegnth; // 4-bit IPv4 version, 4-bit header length
        unsigned char   tos;         // IP type of service
        unsigned short  tlength;     // Total length

        unsigned short  id;          // Unique identifier
        unsigned char   offset;      // Fragment offset field
        
        unsigned char   ttl;         // Time to live
        unsigned char   ptype;       // Protocol type
        unsigned short  checksum;    // IP checksum
        
        unsigned char   ip_srcaddr[IP_ADDR_SIZE];
        unsigned char   ip_dstaddr[IP_ADDR_SIZE];
        unsigned char   ip_data[IP_MAX_DATA_SIZE];

    } IP_HEADER, * PIP_HEADER;

protected:
    IP_HEADER m_sHeader;
};
