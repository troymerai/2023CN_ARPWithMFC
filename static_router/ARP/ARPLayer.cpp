#include "pch.h"
#include "ARPLayer.h"

// IP �ּҿ� MAC �ּҸ� �޾Ƽ� ARP ��带 ���� 
CARPLayer::_ARP_NODE::_ARP_NODE(unsigned char* cipaddr, unsigned char* cenetaddr, unsigned char bincomplete = false) {
	// IP(��������)�ּ� ����
	memcpy(protocol_addr, cipaddr, IP_ADDR_SIZE);
	// MAC(�ϵ����)�ּ� ����
	memcpy(hardware_addr, cenetaddr, ENET_ADDR_SIZE);
	// status�� incomplete�� ����
	status = bincomplete;
	// ARP ���̺� �׸��� �߰��� �ð� ����
	spanTime = CTime::GetCurrentTime();
}

// IP �ּҿ� MAC �ּҸ� 0���� �ʱ�ȭ�� ARP ��带 ����
CARPLayer::_ARP_NODE::_ARP_NODE(unsigned int ipaddr = 0, unsigned int enetaddr = 0, unsigned char incomplete = false) {
	// IP �ּ� �ʱ�ȭ
	memset(protocol_addr, ipaddr, IP_ADDR_SIZE);
	// MAC �ּ� �ʱ�ȭ
	memset(hardware_addr, enetaddr, ENET_ADDR_SIZE);
	// status�� incomplete�� �ʱ�ȭ
	status = incomplete;
	//ARP ���̺� �׸��� �߰��� �ð� ����
	spanTime = CTime::GetCurrentTime();
}

// �ٸ� ARP ��� ��ü�κ��� ������ �����Ͽ� ARP ��带 ����
CARPLayer::_ARP_NODE::_ARP_NODE(const struct _ARP_NODE& ot) {
	// IP �ּ� �ʱ�ȭ
	memcpy(protocol_addr, ot.protocol_addr, IP_ADDR_SIZE);
	// MAC �ּ� �ʱ�ȭ
	memcpy(hardware_addr, ot.hardware_addr, ENET_ADDR_SIZE);
	// ���� ����
	status = ot.status;
	//ARP ���̺� �׸��� �߰��� �ð� ����
	spanTime = ot.spanTime;
}


// ARP ����� �ʱ�ȭ�ϴ� �Լ�
inline void CARPLayer::ResetHeader() {
	// m_sHeader[INNER]�� m_sHeader[OUTER] �� ���� ARP ����� ���ο� ARP_HEADER()�� �缳��
	m_sHeader[INNER] = ARP_HEADER();
	m_sHeader[OUTER] = ARP_HEADER();
}


bool CARPLayer::getMACinARP(unsigned char* dstIP, unsigned char* MAC)
{
	/////////////
	// �־��� IP �ּ�(dstIP)�� ARP ���̺� �ִ��� Ȯ��
	// �ִٸ� �ش� IP �ּ��� MAC �ּҸ� ������ MAC�� ����
	// �ش� IP �ּҰ� ARP ���̺� ������ true�� ��ȯ, ������ false�� ��ȯ
	/////////////

	// inCache() �Լ��� ȣ���Ͽ� �־��� IP �ּ�(dstIP)�� ARP ĳ�ÿ� �ִ��� Ȯ���ϰ�, �� ����� index�� ����
	int index = inCache(dstIP);
	bool isfound = FALSE;
	// index�� -1�� �ƴ϶��, ** == �־��� IP �ּҰ� ARP ĳ�ÿ� �ִٸ� �̾����� �ڵ带 ����
	if (index != -1) {
		// memcpy() �Լ��� ����Ͽ� ARP ���̺��� �ش� �ε������� MAC �ּҸ� ������ MAC�� ����
		memcpy(MAC, m_arpTable[index].hardware_addr, ENET_ADDR_SIZE);
		// isfound ������ TRUE�� �����Ͽ� �־��� IP �ּҰ� ARP ĳ�ÿ� ������ ��Ÿ��
		isfound = TRUE;
	}
	return isfound;
}

