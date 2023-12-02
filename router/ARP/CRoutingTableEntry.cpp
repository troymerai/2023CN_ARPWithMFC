﻿// CRoutingTableEntry.cpp: 구현 파일
//

////////////////////////////////////////////////////
//
// Static Routing Table에 추가
//
//
////////////////////////////////////////////////////

#include "pch.h"
#include "ARP.h"
#include "afxdialogex.h"
#include "CRoutingTableEntry.h"


// CRoutingTableEntry 대화 상자

// CRoutingTableEntry 클래스를 동적으로 생성하는 매크로
IMPLEMENT_DYNAMIC(CRoutingTableEntry, CDialogEx)

// 생성자
CRoutingTableEntry::CRoutingTableEntry(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ROUTING_TABLE, pParent)
{

}

// 소멸자
CRoutingTableEntry::~CRoutingTableEntry()
{
}

// 데이터를 교환하는 함수
// 컨트롤 변수에 대한 동기화를 수행
void CRoutingTableEntry::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_IPADDRESS_ROUTING_DESTINATION, m_IPADDRESS_Destination);
	DDX_Control(pDX, IDC_IPADDRESS_ROUTING_NETMASK, m_IPADDRESS_NetMask);
	DDX_Control(pDX, IDC_IPADDRESS_ROUTING_GATEWAY, m_IPADDRESS_GateWay);
	DDX_Control(pDX, IDC_COMBO_ROUTING_INTERFACE, m_RoutingInterface);

	DDX_Control(pDX, IDC_CHECK_ROUTING_UP, m_RoutingCheckUp);
	DDX_Control(pDX, IDC_CHECK_ROUTING_DOWN, m_RoutingCheckDown);
	DDX_Control(pDX, IDC_CHECK_ROUTING_HOST, m_RoutingCheckHost);


	//IDC_CHECK_ROUTING_UP
	//IDC_CHECK_ROUTING_DOWN
	//IDC_CHECK_ROUTING_HOST
	CDialogEx::DoDataExchange(pDX);
}

// 메시지 맵을 시작하는 매크로
BEGIN_MESSAGE_MAP(CRoutingTableEntry, CDialogEx)
	ON_BN_CLICKED(IDOK, &CRoutingTableEntry::OnBnClickedOk)
END_MESSAGE_MAP()


// CRoutingTableEntry 메시지 처리기

// 대화 상자가 초기화되면 호출되는 함수
BOOL CRoutingTableEntry::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_RoutingInterface.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

// 확인 버튼을 클릭했을 때 호출되는 함수
void CRoutingTableEntry::OnBnClickedOk()
{
	CString mDestination;
	CString mNetmask;
	CString mGateway;
	CString mFlag;
	UCHAR dstip[IP_ADDR_SIZE] = { 0, };
	UCHAR maskip[IP_ADDR_SIZE] = { 0, };
	UCHAR gateway[IP_ADDR_SIZE] = { 0, };
	CString mInterface;
	UCHAR flag = 0;
	m_IPADDRESS_Destination.GetAddress(dstip[0], dstip[1], dstip[2], dstip[3]);
	addrToStr(ARP_IP_TYPE, mDestination, dstip);

	m_IPADDRESS_NetMask.GetAddress(maskip[0], maskip[1], maskip[2], maskip[3]);
	addrToStr(ARP_IP_TYPE, mNetmask, maskip);

	m_IPADDRESS_GateWay.GetAddress(gateway[0], gateway[1], gateway[2], gateway[3]);
	addrToStr(ARP_IP_TYPE, mGateway, gateway);
	
	mFlag.Empty();
	if (m_RoutingCheckUp.GetCheck())
	{
		mFlag.AppendFormat(_T("U"));
		flag |= IP_ROUTE_UP;
	}
	if (m_RoutingCheckDown.GetCheck())
	{
		mFlag.AppendFormat(_T("G"));
		flag |= IP_ROUTE_GATEWAY;
	}
	if (m_RoutingCheckHost.GetCheck())
	{
		mFlag.AppendFormat(_T("H"));
		flag |= IP_ROUTE_HOST;

	}
	m_RoutingInterface.GetLBText(m_RoutingInterface.GetCurSel(), mInterface);
	theApp.MainDlg->AddRoutingTable(0, mDestination, mNetmask, mGateway, mFlag, mInterface);
	theApp.MainDlg->m_IPLayer->AddRouteTable(dstip, maskip, gateway, flag, m_RoutingInterface.GetCurSel());
	
	CDialogEx::OnOK();
}



