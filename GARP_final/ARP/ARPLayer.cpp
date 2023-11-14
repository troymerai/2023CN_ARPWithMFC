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

// ARP ��� �ʱ�ȭ inline �Լ�
inline void CARPLayer::ResetHeader() {
	m_sHeader = ARP_HEADER();
}

// (����)
//ARP ��û ��Ŷ�� ������ ���� ������ IP �ּҰ� �ڽ��� IP �ּ����� Ȯ��
//�����ϴٸ�, �ڽ��� MAC �ּҸ� ARP ���� ��Ŷ�� ������ MAC �ּҷ� �����ϰ�, �׷��� �ʴٸ� ARP ���̺��� �ش� IP �ּҸ� ã�� MAC �ּҸ� ����
//�� ��, ARP ���� ��Ŷ�� �����Ͽ� ����

// (����)
// ARP ��û ��Ŷ�� ������ ���� ARP ĳ�ÿ��� �ش� �ҽ� MAC �ּҸ� ã��
// ã�´ٸ�, �ش� ĳ�� �׸��� �����ϰ�, ã�� ���ϸ� ���ο� �׸��� ĳ�ÿ� �߰�
//�� ��, ��û���� ������ IP �ּҰ� �ڽ��� IP �ּҿ� ���ٸ�, ARP ���� ��Ŷ�� �����Ͽ� ����

// ARP ���� ��Ŷ ���� �Լ�
BOOL CARPLayer::Receive(unsigned char* ppayload) {

	// ARP ��� Ÿ������ ĳ����
	PARP_HEADER arp_data = (PARP_HEADER)ppayload;

	// �̴��� ���� ��ü ����
	CEthernetLayer* m_ether = (CEthernetLayer*)mp_UnderLayer;

	// ARP ĳ�ÿ��� ��Ŷ�� �ҽ� �ϵ���� �ּҸ� ã��
	int index = inCache(arp_data->hardware_srcaddr);

	// ��Ŷ�� opcode�� ���� �б� ó��
	switch (arp_data->opercode) {

	// ARP ��û�� ���
	case ARP_OPCODE_REQUEST:
		// ĳ�ÿ� �̹� ���� �ּҰ� �ִ� ��� (GARP)
		if (index >= 0) {
			// �ڱⰡ ���� GARP�� ���ؼ��� reply�� �Ⱦ��� ���� ó��
			if (memcmp(arp_data->protocol_srcaddr, myip, IP_ADDR_SIZE) == 0) {
				// IP �浹�� �߻��� ��� GARP reply ��Ŷ �ۼ�
				if (memcmp(arp_data->protocol_dstaddr, myip, IP_ADDR_SIZE) == 0) {
					// �ڽ��� MAC �ּҸ� ��Ŷ�� ������ �ϵ���� �ּҷ� ����
					memcpy(arp_data->hardware_dstaddr, mymac, ENET_ADDR_SIZE);
					// ��Ŷ�� �۾� �ڵ带 ARP �������� ����
					arp_data->opercode = ARP_OPCODE_REPLY;
					// ��Ŷ�� �ҽ� �ּҿ� ������ �ּҸ� ��ȯ
					swapaddr(arp_data->hardware_srcaddr, arp_data->hardware_dstaddr, ENET_ADDR_SIZE);
					swapaddr(arp_data->protocol_srcaddr, arp_data->protocol_dstaddr, IP_ADDR_SIZE);

					// �̴��� ������ ������ �ּҸ� ��Ŷ�� ������ �ϵ���� �ּҷ� ����
					m_ether->SetDestinAddress(arp_data->hardware_dstaddr);
					// ��Ŷ�� ���� ����(�̴��� ����)���� ����
					mp_UnderLayer->Send((unsigned char*)arp_data, ARP_HEADER_SIZE);
				}
				break;
			}

			// ĳ���� �ش� �׸��� ����
			memcpy(m_arpTable[index].hardware_addr, arp_data->hardware_srcaddr, ENET_ADDR_SIZE);
			m_arpTable[index].status = true;
			m_arpTable[index].spanTime = CTime::GetCurrentTime();

			break;
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

			// �̴��� ������ ������ �ּҸ� ��Ŷ�� ������ �ϵ���� �ּҷ� ����
			m_ether->SetDestinAddress(arp_data->hardware_dstaddr);
			// ��Ŷ�� ���� ����(�̴��� ����)���� ����
			return mp_UnderLayer->Send((unsigned char*)arp_data, ARP_HEADER_SIZE);
		}
		break;
	// ARP ������ ���
	case ARP_OPCODE_REPLY:

		// GARP�� ���
		if (memcmp(arp_data->protocol_dstaddr, arp_data->protocol_srcaddr, IP_ADDR_SIZE) == 0) {
			// Dlg�� ����� �޽��� ������ �ڵ� �߰�
			break;
		}


		// ARP�� ���
		// ĳ�ø� ��ȸ�ϸ� ��Ŷ�� �ҽ� �������� �ּҿ� ���� �׸��� ã��
		for (auto& node : m_arpTable) {
			if (node == arp_data->protocol_srcaddr) {
				// �ش� �׸��� ����
				memcpy(node.hardware_addr, arp_data->hardware_srcaddr, ENET_ADDR_SIZE);
				node.status = true;
				node.spanTime = CTime::GetCurrentTime();
				break;
			}
		}
		break;

	// �� �� ���� �۾� �ڵ��� ���
	default:
		throw("unknown Opcode Error");
		return false;
	}

	// ��Ŷ ���� ������ ��ȯ
	return true;
}

