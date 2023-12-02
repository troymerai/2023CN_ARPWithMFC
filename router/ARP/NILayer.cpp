// NetworkInterfaceLayer.cpp: implementation of the NetworkInterfaceLayer class.

#include "stdafx.h"
#include "pch.h"
#include "NILayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// ������
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
		// ��� ��Ʈ��ũ ��ġ�� �˻�
		if (pcap_findalldevs(&allDevices, errbuf) == -1){
			allDevices = NULL;
			// ��ġ�� ã�� �� �����ϸ� ���� �߻�
			throw(CString(errbuf));
		}
	}
	// ���� �߻� ��
	catch (CString errorInfo) {
		// ���� �޽����� ����ϰ� �Լ��� ����
		AfxMessageBox(errorInfo);
		return;
	}

	try {
		// OidData�� �޸𸮸� �Ҵ�
		OidData[INNER] = (PPACKET_OID_DATA)malloc(sizeof(PACKET_OID_DATA));
		OidData[OUTER] = (PPACKET_OID_DATA)malloc(sizeof(PACKET_OID_DATA));
		// �޸� �Ҵ翡 �����ϸ� ���ܸ� �߻�
		if (OidData[INNER] == nullptr) throw(CString("MALLOC FAIL"));
		if (OidData[OUTER] == nullptr) throw(CString("MALLOC FAIL"));

		// �ʱⰪ ����
		OidData[INNER]->Oid = 0x01010101;
		OidData[OUTER]->Oid = 0x01010101;
		OidData[INNER]->Length = 6;
		OidData[OUTER]->Length = 6;
		m_AdapterObject[INNER] = nullptr;
		m_AdapterObject[OUTER] = nullptr;
	}
	// ���� �߻� ��
	catch (CString errorInfo) {
		// ���� �޽��� ����ϰ� �Լ� ����
		AfxMessageBox(errorInfo);
		return;
	}
}

// �Ҹ���
CNILayer::~CNILayer() {
	pcap_if_t* temp = nullptr;

	while (allDevices) {
		temp = allDevices;
		allDevices = allDevices->next;
		delete(temp);
	}
	delete(OidData);
}

// network interface layer���� ��Ŷ ���� �Լ�
BOOL CNILayer::Receive(unsigned char* pkt, int iosel) {

	// ��Ŷ�� null�� ��� FALSE ��ȯ (Host���� ��� �� ���)
	if (pkt == nullptr) return FALSE;

	// ���� �������� ��Ŷ�� ���� ���� ��� FALSE ��ȯ (Etherent Layer���� ��Ŷ �޾ƿ� �� ���)
	if (!(mp_aUpperLayer[0]->Receive(pkt, iosel))) return FALSE;

	// ���� ���� �� TRUE ��ȯ
	return TRUE;
}

// network interface layer���� ��Ŷ ���� �Լ�
BOOL CNILayer::Send(unsigned char* ppayload, int nlength, int iosel) {

	// ��Ŷ ����
	if (pcap_sendpacket(m_AdapterObject[iosel], ppayload, nlength)) {
		// ���� �� ���� �޽��� ���
		AfxMessageBox(_T("Fail: Send Packet!"));
		// FALSE ��ȯ
		return FALSE;
	}
	// ���� ���� �� TRUE ��ȯ
	return TRUE;
}

// ��Ʈ��ũ ��� ����� �޺��ڽ��� �����ϴ� �Լ�
void CNILayer::SetAdapterComboBox(CComboBox& adapterlist) {
	// ��� ��Ʈ��ũ ��͸� ��ȸ�ϸ�
	for (pcap_if_t* d = allDevices; d; d = d->next) {
		//�� ��ġ�� ������ �޺� �ڽ��� �߰�
		adapterlist.AddString(CString(d->description));
	}
}

// ������ ��Ʈ��ũ ��͸� �����ϴ� �Լ�
UCHAR* CNILayer::SetAdapter(const int index, const int iosel) {
	char errbuf[PCAP_ERRBUF_SIZE];
	device[iosel] = allDevices;

	// �̹� �����ִ� ��Ͱ� �ִٸ� ����
	if (m_AdapterObject[iosel] != nullptr) pcap_close(m_AdapterObject[iosel]);

	// �Էµ� �ε�����ŭ ��Ʈ��ũ ��� ����� ��ȸ
	for (int i = 0; i < index && device; i++) device[iosel] = (device[iosel])->next;

	// �ش� ��ġ�� ����
	if (device[iosel] != nullptr) 
		m_AdapterObject[iosel] = pcap_open_live((const char*)device[iosel]->name, 65536, 0, 1000, errbuf);
	
	// ��ġ�� ���� ���ߴٸ� ���� �޽����� ����ϰ� nullptr�� ��ȯ
	if (m_AdapterObject[iosel] == nullptr) {
		AfxMessageBox(_T("Fail: Connect Adapter"));
		return nullptr;
	}

	// ����� MAC �ּҸ� ��û
	adapter[iosel] = PacketOpenAdapter(device[iosel]->name);
	PacketRequest(adapter[iosel], FALSE, OidData[iosel]);

	//PacketCloseAdapter(adapter[iosel]);

	// MAC �ּ� �����͸� ��ȯ
	return (OidData[iosel]->Data);
}