// Gratuitous ARP �÷��׸� �����ϴ� �Լ�
void CARPLayer::isGARP(BOOL yes)
{
	is_Garp = !yes;
}

// ARP ���� �Լ�
BOOL CARPLayer::Receive(unsigned char* ppayload, int iosel) {

	// ARP ��� Ÿ������ ĳ����
	PARP_HEADER arp_data = (PARP_HEADER)ppayload;

	// �̴��� ���� ��ü ����
	CEthernetLayer* m_ether = (CEthernetLayer*)mp_UnderLayer;

	// ARP ĳ�ÿ��� ��Ŷ�� �ҽ� �ϵ���� �ּҸ� ã��
	int index = inCache(arp_data->hardware_srcaddr);

	// ������ ARP ��Ŷ�� �۽� IP�� �ڽ��� IP�� ������ Ȯ�� ** ���ٸ� false�� ��ȯ
	if (memcmp(myip, arp_data->protocol_srcaddr, IP_ADDR_SIZE) == 0) {
		return FALSE;
	}

	// ��Ŷ�� opcode�� ���� �б� ó��
	switch (arp_data->opercode) {

	// ARP ��û�� ���
	case ARP_OPCODE_REQUEST:


		// ĳ�ÿ� �̹� �ּҰ� �ִ� ��� (GARP)
		if (index >= 0) {

		// 20231202 ����
		// GARP �ڵ�� inline ���� �����ؾ���
		// 
		//	// �ڱⰡ ���� GARP�� ���ؼ��� reply�� �Ⱦ��� ���� ó��
		//	if (memcmp(arp_data->protocol_srcaddr, myip, IP_ADDR_SIZE) != 0) {
		//		// IP �浹�� �߻��� ��� GARP reply ��Ŷ �ۼ�
		//		if (memcmp(arp_data->protocol_srcaddr, myip, IP_ADDR_SIZE) == 0) {
		//			// �ڽ��� MAC �ּҸ� ��Ŷ�� ������ �ϵ���� �ּҷ� ����
		//			memcpy(arp_data->hardware_dstaddr, mymac, ENET_ADDR_SIZE);
		//			// ��Ŷ�� �۾� �ڵ带 ARP �������� ����
		//			arp_data->opercode = ARP_OPCODE_REPLY;
		//			// ��Ŷ�� �ҽ� �ּҿ� ������ �ּҸ� ��ȯ
		//			swapaddr(arp_data->hardware_srcaddr, arp_data->hardware_dstaddr, ENET_ADDR_SIZE);
		//			swapaddr(arp_data->protocol_srcaddr, arp_data->protocol_dstaddr, IP_ADDR_SIZE);

		//			// �̴��� ������ ������ �ּҸ� ��Ŷ�� ������ �ϵ���� �ּҷ� ����
		//			m_ether->SetDestinAddress(arp_data->hardware_dstaddr);
		//			// ��Ŷ�� ���� ����(�̴��� ����)���� ����
		//			mp_UnderLayer->Send((unsigned char*)arp_data, ARP_HEADER_SIZE);
		//		}
		//		break;
		//	}

			// ĳ���� �ش� �׸��� ����
			memcpy(m_arpTable[index].hardware_addr, arp_data->hardware_srcaddr, ENET_ADDR_SIZE);
			m_arpTable[index].status = true;
			m_arpTable[index].spanTime = CTime::GetCurrentTime();
		}
		// ĳ�ÿ� �ּҰ� ���� ���
		else {
			// �� �׸��� ĳ�ÿ� �߰�
			m_arpTable.push_back(ARP_NODE(arp_data->protocol_srcaddr, arp_data->hardware_srcaddr, TRUE));
		}

		// ARP�� ���
		// ��Ŷ�� ������ IP �ּҰ� �ڽ��� IP �ּҿ� ���� ���
		if (memcmp(arp_data->protocol_dstaddr, myip, IP_ADDR_SIZE) == 0) {
			// �ڽ��� MAC �ּҸ� ��Ŷ�� ������ �ϵ���� �ּҷ� ����
			memcpy(arp_data->hardware_dstaddr, mymac, ENET_ADDR_SIZE);
			// ��Ŷ�� �۾� �ڵ带 ARP �������� ����
			arp_data->opercode = ARP_OPCODE_REPLY;
			// ��Ŷ�� �ҽ� �ּҿ� ������ �ּҸ� ��ȯ
			swapaddr(arp_data->hardware_srcaddr, arp_data->hardware_dstaddr, ENET_ADDR_SIZE);
			swapaddr(arp_data->protocol_srcaddr, arp_data->protocol_dstaddr, IP_ADDR_SIZE);

			// ���� ��Ŷ�� ������ MAC �ּҸ� ����
			m_ether->SetDestinAddress(arp_data->hardware_dstaddr, iosel);
			// ���� ��Ŷ�� ����
			return mp_UnderLayer->Send((unsigned char*)arp_data, ARP_HEADER_SIZE, iosel);
		}

		//���� request�� dst ip�� ���� ip�� �ƴ� ��� proxy table Ȯ�� �� ��ϵ� ������ �ִٸ� reply
		else {
			for (auto &a = m_proxyTable.begin(); a != m_proxyTable.end(); a++) {
				//proxy�� ��ϵ� ip�� arp request�� dst addr�̶�� proxy arp reply ����
				if (memcmp(a->protocol_addr, arp_data->protocol_dstaddr, IP_ADDR_SIZE) == 0) {
					memcpy(arp_data->hardware_dstaddr, a->srchardware_addr, ENET_ADDR_SIZE);
					arp_data->opercode = ARP_OPCODE_REPLY;
					// �۽��ڿ� �������� MAC, IP �ּҸ� ��ȯ
					swapaddr(arp_data->hardware_srcaddr, arp_data->hardware_dstaddr, ENET_ADDR_SIZE);
					swapaddr(arp_data->protocol_srcaddr, arp_data->protocol_dstaddr, IP_ADDR_SIZE);

					// ���� ��Ŷ�� ������ MAC �ּҸ� ����
					m_ether->SetDestinAddress(arp_data->hardware_dstaddr, iosel);
					// ���� ��Ŷ�� ����
					return mp_UnderLayer->Send((unsigned char*)arp_data, ARP_HEADER_SIZE, iosel);
				}
			}
		}
		break;

	// ARP ������ ���
	case ARP_OPCODE_REPLY:
		for (auto& node : m_arpTable) {
			if (node == arp_data->protocol_srcaddr) {
				memcpy(node.hardware_addr, arp_data->hardware_srcaddr, ENET_ADDR_SIZE);
				node.status = true;
				node.spanTime = CTime::GetCurrentTime();
				break;
			}
		}
		break;

	// GARP reply�� ���� �׽�Ʈ�ؾ��� - 20231202

	//// ARP ������ ���
	//case ARP_OPCODE_REPLY:

	//	// GARP�� ���
	//	if (memcmp(arp_data->protocol_dstaddr, arp_data->protocol_srcaddr, IP_ADDR_SIZE) == 0) {

	//		if (isDetected == FALSE) {
	//			// Dlg�� ����� �޽��� ������ �ڵ� �߰�
	//			AfxMessageBox(_T("IP conflict detected!"));

	//			// GARP ��û �� ���� ������ ������ ip detected�� �� ���� ��Ÿ������ ����
	//			isDetected = TRUE;
	//		}

	//		break;
	//	}


	//	// ARP�� ���
	//	// ĳ�ø� ��ȸ�ϸ� ��Ŷ�� �ҽ� �������� �ּҿ� ���� �׸��� ã��
	//	for (auto& node : m_arpTable) {
	//		if (node == arp_data->protocol_srcaddr) {
	//			// �ش� �׸��� ����
	//			memcpy(node.hardware_addr, arp_data->hardware_srcaddr, ENET_ADDR_SIZE);
	//			node.status = true;
	//			node.spanTime = CTime::GetCurrentTime();
	//			break;
	//		}
	//	}
	//	break;

	// �� �� ���� �۾� �ڵ��� ���
	default:
		throw("unknown Opcode Error");
		return false;
	}

	// ��Ŷ ���� ������ ��ȯ
	return true;
}

