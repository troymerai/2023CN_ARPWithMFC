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


// ARP 헤더를 초기화하는 함수
inline void CARPLayer::ResetHeader() {
	// m_sHeader[INNER]와 m_sHeader[OUTER] 두 개의 ARP 헤더를 새로운 ARP_HEADER()로 재설정
	m_sHeader[INNER] = ARP_HEADER();
	m_sHeader[OUTER] = ARP_HEADER();
}


bool CARPLayer::getMACinARP(unsigned char* dstIP, unsigned char* MAC)
{
	/////////////
	// 주어진 IP 주소(dstIP)가 ARP 테이블에 있는지 확인
	// 있다면 해당 IP 주소의 MAC 주소를 가져와 MAC에 복사
	// 해당 IP 주소가 ARP 테이블에 있으면 true를 반환, 없으면 false를 반환
	/////////////

	// inCache() 함수를 호출하여 주어진 IP 주소(dstIP)가 ARP 캐시에 있는지 확인하고, 그 결과를 index에 저장
	int index = inCache(dstIP);
	bool isfound = FALSE;
	// index가 -1이 아니라면, ** == 주어진 IP 주소가 ARP 캐시에 있다면 이어지는 코드를 실행
	if (index != -1) {
		// memcpy() 함수를 사용하여 ARP 테이블의 해당 인덱스에서 MAC 주소를 가져와 MAC에 복사
		memcpy(MAC, m_arpTable[index].hardware_addr, ENET_ADDR_SIZE);
		// isfound 변수를 TRUE로 설정하여 주어진 IP 주소가 ARP 캐시에 있음을 나타냄
		isfound = TRUE;
	}
	return isfound;
}

// Gratuitous ARP 플래그를 설정하는 함수
void CARPLayer::isGARP(BOOL yes)
{
	is_Garp = !yes;
}

