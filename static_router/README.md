# 프로토콜 스택

![image](https://github.com/troymerai/2023CN_ARPWithMFC/assets/107831875/07cc0711-fb2c-4544-a3a3-5ed91797ff18)


# 실습 시나리오
1. 두 대의 PC에서 각각 프로그램을 실행한다.
2. 두 대의 PC는 같은 네트워크로 연결한다. (같은 WIFI)
3. ARP Request 및 Reply를 받을 NIC를 선택 및 개방한다.
4. HostA는 Gratuitous ARP란에 수정된 MAC 주소를 입력한다.
5. HostA에서 GARP request를 broadcast로 전송한다.(이 요청에는 출발지의 MAC 주소와 IP 주소, 그리고 목적지의 MAC 주소와 IP 주소가 모두 자신의 것으로 설정되어있음)
6. 이 요청을 수신한 모든 장치(== HostB)는 자신의 ARP 캐시를 업데이트한다.
7. 만약 장치가 이미 해당 IP 주소를 사용하고 있었다면, (== IP 주소 충돌) 이 경우, 해당 장치는 이를 충돌로 인식하고 출발지)에게 GARP reply을 작성한다.
8. HostA에서 GARP reply 패킷을 받았으면 사용자에게 메시지를 보낸다.

## 실습 시나리오 시퀀스 다이어그램
```mermaid
sequenceDiagram
    participant PC1
    participant Router
    participant PC2

    PC1->>Router: ICMP Echo Request (Ping)
    activate Router
    Router-->>PC2: ICMP Echo Request (Ping)
    activate PC2
    PC2-->>Router: ICMP Echo Reply
    deactivate PC2
    Router-->>PC1: ICMP Echo Reply
    deactivate Router
```

각 시퀀스에 따른 세부 시퀀스 다이어그램

### PC1 -> Router : ICMP Echo Request (Ping)
```mermaid
sequenceDiagram
    participant PC1 as PC1 (IP Stack)
    participant PC1NIC as PC1 (Network Interface)
    participant RouterNIC as Router (Network Interface)
    participant Router as Router (IP Stack)

    PC1->>PC1NIC: Create ICMP Echo Request
    activate PC1NIC
    PC1NIC->>RouterNIC: Send ICMP Echo Request
    activate RouterNIC
    RouterNIC->>Router: Receive ICMP Echo Request
    activate Router
```

### Router -> PC2 : ICMP Echo Request (Ping)
```mermaid
sequenceDiagram
    participant Router as Router (IP Stack)
    participant RouterNIC as Router (Network Interface)
    participant PC2NIC as PC2 (Network Interface)
    participant PC2 as PC2 (IP Stack)

    Router->>RouterNIC: Forward ICMP Echo Request
    activate RouterNIC
    RouterNIC->>PC2NIC: Send ICMP Echo Request
    activate PC2NIC
    PC2NIC->>PC2: Receive ICMP Echo Request
    activate PC2
```

### PC2 -> Router : ICMP Echo Reply
```mermaid
sequenceDiagram
    participant PC2 as PC2 (IP Stack)
    participant PC2NIC as PC2 (Network Interface)
    participant RouterNIC as Router (Network Interface)
    participant Router as Router (IP Stack)

    PC2->>PC2NIC: Create ICMP Echo Reply
    activate PC2NIC
    PC2NIC->>RouterNIC: Send ICMP Echo Reply
    activate RouterNIC
    RouterNIC->>Router: Receive ICMP Echo Reply
    activate Router
```

### Router -> PC1 : ICMP Echo Reply
```mermaid
sequenceDiagram
    participant Router as Router (IP Stack)
    participant RouterNIC as Router (Network Interface)
    participant PC1NIC as PC1 (Network Interface)
    participant PC1 as PC1 (IP Stack)

    Router->>RouterNIC: Forward ICMP Echo Reply
    activate RouterNIC
    RouterNIC->>PC1NIC: Send ICMP Echo Reply
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




  