// ARP ���� ��Ŷ ���� �Լ�
BOOL CARPLayer::Send(unsigned char* ppayload, int nlength, int iosel) {

	// ���޹��� payload�� IP��� Ÿ������ ĳ�����Ͽ� ip data Ȯ��
	PIP_HEADER ip_data = (PIP_HEADER)ppayload;

	// �̴��� ���̾� ����
	CEthernetLayer* m_ether = (CEthernetLayer*)mp_UnderLayer;

	// broadcast �ּ� ����
	unsigned char broadcastAddr[ENET_ADDR_SIZE];

	// ��ε�ĳ��Ʈ �ּҸ� ���� ** GARP�� Ȱ��ȭ�Ǿ� ������ 255�� ����
	memset(broadcastAddr, 255*is_Garp, ENET_ADDR_SIZE);
	
	// ���ο� ARP ��� ����
	ARP_NODE newNode(ip_data->ip_dstaddr, broadcastAddr);

	// �۽� IP�� ������ IP�� ���� ���� ���
	if (memcmp(ip_data->ip_srcaddr, ip_data->ip_dstaddr, IP_ADDR_SIZE) != 0) {
		// �־��� �ּҰ� ARP ĳ�� ���̺� �ִ��� Ȯ��
		int idx = inCache(ip_data->ip_dstaddr);
		// ������ IP�� ARP ĳ�ÿ� �ִٸ�
		if (idx != -1) {
			// �ش� ARP ��Ʈ�� ������Ʈ
			m_arpTable[idx] = newNode;
		}
		// ������ IP�� ARP ĳ�ÿ� ���ٸ�
		else {
			// �� ARP ��Ʈ�� �߰�
			m_arpTable.push_back(newNode);
		}
	}
	// ��Ŷ�� ������ �ּҸ� ����
	m_ether->SetDestinAddress(broadcastAddr, iosel);
	// ��Ŷ�� ������ ARP�� ����
	m_ether->SetType(ETHER_ARP_TYPE, iosel);
	// �۾� �ڵ带 ��û(REQUEST)���� ����
	setOpcode(ARP_OPCODE_REQUEST, iosel);
	// ��Ŷ�� �۽��� �ּҸ� ����
	setSrcAddr(m_ether->GetSourceAddress(iosel), ip_data->ip_srcaddr, iosel);
	// ��Ŷ�� ������ �ּҸ� ����
	setDstAddr(broadcastAddr, ip_data->ip_dstaddr, iosel);

	// ������ ARP ��Ŷ�� ����
	return mp_UnderLayer->Send((unsigned char*)&m_sHeader[iosel], ARP_HEADER_SIZE, iosel);
}

