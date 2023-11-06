#include "pch.h"
#include "ARPLayer.h"

CARPLayer::_ARP_NODE::_ARP_NODE(unsigned char* cipaddr, unsigned char* cenetaddr, unsigned char bincomplete = false) {// IP 주소와 MAC 주소를 받아서 ARP 노드를 생성
	memcpy(prot_addr, cipaddr, IP_ADDR_SIZE);	//IP(프로토콜)주소 저장
	memcpy(hard_addr, cenetaddr, MAC_ADDR_SIZE);	//맥(하드웨어)주소 저장
	status = bincomplete;
	spanTime = CTime::GetCurrentTime();	//ARP 테이블에 항목을 추가한 시간 저장
}
CARPLayer::_ARP_NODE::_ARP_NODE(unsigned int ipaddr = 0, unsigned int enetaddr = 0, unsigned char incomplete = false) {// IP 주소와 MAC 주소를 0으로 초기화한 ARP 노드를 생성
	memset(prot_addr, ipaddr, IP_ADDR_SIZE);	//IP 주소 초기화
	memset(hard_addr, enetaddr, MAC_ADDR_SIZE);	//MAC 주소 초기화
	status = incomplete;
	spanTime = CTime::GetCurrentTime();	//ARP 테이블에 항목을 추가한 시간 저장
}
CARPLayer::_ARP_NODE::_ARP_NODE(const struct _ARP_NODE& ot) {	 // 다른 ARP 노드 객체로부터 정보를 복사하여 ARP 노드를 생성
	memcpy(prot_addr, ot.prot_addr, IP_ADDR_SIZE);	// IP 주소 복사
	memcpy(hard_addr, ot.hard_addr, MAC_ADDR_SIZE);	//MAC 주소 복사
	status = ot.status;
	spanTime = ot.spanTime;	//ARP 테이블에 항목을 추가한 시간 복사
}

inline void CARPLayer::ResetHeader() {
	m_sHeader = ARP_HEADER();
}

BOOL CARPLayer::Receive(unsigned char* ppayload) {	//ARP 패킷을 수신하는 함수.							
	PARP_HEADER arp_data = (PARP_HEADER)ppayload;

	switch (arp_data->op) {
	case ARP_OP_REQUEST:	//ARP Request 패킷을 받으면 ARP Reply 패킷을 생성하여 응답
		if (memcmp(arp_data->prot_dstaddr, myip, IP_ADDR_SIZE) == 0)
			memcpy(arp_data->hard_dstaddr, mymac, MAC_ADDR_SIZE);
		else
			for (auto& node : m_arpTable) {
				if (node == arp_data->prot_dstaddr) {
					memcpy(arp_data->hard_dstaddr, node.hard_addr, MAC_ADDR_SIZE);
					node.spanTime = CTime::GetCurrentTime();
					break;
				}
			}
		arp_data->op = ARP_OP_REPLY;
		swapaddr(arp_data->hard_srcaddr, arp_data->hard_dstaddr, MAC_ADDR_SIZE);
		swapaddr(arp_data->prot_srcaddr, arp_data->prot_dstaddr, IP_ADDR_SIZE);

		return mp_UnderLayer->Send((unsigned char*)arp_data, ARP_HEADER_SIZE);
		break;
	case ARP_OP_REPLY:	//ARP Reply 패킷을 받으면 ARP 테이블 업데이트
		for (auto& node : m_arpTable) {
			if (node == arp_data->prot_srcaddr) {
				memcpy(node.hard_addr, arp_data->hard_srcaddr, MAC_ADDR_SIZE);
				node.status = true;
				node.spanTime = CTime::GetCurrentTime();
				break;
			}
		}
		break;
	case ARP_OP_RREQUEST:
		break;
	case ARP_OP_RREPLY:
		break;
	default:
		throw("unknown Opcode Error");
		return false;
	}

	return true;
}

BOOL CARPLayer::Send(unsigned char* ppayload, int nlength) {
	PIP_HEADER ip_data = (PIP_HEADER)ppayload;
	CEthernetLayer* m_ether = (CEthernetLayer*)mp_UnderLayer;
	unsigned char broadcastAddr[MAC_ADDR_SIZE];
	memset(broadcastAddr, 255, MAC_ADDR_SIZE);
	m_ether->SetDestinAddress(broadcastAddr);
	ARP_NODE newNode(ip_data->dstaddr, broadcastAddr);

	setOpcode(ARP_OP_REQUEST);

	//check given address is in arp cache table
	int idx = inCache(ip_data->dstaddr);
	if (idx != -1) {
		if (m_arpTable[idx].status == FALSE) {
			AfxMessageBox(_T("Already In Cache!"));
			return true;
		}
		else {
			m_arpTable[idx] = newNode;
		}
	}
	else {
		m_arpTable.push_back(newNode);
	}
	setSrcAddr(m_ether->GetSourceAddress(), ip_data->srcaddr);
	setDstAddr(broadcastAddr, ip_data->dstaddr);

	return ((CEthernetLayer*)mp_UnderLayer)->Send((unsigned char*)&m_sHeader, ARP_HEADER_SIZE);

	return true;
}

