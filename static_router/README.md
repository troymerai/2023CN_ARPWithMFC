# 2023CN_ARPWithMFC

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


# 프로토콜 스택




  
