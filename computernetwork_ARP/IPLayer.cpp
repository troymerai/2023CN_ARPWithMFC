
#include "pch.h" // �ٸ� include���ϵ麸�� ���� ���ԵǾ�� ��
#include "stdafx.h"
#include "BaseLayer.h"
#include "IPLayer.h"


// ���� ���
//
// Source IP address 
// Destination IP address
//


CIPLayer::CIPLayer(char* pName) : CBaseLayer(pName) {
	// ������
	// ��Ŷ �ʱ�ȭ?


}

CIPLayer:: ~CIPLayer() {
	// �Ҹ���
}



//
// ���� ���̾�(Ethernet Layer)���� payload �޴� �Լ�
// 
// TODO
// 
// ������ IP �ּҰ� �ڽ��� IP �ּҿ� ������ Ȯ��
// �ٸ��� discard
// ������ ARP Layer�� payload ����
//

//
// dlg���� Destination IP �ּ� �޾Ƽ� ������ ����, ARP Layer�� �����ϴ� �Լ�
//
//

//
// dlg���� Source IP �ּ� �޾Ƽ� ������ ����, ARP Layer�� �����ϴ� �Լ�
//
//
