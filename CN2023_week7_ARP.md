# 컴네 7주차 - ARP 공부 자료

- **Table of contents**

# ARP Protocol이란?

---

ARP 프로토콜은 같은 네트워크 대역에서 통신을 하기 위해 필요한 MAC 주소를 IP 주소를 이용해서 알아오는 프로토콜이다.

같은 네트워크 대역에서 통신을 한다고 하더라도 데이터를 보내기 위해서는 7계층부터 캡슐화를 통해 데이터를 보내기 때문에 IP주소와 MAC주소가 모두 필요하다. 이때, IP 주소는 알고 MAC 주소는 모르더라도 ARP를 통해 통신이 가능하다.

(평상 시에는 컴퓨터가 자동으로 ARP를 해주기 때문에 IP주소만 알아도 통신할 수 있다.)

# ARP 패킷 구조

---

![Untitled](%E1%84%8F%E1%85%A5%E1%86%B7%E1%84%82%E1%85%A6%207%E1%84%8C%E1%85%AE%E1%84%8E%E1%85%A1%20-%20ARP%20%E1%84%80%E1%85%A9%E1%86%BC%E1%84%87%E1%85%AE%20%E1%84%8C%E1%85%A1%E1%84%85%E1%85%AD%20b0d72220c09241a48e9ae4d41f3e222f/Untitled.png)

## Ethernet Frame

**[Ethernet destination address Field]**

Broadcast 주소

**[Ethernet source address Field]**

출발지의 MAC 주소

**[frame type Field]**

ARP 요청과 반환 시, 이 값은 0x0806으로 고정

## ARP Frame

**[hard type Field]**

2계층에서 사용하는 프로토콜

이더넷 프로토콜만 있는 건 아니지만 이더넷 프로토콜이라고 생각하면 됨

0001 또는 1로 표기(이더넷 프로토콜일 경우)

**[prot type Field]**

목적지에서 사용하는 IP 프로토콜 (IPv4로 생각)

0800으로 표기

**[hard size Field]**

MAC 주소 크기

06 또는 6으로 표기

**[prot size Field]**

IPv4 크기

04 또는 4로 표기

**[op(opertion) Field]**

MAC 주소 요청 중 == ARP request(1로 표기)

MAC 주소 반환 중 == ARP reply(2로 표기)

IP 주소 요청 중 == RARP request(3으로 표기)

IP 주소 반환 중 == RARP reply(4로 표기)

**[sender Ethernet address Field]**

출발지 MAC 주소 (6byte)

**[sender IP address Field]**

출발지 IP 주소 (4btye)

**[target Ethernet address Field]**

목적지 MAC 주소 (6byte)

**[target IP address Field]**

목적지 IP 주소 (4byte)

# ARP 데이터 flow

---

![Untitled](%E1%84%8F%E1%85%A5%E1%86%B7%E1%84%82%E1%85%A6%207%E1%84%8C%E1%85%AE%E1%84%8E%E1%85%A1%20-%20ARP%20%E1%84%80%E1%85%A9%E1%86%BC%E1%84%87%E1%85%AE%20%E1%84%8C%E1%85%A1%E1%84%85%E1%85%AD%20b0d72220c09241a48e9ae4d41f3e222f/Untitled%201.png)

같은 LAN 대역(같은 네트워크 대역)에 있는 컴퓨터 A가 컴퓨터 C와 통신하고싶지만 IP주소만 알고있는 상황이라고 가정 

1. 컴퓨터 A가 ARP요청 프로토콜을 작성

ARP 요청 패킷에 이더넷 헤더를 붙여서 작성한다.

근데, ARP 패킷에도 목적지 MAC 주소를 써야하고 이더넷 헤더에도 목적지 MAC 주소를 써야하는데 모르지 않나? 

먼저 ARP Frame을 살펴보자

[request - ARP Frame]

  

![Untitled](%E1%84%8F%E1%85%A5%E1%86%B7%E1%84%82%E1%85%A6%207%E1%84%8C%E1%85%AE%E1%84%8E%E1%85%A1%20-%20ARP%20%E1%84%80%E1%85%A9%E1%86%BC%E1%84%87%E1%85%AE%20%E1%84%8C%E1%85%A1%E1%84%85%E1%85%AD%20b0d72220c09241a48e9ae4d41f3e222f/Untitled%202.png)

출발지 IP 주소는 16진수로 변환한 것이다.

목적지 MAC 주소는 모르니까 00 00 00 00 00 00 으로 비워둔다.

목적지 IP 주소도 16진수로 변환하여 적는다.

그렇다면 이더넷 헤더는 어떨까?

[request - Ethernet Frame]

![Untitled](%E1%84%8F%E1%85%A5%E1%86%B7%E1%84%82%E1%85%A6%207%E1%84%8C%E1%85%AE%E1%84%8E%E1%85%A1%20-%20ARP%20%E1%84%80%E1%85%A9%E1%86%BC%E1%84%87%E1%85%AE%20%E1%84%8C%E1%85%A1%E1%84%85%E1%85%AD%20b0d72220c09241a48e9ae4d41f3e222f/Untitled%203.png)

목적지의 MAC 주소를 모르기때문에 FF FF FF FF FF FF로 작성한다. 이진수로 생각하면 1로 꽉채운 것이다.

== broadcast라는 뜻

이렇게 이더넷 헤더를 broadcast로 지정하면 스위치로 연결된(== 같은 네트워크 대역) 모든 장비에게 프레임 전송

