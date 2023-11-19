#include "pch.h"
#include "ARPLayer.h"

// IP 주소와 MAC 주소를 받아서 ARP 노드를 생성 
CARPLayer::_ARP_NODE::_ARP_NODE(unsigned char* cipaddr, unsigned char* cenetaddr, unsigned char bincomplete = false) {
	// IP(프로토콜)주소 저장
	memcpy(protocol_addr, cipaddr, IP_ADDR_SIZE);
	// MAC(하드웨어)주소 저장
	memcpy(hardware_addr, cenetaddr, ENET_ADDR_SIZE);
	// status는 incomplete로 저장
	status = bincomplete;
	// ARP 테이블에 항목을 추가한 시간 저장
	spanTime = CTime::GetCurrentTime();
}

// IP 주소와 MAC 주소를 0으로 초기화한 ARP 노드를 생성
CARPLayer::_ARP_NODE::_ARP_NODE(unsigned int ipaddr = 0, unsigned int enetaddr = 0, unsigned char incomplete = false) {
	// IP 주소 초기화
	memset(protocol_addr, ipaddr, IP_ADDR_SIZE);
	// MAC 주소 초기화
	memset(hardware_addr, enetaddr, ENET_ADDR_SIZE);
	// status는 incomplete로 초기화
	status = incomplete;
	//ARP 테이블에 항목을 추가한 시간 저장
	spanTime = CTime::GetCurrentTime();
}

// 다른 ARP 노드 객체로부터 정보를 복사하여 ARP 노드를 생성
CARPLayer::_ARP_NODE::_ARP_NODE(const struct _ARP_NODE& ot) {
	// IP 주소 초기화
	memcpy(protocol_addr, ot.protocol_addr, IP_ADDR_SIZE);
	// MAC 주소 초기화
	memcpy(hardware_addr, ot.hardware_addr, ENET_ADDR_SIZE);
	// 상태 복사
	status = ot.status;
	//ARP 테이블에 항목을 추가한 시간 복사
	spanTime = ot.spanTime;
}

// ARP 헤더 초기화 inline 함수
inline void CARPLayer::ResetHeader() {
	m_sHeader = ARP_HEADER();
}

// (기존)
//ARP 요청 패킷을 받으면 먼저 목적지 IP 주소가 자신의 IP 주소인지 확인
//동일하다면, 자신의 MAC 주소를 ARP 응답 패킷의 목적지 MAC 주소로 설정하고, 그렇지 않다면 ARP 테이블에서 해당 IP 주소를 찾아 MAC 주소를 설정
//그 후, ARP 응답 패킷을 생성하여 전송

// (수정)
// ARP 요청 패킷을 받으면 먼저 ARP 캐시에서 해당 소스 MAC 주소를 찾음
// 찾는다면, 해당 캐시 항목을 갱신하고, 찾지 못하면 새로운 항목을 캐시에 추가
//그 후, 요청받은 목적지 IP 주소가 자신의 IP 주소와 같다면, ARP 응답 패킷을 생성하여 전송