int CARPLayer::inCache(const unsigned char* ipaddr) {	//주어진 IP 주소가 ARP 테이블에 캐시되어 있는지 확인
	int res = -1;
	for (int i = 0; i < m_arpTable.size(); i++) {
		if (m_arpTable[i] == ipaddr) {
			res = i;
			break;
		}
	}
	return res;
}

void CARPLayer::setmyAddr(CString MAC, CString IP) {	//나의 맥주소, IP 주소를 설정하는 함수
	StrToaddr(ARP_IP_TYPE, myip, IP);
	StrToaddr(ARP_MAC_TYPE, mymac, MAC);
}

void CARPLayer::setType(const unsigned short htype, const unsigned short ptype) {	//ARP 패킷에서 사용되는 하드웨어 및 프로토콜 유형 설정
	m_sHeader.hard_type = htype;
	m_sHeader.prot_type = ptype;

	switch (m_sHeader.hard_type) {
	case ARP_MAC_TYPE:
		m_sHeader.hard_length = MAC_ADDR_SIZE;
		break;
	default:
		throw("Hardware Type Error!");
	}

	switch (m_sHeader.prot_type) {
	case ARP_IP_TYPE:
		m_sHeader.prot_length = IP_ADDR_SIZE;
		break;
	default:
		throw("Protocol Type Error!");
	}
}

void CARPLayer::setOpcode(const unsigned short opcode) {
	if (opcode >= ARP_OP_REQUEST && opcode <= ARP_OP_RREPLY) {
		m_sHeader.op = opcode;
	}
	else
		throw("Operator code Error!");
}

void CARPLayer::setSrcAddr(const unsigned char enetAddr[], const unsigned char ipAddr[]) {
	memcpy(m_sHeader.hard_srcaddr, enetAddr, MAC_ADDR_SIZE);
	memcpy(m_sHeader.prot_srcaddr, ipAddr, IP_ADDR_SIZE);
}
void CARPLayer::setDstAddr(const unsigned char enetAddr[], const unsigned char ipAddr[]) {
	memcpy(m_sHeader.hard_dstaddr, enetAddr, MAC_ADDR_SIZE);
	memcpy(m_sHeader.prot_dstaddr, ipAddr, IP_ADDR_SIZE);
}

void CARPLayer::swapaddr(unsigned char lAddr[], unsigned char rAddr[], const unsigned char size) {
	unsigned char tempAddr[MAC_ADDR_SIZE] = { 0, };

	memcpy(tempAddr, lAddr, size);
	memcpy(lAddr, rAddr, size);
	memcpy(rAddr, tempAddr, size);
}

CARPLayer::CARPLayer(char* pName)
	: CBaseLayer(pName)
{
	ResetHeader();
	setType(ARP_MAC_TYPE, ARP_IP_TYPE);
	memset(myip, 0, IP_ADDR_SIZE);
	memset(mymac, 0, MAC_ADDR_SIZE);
}

CARPLayer::~CARPLayer() {

}

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

CARPLayer::_ARP_HEADER::_ARP_HEADER() {
	hard_type = prot_type = 0x0000;
	hard_length = prot_length = 0x00;
	op = 0x0000;
	memset(hard_srcaddr, 0x00, MAC_ADDR_SIZE);
	memset(prot_srcaddr, 0x00, IP_ADDR_SIZE);
	memset(hard_dstaddr, 0x00, MAC_ADDR_SIZE);
	memset(prot_dstaddr, 0x00, IP_ADDR_SIZE);
};

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

void addrToStr(unsigned short type, CString& dst, unsigned char* src) {
	switch (type) {
	case ARP_IP_TYPE:
		dst.Format(_T("%hhu.%hhu.%hhu.%hhu"),
			src[0], src[1], src[2], src[3]);
		break;
	case ARP_MAC_TYPE:
		dst.Format(_T("%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx"),
			src[0], src[1], src[2], src[3], src[4], src[5]);
		break;
	default:
		break;
	}
}
void StrToaddr(unsigned short type, unsigned char* dst, CString& src) {
	switch (type) {
	case ARP_IP_TYPE:
		swscanf_s(src, _T("%hhu.%hhu.%hhu.%hhu"),
			&dst[0], &dst[1], &dst[2], &dst[3]);
		break;
	case ARP_MAC_TYPE:
		swscanf_s(src, _T("%hhx:%hhx:%hhx:%hhx:%hhx:%hhx"),
			&dst[0], &dst[1], &dst[2], &dst[3], &dst[4], &dst[5]);
		break;
	default:
		break;
	}
}

void CARPLayer::updateTable() {
	CTime cur = CTime::GetCurrentTime();
	for (int i = 0; i < m_arpTable.size(); i++) {
		if (m_arpTable[i].status == ARP_TIME_OUT) continue;

		if ((cur - m_arpTable[i].spanTime) > (m_arpTable[i].status == TRUE ? 600 : 180)) {
			m_arpTable[i].status = ARP_TIME_OUT;
		}
	}
}

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

std::vector<CARPLayer::ARP_NODE> CARPLayer::getTable() {
	return m_arpTable;
}

void CARPLayer::clearTable() {
	m_arpTable.clear();
}