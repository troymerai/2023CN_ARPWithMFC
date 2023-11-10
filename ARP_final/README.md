# 2023CN_ARPWithMFC

# 실습 시나리오
1. 두 대의 PC에서 각각 프로그램을 실행한다.
2. 두 대의 PC는 같은 네트워크로 연결한다. (같은 WIFI)
3. ARP Request 및 Reply를 받을 NIC를 선택 및 개방한다.
4. 출발지 호스트에서 전송 버튼을 누른다. 
	- ARP cache table에 IP address, MAC address, status를 확인한다.
5. ARP Request 패킷을 작성한다.
	- Hardware Type은 1
	- Protocol Type은 0x0800(IP)
	- Opcode를 1(ARP Request)
	- Sender영역에 자신의 MAC 주소와 IP 주소 설정한다.
	- Target영역에 Hardware는 0으로 protocol address는 목적지 IP주소로 설정한다.
6. Ethernet헤더를 작성한다.
	- Ethernet Header의 Destination Address를 Broadcast 주소로 채운다.
	- Ethernet Header의 Source Address를 자신의 MAC 주소로 채운다.
	- Ethernet Type을 0x0806(ARP)로 설정한다.
7. 레이어 아키텍처에 의해서 Encapsulated Packet(==Ethernet frame)이 생성된다.
8. NI Layer의 pcap라이브러리로 작성된 코드로 packet은 네트워크로 전송된다.
9. OnTimer를 설정(180초)한다.
10. 목적지 호스트에서 패킷(Request)을 수신한다.
	- Ethernet Layer에서 목적지 MAC 주소가 broadcast인지 확인한다.
		- 아니라면, discard(버림)
	- 맞으면, 레이어 아키텍처에 의해서 Demultiplexing을 통해 header를 제외한 	  data부분을 상위 레이어(IP Layer)로 전달한다.
		- Header는 매 레이어마다 receive 함수에서 frame에 대해 나에게 온                       것이 맞는 지 검사할 때 쓰인다.
	- IP Layer에서 수신된 payload의 destination IP address가 자신의 IP 주소와               같은 지 확인한다.
			- 아니라면, discard(버림)
	- 맞으면, ARP Reply 패킷 작성한다.
		- Opcode를 2(ARP Reply)
		- sender영역과 target영역을 swap
		- source MAC address에 자신의 MAC 주소를 넣어준다.
11. 이더넷 헤더를 작성한다.
	- Ethernet Header의 Destination Address를 출발지의 MAC 주소로 채운다.
	- Ethernet Header의 Source Address를 자신의 MAC 주소로 채운다.
	- Ethernet Type을 0x0806(ARP)로 설정한다.		
12. 레이어 아키텍처에 의해서 Encapsulated Packet(==Ethernet frame)이 생성된다.
13. NI Layer의 pcap라이브러리로 작성된 코드로 packet은 네트워크로 전송된다.
14. 출발지 호스트에서 패킷(Reply)을 수신한다.
	- Ethernet Layer에서 목적지 MAC 주소가 자신의 MAC 주소와 같은 지 확인한다.
		- 다르다면, discard(버림)
	- 같다면, 레이어 아키텍처에 의해서 Demultiplexing을 통해 header를 제외한 	  data부분을 ARP 레이어로 전송한다.
15. ARP 레이어에서 ARP cache table을 업데이트한다.
16. 업데이트한 ARP cache table을 APP 계층에서 확인하고 UI를 업데이트한다.

#  시퀀스 다이어그램
```mermaid
sequenceDiagram
    participant PC1 as 출발지 호스트
    participant Net as 네트워크
    participant PC2 as 목적지 호스트
    
    PC1->>+Net: ARP Request 전송 (Ethernet Header: Broadcast, ARP: 자신의 MAC/IP, 목적지 IP)
    Note over PC1,Net: Ethernet Frame 생성
    Net-->>+PC2: ARP Request 수신 
    PC2->>+Net: ARP Reply 전송 (Ethernet Header: 출발지 MAC, ARP: 자신의 MAC/IP, 출발지 MAC/IP)
    Note over PC2,Net: Ethernet Frame 생성
    Net-->>+PC1: ARP Reply 수신
    PC1->>PC1: ARP Cache Table 업데이트
    PC1->>PC1: UI 업데이트
```