// ARP 수신 함수
BOOL CARPLayer::Receive(unsigned char* ppayload, int iosel) {

	// ARP 헤더 타입으로 캐스팅
	PARP_HEADER arp_data = (PARP_HEADER)ppayload;

	// 이더넷 계층 객체 생성
	CEthernetLayer* m_ether = (CEthernetLayer*)mp_UnderLayer;

	// ARP 캐시에서 패킷의 소스 하드웨어 주소를 찾음
	int index = inCache(arp_data->hardware_srcaddr);

	// 수신한 ARP 패킷의 송신 IP가 자신의 IP와 같은지 확인 ** 같다면 false를 반환
	if (memcmp(myip, arp_data->protocol_srcaddr, IP_ADDR_SIZE) == 0) {
		return FALSE;
	}

	// 패킷의 opcode에 따라 분기 처리
	switch (arp_data->opercode) {

	// ARP 요청인 경우
	case ARP_OPCODE_REQUEST:


		// 캐시에 이미 주소가 있는 경우 (GARP)
		if (index >= 0) {

		// 20231202 수정
		// GARP 코드는 inline, outpath 구조 적용해야함
		// 
		//	// 자기가 보낸 GARP에 대해서는 reply를 안쓰게 예외 처리
		//	if (memcmp(arp_data->protocol_srcaddr, myip, IP_ADDR_SIZE) != 0) {
		//		// IP 충돌이 발생한 경우 GARP reply 패킷 작성
		//		if (memcmp(arp_data->protocol_srcaddr, myip, IP_ADDR_SIZE) == 0) {
		//			// 자신의 MAC 주소를 패킷의 목적지 하드웨어 주소로 설정
		//			memcpy(arp_data->hardware_dstaddr, mymac, ENET_ADDR_SIZE);
		//			// 패킷의 작업 코드를 ARP 응답으로 변경
		//			arp_data->opercode = ARP_OPCODE_REPLY;
		//			// 패킷의 소스 주소와 목적지 주소를 교환
		//			swapaddr(arp_data->hardware_srcaddr, arp_data->hardware_dstaddr, ENET_ADDR_SIZE);
		//			swapaddr(arp_data->protocol_srcaddr, arp_data->protocol_dstaddr, IP_ADDR_SIZE);

		//			// 이더넷 계층의 목적지 주소를 패킷의 목적지 하드웨어 주소로 설정
		//			m_ether->SetDestinAddress(arp_data->hardware_dstaddr);
		//			// 패킷을 하위 계층(이더넷 계층)으로 전송
		//			mp_UnderLayer->Send((unsigned char*)arp_data, ARP_HEADER_SIZE);
		//		}
		//		break;
		//	}

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

			// 응답 패킷의 목적지 MAC 주소를 설정
			m_ether->SetDestinAddress(arp_data->hardware_dstaddr, iosel);
			// 응답 패킷을 전송
			return mp_UnderLayer->Send((unsigned char*)arp_data, ARP_HEADER_SIZE, iosel);
		}

		//만약 request의 dst ip가 나의 ip가 아닌 경우 proxy table 확인 후 등록된 정보가 있다면 reply
		else {
			for (auto &a = m_proxyTable.begin(); a != m_proxyTable.end(); a++) {
				//proxy에 등록된 ip가 arp request의 dst addr이라면 proxy arp reply 수행
				if (memcmp(a->protocol_addr, arp_data->protocol_dstaddr, IP_ADDR_SIZE) == 0) {
					memcpy(arp_data->hardware_dstaddr, a->srchardware_addr, ENET_ADDR_SIZE);
					arp_data->opercode = ARP_OPCODE_REPLY;
					// 송신자와 수신자의 MAC, IP 주소를 교환
					swapaddr(arp_data->hardware_srcaddr, arp_data->hardware_dstaddr, ENET_ADDR_SIZE);
					swapaddr(arp_data->protocol_srcaddr, arp_data->protocol_dstaddr, IP_ADDR_SIZE);

					// 응답 패킷의 목적지 MAC 주소를 설정
					m_ether->SetDestinAddress(arp_data->hardware_dstaddr, iosel);
					// 응답 패킷을 전송
					return mp_UnderLayer->Send((unsigned char*)arp_data, ARP_HEADER_SIZE, iosel);
				}
			}
		}
		break;

	// ARP 응답인 경우
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

	// GARP reply인 경우는 테스트해야함 - 20231202

	//// ARP 응답인 경우
	//case ARP_OPCODE_REPLY:

	//	// GARP인 경우
	//	if (memcmp(arp_data->protocol_dstaddr, arp_data->protocol_srcaddr, IP_ADDR_SIZE) == 0) {

	//		if (isDetected == FALSE) {
	//			// Dlg에 사용자 메시지 보내는 코드 추가
	//			AfxMessageBox(_T("IP conflict detected!"));

	//			// GARP 요청 한 번에 여러번 보내도 ip detected는 한 번만 나타나도록 수정
	//			isDetected = TRUE;
	//		}

	//		break;
	//	}


	//	// ARP인 경우
	//	// 캐시를 순회하며 패킷의 소스 프로토콜 주소와 같은 항목을 찾음
	//	for (auto& node : m_arpTable) {
	//		if (node == arp_data->protocol_srcaddr) {
	//			// 해당 항목을 갱신
	//			memcpy(node.hardware_addr, arp_data->hardware_srcaddr, ENET_ADDR_SIZE);
	//			node.status = true;
	//			node.spanTime = CTime::GetCurrentTime();
	//			break;
	//		}
	//	}
	//	break;

	// 알 수 없는 작업 코드인 경우
	default:
		throw("unknown Opcode Error");
		return false;
	}

	// 패킷 수신 성공값 반환
	return true;
}

