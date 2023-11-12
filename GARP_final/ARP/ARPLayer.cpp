#include "pch.h"
#include "ARPLayer.h"


// IP 주소와 MAC 주소를 받아서 ARP 노드를 생성 
CARPLayer::_ARP_NODE::_ARP_NODE(unsigned char* cipaddr, unsigned char* cenetaddr, unsigned char bincomplete = false) {
	// IP(프로토콜)주소 저장
	memcpy(prot_addr, cipaddr, IP_ADDR_SIZE);	
	// MAC(하드웨어)주소 저장
	memcpy(hard_addr, cenetaddr, MAC_ADDR_SIZE);	
	// status는 incomplete로 저장
	status = bincomplete;
	// ARP 테이블에 항목을 추가한 시간 저장
	spanTime = CTime::GetCurrentTime();	
}

// IP 주소와 MAC 주소를 0으로 초기화한 ARP 노드를 생성
CARPLayer::_ARP_NODE::_ARP_NODE(unsigned int ipaddr = 0, unsigned int enetaddr = 0, unsigned char incomplete = false) {
	// IP 주소 초기화
	memset(prot_addr, ipaddr, IP_ADDR_SIZE);
	// MAC 주소 초기화
	memset(hard_addr, enetaddr, MAC_ADDR_SIZE);	
	// status는 incomplete로 초기화
	status = incomplete;
	//ARP 테이블에 항목을 추가한 시간 저장
	spanTime = CTime::GetCurrentTime();
}

// 다른 ARP 노드 객체로부터 정보를 복사하여 ARP 노드를 생성
CARPLayer::_ARP_NODE::_ARP_NODE(const struct _ARP_NODE& ot) {
	// IP 주소 복사
	memcpy(prot_addr, ot.prot_addr, IP_ADDR_SIZE);	
	//MAC 주소 복사
	memcpy(hard_addr, ot.hard_addr, MAC_ADDR_SIZE);	
	// 상태 복사
	status = ot.status;
	//ARP 테이블에 항목을 추가한 시간 복사
	spanTime = ot.spanTime;	
}


// ARP 헤더 초기화 inline 함수
inline void CARPLayer::ResetHeader() {
	m_sHeader = ARP_HEADER();
}


// ARP 계층 패킷 수신 함수
BOOL CARPLayer::Receive(unsigned char* ppayload) {
	PARP_HEADER arp_data = (PARP_HEADER)ppayload;

	// ARP Request 패킷을 받으면 ARP Reply 패킷을 생성하여 응답
	switch (arp_data->op) {
	case ARP_OP_REQUEST:
		// ARP 테이블에서 해당 IP 주소를 찾아 MAC 주소를 목적지 MAC 주소로 설정
		for (auto& node : m_arpTable) {
			if (node == arp_data->prot_dstaddr) {
				memcpy(arp_data->hard_dstaddr, node.hard_addr, MAC_ADDR_SIZE);
				node.spanTime = CTime::GetCurrentTime();
				break;
			}
		}
		// opcode를 ARP Reply(0x0200)로 설정
		arp_data->op = ARP_OP_REPLY;
		// 출발지 주소와 목적지 주소를 swap
		swapaddr(arp_data->hard_srcaddr, arp_data->hard_dstaddr, MAC_ADDR_SIZE);
		swapaddr(arp_data->prot_srcaddr, arp_data->prot_dstaddr, IP_ADDR_SIZE);

		// ARP Reply 패킷을 하위 레이어(Ethernet Layer)로 전송
		return mp_UnderLayer->Send((unsigned char*)arp_data, ARP_HEADER_SIZE);
		break;
		//ARP Reply 패킷을 받으면 ARP 테이블 업데이트
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
		// ReRequest 받은 경우
	case ARP_OP_RREQUEST:
		// 아직 구현 안함
		break;
		// ReReply 받은 경우
	case ARP_OP_RREPLY:
		// 아직 구현 안함
		break;
	default:
		// 알 수 없는 opcode가 들어온 경우 에러 발생
		throw("unknown Opcode Error");
		return false;
	}

	// 패킷 수신 성공값 반환
	return true;
}