## 출발지 - ARP Request
```mermaid
sequenceDiagram
	autonumber
	actor A as HostA
	participant C as Application Layer
	participant D as IP Layer
	participant E as ARP Layer
	participant F as Ethernet Layer
	participant G as NI Layer
	participant H as network

	A->>C: NIC 선택 및 개방, HostB IP 주소 입력
	Note over A,C: UCHAR* CNILayer::SetAdapter()
	A->>C: ARP send 버튼 클릭
	Note over A,C: void CARPDlg::OnBnClickedButtonSendArp()
	C->>D: IP data 전송 
	Note over C,D: BOOL CIPLayer::Send()
	D->>D: IP data 저장
	D->>E: IP data 전송
	Note over D,E: BOOL CARPLayer::Send()
	alt is IP가 ARP cache table에 존재
		alt is 상태가 TRUE
			E->>C: 사용자에게 알림
		else 상태가 FALSE
			E->>E: ARP cache table 업데이트
		end
	else is IP가 ARP cache table에 없는 경우
		E->>E: ARP cache table 업데이트
	end
	E->>E: ARP Request ARP frame 작성
	E->>F: ARP Request ARP frame 전송
	Note over E,F: BOOL CARPLayer::Send()
	F->>F: ARP Request 이더넷 frame 작성
	F->>G: ARP Request 패킷 전송
	Note over F,G: BOOL CEthernetLayer::Send()
	G->>H: ARP Request 패킷을 네트워크로 전송
	Note over G,H: BOOL CNILayer::Send
```

## 목적지 - ARP Request
```mermaid
sequenceDiagram
	autonumber
	participant M as network
 	participant H as NI Layer
	participant I as Ethernet Layer
	participant J as ARP Layer
	participant K as IP Layer
	participant L as Application Layer
	actor B as HostB	

	B->>L: NIC 선택 및 개방, HostA IP 주소 입력
	Note over B,L: UCHAR* CNILayer::SetAdapter()
	M->>H: ARP Request 패킷 전송
	Note over M,H: UINT CNILayer::ThreadFunction_RECEIVE()
	H->>I: ARP Request 패킷 전송
	Note over H,I: BOOL CNILayer::Receive()
	alt is 이더넷 헤더의 목적지 MAC이 broadcast
		I->>K: ARP 프레임만 전송
		Note over I,K: (request인 경우)BOOL CIPLayer::Receive()
		alt is 프레임의 목적지 IP가 내 IP와 같은 경우
			K->>J: ARP 프레임만 전송
			Note over K,J: BOOL CARPLayer::Receive()
			alt is ARP request
				J->>J: ARP reply ARP 프레임 작성
				J->>I: ARP reply ARP 프레임 전송
			else ARP reply
				Note over J,I: 출발지에서 사용됨
			end
		else 프레임의 목적지 IP가 내 IP와 다른 경우
			Note over K,J: discard
		end
	else 이더넷 헤더의 목적지 MAC이 내 MAC
		Note over I,J: (reply인 경우) 출발지에서 사용됨
	end
	I->>H: ARP reply 패킷 전송
	H->>M: ARP reply 패킷을 네트워크로 전송
```
## 출발지 - ARP Reply
```mermaid
sequenceDiagram
	autonumber
	actor A as HostA
	participant C as Application Layer
	participant D as IP Layer
	participant E as ARP Layer
	participant F as Ethernet Layer
	participant G as NI Layer
	participant H as network

	H->>G: ARP reply 패킷 전송
	alt is 목적지 MAC이 내 MAC과 같다면
		G->>E: ARP reply ARP프레임만 전송
	else 목적지 MAC이 내 MAC과 다르다면
		Note over G,E: dicard
	end
	E->>E: ARP cache table 업데이트
	C->>E: ARP cache table 업데이트 확인
	C->>C: ARP cache table UI 업데이트
	A->>C: ARP cache table 확인
```


# 프로토콜 스택




  