// ARP 계층 패킷 전송 함수
BOOL CARPLayer::Send(unsigned char* ppayload, int nlength, int iosel) {

	// 전달받은 payload를 IP헤더 타입으로 캐스팅하여 ip data 확보
	PIP_HEADER ip_data = (PIP_HEADER)ppayload;

	// 이더넷 레이어 참조
	CEthernetLayer* m_ether = (CEthernetLayer*)mp_UnderLayer;

	// broadcast 주소 설정
	unsigned char broadcastAddr[ENET_ADDR_SIZE];

	// 브로드캐스트 주소를 설정 ** GARP가 활성화되어 있으면 255로 설정
	memset(broadcastAddr, 255*is_Garp, ENET_ADDR_SIZE);
	
	// 새로운 ARP 노드 생성
	ARP_NODE newNode(ip_data->ip_dstaddr, broadcastAddr);

	// 송신 IP와 목적지 IP가 같지 않은 경우
	if (memcmp(ip_data->ip_srcaddr, ip_data->ip_dstaddr, IP_ADDR_SIZE) != 0) {
		// 주어진 주소가 ARP 캐시 테이블에 있는지 확인
		int idx = inCache(ip_data->ip_dstaddr);
		// 목적지 IP가 ARP 캐시에 있다면
		if (idx != -1) {
			// 해당 ARP 엔트리 업데이트
			m_arpTable[idx] = newNode;
		}
		// 목적지 IP가 ARP 캐시에 없다면
		else {
			// 새 ARP 엔트리 추가
			m_arpTable.push_back(newNode);
		}
	}
	// 패킷의 목적지 주소를 설정
	m_ether->SetDestinAddress(broadcastAddr, iosel);
	// 패킷의 유형을 ARP로 설정
	m_ether->SetType(ETHER_ARP_TYPE, iosel);
	// 작업 코드를 요청(REQUEST)으로 설정
	setOpcode(ARP_OPCODE_REQUEST, iosel);
	// 패킷의 송신자 주소를 설정
	setSrcAddr(m_ether->GetSourceAddress(iosel), ip_data->ip_srcaddr, iosel);
	// 패킷의 목적지 주소를 설정
	setDstAddr(broadcastAddr, ip_data->ip_dstaddr, iosel);

	// 설정된 ARP 패킷을 전송
	return mp_UnderLayer->Send((unsigned char*)&m_sHeader[iosel], ARP_HEADER_SIZE, iosel);
}

// 주어진 시간 동안 대기하는 함수
// 특정 시간(dwMillisecond) 동안 메시지 루프를 실행하며 대기
void CARPLayer::Wait(DWORD dwMillisecond)
{
	// 윈도우 메시지를 저장할 MSG 구조체를 선언
	MSG msg;
	// 시작 시간을 저장할 변수를 선언하고 현재 시간을 저장
	DWORD dwStart;
	dwStart = GetTickCount();

	// 주어진 시간이 경과할 때까지 루프를 실행
	while (GetTickCount() - dwStart < dwMillisecond)
	{
		// 메시지 큐에서 메시지를 가져옵니다. 메시지가 없을 때까지 반복
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// 문자 메시지를 적절한 문자 메시지로 변환
			TranslateMessage(&msg);
			// 메시지를 해당 윈도우 프로시저로 전달
			DispatchMessage(&msg);
		}
	}
}