// �־��� �ð� ���� ����ϴ� �Լ�
// Ư�� �ð�(dwMillisecond) ���� �޽��� ������ �����ϸ� ���
void CARPLayer::Wait(DWORD dwMillisecond)
{
	// ������ �޽����� ������ MSG ����ü�� ����
	MSG msg;
	// ���� �ð��� ������ ������ �����ϰ� ���� �ð��� ����
	DWORD dwStart;
	dwStart = GetTickCount();

	// �־��� �ð��� ����� ������ ������ ����
	while (GetTickCount() - dwStart < dwMillisecond)
	{
		// �޽��� ť���� �޽����� �����ɴϴ�. �޽����� ���� ������ �ݺ�
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// ���� �޽����� ������ ���� �޽����� ��ȯ
			TranslateMessage(&msg);
			// �޽����� �ش� ������ ���ν����� ����
			DispatchMessage(&msg);
		}
	}
}

// Ư�� ����Ʈ���� IP�� ���� ��Ŷ�� �����ϴ� �Լ�
BOOL CARPLayer::RSend(unsigned char* ppayload, int nlength, unsigned char* gatewayIP, int iosel) {

	// ������ IP ��� �����͸� �Ľ�
	PIP_HEADER ip_data = (PIP_HEADER)ppayload;

	// �ӽ� ���۸� �����ϰ� �ʱ�ȭ
	unsigned char temp[IP_MAX_SIZE] = { 0, };

	// ������ �������� ���̰� �ִ� ũ�⸦ �ʰ��ϸ� �ִ� ũ��� ����
	nlength = nlength > IP_MAX_SIZE ? IP_MAX_SIZE : nlength;

	// �ӽ� ���ۿ� ������ �����͸� ����
	memcpy(temp, ppayload, nlength);
	

	// ���� ���̾�(Ethernet ���̾�) ��ü ����
	CEthernetLayer* m_ether = (CEthernetLayer*)mp_UnderLayer;
	int idx = 0;

	// proxy���̺� �ִٸ� �������� �ٷ� ������
	for (int i = 0; i < m_proxyTable.size(); i++) {
		if (memcmp(m_proxyTable[i].protocol_addr, gatewayIP, IP_ADDR_SIZE) == 0)
		{
			m_ether->SetDestinAddress(m_proxyTable[i].hardware_addr, iosel);
			m_ether->SetType(ETHER_IP_TYPE, iosel);

			// proxy table���� ã������ �������� ��Ŷ ���� ��û�ϰ� �Լ� ����
			return mp_UnderLayer->Send(temp, nlength, iosel);
		}
	}

	//proxy table�� ���ٸ� arp table ã�ƺ���
	idx = inCache(gatewayIP);
	if (idx == -1) {
		//arp table���� ������ �ش� ip�ּҷ� ARP request ��������
		IP_HEADER forarp;
		memcpy(forarp.ip_srcaddr, myip[iosel], IP_ADDR_SIZE);
		memcpy(forarp.ip_dstaddr, gatewayIP, IP_ADDR_SIZE);
		Send((unsigned char*)&forarp, nlength, iosel);
		
		// ARP reply�� ��ٸ�
		Wait(3000);
		
		// ARP reply ������ arp table �ٽ� �˻� 
		idx = inCache(gatewayIP);

		// arp table���� ������ FALSE ���� �Լ� ����
		if (idx == -1) {
			return FALSE;
		}
	}
	// ARP ���̺� ����Ʈ���� IP�� �ִٸ� �ش� �ּҷ� ��Ŷ�� ����
	m_ether->SetDestinAddress(m_arpTable[idx].hardware_addr, iosel);
	m_ether->SetType(ETHER_IP_TYPE, iosel);
	return mp_UnderLayer->Send(temp, nlength, iosel);
}


