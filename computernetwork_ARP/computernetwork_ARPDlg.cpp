﻿
// computernetwork_ARPDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "computernetwork_ARP.h"
#include "computernetwork_ARPDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CcomputernetworkARPDlg 대화 상자



CcomputernetworkARPDlg::CcomputernetworkARPDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_COMPUTERNETWORK_ARP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CcomputernetworkARPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	// 변수이름
	DDX_Control(pDX, IDC_LIST1, m_list_arp_cache);
	DDX_Control(pDX, IDC_IPADDRESS2, m_ip_address);
}

BEGIN_MESSAGE_MAP(CcomputernetworkARPDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CcomputernetworkARPDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON4, &CcomputernetworkARPDlg::OnBnClickedButton4)
	ON_CBN_SELCHANGE(IDC_COMBOBOXEX1, &CcomputernetworkARPDlg::OnCbnSelchangeComboboxex1)
	ON_BN_CLICKED(IDC_BNT_ARP_DEL, &CcomputernetworkARPDlg::OnBnClickedBntArpDel)
	ON_BN_CLICKED(IDC_BNT_LIST_ALL_DEL, &CcomputernetworkARPDlg::OnBnClickedBntListAllDel)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS1, &CcomputernetworkARPDlg::OnIpnFieldchangedIpaddress1)
	ON_LBN_SELCHANGE(IDC_LIST8, &CcomputernetworkARPDlg::OnLbnSelchangeList8)
	ON_BN_CLICKED(IDC_BNT_ARP_ADD, &CcomputernetworkARPDlg::OnBnClickedBntArpAdd)
	ON_LBN_SELCHANGE(IDC_LIST, &CcomputernetworkARPDlg::OnLbnSelchangeList)
	ON_LBN_SELCHANGE(IDC_LIST_ARP, &CcomputernetworkARPDlg::OnLbnSelchangeListArp)
	ON_BN_CLICKED(IDC_BNT_LIST_ITEM_DEL, &CcomputernetworkARPDlg::OnBnClickedBntListItemDel)
	ON_BN_CLICKED(IDC_BNT_SEND, &CcomputernetworkARPDlg::OnBnClickedBntSend)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CcomputernetworkARPDlg::OnLvnItemchangedList1)
	ON_EN_CHANGE(IDC_NETADDRESS2, &CcomputernetworkARPDlg::OnEnChangeNetaddress2)
END_MESSAGE_MAP()


// CcomputernetworkARPDlg 메시지 처리기

BOOL CcomputernetworkARPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	CRect rect;
	m_list_arp_cache.GetClientRect(&rect);

	m_list_arp_cache.InsertColumn(0, _T("IP Address", LVCFMT_LEFT, 500));
	m_list_arp_cache.InsertColumn(1, _T("Ethernet Address", LVCFMT_LEFT, 500));
	m_list_arp_cache.InsertColumn(2, _T("Status", LVCFMT_LEFT, 500));


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CcomputernetworkARPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CcomputernetworkARPDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CcomputernetworkARPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CcomputernetworkARPDlg::OnLvnItemchangedMfcshelllist1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CcomputernetworkARPDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// IP Layer에 source IP 주소 전달 함수 넣기
}





void CcomputernetworkARPDlg::OnBnClickedButton4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


// Source NIC 선택하는 곳
void CcomputernetworkARPDlg::OnCbnSelchangeComboboxex1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// NI Layer에서 네트워크 어댑터 목록 얻어오는 코드 추가

	
}


void CcomputernetworkARPDlg::OnBnClickedButton5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CcomputernetworkARPDlg::OnBnClickedBntListAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CcomputernetworkARPDlg::OnBnClickedBntArpDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CcomputernetworkARPDlg::OnBnClickedBntListAllDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CcomputernetworkARPDlg::OnIpnFieldchangedIpaddress1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CcomputernetworkARPDlg::OnLbnSelchangeList8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CcomputernetworkARPDlg::OnBnClickedBntArpAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CcomputernetworkARPDlg::OnLbnSelchangeList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CcomputernetworkARPDlg::OnLbnSelchangeListArp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CcomputernetworkARPDlg::OnBnClickedBntListItemDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// ARP Cache의 item 삭제 버튼

	int count = m_list_arp_cache.GetItemCount();

	for (int i = count; i >= 0; i--) {// 모든 항목 조사
		if (m_list_arp_cache.GetItemState(i, LVIS_SELECTED) != 0) //선택된 항목을 삭제
			m_list_arp_cache.DeleteItem(i);
	}
}


void CcomputernetworkARPDlg::OnBnClickedBntSend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// ip 주소 보내는 버튼

	// 추가할 행의 위치를 얻어오기 위해서 리스트 컨트롤의 항목 개수를 얻어온다.
	CString strIPAddress;
	m_ip_address.GetWindowText(strIPAddress);

	int randomValue = rand() % 5;

	int nItemNum = m_list_arp_cache.GetItemCount();
	m_list_arp_cache.InsertItem(nItemNum, strIPAddress);

	CString strRandomValue;
	strRandomValue.Format(_T("%d"), randomValue);
	m_list_arp_cache.SetItemText(nItemNum, 1, strRandomValue);

	m_ip_address.SetAddress(0, 0, 0, 0);

	// IP Layer에 Destination Ip 주소 전달 함수 넣기

	// 타이머 start 함수

	// status를 incomplete로 만드는 함수

	// ARP Layer에 
	// Source MAC address
	// Source IP address
	// Destination IP address 
	// 있는 지 ARP cache table 조회 함수
	//
	// 있으면 dlg에 띄우기
	// 없으면 ARP Layer에 ARP 요청 함수
	//		요청 후 3분 안에 ARP cache이 업데이트 됐다는 신호가 오면(?) dlg에 띄우기



}


void CcomputernetworkARPDlg::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CcomputernetworkARPDlg::OnEnChangeNetaddress2()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// Ethernet Layer에서 MAC 주소 추출하는 함수 넣기
}
