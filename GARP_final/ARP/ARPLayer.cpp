#include "pch.h"
#include "ARPLayer.h"


// IP �ּҿ� MAC �ּҸ� �޾Ƽ� ARP ��带 ���� 
CARPLayer::_ARP_NODE::_ARP_NODE(unsigned char* cipaddr, unsigned char* cenetaddr, unsigned char bincomplete = false) {
	// IP(��������)�ּ� ����
	memcpy(prot_addr, cipaddr, IP_ADDR_SIZE);	
	// MAC(�ϵ����)�ּ� ����
	memcpy(hard_addr, cenetaddr, MAC_ADDR_SIZE);	
	// status�� incomplete�� ����
	status = bincomplete;
	// ARP ���̺� �׸��� �߰��� �ð� ����
	spanTime = CTime::GetCurrentTime();	
}

// IP �ּҿ� MAC �ּҸ� 0���� �ʱ�ȭ�� ARP ��带 ����
CARPLayer::_ARP_NODE::_ARP_NODE(unsigned int ipaddr = 0, unsigned int enetaddr = 0, unsigned char incomplete = false) {
	// IP �ּ� �ʱ�ȭ
	memset(prot_addr, ipaddr, IP_ADDR_SIZE);
	// MAC �ּ� �ʱ�ȭ
	memset(hard_addr, enetaddr, MAC_ADDR_SIZE);	
	// status�� incomplete�� �ʱ�ȭ
	status = incomplete;
	//ARP ���̺� �׸��� �߰��� �ð� ����
	spanTime = CTime::GetCurrentTime();
}

// �ٸ� ARP ��� ��ü�κ��� ������ �����Ͽ� ARP ��带 ����
CARPLayer::_ARP_NODE::_ARP_NODE(const struct _ARP_NODE& ot) {
	// IP �ּ� ����
	memcpy(prot_addr, ot.prot_addr, IP_ADDR_SIZE);	
	//MAC �ּ� ����
	memcpy(hard_addr, ot.hard_addr, MAC_ADDR_SIZE);	
	// ���� ����
	status = ot.status;
	//ARP ���̺� �׸��� �߰��� �ð� ����
	spanTime = ot.spanTime;	
}


// ARP ��� �ʱ�ȭ inline �Լ�
inline void CARPLayer::ResetHeader() {
	m_sHeader = ARP_HEADER();
}