// IP �ּҰ� ARP cache table�� �ִ� �� Ȯ���ϴ� �Լ�
int CARPLayer::inCache(unsigned char* ipaddr) {
	int res = -1;
	for (int i = 0; i < m_arpTable.size(); i++) {
		if (m_arpTable[i] == ipaddr) {
			res = i;
			break;
		}
	}
	return res;
}

// ARP Layer�� MAC, IP �ּ� ���� �Լ�
void CARPLayer::setmyAddr(CString MAC, CString IP, int iosel) {
	StrToaddr(ARP_IP_TYPE, &myip[iosel][0], IP);
	StrToaddr(ARP_ENET_TYPE, &mymac[iosel][0], MAC);
}

// ARP ����� �ϵ���� Ÿ�԰� �������� Ÿ���� �����ϴ� �Լ�
void CARPLayer::setType(const unsigned short htype, const unsigned short ptype, int iosel) {

	//ARP ��Ŷ���� ���Ǵ� �ϵ���� �� �������� ���� ����
	m_sHeader[iosel].hardware_type = htype;
	m_sHeader[iosel].protocol_type = ptype;

	// �ϵ���� Ÿ�Կ� ���� �ϵ���� �ּ��� ���̸� ����
	switch (m_sHeader[iosel].hardware_type) {
	case ARP_ENET_TYPE:
		m_sHeader[iosel].hardware_length = ENET_ADDR_SIZE;
		break;
	default:
		// �������� �ʴ� �ϵ���� Ÿ���̶�� ���ܸ� �߻���Ŵ
		throw("Hardware Type Error!");
	}

	// �������� Ÿ�Կ� ���� �������� �ּ��� ���̸� ����
	switch (m_sHeader[iosel].protocol_type) {
	case ARP_IP_TYPE:
		m_sHeader[iosel].protocol_length = IP_ADDR_SIZE;
		break;
	default:
		// �������� �ʴ� �������� Ÿ���̶�� ���ܸ� �߻���Ŵ
		throw("Protocol Type Error!");
	}
}