// ��Ʈ��ũ ������� MAC �ּ� ����� �������� �Լ�
void CNILayer::GetMacAddressList(CStringArray& adapterlist) {

	// ��� ��Ʈ��ũ ��ġ�� ��ȸ�ϸ� ������ ����� ��Ͽ� �߰�
	for (pcap_if_t* d = allDevices; d; d = d->next) {
		adapterlist.Add(CString(d->description));
	}
}

// Ư�� ��Ʈ��ũ ������� MAC �ּҸ� �������� �Լ�
void CNILayer::GetMacAddress(const int index, UCHAR *mac, const int iosel) {
	pcap_if_t* d = allDevices;
	pcap_t* tadapter = nullptr;
	LPADAPTER ad;
	char errbuf[PCAP_ERRBUF_SIZE];

	// �Էµ� �ε�����ŭ ��Ʈ��ũ ��ġ ����� ��ȸ
	for (int i = 0; i < index && d; i++) d = d->next;

	// ��û�� ��ġ�� ���� ���õ� ��ġ�� ���ٸ� MAC �ּҸ� ����
	if (d == device[iosel]) memcpy(mac, OidData[iosel]->Data, ENET_ADDR_SIZE);
	else {
		// �ƴ϶�� �ش� ��ġ�� ���� MAC �ּҸ� ��û
		if (d != nullptr)
			tadapter = pcap_open_live((const char*)(d->name), 65536, 0, 1000, errbuf);

		ad = PacketOpenAdapter(d->name);
		PacketRequest(ad, FALSE, OidData[iosel] );

		// ��û�� MAC �ּҸ� ����
		memcpy(mac, OidData[iosel]->Data, ENET_ADDR_SIZE);
		//PacketCloseAdapter(ad);
		//pcap_close(tadapter);
	}
}

// Ư�� ��Ʈ��ũ ������� IP �ּҸ� �������� �Լ�
void CNILayer::GetIPAddress(CString& ipv4addr, CString& ipv6addr, const int iosel, bool isIOsel=FALSE) {
	
	char ip[IPV6_ADDR_STR_LEN];
	ipv4addr = ipv6addr =DEFAULT_EDIT_TEXT;
	pcap_if_t* d = allDevices;

	// �Էµ� �ε�����ŭ ��Ʈ��ũ ��ġ ����� ��ȸ
	for (int i = 0; i < iosel; i++)d = d->next;
	pcap_addr* addr = d->addresses;

	// isIOsel �ɼ��� true��� ���� ���õ� ��ġ�� �ּҸ� ���
	if (isIOsel) {
		addr = (device[iosel])->addresses;
	}

	// ��� �ּҸ� ��ȸ�ϸ� IP �ּҸ� ������
	for (; addr != nullptr; addr = addr->next) {
		auto realaddr = addr->addr;
		const int sa_family = realaddr->sa_family;

		const char* ptr = 
			inet_ntop(sa_family, &(realaddr->sa_data)[sa_family == AF_INET ? 2 : 6], ip, IPV6_ADDR_STR_LEN);

		switch (sa_family){
		case AF_INET:
			// IPv4 �ּҸ� ������
			ipv4addr = ptr;
			break;
		case AF_INET6:
			// IPv6 �ּҸ� ������
			ipv6addr = ptr;
			break;
		default:
			return;
		}
	}
}

// �ܺ� ��Ʈ��ũ ����Ϳ��� ��Ŷ�� �����ϴ� ������ �Լ�
UINT CNILayer::ThreadFunction_RECEIVE(LPVOID pParam) {

	// ��Ŷ ���
	struct pcap_pkthdr* header;
	// ��Ŷ ������
	const u_char* pkt_data;
	// CNILayer ��ü
	CNILayer* pNI = (CNILayer*)pParam;
	int result = 1;

	// ���� ���� ������ �� ��Ŷ�� ����
	while (pNI->canRead){
		result = pcap_next_ex((pNI->m_AdapterObject)[OUTER], &header, &pkt_data);
		if (result == 1) {
			memcpy((pNI->data)[OUTER], pkt_data, ETHER_MAX_SIZE);
			pNI->Receive((pNI->data)[OUTER], OUTER);
		}
	}
	return 0;
}

// ���� ��Ʈ��ũ ����Ϳ��� ��Ŷ�� �����ϴ� ������ �Լ�
UINT CNILayer::ThreadFunction_RECEIVE2(LPVOID pParam) {

	// ��Ŷ ���
	struct pcap_pkthdr* header;
	// ��Ŷ ������
	const u_char* pkt_data;
	// CNILayer ��ü
	CNILayer* pNI = (CNILayer*)pParam;
	int result = 1;

	// ���� ���� ������ �� ��Ŷ�� ����
	while (pNI->canRead) {
		result = pcap_next_ex((pNI->m_AdapterObject)[INNER], &header, &pkt_data);
		if (result == 1) {
			memcpy((pNI->data)[INNER], pkt_data, ETHER_MAX_SIZE);
			pNI->Receive((pNI->data)[INNER], INNER);
		}
	}
	return 0;
}

// NI Layer ��Ŷ ���� ���� ���� �Լ�
void CNILayer::Receiveflip() {
	canRead = !canRead;
}