// ARP ���� ��Ŷ ���� �Լ�
BOOL CARPLayer::Receive(unsigned char* ppayload) {
	PARP_HEADER arp_data = (PARP_HEADER)ppayload;

	// ARP Request ��Ŷ�� ������ ARP Reply ��Ŷ�� �����Ͽ� ����
	switch (arp_data->op) {
	case ARP_OP_REQUEST:
		// ARP ���̺��� �ش� IP �ּҸ� ã�� MAC �ּҸ� ������ MAC �ּҷ� ����
		for (auto& node : m_arpTable) {
			if (node == arp_data->prot_dstaddr) {
				memcpy(arp_data->hard_dstaddr, node.hard_addr, MAC_ADDR_SIZE);
				node.spanTime = CTime::GetCurrentTime();
				break;
			}
		}
		// opcode�� ARP Reply(0x0200)�� ����
		arp_data->op = ARP_OP_REPLY;
		// ����� �ּҿ� ������ �ּҸ� swap
		swapaddr(arp_data->hard_srcaddr, arp_data->hard_dstaddr, MAC_ADDR_SIZE);
		swapaddr(arp_data->prot_srcaddr, arp_data->prot_dstaddr, IP_ADDR_SIZE);

		// ARP Reply ��Ŷ�� ���� ���̾�(Ethernet Layer)�� ����
		return mp_UnderLayer->Send((unsigned char*)arp_data, ARP_HEADER_SIZE);
		break;
		//ARP Reply ��Ŷ�� ������ ARP ���̺� ������Ʈ
	case ARP_OP_REPLY:
		for (auto& node : m_arpTable) {
			if (node == arp_data->prot_srcaddr) {
				memcpy(node.hard_addr, arp_data->hard_srcaddr, MAC_ADDR_SIZE);
				node.status = true;
				node.spanTime = CTime::GetCurrentTime();
				break;
			}
		}
		break;
		// ReRequest ���� ���
	case ARP_OP_RREQUEST:
		// ���� ���� ����
		break;
		// ReReply ���� ���
	case ARP_OP_RREPLY:
		// ���� ���� ����
		break;
	default:
		// �� �� ���� opcode�� ���� ��� ���� �߻�
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
	unsigned char broadcastAddr[MAC_ADDR_SIZE];
	memset(broadcastAddr, 255, MAC_ADDR_SIZE);
	m_ether->SetDestinAddress(broadcastAddr);

	// ���ο� ARP ��� ����
	ARP_NODE newNode(ip_data->dstaddr, broadcastAddr);

	// opcode�� ARP request(0x0100)�� ����
	setOpcode(ARP_OP_REQUEST);

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

	// ����� �ּҿ� ������ �ּ� ����
	setSrcAddr(m_ether->GetSourceAddress(), ip_data->srcaddr);
	
	// 
	// ���⼭ request���� ARP ����� dest IP�� braodcast�� ä��? -> ����� �غ���
	// ����� ��� �� �ٽ� �����ϱ�
	// 
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
	StrToaddr(ARP_MAC_TYPE, mymac, MAC);
}

// ARP ����� �ϵ���� Ÿ�԰� �������� Ÿ���� �����ϴ� �Լ�
void CARPLayer::setType(const unsigned short htype, const unsigned short ptype) {	

	//ARP ��Ŷ���� ���Ǵ� �ϵ���� �� �������� ���� ����
	m_sHeader.hard_type = htype;
	m_sHeader.prot_type = ptype;

	// �ϵ���� Ÿ�Կ� ���� �ϵ���� �ּ��� ���̸� ����
	switch (m_sHeader.hard_type) {
	case ARP_MAC_TYPE:
		m_sHeader.hard_length = MAC_ADDR_SIZE;
		break;
	default:
		throw("Hardware Type Error!");
	}

	// �������� Ÿ�Կ� ���� �������� �ּ��� ���̸� ����
	switch (m_sHeader.prot_type) {
	case ARP_IP_TYPE:
		m_sHeader.prot_length = IP_ADDR_SIZE;
		break;
	default:
		throw("Protocol Type Error!");
	}
}

// ARP ����� opcode�� �����ϴ� �Լ�
void CARPLayer::setOpcode(const unsigned short opcode) {
	// ��ȿ�� opcode ���� ����� ARP ����� opcode�� ����
	if (opcode >= ARP_OP_REQUEST && opcode <= ARP_OP_RREPLY) {
		m_sHeader.op = opcode;
	}
	else
		throw("Operator code Error!");
}

// ARP ����� ����� �ּҸ� �����ϴ� �Լ�
void CARPLayer::setSrcAddr(const unsigned char enetAddr[], const unsigned char ipAddr[]) {
	memcpy(m_sHeader.hard_srcaddr, enetAddr, MAC_ADDR_SIZE);
	memcpy(m_sHeader.prot_srcaddr, ipAddr, IP_ADDR_SIZE);
}

// ARP ����� ������ �ּҸ� �����ϴ� �Լ�
void CARPLayer::setDstAddr(const unsigned char enetAddr[], const unsigned char ipAddr[]) {
	memcpy(m_sHeader.hard_dstaddr, enetAddr, MAC_ADDR_SIZE);
	memcpy(m_sHeader.prot_dstaddr, ipAddr, IP_ADDR_SIZE);
}

// ���ڷ� ���� �ּ� swap �Լ�
void CARPLayer::swapaddr(unsigned char lAddr[], unsigned char rAddr[], const unsigned char size) {
	unsigned char tempAddr[MAC_ADDR_SIZE] = { 0, };

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
	setType(ARP_MAC_TYPE, ARP_IP_TYPE);
	// ip, mac �ּ� �ʱ�ȭ
	memset(myip, 0, IP_ADDR_SIZE);
	memset(mymac, 0, MAC_ADDR_SIZE);
}

// �Ҹ���
CARPLayer::~CARPLayer() {

}

// �Ʒ��� ������ �����ε����� ARP ��� ���� �񱳸� ����
// IP �ּҸ� �������� ��
bool CARPLayer::_ARP_NODE::operator==(const unsigned char* ipaddr) {
	return memcmp(prot_addr, ipaddr, IP_ADDR_SIZE) == 0;
}
bool CARPLayer::_ARP_NODE::operator==(const struct _ARP_NODE& ot) {
	return *this == ot.prot_addr;
}
bool CARPLayer::_ARP_NODE::operator<(const unsigned char* ipaddr) {
	return memcmp(prot_addr, ipaddr, IP_ADDR_SIZE) == -1;
}
bool CARPLayer::_ARP_NODE::operator<(const struct _ARP_NODE& ot) {
	return *this < ot.prot_addr;
}
bool CARPLayer::_ARP_NODE::operator>(const unsigned char* ipaddr) {
	return memcmp(prot_addr, ipaddr, IP_ADDR_SIZE) == 1;
}
bool CARPLayer::_ARP_NODE::operator>(const struct _ARP_NODE& ot) {
	return *this > ot.prot_addr;
}

// ARP ��� �ʱ�ȭ
CARPLayer::_ARP_HEADER::_ARP_HEADER() {
	hard_type = prot_type = 0x0000;
	hard_length = prot_length = 0x00;
	op = 0x0000;
	memset(hard_srcaddr, 0x00, MAC_ADDR_SIZE);
	memset(prot_srcaddr, 0x00, IP_ADDR_SIZE);
	memset(hard_dstaddr, 0x00, MAC_ADDR_SIZE);
	memset(prot_dstaddr, 0x00, IP_ADDR_SIZE);
};

// ���ڷ� ���� ARP ������� ������ �����Ͽ� ARP ��� ���� 
CARPLayer::_ARP_HEADER::_ARP_HEADER(const struct _ARP_HEADER& ot)
	:hard_type(ot.hard_type), prot_type(ot.prot_type),
	hard_length(ot.hard_length), prot_length(ot.prot_length),
	op(ot.op)
{
	memcpy(hard_srcaddr, ot.hard_srcaddr, MAC_ADDR_SIZE);
	memcpy(hard_dstaddr, ot.hard_dstaddr, MAC_ADDR_SIZE);
	memcpy(prot_srcaddr, ot.prot_srcaddr, IP_ADDR_SIZE);
	memcpy(prot_dstaddr, ot.prot_dstaddr, IP_ADDR_SIZE);
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
	case ARP_MAC_TYPE:
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
	case ARP_MAC_TYPE:
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