// ARP ����� opcode�� �����ϴ� �Լ�
void CARPLayer::setOpcode(const unsigned short opcode, int iosel) {
	// ��ȿ�� opcode ���� ����� ARP ����� opcode�� ����
	if (opcode >= ARP_OPCODE_REQUEST && opcode <= ARP_OPCODE_RREPLY) {
		m_sHeader[iosel].opercode = opcode;
	}
	else
		throw("Operator code Error!");
}

// ARP ����� ����� �ּҸ� �����ϴ� �Լ�
void CARPLayer::setSrcAddr(unsigned char enetAddr[], unsigned char ipAddr[], int iosel) {
	memcpy(m_sHeader[iosel].hardware_srcaddr, enetAddr, ENET_ADDR_SIZE);
	memcpy(m_sHeader[iosel].protocol_srcaddr, ipAddr, IP_ADDR_SIZE);
}

// ARP ����� ������ �ּҸ� �����ϴ� �Լ�
void CARPLayer::setDstAddr(unsigned char enetAddr[], unsigned char ipAddr[], int iosel) {
	memcpy(m_sHeader[iosel].hardware_dstaddr, enetAddr, ENET_ADDR_SIZE);
	memcpy(m_sHeader[iosel].protocol_dstaddr, ipAddr, IP_ADDR_SIZE);
}

// ���ڷ� ���� �ּ� swap �Լ�
void CARPLayer::swapaddr(unsigned char lAddr[], unsigned char rAddr[], unsigned char size) {
	unsigned char tempAddr[ENET_ADDR_SIZE] = { 0, };

	memcpy(tempAddr, lAddr, size);
	memcpy(lAddr, rAddr, size);
	memcpy(rAddr, tempAddr, size);
}

// ������
CARPLayer::CARPLayer(char* pName)
	: CBaseLayer(pName)
{
	// ARP ��� �ʱ�ȭ
	ResetHeader();
	// ���ο� �ܺ� ��Ʈ��ũ ARP ����� �ϵ���� Ÿ��, �������� Ÿ�� ����
	setType(ARP_ENET_TYPE, ARP_IP_TYPE, INNER);
	setType(ARP_ENET_TYPE, ARP_IP_TYPE, OUTER);
	// ip, mac �ּ� �ʱ�ȭ
	memset(myip, 0, IP_ADDR_SIZE);
	memset(mymac, 0, ENET_ADDR_SIZE);
	// GARP Ȱ��ȭ
	is_Garp = 1;
}

// �Ҹ���
CARPLayer::~CARPLayer() {

}

