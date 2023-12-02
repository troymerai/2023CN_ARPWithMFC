# 프로토콜 스택

![image](https://github.com/troymerai/2023CN_ARPWithMFC/assets/107831875/74465daa-b957-454d-8b0c-e03aa491155a)


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
		autonumber
    participant HostA as Host A
    participant Network as Network
    participant HostB as Host B

    Note over HostA: Gratuitous ARP에 수정된 MAC 주소 입력
    HostA->>Network: GARP request broadcast 전송
    Network->>HostB: GARP request 패킷 전달
    Note over HostB: GARP request 수신 후 ARP 캐시 업데이트
    alt IP 주소 충돌 발생
        Note over HostB: 해당 IP 주소를 이미 사용 중인 경우
        HostB->>Network: GARP reply 작성 후 전송
        Network->>HostA: GARP reply 패킷 전달
        Note over HostA: GARP reply 패킷 수신 후 사용자에게 메시지 전송
    end
```





  
