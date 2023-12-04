# 프로토콜 스택

![image](https://github.com/troymerai/2023CN_ARPWithMFC/assets/107831875/07cc0711-fb2c-4544-a3a3-5ed91797ff18)


# 실습 시나리오
1. 두 대의 PC와 라우터(이번 실습에서는 공유기를 사용)를 준비한다.
2. 라우터 기능을 할 PC(앞으로 Router1이라고 하겠습니다)는 네트워크 연결을 끊는다.
3. 이더넷 케이블을 이용하여 [PC1 - Router1 - Router2] 구조로 연결한다.
4. Router1에서 Static Router 프로그램을 이용해 내부 네트워크와 외부 네트워크를 설정한다.
5. Router1에서 Routing entry에 맞게 모든 항목을 채워서 라우팅 테이블을 채운다.
6. PC1에서 ping 명령어를 이용해 PC2(다른 네트워크 호스트)로 ICMP Echo Request를 전송한다.
7. Router1이 PC1에서 온 ICMP Echo Request 패킷을 받는다.
8. Router1은 Routing Table을 확인하여 패킷의 목적지인 PC2를 찾는다.
[과정]
    - IP 계층에서는 Routing Table 한 열씩 넷마스크와 AND연산을 수행한다.
        - 마스킹 결과와 패킷의 목적지 IP가 일치하면, 대상이 호스트인지 default gateway인지 설정하여 하위 계층(ARP 계층)으로 넘긴다.
        - 마스킹 결과와 패킷의 목적지 IP가 일치하지 않으면 에러 로그를 남긴다.
    - ARP 계층에서는 호스트 또는 default gateway의 IP 주소를 찾기 위해 Proxy ARP Cache Table을 확인한다.
        - Proxy ARP Cache Table에 있으면 ARP 헤더를 작성하여 목적지에 따라 패킷을 호스트 또는 default gateway로 전송한다.
        - Proxy ARP Cache Table에 없으면 ARP Cache Table을 확인한다.
            - ARP Cache Table에도 없으면 ARP Request를 보낸다.
            - 3초 기다렸다가 ARP Reply를 받은 뒤 다시 ARP Cache Table을 확인한다.
            - ARP Cache Table에 호스트 또는 default gateway의 정보가 없으면 에러 로그를 남긴다.
    - ARP Cache Table에 호스트 또는 default gateway의 정보가 있으면 ARP 헤더를 작성하여 목적지에 따라 패킷을 호스트 또는 default gateway로 전송한다.
9. 패킷을 받은 Router2(default gateway)는 Routing Table을 확인하여 패킷의 목적지인 PC2를 찾는다.
    - 8과 동일한 과정을 거친다.
10. PC2는 ICMP Echo Request 패킷을 받아 ICMP Echo Reply 패킷을 생성하여 PC1으로 전송한다.
    - 8과 동일한 과정을 거친다.
11. Router2는 Routing Table을 확인하여 패킷의 목적지로 PC1을 찾는다.
    - 8과 동일한 과정을 거친다.
12. 패킷을 받은 Router2는 패킷을 Router1(default gateway)으로 전송한다.
    - 8과 동일한 과정을 거친다.
13. 패킷을 받은 Router1은 Routing Table을 확인하여 패킷을 목적지인 PC1으로 전송한다.
14. PC1은 ICMP Echo Reply 패킷을 받아 연결이 성공적으로 이루어졌음을 확인한다.

## 실습 시나리오 시퀀스 다이어그램
```mermaid
sequenceDiagram
    participant PC1
    participant Router1
    participant Router2
    participant PC2(외부 네트워크 호스트)

    autonumber
    PC1->>Router1: Set up Routing Table
    activate Router1
    PC1->>Router1: ICMP Echo Request (Ping)
    activate Router1
    Router1->>Router2: Forward ICMP Echo Request (Ping)
    activate Router2
    Router2-->>PC2(외부 네트워크 호스트): ICMP Echo Request (Ping)
    activate PC2(외부 네트워크 호스트)
    PC2(외부 네트워크 호스트)-->>Router2: ICMP Echo Reply
    deactivate PC2(외부 네트워크 호스트)
    Router2-->>Router1: Forward ICMP Echo Reply
    deactivate Router2
    Router1-->>PC1: ICMP Echo Reply
    deactivate Router1
```

각 시퀀스에 따른 세부 시퀀스 다이어그램

### PC1 -> Router : ICMP Echo Request (Ping)
```mermaid
sequenceDiagram
    autonumber
    participant PC1 as PC1 (IP Stack)
    participant PC1NIC as PC1 (Network Interface)
    participant Router1NIC as Router1 (Network Interface)
    participant Router1 as Router1 (IP Stack)
    participant Router2NIC as Router2 (Network Interface)
    participant Router2 as Router2 (IP Stack)
    participant PC2NIC as PC2 (Network Interface)
    participant PC2 as PC2 (IP Stack)

    PC1->>PC1NIC: ICMP Echo Request (Ping)
    activate PC1NIC
    
    PC1NIC->>Router1NIC: Send ICMP Echo Request
    activate Router1NIC
    
    Router1NIC->>Router1: Receive ICMP Echo Request
    activate Router1
    
    Router1->>Router1: Check Routing Table for PC2 IP
    opt PC2(또는 default gateway)의 IP 정보가 Routing Table에 없는 경우
        Router1->>Router1NIC: Send ARP Request for default gateway
        activate Router1NIC
        
        Router1NIC->>Router2NIC: Broadcast ARP Request
        activate Router2NIC
        
        Router2NIC->>Router2: Receive ARP Request
        activate Router2
        
        Router2->>Router2NIC: Send ARP Reply
        deactivate Router2
        
        Router2NIC->>Router1NIC: Send ARP Reply
        deactivate Router2NIC
        
        Router1NIC->>Router1: Receive ARP Reply
        activate Router1
        
        Router1->>Router1: Update ARP Cache with default gateway MAC
        deactivate Router1
    end
    
    Router1->>Router1NIC: Forward ICMP Echo Request
    activate Router1NIC
    
    Router1NIC->>Router2NIC: Send ICMP Echo Request
    activate Router2NIC

    Router2NIC->>Router2: Receive ICMP Echo Request
    activate Router2

    Router2->>Router2: Check Routing Table for PC2 IP
    opt PC2(또는 default gateway)의 IP 정보가 Routing Table에 없는 경우
        Note over Router2NIC,Router2: Repeat process 5 to 11
    end

    Router2->>PC2NIC: Forward ICMP Echo Request
    activate PC2NIC

    PC2NIC->>PC2: Receive ICMP Echo Request
    activate PC2

    PC2->>PC2NIC: Create ICMP Echo Reply
    activate PC2NIC

    PC2NIC->>Router2: Send ICMP Echo Reply
    activate Router2

    Router2->>Router2NIC: Forward ICMP Echo Reply
    activate Router2NIC

    Router2NIC->>Router1NIC: Send ICMP Echo Reply
    activate Router1NIC

    Router1NIC->>Router1: Receive ICMP Echo Reply
    activate Router1

    Router1->>Router1NIC: Forward ICMP Echo Reply
    activate Router1NIC

    Router1NIC->>PC1NIC: Send ICMP Echo Reply
    activate PC1NIC

    PC1NIC->>PC1: Receive ICMP Echo Reply
    activate PC1
```

## 클래스 다이어그램
```mermaid
classDiagram
    class CBaseLayer{
        +CBaseLayer(char* pName)
        +~CBaseLayer()
        +SetUnderUpperLayer(CBaseLayer* pUULayer)
        +SetUpperUnderLayer(CBaseLayer* pUULayer)
        +SetUpperLayer(CBaseLayer* pUpperLayer)
        +SetUnderLayer(CBaseLayer* pUnderLayer)
        +GetUpperLayer(int nindex)
        +GetUnderLayer()
        +GetLayerName()
    }
    class CIPLayer{
      +CIPLayer(char* pName)
      +~CIPLayer()
      +void ResetHeader(int iosel)
      +void SetHeaderFields(unsigned char* ppayload, int iosel)
      +unsigned char* GetSourceAddress(int iosel)
      +unsigned char* GetDestinAddress(int iosel)
      +void SetSourceAddress(unsigned char* pAddress, int iosel)
      +void SetDestinAddress(unsigned char* pAddress, int iosel)
      +BOOL Send(unsigned char* ppayload, int nlength, int iosel)
      +BOOL RSend(unsigned char* ppayload, int nlength, unsigned char* gatewayIP, int iosel)
      +BOOL Receive(unsigned char* ppayload, int iosel)
      +void AddRouteTable(unsigned char* _destination_ip, unsigned char* _netmask, unsigned char* _gateway, unsigned char _flag, unsigned char interFace)
      +void DelRouteTable(unsigned char index)
      +void SetDefaultGateway(unsigned char* _gateway, unsigned char _flag, unsigned char interFace)
      +bool LongestPrefix(unsigned char* a, unsigned char* b)
      +void Routing(unsigned char* dest_ip, unsigned char* ppayload, int iosel)
    }
    CIPLayer --|> CBaseLayer : inherits
    class CARPLayer{
      +CARPLayer(char* pName)
      +~CARPLayer()
      +bool Receive(unsigned char* ppayload, int iosel)
      +bool Send(unsigned char* ppayload, int nlength, int iosel)
      +void Wait(DWORD dwMillisecond)
      +bool RSend(unsigned char* ppayload, int nlength, unsigned char* gatewayIP, int iosel)
      +void setmyAddr(CString MAC, CString IP, int iosel)
      +void setType(const unsigned short htype, const unsigned short ptype, int iosel)
      +void setOpcode(const unsigned short opcode, int iosel)
      +void setSrcAddr(unsigned char enetAddr[], unsigned char ipAddr[], int iosel)
      +void setDstAddr(unsigned char enetAddr[], unsigned char ipAddr[], int iosel)
      +void swapaddr(unsigned char lAddr[], unsigned char rAddr[], unsigned char size)
      +void updateTable()
      +void deleteItem(CString IP)
      +std::vector<ARP_NODE> getTable()
      +void clearTable()
      +void createProxy(unsigned char* src, unsigned char* ip, unsigned char* enet)
      +void deleteProxy(const int index)
    }
    CARPLayer --|> CBaseLayer : inherits
    class CEthernetLayer{
        +CEthernetLayer(char* pName)
        +~CEthernetLayer()
        +void ResetHeader(int iosel)
        +unsigned char* GetSourceAddress(int iosel)
        +unsigned char* GetDestinAddress(int iosel)
        +void SetSourceAddress(unsigned char* pAddress, int iosel)
        +void SetDestinAddress(unsigned char* pAddress, int iosel)
        +BOOL Send(unsigned char* ppayload, int nlength, int iosel)
        +void SetType(unsigned short type, int iosel)
        +BOOL Receive(unsigned char* ppayload, int iosel)
    }
    CEthernetLayer --|> CBaseLayer : inherits
    class CNILayer{
        +CNILayer(char* pName)
        +~CNILayer()
        +BOOL Receive(unsigned char* pkt, int iosel)
        +BOOL Send(unsigned char* ppayload, int nlength, int iosel)
        +void SetAdapterComboBox(CComboBox& adapterlist)
        +UCHAR* SetAdapter(const int index, const int iosel)
        +void GetMacAddressList(CStringArray& adapterlist)
        +void GetMacAddress(const int index, UCHAR *mac, const int iosel)
        +void GetIPAddress(CString& ipv4addr, CString& ipv6addr, const int iosel, bool isIOsel)
        +UINT ThreadFunction_RECEIVE(LPVOID pParam)
        +UINT ThreadFunction_RECEIVE2(LPVOID pParam)
        +void Receiveflip()
    }
    CNILayer --|> CBaseLayer : inherits
```




  