// �Ʒ��� ������ �����ε����� ARP ��� ���� �񱳸� ����
// IP �ּҸ� �������� ��
bool CARPLayer::_ARP_NODE::operator==(const unsigned char* ipaddr) {
	return memcmp(protocol_addr, ipaddr, IP_ADDR_SIZE) == 0;
}
bool CARPLayer::_ARP_NODE::operator==(const struct _ARP_NODE& ot) {
	return *this == ot.protocol_addr;
}
bool CARPLayer::_ARP_NODE::operator<(const unsigned char* ipaddr) {
	return memcmp(protocol_addr, ipaddr, IP_ADDR_SIZE) == -1;
}
bool CARPLayer::_ARP_NODE::operator<(const struct _ARP_NODE& ot) {
	return *this < ot.protocol_addr;
}
bool CARPLayer::_ARP_NODE::operator>(const unsigned char* ipaddr) {
	return memcmp(protocol_addr, ipaddr, IP_ADDR_SIZE) == 1;
}
bool CARPLayer::_ARP_NODE::operator>(const struct _ARP_NODE& ot) {
	return *this > ot.protocol_addr;
}

// ARP ��� �ʱ�ȭ �Լ�
CARPLayer::_ARP_HEADER::_ARP_HEADER() {
	hardware_type = protocol_type = 0x0000;
	hardware_length = protocol_length = 0x00;
	opercode = 0x0000;
	memset(hardware_srcaddr, 0x00, ENET_ADDR_SIZE);
	memset(protocol_srcaddr, 0x00, IP_ADDR_SIZE);
	memset(hardware_dstaddr, 0x00, ENET_ADDR_SIZE);
	memset(protocol_dstaddr, 0x00, IP_ADDR_SIZE);
};

// ���ڷ� ���� ARP ������� ������ �����Ͽ� ARP ��� �����ϴ� �Լ�
CARPLayer::_ARP_HEADER::_ARP_HEADER(const struct _ARP_HEADER& ot)
	:hardware_type(ot.hardware_type), protocol_type(ot.protocol_type),
	hardware_length(ot.hardware_length), protocol_length(ot.protocol_length),
	opercode(ot.opercode)
{
	memcpy(hardware_srcaddr, ot.hardware_srcaddr, ENET_ADDR_SIZE);
	memcpy(hardware_dstaddr, ot.hardware_dstaddr, ENET_ADDR_SIZE);
	memcpy(protocol_srcaddr, ot.protocol_srcaddr, IP_ADDR_SIZE);
	memcpy(protocol_dstaddr, ot.protocol_dstaddr, IP_ADDR_SIZE);
}

// �ּҸ� ���ڿ��� ��ȯ�ϴ� �Լ�
void addrToStr(unsigned short type, CString& dst, unsigned char* src) {
	switch (type) {
	// �ּ� Ÿ���� IP�� ���
	case ARP_IP_TYPE:
		// IP �ּ� ����(0.0.0.0)���� ���ڿ� ����
		dst.Format(_T("%hhu.%hhu.%hhu.%hhu"),
			src[0], src[1], src[2], src[3]);
		break;
	// �ּ� Ÿ���� MAC�� ���
	case ARP_ENET_TYPE:
		// MAC �ּ� ����(00:00 : 00 : 00 : 00 : 00)���� ���ڿ� ����
		dst.Format(_T("%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx"),
			src[0], src[1], src[2], src[3], src[4], src[5]);
		break;
	default:
		break;
	}
}

// ���ڿ��� �ּҷ� �������ϴ� �Լ�
void StrToaddr(unsigned short type, unsigned char* dst, CString& src) {
	switch (type) {

	// �ּ� Ÿ���� IP�� ���
	case ARP_IP_TYPE:
		// IP �ּ� ����(0.0.0.0)�� ���ڿ��� IP �ּҷ� ��ȯ
		swscanf_s(src, _T("%hhu.%hhu.%hhu.%hhu"),
			&dst[0], &dst[1], &dst[2], &dst[3]);
		break;
	// �ּ� Ÿ���� MAC�� ���
	case ARP_ENET_TYPE:
		// MAC �ּ� ����(00:00 : 00 : 00 : 00 : 00)�� ���ڿ��� MAC �ּҷ� ��ȯ
		swscanf_s(src, _T("%hhx:%hhx:%hhx:%hhx:%hhx:%hhx"),
			&dst[0], &dst[1], &dst[2], &dst[3], &dst[4], &dst[5]);
		break;
	default:
		break;
	}
}

