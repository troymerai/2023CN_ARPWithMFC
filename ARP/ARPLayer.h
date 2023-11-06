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
    // ARP ��� �ʱ�ȭ �Լ�
    inline void     ResetHeader();
public:
    // ARP ���� ��Ŷ ���� �Լ�
    BOOL            Receive(unsigned char* ppayload);
    // ARP ���� ��Ŷ ���� �Լ�
    BOOL            Send(unsigned char* ppayload, int nlength);
    // IP �ּҰ� ARP cache table�� �ִ� �� Ȯ���ϴ� �Լ�
    int             inCache(const unsigned char* ipaddr);
    // ARP ����� �ϵ���� Ÿ�԰� �������� Ÿ���� �����ϴ� �Լ�
    void            setType(const unsigned short htype, const unsigned short ptype);
    // ARP ����� opcode�� �����ϴ� �Լ�
    void            setOpcode(const unsigned short opcode);
    // ARP ����� ����� �ּҸ� �����ϴ� �Լ�
    void            setSrcAddr(const unsigned char enetAddr[], const unsigned char ipAddr[]);
    // ARP ����� ������ �ּҸ� �����ϴ� �Լ�
    void            setDstAddr(const unsigned char enetAddr[], const unsigned char ipAddr[]);
    // ���ڷ� ���� �ּ� swap �Լ�
    void            swapaddr(unsigned char lAddr[], unsigned char rAddr[], const unsigned char size);
    // ARP cache table ������Ʈ �Լ�
    void            updateTable();
    // ARP Layer�� MAC, IP �ּ� ���� �Լ�
    void            setmyAddr(CString MAC, CString IP);
    // ���ڷ� IP �ּҸ� ���� �׸��� ARP cache table���� �����ϴ� �Լ�
    void            deleteItem(CString IP);
    // ARP cache table ���� �Լ�
    void            clearTable();
    
    // ������, �Ҹ���
    CARPLayer(char* pName);
    virtual ~CARPLayer();


    // ARP ��� ����ü ����
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

    // ARP ��� ����ü ����
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

    // ���� ARP cache table ��ȯ �Լ�
    std::vector<ARP_NODE> getTable();

protected:
    unsigned char myip[IP_ADDR_SIZE];
    unsigned char mymac[MAC_ADDR_SIZE];
    ARP_HEADER m_sHeader;
    std::vector<ARP_NODE> m_arpTable;
};