// ARP 계층 ARP 패킷 전송 함수
BOOL CARPLayer::SendARP(unsigned char* ppayload, int nlength) {

	// 전달받은 payload를 IP헤더 타입으로 캐스팅하여 ip data 확보
	PIP_HEADER ip_data = (PIP_HEADER)ppayload;

	// 이더넷 레이어 참조
	CEthernetLayer* m_ether = (CEthernetLayer*)mp_UnderLayer;

	// broadcast 주소 설정
	unsigned char broadcastAddr[MAC_ADDR_SIZE];
	memset(broadcastAddr, 255, MAC_ADDR_SIZE);
	m_ether->SetDestinAddress(broadcastAddr);

	// 새로운 ARP 노드 생성
	ARP_NODE newNode(ip_data->dstaddr, broadcastAddr);

	// opcode를 ARP request(0x0100)로 설정
	setOpcode(ARP_OP_REQUEST);

	int idx = inCache(ip_data->dstaddr);
	// 주어진 주소가 ARP cache table에 있는 지 확인
	if (idx != -1) {

		// 주소가 캐시에 있고 상태가 FALSE인 경우 
		if (m_arpTable[idx].status == FALSE) {

			// 메시지를 출력하고 true를 반환
			AfxMessageBox(_T("Already In Cache!"));
			return true;
		}

		// 주소가 캐시에 있지만 상태가 TRUE인 경우 
		else {
			// 캐시를 새 노드로 업데이트
			m_arpTable[idx] = newNode;
		}
	}
	// 주소가 캐시에 없는 경우
	else {
		// 새 노드를 캐시에 추가
		m_arpTable.push_back(newNode);
	}

	// 출발지 주소와 목적지 주소 설정
	setSrcAddr(m_ether->GetSourceAddress(), ip_data->srcaddr);
	
	// 
	// 여기서 request에서 ARP 헤더의 dest IP를 braodcast로 채움? -> 디버깅 해보기
	// 디버깅 찍는 거 다시 공부하기
	// 
	setDstAddr(broadcastAddr, ip_data->dstaddr);

	// 패킷을 하위 레이어(여기서는 Ethernet Layer)로 전송
	return ((CEthernetLayer*)mp_UnderLayer)->Send((unsigned char*)&m_sHeader, ARP_HEADER_SIZE);

	// 패킷 전송 성공값 반환
	return true;
}

////////////////// ARP 패킷이랑 GARP 패킷이랑 어떻게 구분하지..?

// ARP 계층 GARP 패킷 전송 함수
BOOL CARPLayer::SendGARP(unsigned char* ppayload, int nlength) {

	// 전달받은 payload를 IP헤더 타입으로 캐스팅하여 ip data 확보
	PIP_HEADER ip_data = (PIP_HEADER)ppayload;

	// 이더넷 레이어 참조
	CEthernetLayer* m_ether = (CEthernetLayer*)mp_UnderLayer;

	// 새로운 ARP 노드 생성
	ARP_NODE newNode(ip_data->dstaddr, m_ether->GetSourceAddress());

	// opcode를 ARP request(0x0100)로 설정
	setOpcode(ARP_OP_REQUEST);

	// 출발지 주소와 목적지 주소 설정
	setSrcAddr(m_ether->GetSourceAddress(), ip_data->srcaddr);

	// GARP에서는 목적지 IP와 목적지 MAC이 자신의 IP와 MAC이므로 자신의 주소로 설정
	setDstAddr(m_ether->GetSourceAddress(), ip_data->srcaddr);

	// 패킷을 하위 레이어(여기서는 Ethernet Layer)로 전송
	return ((CEthernetLayer*)mp_UnderLayer)->Send((unsigned char*)&m_sHeader, ARP_HEADER_SIZE);

	// 패킷 전송 성공값 반환
	return true;
}



// IP 주소가 ARP cache table에 있는 지 확인하는 함수
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

// ARP Layer의 MAC, IP 주소 설정 함수
void CARPLayer::setmyAddr(CString MAC, CString IP) {
	StrToaddr(ARP_IP_TYPE, myip, IP);
	StrToaddr(ARP_MAC_TYPE, mymac, MAC);
}

// ARP 헤더의 하드웨어 타입과 프로토콜 타입을 설정하는 함수
void CARPLayer::setType(const unsigned short htype, const unsigned short ptype) {	

	//ARP 패킷에서 사용되는 하드웨어 및 프로토콜 유형 설정
	m_sHeader.hard_type = htype;
	m_sHeader.prot_type = ptype;

	// 하드웨어 타입에 따라 하드웨어 주소의 길이를 설정
	switch (m_sHeader.hard_type) {
	case ARP_MAC_TYPE:
		m_sHeader.hard_length = MAC_ADDR_SIZE;
		break;
	default:
		throw("Hardware Type Error!");
	}

	// 프로토콜 타입에 따라 프로토콜 주소의 길이를 설정
	switch (m_sHeader.prot_type) {
	case ARP_IP_TYPE:
		m_sHeader.prot_length = IP_ADDR_SIZE;
		break;
	default:
		throw("Protocol Type Error!");
	}
}

// ARP 헤더의 opcode를 설정하는 함수
void CARPLayer::setOpcode(const unsigned short opcode) {
	// 유효한 opcode 범위 내라면 ARP 헤더의 opcode를 설정
	if (opcode >= ARP_OP_REQUEST && opcode <= ARP_OP_RREPLY) {
		m_sHeader.op = opcode;
	}
	else
		throw("Operator code Error!");
}

