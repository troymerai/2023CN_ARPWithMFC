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
	: CBaseLayer(pName), device(NULL) {
	char errbuf[PCAP_ERRBUF_SIZE];
	m_AdapterObject = nullptr;
	canRead = false;
	adapter = nullptr;
	OidData = nullptr;
	memset(data, 0, MAC_MAX_SIZE);
	try {
		// ��� ��Ʈ��ũ ��ġ�� �˻�
		if (pcap_findalldevs(&allDevices, errbuf) == -1)
		{
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
		OidData = (PPACKET_OID_DATA)malloc(sizeof(PACKET_OID_DATA));
		if (OidData == nullptr) {
			// �޸� �Ҵ翡 �����ϸ� ���ܸ� �߻�
			throw(CString("MALLOC FAIL"));
		}
		OidData->Oid = 0x01010101;
		OidData->Length = 6;
		m_AdapterObject = nullptr;
		memset(data, 0, MAC_MAX_SIZE);
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

	// allDevices�� �Ҵ��� �޸𸮸� ����
	while (allDevices) {
		temp = allDevices;
		allDevices = allDevices->next;
		delete(temp);
	}

	// OidData�� �Ҵ��� �޸𸮸� ����
	delete(OidData);
}

// network interface layer���� ��Ŷ ���� �Լ�
BOOL CNILayer::Receive(unsigned char* pkt) {

	// ��Ŷ�� null�� ��� FALSE ��ȯ (Host���� ��� �� ���)
	if (pkt == nullptr) {
		return FALSE;
	}

	// ���� �������� ��Ŷ�� ���� ���� ��� FALSE ��ȯ (Etherent Layer���� ��Ŷ �޾ƿ� �� ���)
	if (!(mp_aUpperLayer[0]->Receive(pkt))) {
		return FALSE;
	}

	// ���� ���� �� TRUE ��ȯ
	return TRUE;
}

// network interface layer���� ��Ŷ ���� �Լ�
BOOL CNILayer::Send(unsigned char* ppayload, int nlength) {

	// ��Ŷ ����
	if (pcap_sendpacket(m_AdapterObject, ppayload, nlength))
	{
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
UCHAR* CNILayer::SetAdapter(const int index) {
	char errbuf[PCAP_ERRBUF_SIZE];
	device = allDevices;

	// �̹� �����ִ� ��Ͱ� �ִٸ� ����
	if (m_AdapterObject != nullptr)
		pcap_close(m_AdapterObject);

	// �Էµ� �ε�����ŭ ��Ʈ��ũ ��� ����� ��ȸ
	for (int i = 0; i < index && device; i++) {
		device = device->next;
	}
	// �ش� ��ġ�� ����
	if (device != nullptr)
		m_AdapterObject = pcap_open_live((const char*)device->name, 65536, 0, 1000, errbuf);
	// ��ġ�� ���� ���ߴٸ� ���� �޽����� ����ϰ� nullptr�� ��ȯ
	if (m_AdapterObject == nullptr)
	{
		AfxMessageBox(_T("Fail: Connect Adapter"));
		return nullptr;
	}

	// ����� MAC �ּҸ� ��û
	adapter = PacketOpenAdapter(device->name);
	PacketRequest(adapter, FALSE, OidData);

	// ��͸� ����
	PacketCloseAdapter(adapter);
	//AfxBeginThread(ThreadFunction_RECEIVE, this);

	// MAC �ּ� ��ȯ
	return (OidData->Data);
}

// ��Ʈ��ũ ��ġ�� MAC �ּ� ����� �������� �Լ�
void CNILayer::GetMacAddressList(CStringArray& adapterlist) {
	for (pcap_if_t* d = allDevices; d; d = d->next) {
		adapterlist.Add(CString(d->description));
	}
}


// �˻�
// ������ ��Ʈ��ũ ������� IP �ּҸ� �������� �Լ�
// �Ű������� ipv4 ipv6 ���� ������ ���ڿ��� ����
// ��Ʈ��ũ ����͸� ���� �����ؾ� ��
void CNILayer::GetIPAddress(CString& ipv4addr, CString& ipv6addr) {

	// IP �ּҸ� �����ϱ� ���� ���ڿ� �迭�� ����
	char ip[IPV6_ADDR_STR_LEN];
	// �ʱⰪ���� ipv4addr, ipv6addr�� ����
	ipv4addr = DEFAULT_EDIT_TEXT;
	ipv6addr = DEFAULT_EDIT_TEXT;

	// ��ġ�� ��� �ּҸ� ��ȸ
	for (auto addr = device->addresses; addr != nullptr; addr = addr->next)
	{
		// ���� �ּҸ� ����
		auto realaddr = addr->addr;
		// �ּ��� �йи� Ÿ���� ������
		const int sa_family = realaddr->sa_family;

		// �ּҸ� ���ڿ��� ��ȯ�Ͽ� ptr�� ����
		const char* ptr = inet_ntop(sa_family, &realaddr->sa_data[sa_family == AF_INET ? 2 : 6], ip, IPV6_ADDR_STR_LEN);

		// �йи� Ÿ�Կ� ���� ipv4addr �Ǵ� ipv6addr�� �ּҸ� ����
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

// ��Ŷ �����ϴ� ������ �Լ�
UINT CNILayer::ThreadFunction_RECEIVE(LPVOID pParam) {
	struct pcap_pkthdr* header;
	const u_char* pkt_data;
	CNILayer* pNI = (CNILayer*)pParam;

	int result = 1;

	while (pNI->canRead)
	{
		result = pcap_next_ex(pNI->m_AdapterObject, &header, &pkt_data);
		if (result == 1) {
			memcpy(pNI->data, pkt_data, MAC_MAX_SIZE);
			pNI->Receive(pNI->data);
		}
	}
	return 0;
}

// NI Layer ��Ŷ ���� ���� ���� �Լ�
void CNILayer::Receiveflip() {
	canRead = !canRead;
}