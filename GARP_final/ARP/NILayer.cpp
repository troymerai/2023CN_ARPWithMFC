// NetworkInterfaceLayer.cpp: implementation of the NetworkInterfaceLayer class.

#include "stdafx.h"
#include "pch.h"
#include "NILayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


// 생성자
CNILayer::CNILayer(char* pName)
	: CBaseLayer(pName), device(NULL) {
	char errbuf[PCAP_ERRBUF_SIZE];
	m_AdapterObject = nullptr;
	canRead = false;
	adapter = nullptr;
	OidData = nullptr;
	memset(data, 0, ETHER_MAX_SIZE);
	try {
		// 모든 네트워크 장치를 검색
		if (pcap_findalldevs(&allDevices, errbuf) == -1)
		{
			allDevices = NULL;

			// 장치를 찾는 데 실패하면 예외 발생
			throw(CString(errbuf));
		}
	}
	// 에러 발생 시
	catch (CString errorInfo) {

		// 에러 메시지를 출력하고 함수를 종료
		AfxMessageBox(errorInfo);
		return;
	}

	try {
		// OidData에 메모리를 할당
		OidData = (PPACKET_OID_DATA)malloc(sizeof(PACKET_OID_DATA));
		if (OidData == nullptr) {
			// 메모리 할당에 실패하면 예외를 발생
			throw(CString("MALLOC FAIL"));
		}
		OidData->Oid = 0x01010101;
		OidData->Length = 6;
		m_AdapterObject = nullptr;
		memset(data, 0, ETHER_MAX_SIZE);
	}
	// 에러 발생 시
	catch (CString errorInfo) {
		// 에러 메시지 출력하고 함수 종료
		AfxMessageBox(errorInfo);
		return;
	}
}

// 소멸자
CNILayer::~CNILayer() {
	pcap_if_t* temp = nullptr;

	while (allDevices) {
		temp = allDevices;
		allDevices = allDevices->next;
		delete(temp);
	}
	delete(OidData);
}

// network interface layer에서 패킷 수신 함수
BOOL CNILayer::Receive(unsigned char* pkt) {

	// 패킷이 null인 경우 FALSE 반환 (Host끼리 통신 시 사용)
	if (pkt == nullptr) {
		return FALSE;
	}

	// 상위 계층에서 패킷을 받지 못한 경우 FALSE 반환 (Etherent Layer에서 패킷 받아올 때 사용)
	if (!(mp_aUpperLayer[0]->Receive(pkt))) {
		return FALSE;
	}

	// 수신 성공 시 TRUE 반환
	return TRUE;
}

// network interface layer에서 패킷 전송 함수
BOOL CNILayer::Send(unsigned char* ppayload, int nlength) {

	// 패킷 전송
	if (pcap_sendpacket(m_AdapterObject, ppayload, nlength))
	{
		// 실패 시 에러 메시지 출력
		AfxMessageBox(_T("Fail: Send Packet!"));
		// FALSE 반환
		return FALSE;
	}

	// 전송 성공 시 TRUE 반환
	return TRUE;
}

// 네트워크 어뎁터 목록을 콤보박스에 설정하는 함수
void CNILayer::SetAdapterComboBox(CComboBox& adapterlist) {
	// 모든 네트워크 어뎁터를 순회하며
	for (pcap_if_t* d = allDevices; d; d = d->next) {
		//각 장치의 설명을 콤보 박스에 추가
		adapterlist.AddString(CString(d->description));
	}
}

// 선택한 네트워크 어뎁터를 설정하는 함수
UCHAR* CNILayer::SetAdapter(const int index) {
	char errbuf[PCAP_ERRBUF_SIZE];
	device = allDevices;

	// 이미 열려있는 어뎁터가 있다면 닫음
	if (m_AdapterObject != nullptr)
		pcap_close(m_AdapterObject);

	// 입력된 인덱스만큼 네트워크 어뎁터 목록을 순회
	for (int i = 0; i < index && device; i++) {
		device = device->next;
	}
	// 해당 장치를 열음
	if (device != nullptr)
		m_AdapterObject = pcap_open_live((const char*)device->name, 65536, 0, 1000, errbuf);
	// 장치를 열지 못했다면 에러 메시지를 출력하고 nullptr를 반환
	if (m_AdapterObject == nullptr)
	{
		AfxMessageBox(_T("Fail: Connect Adapter"));
		return nullptr;
	}

	// 어뎁터의 MAC 주소를 요청
	adapter = PacketOpenAdapter(device->name);
	PacketRequest(adapter, FALSE, OidData);

	// 어뎁터를 닫음
	PacketCloseAdapter(adapter);
	//AfxBeginThread(ThreadFunction_RECEIVE, this);

	// MAC 주소 반환
	return (OidData->Data);
}


void CNILayer::GetMacAddressList(CStringArray& adapterlist) {
	for (pcap_if_t* d = allDevices; d; d = d->next) {
		adapterlist.Add(CString(d->description));
	}
}

// 검색
// 선택한 네트워크 어댑터의 IP 주소를 가져오는 함수
// 매개변수로 ipv4 ipv6 값을 저장할 문자열을 받음
// 네트워크 어댑터를 먼저 설정해야 함
void CNILayer::GetIPAddress(CString& ipv4addr, CString& ipv6addr) {

	// IP 주소를 저장하기 위한 문자열 배열을 선언
	char ip[IPV6_ADDR_STR_LEN];
	// 초기값으로 ipv4addr, ipv6addr를 설정
	ipv4addr = DEFAULT_EDIT_TEXT;
	ipv6addr = DEFAULT_EDIT_TEXT;

	// 장치의 모든 주소를 순회
	for (auto addr = device->addresses; addr != nullptr; addr = addr->next)
	{
		// 실제 주소를 저장
		auto realaddr = addr->addr;
		// 주소의 패밀리 타입을 가져옴
		const int sa_family = realaddr->sa_family;

		// 주소를 문자열로 변환하여 ptr에 저장
		const char* ptr = inet_ntop(sa_family, &realaddr->sa_data[sa_family == AF_INET ? 2 : 6], ip, IPV6_ADDR_STR_LEN);

		// 패밀리 타입에 따라 ipv4addr 또는 ipv6addr에 주소를 저장
		switch (sa_family)
		{
		case AF_INET:
			ipv4addr = ptr;
			break;
		case AF_INET6:
			ipv6addr = ptr;
			break;
		default:
			return;
		}
	}
}


// 패킷 수신하는 스레드 함수
UINT CNILayer::ThreadFunction_RECEIVE(LPVOID pParam) {
	struct pcap_pkthdr* header;
	const u_char* pkt_data;
	CNILayer* pNI = (CNILayer*)pParam;

	int result = 1;

	while (pNI->canRead)
	{
		result = pcap_next_ex(pNI->m_AdapterObject, &header, &pkt_data);
		if (result == 1) {
			memcpy(pNI->data, pkt_data, ETHER_MAX_SIZE);
			pNI->Receive(pNI->data);
		}
	}
	return 0;
}

// NI Layer 패킷 수신 상태 변경 함수
void CNILayer::Receiveflip() {
	canRead = !canRead;
}