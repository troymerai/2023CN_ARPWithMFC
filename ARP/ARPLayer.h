#pragma once
#include "BaseLayer.h"
#include "pch.h"
#include <vector>


void  addrToStr(unsigned short type, CString& dst, unsigned char* src);
void  StrToaddr(unsigned short type, unsigned char* dst, CString& src);

class CARPLayer :
    public CBaseLayer
{
private:
    // ARP 헤더 초기화 함수
    inline void     ResetHeader();
public:
    // ARP 계층 패킷 수신 함수
    BOOL            Receive(unsigned char* ppayload);
    // ARP 계층 패킷 전송 함수
    BOOL            Send(unsigned char* ppayload, int nlength);
    // IP 주소가 ARP cache table에 있는 지 확인하는 함수
    int             inCache(const unsigned char* ipaddr);
    // ARP 헤더의 하드웨어 타입과 프로토콜 타입을 설정하는 함수
    void            setType(const unsigned short htype, const unsigned short ptype);
    // ARP 헤더의 opcode를 설정하는 함수
    void            setOpcode(const unsigned short opcode);
    // ARP 헤더의 출발지 주소를 설정하는 함수
    void            setSrcAddr(const unsigned char enetAddr[], const unsigned char ipAddr[]);
    // ARP 헤더의 목적지 주소를 설정하는 함수
    void            setDstAddr(const unsigned char enetAddr[], const unsigned char ipAddr[]);
    // 인자로 받은 주소 swap 함수
    void            swapaddr(unsigned char lAddr[], unsigned char rAddr[], const unsigned char size);
    // ARP cache table 업데이트 함수
    void            updateTable();
    // ARP Layer의 MAC, IP 주소 설정 함수
    void            setmyAddr(CString MAC, CString IP);
    // 인자로 IP 주소를 가진 항목을 ARP cache table에서 삭제하는 함수
    void            deleteItem(CString IP);
    // ARP cache table 비우는 함수
    void            clearTable();
    
    // 생성자, 소멸자
    CARPLayer(char* pName);
    virtual ~CARPLayer();


    // ARP 노드 구조체 정의
    typedef struct _ARP_NODE {
        unsigned char   prot_addr[IP_ADDR_SIZE];
        unsigned char   hard_addr[MAC_ADDR_SIZE];
        unsigned char   status;
        CTime           spanTime;
        struct _ARP_NODE(unsigned int ipaddr, unsigned int enetaddr, unsigned char incomplete);
        struct _ARP_NODE(unsigned char* cipaddr, unsigned char* cenetaddr, unsigned char bincomplete);
        struct _ARP_NODE(const struct _ARP_NODE& ot);

        bool operator==(const unsigned char* ipaddr);
        bool operator==(const struct _ARP_NODE& ot);
        bool operator<(const unsigned char* ipaddr);
        bool operator<(const struct _ARP_NODE& ot);
        bool operator>(const unsigned char* ipaddr);
        bool operator>(const struct _ARP_NODE& ot);
    }ARP_NODE;

    // ARP 헤더 구조체 정의
    typedef struct _ARP_HEADER {
        unsigned short  hard_type;
        unsigned short  prot_type;
        unsigned char   hard_length;
        unsigned char   prot_length;
        unsigned short  op;
        unsigned char   hard_srcaddr[MAC_ADDR_SIZE];
        unsigned char   prot_srcaddr[IP_ADDR_SIZE];
        unsigned char   hard_dstaddr[MAC_ADDR_SIZE];
        unsigned char   prot_dstaddr[IP_ADDR_SIZE];
        struct _ARP_HEADER();
        struct _ARP_HEADER(const struct _ARP_HEADER& ot);
    } ARP_HEADER, * PARP_HEADER;

    //DEBUG
    typedef struct _IP_HEADER {
        unsigned char   ver_hlegnth;
        unsigned char   tos;
        unsigned short  tlength;

        unsigned short  id;
        unsigned char   offset;

        unsigned char   ttl;
        unsigned char   ptype;
        unsigned short  checksum;

        unsigned char   srcaddr[IP_ADDR_SIZE];
        unsigned char   dstaddr[IP_ADDR_SIZE];
        unsigned char   data[MAC_MAX_SIZE - MAC_HEADER_SIZE - 20];
    }IP_HEADER, * PIP_HEADER;

    // 현재 ARP cache table 반환 함수
    std::vector<ARP_NODE> getTable();

protected:
    unsigned char myip[IP_ADDR_SIZE];
    unsigned char mymac[MAC_ADDR_SIZE];
    ARP_HEADER m_sHeader;
    std::vector<ARP_NODE> m_arpTable;
};