// ARP 계층 패킷 수신 함수
BOOL CARPLayer::Receive(unsigned char* ppayload) {

	// ARP 헤더 타입으로 캐스팅
	PARP_HEADER arp_data = (PARP_HEADER)ppayload;

	// 이더넷 계층 객체 생성
	CEthernetLayer* m_ether = (CEthernetLayer*)mp_UnderLayer;

	// ARP 캐시에서 패킷의 소스 하드웨어 주소를 찾음
	int index = inCache(arp_data->hardware_srcaddr);

	// 패킷의 opcode에 따라 분기 처리
	switch (arp_data->opercode) {

	// ARP 요청인 경우
	case ARP_OPCODE_REQUEST:

		// 자신이 보낸 GARP에 대해서는 아무 일도 일어나지 않게 예외 처리
		// 작동 안함
		//if (memcmp(arp_data->hardware_srcaddr, mymac, ENET_ADDR_SIZE) == 0) {
		//	break;
		//}

		// 캐시에 이미 주소가 있는 경우 (GARP)
		if (index >= 0) {
			// 자기가 보낸 GARP에 대해서는 reply를 안쓰게 예외 처리
			if (memcmp(arp_data->protocol_srcaddr, myip, IP_ADDR_SIZE) != 0) {
				// IP 충돌이 발생한 경우 GARP reply 패킷 작성
				if (memcmp(arp_data->protocol_srcaddr, myip, IP_ADDR_SIZE) == 0) {
					// 자신의 MAC 주소를 패킷의 목적지 하드웨어 주소로 설정
					memcpy(arp_data->hardware_dstaddr, mymac, ENET_ADDR_SIZE);
					// 패킷의 작업 코드를 ARP 응답으로 변경
					arp_data->opercode = ARP_OPCODE_REPLY;
					// 패킷의 소스 주소와 목적지 주소를 교환
					swapaddr(arp_data->hardware_srcaddr, arp_data->hardware_dstaddr, ENET_ADDR_SIZE);
					swapaddr(arp_data->protocol_srcaddr, arp_data->protocol_dstaddr, IP_ADDR_SIZE);

					// 이더넷 계층의 목적지 주소를 패킷의 목적지 하드웨어 주소로 설정
					m_ether->SetDestinAddress(arp_data->hardware_dstaddr);
					// 패킷을 하위 계층(이더넷 계층)으로 전송
					mp_UnderLayer->Send((unsigned char*)arp_data, ARP_HEADER_SIZE);
				}
				break;
			}

			// 캐시의 해당 항목을 갱신
			memcpy(m_arpTable[index].hardware_addr, arp_data->hardware_srcaddr, ENET_ADDR_SIZE);
			m_arpTable[index].status = true;
			m_arpTable[index].spanTime = CTime::GetCurrentTime();

		}
		// 캐시에 주소가 없는 경우
		else {
			// 새 항목을 캐시에 추가
			m_arpTable.push_back(ARP_NODE(arp_data->protocol_srcaddr, arp_data->hardware_srcaddr, TRUE));
		}
		
		// ARP의 경우
		// 패킷의 목적지 IP 주소가 자신의 IP 주소와 같은 경우
		if (memcmp(arp_data->protocol_dstaddr, myip, IP_ADDR_SIZE) == 0) {
			// 자신의 MAC 주소를 패킷의 목적지 하드웨어 주소로 설정
			memcpy(arp_data->hardware_dstaddr, mymac, ENET_ADDR_SIZE);
			// 패킷의 작업 코드를 ARP 응답으로 변경
			arp_data->opercode = ARP_OPCODE_REPLY;
			// 패킷의 소스 주소와 목적지 주소를 교환
			swapaddr(arp_data->hardware_srcaddr, arp_data->hardware_dstaddr, ENET_ADDR_SIZE);
			swapaddr(arp_data->protocol_srcaddr, arp_data->protocol_dstaddr, IP_ADDR_SIZE);

			// 이더넷 계층의 목적지 주소를 패킷의 목적지 하드웨어 주소로 설정
			m_ether->SetDestinAddress(arp_data->hardware_dstaddr);
			// 패킷을 하위 계층(이더넷 계층)으로 전송
			return mp_UnderLayer->Send((unsigned char*)arp_data, ARP_HEADER_SIZE);
		}
		break;
	// ARP 응답인 경우
	case ARP_OPCODE_REPLY:

		// GARP인 경우
		if (memcmp(arp_data->protocol_dstaddr, arp_data->protocol_srcaddr, IP_ADDR_SIZE) == 0) {
			// Dlg에 사용자 메시지 보내는 코드 추가
			AfxMessageBox(_T("IP conflict detected!"));
			break;
		}


		// ARP인 경우
		// 캐시를 순회하며 패킷의 소스 프로토콜 주소와 같은 항목을 찾음
		for (auto& node : m_arpTable) {
			if (node == arp_data->protocol_srcaddr) {
				// 해당 항목을 갱신
				memcpy(node.hardware_addr, arp_data->hardware_srcaddr, ENET_ADDR_SIZE);
				node.status = true;
				node.spanTime = CTime::GetCurrentTime();
				break;
			}
		}
		break;

	// 알 수 없는 작업 코드인 경우
	default:
		throw("unknown Opcode Error");
		return false;
	}

	// 패킷 수신 성공값 반환
	return true;
}