// ARP ���� ��Ŷ ���� �Լ�
BOOL CARPLayer::Send(unsigned char* ppayload, int nlength) {

	// ���޹��� payload�� IP��� Ÿ������ ĳ�����Ͽ� ip data Ȯ��
	PIP_HEADER ip_data = (PIP_HEADER)ppayload;

	// �̴��� ���̾� ����
	CEthernetLayer* m_ether = (CEthernetLayer*)mp_UnderLayer;

	// broadcast �ּ� ����
	unsigned char broadcastAddr[ENET_ADDR_SIZE];
	memset(broadcastAddr, 255, ENET_ADDR_SIZE);
	

	// ���ο� ARP ��� ����
	ARP_NODE newNode(ip_data->dstaddr, broadcastAddr);
	m_ether->SetDestinAddress(broadcastAddr);

	// opcode�� ARP request(0x0100)�� ����
	setOpcode(ARP_OPCODE_REQUEST);

	if (memcmp(ip_data->srcaddr, ip_data->dstaddr, IP_ADDR_SIZE) != 0) {
		int idx = inCache(ip_data->dstaddr);
		// �־��� �ּҰ� ARP cache table�� �ִ� �� Ȯ��
		if (idx != -1) {

			// �ּҰ� ĳ�ÿ� �ְ� ���°� FALSE�� ��� 
			if (m_arpTable[idx].status == FALSE) {

				// �޽����� ����ϰ� true�� ��ȯ
				AfxMessageBox(_T("Already In Cache!"));
				return true;
			}
			// �ּҰ� ĳ�ÿ� ������ ���°� TRUE�� ��� 
			else {
				// ĳ�ø� �� ���� ������Ʈ
				m_arpTable[idx] = newNode;
			}
		}
		// �ּҰ� ĳ�ÿ� ���� ���
		else {
			// �� ��带 ĳ�ÿ� �߰�
			m_arpTable.push_back(newNode);
		}
	}

	// ����� �ּҿ� ������ �ּ� ����
	setSrcAddr(m_ether->GetSourceAddress(), ip_data->srcaddr);
	setDstAddr(broadcastAddr, ip_data->dstaddr);


	// ��Ŷ�� ���� ���̾�(���⼭�� Ethernet Layer)�� ����
	return ((CEthernetLayer*)mp_UnderLayer)->Send((unsigned char*)&m_sHeader, ARP_HEADER_SIZE);

	// ��Ŷ ���� ������ ��ȯ
	return true;
}

// IP �ּҰ� ARP cache table�� �ִ� �� Ȯ���ϴ� �Լ�
int CARPLayer::inCache(const unsigned char* ipaddr) {
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
void CARPLayer::setmyAddr(CString MAC, CString IP) {
	StrToaddr(ARP_IP_TYPE, myip, IP);
	StrToaddr(ARP_ENET_TYPE, mymac, MAC);
}

// ARP ����� �ϵ���� Ÿ�԰� �������� Ÿ���� �����ϴ� �Լ�
void CARPLayer::setType(const unsigned short htype, const unsigned short ptype) {

	//ARP ��Ŷ���� ���Ǵ� �ϵ���� �� �������� ���� ����
	m_sHeader.hardware_type = htype;
	m_sHeader.protocol_type = ptype;


	// �ϵ���� Ÿ�Կ� ���� �ϵ���� �ּ��� ���̸� ����
	switch (m_sHeader.hardware_type) {
	case ARP_ENET_TYPE:
		m_sHeader.hardware_length = ENET_ADDR_SIZE;
		break;
	default:
		throw("Hardware Type Error!");
	}

	// �������� Ÿ�Կ� ���� �������� �ּ��� ���̸� ����
	switch (m_sHeader.protocol_type) {
	case ARP_IP_TYPE:
		m_sHeader.protocol_length = IP_ADDR_SIZE;
		break;
	default:
		throw("Protocol Type Error!");
	}
}

// ARP ����� opcode�� �����ϴ� �Լ�
void CARPLayer::setOpcode(const unsigned short opcode) {

	// ��ȿ�� opcode ���� ����� ARP ����� opcode�� ����
	if (opcode >= ARP_OPCODE_REQUEST && opcode <= ARP_OPCODE_RREPLY) {
		m_sHeader.opercode = opcode;
	}
	else
		throw("Operator code Error!");
}


// ARP ����� ����� �ּҸ� �����ϴ� �Լ�
void CARPLayer::setSrcAddr(const unsigned char enetAddr[], const unsigned char ipAddr[]) {
	memcpy(m_sHeader.hardware_srcaddr, enetAddr, ENET_ADDR_SIZE);
	memcpy(m_sHeader.protocol_srcaddr, ipAddr, IP_ADDR_SIZE);
}

// ARP ����� ������ �ּҸ� �����ϴ� �Լ�
void CARPLayer::setDstAddr(const unsigned char enetAddr[], const unsigned char ipAddr[]) {
	memcpy(m_sHeader.hardware_dstaddr, enetAddr, ENET_ADDR_SIZE);
	memcpy(m_sHeader.protocol_dstaddr, ipAddr, IP_ADDR_SIZE);
}

// ���ڷ� ���� �ּ� swap �Լ�
void CARPLayer::swapaddr(unsigned char lAddr[], unsigned char rAddr[], const unsigned char size) {
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
	// ARP ����� �ϵ���� Ÿ��, �������� Ÿ�� ����
	setType(ARP_ENET_TYPE, ARP_IP_TYPE);
	// ip, mac �ּ� �ʱ�ȭ
	memset(myip, 0, IP_ADDR_SIZE);
	memset(mymac, 0, ENET_ADDR_SIZE);
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