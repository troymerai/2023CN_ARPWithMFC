#pragma once
#include "BaseLayer.h"
#include "pch.h"
#include <pcap.h>
#include <tchar.h>
#include <Packet32.h>
#include <WinSock2.h>
#include <thread>
class CNILayer :
    public CBaseLayer
{

public:
    CNILayer(char* pName);
    virtual ~CNILayer();
    virtual BOOL    Receive(unsigned char* pkt) override;
    virtual BOOL    Send(unsigned char* ppayload, int nlength) override;
    //from index number, return MAC ADDRESS
    UCHAR*          SetAdapter(const int index); 
    //Set Adapter List into combobox
    void            SetAdapterComboBox(CComboBox& adpaterlist);
    //Get MacAddressList CStringArray
    void            GetMacAddressList(CStringArray& adapterlist);

    // 선택한 네트워크 어댑터의 IP 주소를 가져오는 함수
    // 매개변수로 ipv4 ipv6 값을 저장할 문자열을 받음
    // 네트워크 어댑터를 먼저 설정해야 함
    void            GetIPAddress(CString& ipv4addr, CString& ipv6addr);

    //if canread is True Thread activate
    static UINT     ThreadFunction_RECEIVE(LPVOID pParam);
    // NI Layer 패킷 수신 상태 변경 함수
    void Receiveflip();
protected:
    pcap_if_t* allDevices; //all information of adapters
    pcap_if_t* device; //selected adapter
    pcap_t* m_AdapterObject; 
    LPADAPTER adapter;
    PPACKET_OID_DATA OidData;
    UCHAR data[MAC_MAX_SIZE];
    bool canRead;
};