1. 컴퓨터 A가 스위치(L2 장비)한테 프레임 전송
2. 스위치는 L2장비니까 이더넷 헤더만 까서 확인
3. broadcast니까 스위치에 연결된 모든 장비에 프레임 재전송
4. 프레임을 받은 장비들(컴퓨터 C 포함)은 이더넷 헤더를 까고 이더넷 헤더에 적힌 MAC주소를 확인, broadcast임을 확인하고 ARP 프레임을 까봄(L3 장비니까)
5. 각 컴퓨터는 목적지 IP주소를 확인하고 자신의 IP 주소와 다르면 discard, 같으면 출발지 MAC 주소에 자신의 MAC 주소(컴퓨터 C의 MAC 주소)를 넣어서 ARP reply 패킷의 ARP Frame 작성

![Untitled](%E1%84%8F%E1%85%A5%E1%86%B7%E1%84%82%E1%85%A6%207%E1%84%8C%E1%85%AE%E1%84%8E%E1%85%A1%20-%20ARP%20%E1%84%80%E1%85%A9%E1%86%BC%E1%84%87%E1%85%AE%20%E1%84%8C%E1%85%A1%E1%84%85%E1%85%AD%20b0d72220c09241a48e9ae4d41f3e222f/Untitled%204.png)

1. ARP reply 패킷의 Ethernet Frame에서도 어디서 온 프레임인지 알기 때문에 도착지 MAC 주소(컴퓨터 A의 MAC 주소)를 잘 쓸 수 있음

![Untitled](%E1%84%8F%E1%85%A5%E1%86%B7%E1%84%82%E1%85%A6%207%E1%84%8C%E1%85%AE%E1%84%8E%E1%85%A1%20-%20ARP%20%E1%84%80%E1%85%A9%E1%86%BC%E1%84%87%E1%85%AE%20%E1%84%8C%E1%85%A1%E1%84%85%E1%85%AD%20b0d72220c09241a48e9ae4d41f3e222f/Untitled%205.png)

1. ARP - reply 패킷을 받은 컴퓨터 A는 다 까서 컴퓨터 C의 MAC 주소를 확인하고 자신의 ARP cache table을 갱신

# ARP cache table이란?

---

나(내 컴퓨터)와 통신했던 컴퓨터들의 MAC 주소와 IP 주소들을 하나씩 맵핑 시켜놓은 것

하지만, cache table이기에 일정 시간이 지나면 사라진다. 사라지고 나서 다시 그 컴퓨터와 통신하려면 ARP를 통해 cache table을 채워넣고 통신한다. (수동으로 등록해서 영구적으로 사용하는 방법도 있음)

20분마다 없앤다..?

# ARP 레이어 분석

---

![Untitled](%E1%84%8F%E1%85%A5%E1%86%B7%E1%84%82%E1%85%A6%207%E1%84%8C%E1%85%AE%E1%84%8E%E1%85%A1%20-%20ARP%20%E1%84%80%E1%85%A9%E1%86%BC%E1%84%87%E1%85%AE%20%E1%84%8C%E1%85%A1%E1%84%85%E1%85%AD%20b0d72220c09241a48e9ae4d41f3e222f/Untitled%206.png)

## 출발지 (ARP - request)

### Application Layer

GUI

### IP Layer

### ARP Layer

Ethernet 프레임, ARP 프레임 까보는 곳?

ARP - request 패킷 작성

- 이더넷 프레임
- ARP 프레임

### Ethernet Layer

Ethernet 프레임만 까보는 곳?

이더넷 프레임을 까보고 브로드캐스트임을 확인

NI Layer로 다시 패킷으로 만들어서 전송

### NI Layer

패킷 전송, 수신만 하는 곳?

브로드캐스트로 전송

## 목적지

### NI Layer

패킷 수신

### Etherent Layer

패킷에서 이더넷 프레임만 까보고 브로드캐스트임을 확인

### ARP Layer

패킷에서 ARP 프레임 까보고 자신의 IP 주소가 맞는 지 확인

다르면 discard

같으면 자신의 MAC 주소를 채워 넣어서 ARP - reply 패킷 작성

이때 reply 패킷의 이더넷 프레임에는 출발지의 MAC 주소를 넣어줘야 함

### IP Layer

필요 없음?

### Application Layer

필요 없음?

## 출발지 (ARP - reply)

### NI Layer

패킷 수신

### Etherent Layer

패킷에서 이더넷 프레임만 까보고 자신의 MAC 주소임을 확인

### ARP Layer

패킷에서 ARP 프레임 까보고 필요한 MAC 주소를 받아서 ARP cache table 업데이트

### IP Layer

### Application Layer

GUI에 업데이트

# ARP 레이어별 구현 목록

## Application Layer

MFC 코드

## IP Layer

??

## ARP Layer

ARP - request 패킷 작성하는 함수

ARP 프레임 까서 자신의 IP 주소와 맞는 지 확인하는 함수

ARP - reply 패킷(이더넷 프레임, ARP 프레임 모두) 작성하는 함수

## Ethernet Layer

ARP 패킷에서 이더넷 프레임을 깐 뒤, 자신의 MAC 주소인지, broadcast인지 확인하는 함수 (broadcast도 아니고 자신의 MAC 주소도 아닐 경우에만 discard)

NI로 패킷 전송 함수

NI에서 패킷 받아오는 함수

## NI Layer

패킷 전송 함수

패킷 받아오는 함수