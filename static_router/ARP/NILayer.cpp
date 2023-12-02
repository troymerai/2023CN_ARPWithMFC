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
	: CBaseLayer(pName), device()
	, m_AdapterObject()
	, canRead(false)
	, adapter() {
	char errbuf[PCAP_ERRBUF_SIZE];
	OidData[INNER] = nullptr;
	OidData[OUTER] = nullptr;
	memset(data[INNER], 0, ETHER_MAX_SIZE);
	memset(data[OUTER], 0, ETHER_MAX_SIZE);
	try {
		// 모든 네트워크 장치를 검색
		if (pcap_findalldevs(&allDevices, errbuf) == -1){
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
		OidData[INNER] = (PPACKET_OID_DATA)malloc(sizeof(PACKET_OID_DATA));
		OidData[OUTER] = (PPACKET_OID_DATA)malloc(sizeof(PACKET_OID_DATA));
		// 메모리 할당에 실패하면 예외를 발생
		if (OidData[INNER] == nullptr) throw(CString("MALLOC FAIL"));
		if (OidData[OUTER] == nullptr) throw(CString("MALLOC FAIL"));

		// 초기값 설정
		OidData[INNER]->Oid = 0x01010101;
		OidData[OUTER]->Oid = 0x01010101;
		OidData[INNER]->Length = 6;
		OidData[OUTER]->Length = 6;
		m_AdapterObject[INNER] = nullptr;
		m_AdapterObject[OUTER] = nullptr;
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
BOOL CNILayer::Receive(unsigned char* pkt, int iosel) {

	// 패킷이 null인 경우 FALSE 반환 (Host끼리 통신 시 사용)
	if (pkt == nullptr) return FALSE;

	// 상위 계층에서 패킷을 받지 못한 경우 FALSE 반환 (Etherent Layer에서 패킷 받아올 때 사용)
	if (!(mp_aUpperLayer[0]->Receive(pkt, iosel))) return FALSE;

	// 수신 성공 시 TRUE 반환
	return TRUE;
}

// network interface layer에서 패킷 전송 함수
BOOL CNILayer::Send(unsigned char* ppayload, int nlength, int iosel) {

	// 패킷 전송
	if (pcap_sendpacket(m_AdapterObject[iosel], ppayload, nlength)) {
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
UCHAR* CNILayer::SetAdapter(const int index, const int iosel) {
	char errbuf[PCAP_ERRBUF_SIZE];
	device[iosel] = allDevices;

	// 이미 열려있는 어뎁터가 있다면 닫음
	if (m_AdapterObject[iosel] != nullptr) pcap_close(m_AdapterObject[iosel]);

	// 입력된 인덱스만큼 네트워크 어뎁터 목록을 순회
	for (int i = 0; i < index && device; i++) device[iosel] = (device[iosel])->next;

	// 해당 장치를 열음
	if (device[iosel] != nullptr) 
		m_AdapterObject[iosel] = pcap_open_live((const char*)device[iosel]->name, 65536, 0, 1000, errbuf);
	
	// 장치를 열지 못했다면 에러 메시지를 출력하고 nullptr를 반환
	if (m_AdapterObject[iosel] == nullptr) {
		AfxMessageBox(_T("Fail: Connect Adapter"));
		return nullptr;
	}

	// 어뎁터의 MAC 주소를 요청
	adapter[iosel] = PacketOpenAdapter(device[iosel]->name);
	PacketRequest(adapter[iosel], FALSE, OidData[iosel]);

	//PacketCloseAdapter(adapter[iosel]);

	// MAC 주소 데이터를 반환
	return (OidData[iosel]->Data);
}

// 네트워크 어댑터의 MAC 주소 목록을 가져오는 함수
void CNILayer::GetMacAddressList(CStringArray& adapterlist) {

	// 모든 네트워크 장치를 순회하며 설명을 어댑터 목록에 추가
	for (pcap_if_t* d = allDevices; d; d = d->next) {
		adapterlist.Add(CString(d->description));
	}
}

// 특정 네트워크 어댑터의 MAC 주소를 가져오는 함수
void CNILayer::GetMacAddress(const int index, UCHAR *mac, const int iosel) {
	pcap_if_t* d = allDevices;
	pcap_t* tadapter = nullptr;
	LPADAPTER ad;
	char errbuf[PCAP_ERRBUF_SIZE];

	// 입력된 인덱스만큼 네트워크 장치 목록을 순회
	for (int i = 0; i < index && d; i++) d = d->next;

	// 요청한 장치가 현재 선택된 장치와 같다면 MAC 주소를 복사
	if (d == device[iosel]) memcpy(mac, OidData[iosel]->Data, ENET_ADDR_SIZE);
	else {
		// 아니라면 해당 장치를 열고 MAC 주소를 요청
		if (d != nullptr)
			tadapter = pcap_open_live((const char*)(d->name), 65536, 0, 1000, errbuf);

		ad = PacketOpenAdapter(d->name);
		PacketRequest(ad, FALSE, OidData[iosel] );

		// 요청한 MAC 주소를 복사
		memcpy(mac, OidData[iosel]->Data, ENET_ADDR_SIZE);
		//PacketCloseAdapter(ad);
		//pcap_close(tadapter);
	}
}

// 특정 네트워크 어댑터의 IP 주소를 가져오는 함수
void CNILayer::GetIPAddress(CString& ipv4addr, CString& ipv6addr, const int iosel, bool isIOsel=FALSE) {
	
	char ip[IPV6_ADDR_STR_LEN];
	ipv4addr = ipv6addr =DEFAULT_EDIT_TEXT;
	pcap_if_t* d = allDevices;

	// 입력된 인덱스만큼 네트워크 장치 목록을 순회
	for (int i = 0; i < iosel; i++)d = d->next;
	pcap_addr* addr = d->addresses;

	// isIOsel 옵션이 true라면 현재 선택된 장치의 주소를 사용
	if (isIOsel) {
		addr = (device[iosel])->addresses;
	}

	// 모든 주소를 순회하며 IP 주소를 가져옴
	for (; addr != nullptr; addr = addr->next) {
		auto realaddr = addr->addr;
		const int sa_family = realaddr->sa_family;

		const char* ptr = 
			inet_ntop(sa_family, &(realaddr->sa_data)[sa_family == AF_INET ? 2 : 6], ip, IPV6_ADDR_STR_LEN);

		switch (sa_family){
		case AF_INET:
			// IPv4 주소를 가져옴
			ipv4addr = ptr;
			break;
		case AF_INET6:
			// IPv6 주소를 가져옴
			ipv6addr = ptr;
			break;
		default:
			return;
		}
	}
}

// 외부 네트워크 어댑터에서 패킷을 수신하는 스레드 함수
UINT CNILayer::ThreadFunction_RECEIVE(LPVOID pParam) {

	// 패킷 헤더
	struct pcap_pkthdr* header;
	// 패킷 데이터
	const u_char* pkt_data;
	// CNILayer 객체
	CNILayer* pNI = (CNILayer*)pParam;
	int result = 1;

	// 수신 가능 상태일 때 패킷을 수신
	while (pNI->canRead){
		result = pcap_next_ex((pNI->m_AdapterObject)[OUTER], &header, &pkt_data);
		if (result == 1) {
			memcpy((pNI->data)[OUTER], pkt_data, ETHER_MAX_SIZE);
			pNI->Receive((pNI->data)[OUTER], OUTER);
		}
	}
	return 0;
}

// 내부 네트워크 어댑터에서 패킷을 수신하는 스레드 함수
UINT CNILayer::ThreadFunction_RECEIVE2(LPVOID pParam) {

	// 패킷 헤더
	struct pcap_pkthdr* header;
	// 패킷 데이터
	const u_char* pkt_data;
	// CNILayer 객체
	CNILayer* pNI = (CNILayer*)pParam;
	int result = 1;

	// 수신 가능 상태일 때 패킷을 수신
	while (pNI->canRead) {
		result = pcap_next_ex((pNI->m_AdapterObject)[INNER], &header, &pkt_data);
		if (result == 1) {
			memcpy((pNI->data)[INNER], pkt_data, ETHER_MAX_SIZE);
			pNI->Receive((pNI->data)[INNER], INNER);
		}
	}
	return 0;
}

// NI Layer 패킷 수신 상태 변경 함수
void CNILayer::Receiveflip() {
	canRead = !canRead;
}