// ARP cache table ������Ʈ �Լ�
void CARPLayer::updateTable() {
	CTime cur = CTime::GetCurrentTime();
	for (int i = 0; i < m_arpTable.size(); i++) {
		if (m_arpTable[i].status == ARP_TIME_OUT) continue;

		if ((cur - m_arpTable[i].spanTime) > (m_arpTable[i].status == TRUE ? 600 : 180)) {
			m_arpTable[i].status = ARP_TIME_OUT;
		}
	}
}

// ���ڷ� IP �ּҸ� ���� �׸��� ARP cache table���� �����ϴ� �Լ�
void CARPLayer::deleteItem(CString IP) {
	auto k = m_arpTable.begin();
	unsigned char addr[IP_ADDR_SIZE] = { 0, };
	StrToaddr(ARP_IP_TYPE, addr, IP);

	for (; k != m_arpTable.end(); k++) {
		if (*k == addr) {
			break;
		}
	}

	m_arpTable.erase(k);
}

// ���� ARP cache table ��ȯ �Լ�
std::vector<CARPLayer::ARP_NODE> CARPLayer::getTable() {
	return m_arpTable;
}

// ARP cache table ���� �Լ�
void CARPLayer::clearTable() {
	m_arpTable.clear();
}

// _PROXY_ARP_NODE ����ü�� ������
CARPLayer::_PROXY_ARP_NODE::_PROXY_ARP_NODE(unsigned char* srcenet, unsigned char* dstip, unsigned char* dstenet) {
	// �۽����� �ϵ���� �ּҸ� ���� ** ���� ���� ���ٸ� 0���� �ʱ�ȭ
	if (srcenet) memcpy(srchardware_addr, srcenet, ENET_ADDR_SIZE);
	else memset(srchardware_addr, 0, ENET_ADDR_SIZE);
	// �������� �������� �ּҸ� ���� ** ���� ���� ���ٸ� 0���� �ʱ�ȭ
	if (dstip) memcpy(protocol_addr, dstip, IP_ADDR_SIZE);
	else memset(protocol_addr, 0, IP_ADDR_SIZE);
	// �������� �ϵ���� �ּҸ� ���� ** ���� ���� ���ٸ� 0���� �ʱ�ȭ
	if (dstenet) memcpy(hardware_addr, dstenet, ENET_ADDR_SIZE);
	else memset(hardware_addr, 0, ENET_ADDR_SIZE);
}

// _PROXY_ARP_NODE ����ü�� ������ �����ε�
// �ٸ� proxy ARP ���� ������ ��
bool CARPLayer::_PROXY_ARP_NODE::operator==(const struct _PROXY_ARP_NODE& ot) {
	return (memcmp(ot.hardware_addr, hardware_addr, ENET_ADDR_SIZE) == 0) &&
		(memcmp(ot.srchardware_addr, srchardware_addr, ENET_ADDR_SIZE) == 0) &&
		(memcmp(ot.protocol_addr, protocol_addr, IP_ADDR_SIZE) == 0);
}

// proxy ARP ��带 �����ϴ� �Լ�
void CARPLayer::createProxy(unsigned char* src, unsigned char* ip, unsigned char* enet) {
	// proxy ARP ���̺� �� ��带 �߰�
	m_proxyTable.push_back(PROXY_ARP_NODE(src, ip, enet));
}

// proxy ARP ��带 �����ϴ� �Լ�
void CARPLayer::deleteProxy(const int index) {
	// proxy ARP ���̺��� ������ �ε����� ��带 ����
	m_proxyTable.erase(m_proxyTable.begin() + index);
}