// 특정 게이트웨이 IP를 통해 패킷을 전송하는 함수
BOOL CARPLayer::RSend(unsigned char* ppayload, int nlength, unsigned char* gatewayIP, int iosel) {

	// 전송할 IP 헤더 데이터를 파싱
	PIP_HEADER ip_data = (PIP_HEADER)ppayload;

	// 임시 버퍼를 선언하고 초기화
	unsigned char temp[IP_MAX_SIZE] = { 0, };

	// 전송할 데이터의 길이가 최대 크기를 초과하면 최대 크기로 제한
	nlength = nlength > IP_MAX_SIZE ? IP_MAX_SIZE : nlength;

	// 임시 버퍼에 전송할 데이터를 복사
	memcpy(temp, ppayload, nlength);
	

	// 하위 레이어(Ethernet 레이어)를 가져옴
	CEthernetLayer* m_ether = (CEthernetLayer*)mp_UnderLayer;
	int idx = 0;

	// proxy테이블에 있다면 그쪽으로 바로 보내줌
	for (int i = 0; i < m_proxyTable.size(); i++) {
		if (memcmp(m_proxyTable[i].protocol_addr, gatewayIP, IP_ADDR_SIZE) == 0)
		{
			m_ether->SetDestinAddress(m_proxyTable[i].hardware_addr, iosel);
			m_ether->SetType(ETHER_IP_TYPE, iosel);
			return mp_UnderLayer->Send(temp, nlength, iosel);
		}
	}

	//proxy table에 없다면 arp table 찾아보기
	idx = inCache(gatewayIP);
	if (idx == -1) {
		//arp table에도 없으면 해당 ip주소로 ARP request 날려보기
		IP_HEADER forarp;
		memcpy(forarp.ip_srcaddr, myip[iosel], IP_ADDR_SIZE);
		memcpy(forarp.ip_dstaddr, gatewayIP, IP_ADDR_SIZE);
		Send((unsigned char*)&forarp, nlength, iosel);
		
		// ARP reply을 기다림
		Wait(3000);
		
		// ARP reply 받으면 그쪽으로 다시 보내주기
		idx = inCache(gatewayIP);
		if (idx == -1) {
			return FALSE;
		}
	}
	// ARP 테이블에 게이트웨이 IP가 있다면 해당 주소로 패킷을 전송
	m_ether->SetDestinAddress(m_arpTable[idx].hardware_addr, iosel);
	m_ether->SetType(ETHER_IP_TYPE, iosel);
	return mp_UnderLayer->Send(temp, nlength, iosel);
}


// IP 주소가 ARP cache table에 있는 지 확인하는 함수
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

// ARP Layer의 MAC, IP 주소 설정 함수
void CARPLayer::setmyAddr(CString MAC, CString IP, int iosel) {
	StrToaddr(ARP_IP_TYPE, &myip[iosel][0], IP);
	StrToaddr(ARP_ENET_TYPE, &mymac[iosel][0], MAC);
}

// ARP 헤더의 하드웨어 타입과 프로토콜 타입을 설정하는 함수
void CARPLayer::setType(const unsigned short htype, const unsigned short ptype, int iosel) {

	//ARP 패킷에서 사용되는 하드웨어 및 프로토콜 유형 설정
	m_sHeader[iosel].hardware_type = htype;
	m_sHeader[iosel].protocol_type = ptype;

	// 하드웨어 타입에 따라 하드웨어 주소의 길이를 설정
	switch (m_sHeader[iosel].hardware_type) {
	case ARP_ENET_TYPE:
		m_sHeader[iosel].hardware_length = ENET_ADDR_SIZE;
		break;
	default:
		// 지원하지 않는 하드웨어 타입이라면 예외를 발생시킴
		throw("Hardware Type Error!");
	}

	// 프로토콜 타입에 따라 프로토콜 주소의 길이를 설정
	switch (m_sHeader[iosel].protocol_type) {
	case ARP_IP_TYPE:
		m_sHeader[iosel].protocol_length = IP_ADDR_SIZE;
		break;
	default:
		// 지원하지 않는 프로토콜 타입이라면 예외를 발생시킴
		throw("Protocol Type Error!");
	}
}

// ARP 헤더의 opcode를 설정하는 함수
void CARPLayer::setOpcode(const unsigned short opcode, int iosel) {
	// 유효한 opcode 범위 내라면 ARP 헤더의 opcode를 설정
	if (opcode >= ARP_OPCODE_REQUEST && opcode <= ARP_OPCODE_RREPLY) {
		m_sHeader[iosel].opercode = opcode;
	}
	else
		throw("Operator code Error!");
}

