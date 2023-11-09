// EthernetLayer.h: interface for the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ETHERNETLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
#define AFX_ETHERNETLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include "pch.h"

class CEthernetLayer
	: public CBaseLayer
{
private:
	inline void		ResetHeader();

public:
	// 이더넷 계층 패킷 수신 함수
    //수신받은 패킷의 Destination MAC ADDRESS가 현재 프로세스의 MAC_ADDRESS가 맞을 경우 상위 레이어로 이동 만약 아니라면 receive 종료
    //만약 FrameType이 0x2080이면 Chat App Layer, 0x2090이면 File App Layer로 Receive()
	BOOL			Receive(unsigned char* ppayload);
	
	// 이더넷 계층 패킷 전송 함수
	BOOL			Send(unsigned char* ppayload, int nlength);
	void			SetDestinAddress(unsigned char* pAddress);
	void			SetSourceAddress(unsigned char* pAddress);
	unsigned char* GetDestinAddress();
	unsigned char* GetSourceAddress();

	CEthernetLayer(char* pName);
	virtual ~CEthernetLayer();

	// 이더넷 헤더 구조체 
	typedef struct _ETHERNET_HEADER {

		unsigned char	mac_dstaddr[6];		// destination address of ethernet layer
		unsigned char	mac_srcaddr[6];		// source address of ethernet layer
		unsigned short	mac_type;		// type of ethernet layer
		unsigned char	mac_data[MAC_MAX_DATA_SIZE]; // frame data

	} ETHERNET_HEADER, * PETHERNET_HEADER;

protected:
	ETHERNET_HEADER	m_sHeader;
};

#endif // !defined(AFX_ETHERNETLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
