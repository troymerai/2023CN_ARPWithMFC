#pragma once
#include "BaseLayer.h"
#include "pch.h"

class CARPLayer; // CARPLayer Ŭ������ ���� ����


class CIPLayer : public CBaseLayer{

private:
    // IP �ּ� �ʱ�ȭ �Լ�
    void ResetHeader();

public:

    // IP ���� ��Ŷ ���� �Լ�
    BOOL			Receive(unsigned char* ppayload);
    // IP ���� ��Ŷ ���� �Լ�
    BOOL			Send(unsigned char* ppayload, int nlength);
    // ������ IP �ּ� ���� �Լ�
    void			SetDestinAddress(unsigned char* pAddress);
    // ����� IP �ּ� ���� �Լ�
    void			SetSourceAddress(unsigned char* pAddress);
    // ������ IP �ּ� ��ȯ �Լ�
    unsigned char*  GetDestinAddress();
    // ����� IP �ּ� ��ȯ �Լ�
    unsigned char*  GetSourceAddress();

    CARPLayer* m_ARPLayer;

    CIPLayer(char* pName);
	virtual ~CIPLayer();

    // ARP Layer ����


    // IP ��� ����ü
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