// ARP 헤더의 출발지 주소를 설정하는 함수
void CARPLayer::setSrcAddr(unsigned char enetAddr[], unsigned char ipAddr[], int iosel) {
	memcpy(m_sHeader[iosel].hardware_srcaddr, enetAddr, ENET_ADDR_SIZE);
	memcpy(m_sHeader[iosel].protocol_srcaddr, ipAddr, IP_ADDR_SIZE);
}

// ARP 헤더의 목적지 주소를 설정하는 함수
void CARPLayer::setDstAddr(unsigned char enetAddr[], unsigned char ipAddr[], int iosel) {
	memcpy(m_sHeader[iosel].hardware_dstaddr, enetAddr, ENET_ADDR_SIZE);
	memcpy(m_sHeader[iosel].protocol_dstaddr, ipAddr, IP_ADDR_SIZE);
}

// 인자로 받은 주소 swap 함수
void CARPLayer::swapaddr(unsigned char lAddr[], unsigned char rAddr[], unsigned char size) {
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
	// 내부와 외부 네트워크 ARP 헤더의 하드웨어 타입, 프로토콜 타입 설정
	setType(ARP_ENET_TYPE, ARP_IP_TYPE, INNER);
	setType(ARP_ENET_TYPE, ARP_IP_TYPE, OUTER);
	// ip, mac 주소 초기화
	memset(myip, 0, IP_ADDR_SIZE);
	memset(mymac, 0, ENET_ADDR_SIZE);
	// GARP 활성화
	is_Garp = 1;
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

// _PROXY_ARP_NODE 구조체의 생성자
CARPLayer::_PROXY_ARP_NODE::_PROXY_ARP_NODE(unsigned char* srcenet, unsigned char* dstip, unsigned char* dstenet) {
	// 송신자의 하드웨어 주소를 설정 ** 만약 값이 없다면 0으로 초기화
	if (srcenet) memcpy(srchardware_addr, srcenet, ENET_ADDR_SIZE);
	else memset(srchardware_addr, 0, ENET_ADDR_SIZE);
	// 목적지의 프로토콜 주소를 설정 ** 만약 값이 없다면 0으로 초기화
	if (dstip) memcpy(protocol_addr, dstip, IP_ADDR_SIZE);
	else memset(protocol_addr, 0, IP_ADDR_SIZE);
	// 목적지의 하드웨어 주소를 설정 ** 만약 값이 없다면 0으로 초기화
	if (dstenet) memcpy(hardware_addr, dstenet, ENET_ADDR_SIZE);
	else memset(hardware_addr, 0, ENET_ADDR_SIZE);
}

// _PROXY_ARP_NODE 구조체의 연산자 오버로딩
// 다른 proxy ARP 노드와 같은지 비교
bool CARPLayer::_PROXY_ARP_NODE::operator==(const struct _PROXY_ARP_NODE& ot) {
	return (memcmp(ot.hardware_addr, hardware_addr, ENET_ADDR_SIZE) == 0) &&
		(memcmp(ot.srchardware_addr, srchardware_addr, ENET_ADDR_SIZE) == 0) &&
		(memcmp(ot.protocol_addr, protocol_addr, IP_ADDR_SIZE) == 0);
}

// proxy ARP 노드를 생성하는 함수
void CARPLayer::createProxy(unsigned char* src, unsigned char* ip, unsigned char* enet) {
	// proxy ARP 테이블에 새 노드를 추가
	m_proxyTable.push_back(PROXY_ARP_NODE(src, ip, enet));
}

// proxy ARP 노드를 삭제하는 함수
void CARPLayer::deleteProxy(const int index) {
	// proxy ARP 테이블에서 지정된 인덱스의 노드를 삭제
	m_proxyTable.erase(m_proxyTable.begin() + index);
}