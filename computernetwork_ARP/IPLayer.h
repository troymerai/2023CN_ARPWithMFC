#pragma once
#include "pch.h" // �ٸ� include���ϵ麸�� ���� ���ԵǾ�� ��
#include "BaseLayer.h"

class CIPLayer : public CBaseLayer {

	// ���⿡ ���� ����


public:
	CIPLayer(char* pName);
	~CIPLayer(); // �Ҹ���

	// ���⿡ �Լ� ����

	// �ּ� �޾ƾ� ��
	void ResetHeader(int iosel);


};