// ARP 계층 패킷 전송 함수
BOOL CARPLayer::Send(unsigned char* ppayload, int nlength) {

	// 전달받은 payload를 IP헤더 타입으로 캐스팅하여 ip data 확보
	PIP_HEADER ip_data = (PIP_HEADER)ppayload;

	// 이더넷 레이어 참조
	CEthernetLayer* m_ether = (CEthernetLayer*)mp_UnderLayer;

	// broadcast 주소 설정
	unsigned char broadcastAddr[ENET_ADDR_SIZE];
	memset(broadcastAddr, 255, ENET_ADDR_SIZE);
	

	// 새로운 ARP 노드 생성
	ARP_NODE newNode(ip_data->dstaddr, broadcastAddr);
	m_ether->SetDestinAddress(broadcastAddr);

	// opcode를 ARP request(0x0100)로 설정
	setOpcode(ARP_OPCODE_REQUEST);

	if (memcmp(ip_data->srcaddr, ip_data->dstaddr, IP_ADDR_SIZE) != 0) {
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
	}

	// 출발지 주소와 목적지 주소 설정
	setSrcAddr(m_ether->GetSourceAddress(), ip_data->srcaddr);
	setDstAddr(broadcastAddr, ip_data->dstaddr);


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
	StrToaddr(ARP_ENET_TYPE, mymac, MAC);
}

// ARP 헤더의 하드웨어 타입과 프로토콜 타입을 설정하는 함수
void CARPLayer::setType(const unsigned short htype, const unsigned short ptype) {

	//ARP 패킷에서 사용되는 하드웨어 및 프로토콜 유형 설정
	m_sHeader.hardware_type = htype;
	m_sHeader.protocol_type = ptype;


	// 하드웨어 타입에 따라 하드웨어 주소의 길이를 설정
	switch (m_sHeader.hardware_type) {
	case ARP_ENET_TYPE:
		m_sHeader.hardware_length = ENET_ADDR_SIZE;
		break;
	default:
		throw("Hardware Type Error!");
	}

	// 프로토콜 타입에 따라 프로토콜 주소의 길이를 설정
	switch (m_sHeader.protocol_type) {
	case ARP_IP_TYPE:
		m_sHeader.protocol_length = IP_ADDR_SIZE;
		break;
	default:
		throw("Protocol Type Error!");
	}
}

// ARP 헤더의 opcode를 설정하는 함수
void CARPLayer::setOpcode(const unsigned short opcode) {

	// 유효한 opcode 범위 내라면 ARP 헤더의 opcode를 설정
	if (opcode >= ARP_OPCODE_REQUEST && opcode <= ARP_OPCODE_RREPLY) {
		m_sHeader.opercode = opcode;
	}
	else
		throw("Operator code Error!");
}


// ARP 헤더의 출발지 주소를 설정하는 함수
void CARPLayer::setSrcAddr(const unsigned char enetAddr[], const unsigned char ipAddr[]) {
	memcpy(m_sHeader.hardware_srcaddr, enetAddr, ENET_ADDR_SIZE);
	memcpy(m_sHeader.protocol_srcaddr, ipAddr, IP_ADDR_SIZE);
}

// ARP 헤더의 목적지 주소를 설정하는 함수
void CARPLayer::setDstAddr(const unsigned char enetAddr[], const unsigned char ipAddr[]) {
	memcpy(m_sHeader.hardware_dstaddr, enetAddr, ENET_ADDR_SIZE);
	memcpy(m_sHeader.protocol_dstaddr, ipAddr, IP_ADDR_SIZE);
}

// 인자로 받은 주소 swap 함수
void CARPLayer::swapaddr(unsigned char lAddr[], unsigned char rAddr[], const unsigned char size) {
	unsigned char tempAddr[ENET_ADDR_SIZE] = { 0, };

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
	setType(ARP_ENET_TYPE, ARP_IP_TYPE);
	// ip, mac 주소 초기화
	memset(myip, 0, IP_ADDR_SIZE);
	memset(mymac, 0, ENET_ADDR_SIZE);
}

// 소멸자
CARPLayer::~CARPLayer() {

}

// 아래의 연산자 오버로딩들은 ARP 노드 간의 비교를 수행
// IP 주소를 기준으로 비교
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

// ARP 헤더 초기화 함수
CARPLayer::_ARP_HEADER::_ARP_HEADER() {
	hardware_type = protocol_type = 0x0000;
	hardware_length = protocol_length = 0x00;
	opercode = 0x0000;
	memset(hardware_srcaddr, 0x00, ENET_ADDR_SIZE);
	memset(protocol_srcaddr, 0x00, IP_ADDR_SIZE);
	memset(hardware_dstaddr, 0x00, ENET_ADDR_SIZE);
	memset(protocol_dstaddr, 0x00, IP_ADDR_SIZE);
};

// 인자로 받은 ARP 헤더에서 정보를 복사하여 ARP 헤더 생성하는 함수
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
	case ARP_ENET_TYPE:
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
	case ARP_ENET_TYPE:
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