// ARP 헤더의 출발지 주소를 설정하는 함수
void CARPLayer::setSrcAddr(const unsigned char enetAddr[], const unsigned char ipAddr[]) {
	memcpy(m_sHeader.hard_srcaddr, enetAddr, MAC_ADDR_SIZE);
	memcpy(m_sHeader.prot_srcaddr, ipAddr, IP_ADDR_SIZE);
}

// ARP 헤더의 목적지 주소를 설정하는 함수
void CARPLayer::setDstAddr(const unsigned char enetAddr[], const unsigned char ipAddr[]) {
	memcpy(m_sHeader.hard_dstaddr, enetAddr, MAC_ADDR_SIZE);
	memcpy(m_sHeader.prot_dstaddr, ipAddr, IP_ADDR_SIZE);
}

// 인자로 받은 주소 swap 함수
void CARPLayer::swapaddr(unsigned char lAddr[], unsigned char rAddr[], const unsigned char size) {
	unsigned char tempAddr[MAC_ADDR_SIZE] = { 0, };

	memcpy(tempAddr, lAddr, size);
	memcpy(lAddr, rAddr, size);
	memcpy(rAddr, tempAddr, size);
}

// 생성자
CARPLayer::CARPLayer(char* pName)
	: CBaseLayer(pName)
{
	// ARP 헤더 초기화
	ResetHeader();
	// ARP 헤더의 하드웨어 타입, 프로토콜 타입 설정
	setType(ARP_MAC_TYPE, ARP_IP_TYPE);
	// ip, mac 주소 초기화
	memset(myip, 0, IP_ADDR_SIZE);
	memset(mymac, 0, MAC_ADDR_SIZE);
}

// 소멸자
CARPLayer::~CARPLayer() {

}

// 아래의 연산자 오버로딩들은 ARP 노드 간의 비교를 수행
// IP 주소를 기준으로 비교
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

// ARP 헤더 초기화
CARPLayer::_ARP_HEADER::_ARP_HEADER() {
	hard_type = prot_type = 0x0000;
	hard_length = prot_length = 0x00;
	op = 0x0000;
	memset(hard_srcaddr, 0x00, MAC_ADDR_SIZE);
	memset(prot_srcaddr, 0x00, IP_ADDR_SIZE);
	memset(hard_dstaddr, 0x00, MAC_ADDR_SIZE);
	memset(prot_dstaddr, 0x00, IP_ADDR_SIZE);
};

// 인자로 받은 ARP 헤더에서 정보를 복사하여 ARP 헤더 생성 
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

// 주소를 문자열로 변환하는 함수
void addrToStr(unsigned short type, CString& dst, unsigned char* src) {
	switch (type) {
	// 주소 타입이 IP인 경우
	case ARP_IP_TYPE:
		// IP 주소 형식(0.0.0.0)으로 문자열 생성
		dst.Format(_T("%hhu.%hhu.%hhu.%hhu"),
			src[0], src[1], src[2], src[3]);
		break;
	// 주소 타입이 MAC인 경우
	case ARP_MAC_TYPE:
		// MAC 주소 형식(00:00 : 00 : 00 : 00 : 00)으로 문자열 생성
		dst.Format(_T("%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx"),
			src[0], src[1], src[2], src[3], src[4], src[5]);
		break;
	default:
		break;
	}
}

// 문자열을 주소로 포맷팅하는 함수
void StrToaddr(unsigned short type, unsigned char* dst, CString& src) {
	switch (type) {
	// 주소 타입이 IP인 경우
	case ARP_IP_TYPE:
		// IP 주소 형식(0.0.0.0)의 문자열을 IP 주소로 변환
		swscanf_s(src, _T("%hhu.%hhu.%hhu.%hhu"),
			&dst[0], &dst[1], &dst[2], &dst[3]);
		break;
	// 주소 타입이 MAC인 경우
	case ARP_MAC_TYPE:
		// MAC 주소 형식(00:00 : 00 : 00 : 00 : 00)의 문자열을 MAC 주소로 변환
		swscanf_s(src, _T("%hhx:%hhx:%hhx:%hhx:%hhx:%hhx"),
			&dst[0], &dst[1], &dst[2], &dst[3], &dst[4], &dst[5]);
		break;
	default:
		break;
	}
}

// ARP cache table 업데이트 함수
void CARPLayer::updateTable() {
	CTime cur = CTime::GetCurrentTime();
	for (int i = 0; i < m_arpTable.size(); i++) {
		if (m_arpTable[i].status == ARP_TIME_OUT) continue;

		if ((cur - m_arpTable[i].spanTime) > (m_arpTable[i].status == TRUE ? 600 : 180)) {
			m_arpTable[i].status = ARP_TIME_OUT;
		}
	}
}

// 인자로 IP 주소를 가진 항목을 ARP cache table에서 삭제하는 함수
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

// 현재 ARP cache table 반환 함수
std::vector<CARPLayer::ARP_NODE> CARPLayer::getTable() {
	return m_arpTable;
}

// ARP cache table 비우는 함수
void CARPLayer::clearTable() {
	m_arpTable.clear();
}