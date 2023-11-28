
// ARPDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "ARP.h"
#include "ARPDlg.h"
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


// CARPDlg 대화 상자



CARPDlg::CARPDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ARP_DIALOG, pParent)
	, CBaseLayer("Dlg")
	, m_ARPLayer(nullptr)
	, m_EtherLayer(nullptr)
	, m_NILayer(nullptr)
	, m_IPLayer(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//Protocol Layer Setting
	m_ARPLayer = new CARPLayer("ARP");
	m_IPLayer = new CIPLayer("Network");
	m_EtherLayer = new CEthernetLayer("Ethernet");
	m_NILayer = new CNILayer("NI");

	if (m_ARPLayer == nullptr || m_IPLayer == nullptr || m_EtherLayer == nullptr || m_NILayer == nullptr) {
		AfxMessageBox(_T("Fail : Layer Link"));
		return;
	}

	m_LayerMgr.AddLayer(m_ARPLayer);
	m_LayerMgr.AddLayer(m_IPLayer);
	m_LayerMgr.AddLayer(m_EtherLayer);
	m_LayerMgr.AddLayer(m_NILayer);
	m_LayerMgr.AddLayer(this);

	m_LayerMgr.ConnectLayers("NI ( *Ethernet ( *Network ( *Dlg  -ARP ) *ARP ) )");
}

void CARPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CONTROL, m_ListARPTable);
	DDX_Control(pDX, IDC_LIST_CONTROL_PROXY, m_ctrlListControlProxy);
	DDX_Control(pDX, IDC_IPADDRESS_SRC, m_SrcIPADDRESS);
	DDX_Control(pDX, IDC_IPADDRESS_DST, m_DstIPADDRESS);
	DDX_Control(pDX, IDC_EDIT_HW_ADDR, m_editHWAddr);
	DDX_Control(pDX, IDC_EDIT_MACADDR, m_editSrcHwAddr);
	DDX_Control(pDX, IDC_COMBO_ADAPTER, m_ComboxAdapter);
	DDX_Control(pDX, IDC_LIST_STATIC_ROUTING_TABLE, m_ListStaticRoutingTable);

	DDX_Control(pDX, IDC_EDIT_MACADDR2, m_editSrcHwAddr2);
	DDX_Control(pDX, IDC_IPADDRESS_SRC2, m_SrcIPADDRESS2);
	DDX_Control(pDX, IDC_COMBO_ADAPTER2, m_ComboxAdapter2);
}

BEGIN_MESSAGE_MAP(CARPDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CARPDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CARPDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_ITEM_DEL, &CARPDlg::OnBnClickedButtonItemDel)
	ON_BN_CLICKED(IDC_BUTTON_ALL_DEL, &CARPDlg::OnBnClickedButtonAllDel)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_ADAPTER, &CARPDlg::OnCbnSelchangeComboAdapter)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, &CARPDlg::OnBnClickedButtonSelect)
	ON_BN_CLICKED(IDC_BUTTON_SEND_ARP, &CARPDlg::OnBnClickedButtonSendArp)
	ON_BN_CLICKED(IDC_BUTTON_G_ARP_SEND, &CARPDlg::OnBnClickedButtonGArpSend)
	ON_BN_CLICKED(IDC_BUTTON_ADD_PROXY_ENTRY, &CARPDlg::OnBnClickedButtonAddProxyEntry)
	ON_BN_CLICKED(IDC_BUTTON_ADD_ROUTING_TABLE_ENTRY, &CARPDlg::OnBnClickedButtonAddRoutingTableEntry)
	ON_BN_CLICKED(IDC_BUTTON_DEL_ROUTING_TABLE_ENTRY, &CARPDlg::OnBnClickedButtonDelRoutingTableEntry)
	ON_CBN_SELCHANGE(IDC_COMBO_ADAPTER2, &CARPDlg::OnCbnSelchangeComboAdapter2)
END_MESSAGE_MAP()


// CARPDlg 메시지 처리기

BOOL CARPDlg::OnInitDialog()
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

	theApp.MainDlg = (CARPDlg*)AfxGetApp()->m_pMainWnd;


	//초기화
	SetTable();
	SetComboBox();
	InitFn();
	
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CARPDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CARPDlg::OnPaint()
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
HCURSOR CARPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//ARP테이블과 Static Routing 테이블을 설정하는 함수
void CARPDlg::SetTable()
{
	CRect rt;
	m_ListARPTable.GetWindowRect(&rt);

	// ARP table 설정
	m_ListARPTable.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ListARPTable.InsertColumn(1, _T("IP Address"), LVCFMT_CENTER, int(rt.Width() * 0.35));
	m_ListARPTable.InsertColumn(2, _T("MAC Address"), LVCFMT_CENTER, int(rt.Width() * 0.4));
	m_ListARPTable.InsertColumn(3, _T("Status"), LVCFMT_CENTER, int(rt.Width() * 0.25));

	//Static Routing Table 설정
	m_ListStaticRoutingTable.GetWindowRect(&rt);
	m_ListStaticRoutingTable.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ListStaticRoutingTable.InsertColumn(1, _T("Destination"), LVCFMT_CENTER, int(rt.Width() * 0.2));
	m_ListStaticRoutingTable.InsertColumn(2, _T("NetMask"), LVCFMT_CENTER, int(rt.Width() * 0.2));
	m_ListStaticRoutingTable.InsertColumn(3, _T("GateWay"), LVCFMT_CENTER, int(rt.Width() * 0.2));
	m_ListStaticRoutingTable.InsertColumn(4, _T("Flag"), LVCFMT_CENTER, int(rt.Width() * 0.1));
	m_ListStaticRoutingTable.InsertColumn(5, _T("Interface"), LVCFMT_CENTER, int(rt.Width() * 0.2));
	m_ListStaticRoutingTable.InsertColumn(6, _T("Metric"), LVCFMT_CENTER, int(rt.Width() * 0.1));
}

// 네트워크 인터페이스 선택 박스
void CARPDlg::SetComboBox()
{
	m_NILayer->SetAdapterComboBox(m_ComboxAdapter);
	m_NILayer->SetAdapterComboBox(m_ComboxAdapter2);
}

// 대화 상자 위치 설정
void CARPDlg::SetPosition(CDialogEx& dlg, int IDD_dlg)
{
	CRect Mainrect;
	CRect Rect;
	GetClientRect(&Mainrect);
	CPoint pos;
	pos.x = GetSystemMetrics(SM_CXSCREEN) / 2.0f - Mainrect.Width() / 2.0f;
	pos.y = GetSystemMetrics(SM_CYSCREEN) / 2.0f - Mainrect.Height() / 2.0f;;

	dlg.Create(IDD_dlg, this);
	dlg.ShowWindow(SW_HIDE);
	dlg.GetClientRect(&Rect);
	dlg.SetWindowPos(NULL, pos.x + (Rect.Width() / 2), pos.y + (Rect.Height() / 2), 0, 0, SWP_NOSIZE);
}


bool CARPDlg::bgetMACaddrInARP(unsigned char* IP, unsigned char* MAC)
{
	// ARP table에 해당 주소값이 있으면 TRUE 반환
	return m_ARPLayer->getMACinARP(IP, MAC);
}

// 프로그램 초기화 함수
// Proxy ARP cache 초기화
void CARPDlg::InitFn()
{
	//--------------------------------------------------------------------------------------
	// 
	//
	//Proxy ARP Cache Set
	//
	// 
	//--------------------------------------------------------------------------------------
	CRect rt;
	m_ctrlListControlProxy.GetWindowRect(&rt);
	m_ctrlListControlProxy.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ctrlListControlProxy.InsertColumn(1, _T("Device"), LVCFMT_CENTER, int(rt.Width() * 0.25));
	m_ctrlListControlProxy.InsertColumn(2, _T("IP Address"), LVCFMT_CENTER, int(rt.Width() * 0.35));
	m_ctrlListControlProxy.InsertColumn(3, _T("Ethernet Address"), LVCFMT_CENTER, int(rt.Width() * 0.4));
	//--------------------------------------------------------------------------------------
	// 
	//
	//Device Add Dlg Set
	//
	// 
	//--------------------------------------------------------------------------------------
	SetPosition(mDeviceAddDlg, IDD_DIALOG_DEVICE_ADD);
	SetPosition(mRoutingTabledlg, IDD_DIALOG_ROUTING_TABLE);
}

// Proxy ARP cache에 항목 추가
// 새 장치 추가 대화 상자 표시
void CARPDlg::AddProxyArpCache(const int _index, unsigned char* ip, unsigned char* addr)
{
	int nListIndex = m_ctrlListControlProxy.GetItemCount();
	UCHAR mac[ENET_ADDR_SIZE] = { 0, };
	CString deviceName, IP, ADDR;

	// 선택된 어뎁터 이름을 불러옴
	m_ComboxAdapter.GetLBText(_index, deviceName);
	// 선택된 어뎁터의 맥 주소를 불러옴
	m_NILayer->GetMacAddress(_index, mac, OUTER);
	// UCHAR에서 STRING으로 IP 및 ETERNET 주소를 변환
	addrToStr(ARP_IP_TYPE, IP, ip);
	addrToStr(ARP_ENET_TYPE, ADDR, addr);

	// ARPLayer 계층의 Proxy Table에 등록

	// Dlg Layer의 Proxy Table에 등록
	m_ctrlListControlProxy.InsertItem(nListIndex, deviceName);
	m_ctrlListControlProxy.SetItemText(nListIndex, 1, IP);
	m_ctrlListControlProxy.SetItemText(nListIndex, 2, ADDR);
	m_ARPLayer->createProxy(mac, ip, addr);
}

void CARPDlg::OnBnClickedButtonAdd()
{
	// "Add" 버튼이 클릭되었을 때 호출되며, 새 장치 추가 대화 상자를 표시
	mDeviceAddDlg.InitDeviceAddDlg(m_ComboxAdapter.GetCurSel());
	mDeviceAddDlg.ShowWindow(SW_SHOW);
}


void CARPDlg::OnBnClickedButtonDelete()
{
	// "Delete" 버튼이 클릭되었을 때 호출되며, 선택된 Proxy ARP 캐시 항목을 삭제
	POSITION pos = m_ctrlListControlProxy.GetFirstSelectedItemPosition();
	int nItem = m_ctrlListControlProxy.GetNextSelectedItem(pos);
	m_ARPLayer->deleteProxy(nItem);
	m_ctrlListControlProxy.DeleteItem(nItem);
}


void CARPDlg::OnBnClickedButtonItemDel()
{
	// "Item Delete" 버튼이 클릭되었을 때 호출되며, 선택된 ARP 테이블 항목을 삭제

	int sindex = m_ListARPTable.GetSelectionMark();

	if (sindex < 0) {
		AfxMessageBox(_T("Select Table first"));
		return;
	}
	CString ip;
	ip = m_ListARPTable.GetItemText(sindex, 0);
	m_ARPLayer->deleteItem(ip);
	m_ListARPTable.DeleteAllItems();
}


void CARPDlg::OnBnClickedButtonAllDel()
{
	// "All Delete" 버튼이 클릭되었을 때 호출되며, 모든 ARP 테이블 항목을 삭제

	m_ARPLayer->clearTable();
	m_ListARPTable.DeleteAllItems();
}

// 1초마다 arp cache table 업데이트
void CARPDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent) {
	case 1:
		updateTable();
		break;
	default:
		break;
	}
	__super::OnTimer(nIDEvent);
}

// arp cache table 업데이트 함수
void CARPDlg::updateTable()
{
	// ARP 계층 참조
	m_ARPLayer->updateTable();
	std::vector<CARPLayer::ARP_NODE> table = m_ARPLayer->getTable();
	CTime cur = CTime::GetCurrentTime();

	for (int i = 0; i < table.size(); i++) {
		CString dstip, dstmac;
		addrToStr(ARP_IP_TYPE, dstip, table[i].protocol_addr);
		addrToStr(ARP_ENET_TYPE, dstmac, table[i].hardware_addr);
		LVFINDINFO l = { LVFI_STRING, dstip };
		int fi = m_ListARPTable.FindItem(&l);
		if (fi == -1) {
			fi = m_ListARPTable.GetItemCount();
			m_ListARPTable.InsertItem(fi, dstip);
		}
		m_ListARPTable.SetItemText(fi, 1, dstmac);
		switch (table[i].status) {
		case ARP_TIME_OUT:
			m_ListARPTable.SetItemText(fi, 2, _T("Timeout"));
			break;
		case FALSE:
			m_ListARPTable.SetItemText(fi, 2, _T("incomplete"));
			break;
		case TRUE:
			m_ListARPTable.SetItemText(fi, 2, _T("Complete"));
			break;
		default:
			break;
		}

	}
}

// 내부 네트워크 어뎁터 선택 함수
void CARPDlg::OnCbnSelchangeComboAdapter()
{
	// MAC 주소, IPv4, IPv6 주소를 저장할 CString 변수 선언
	CString MAC, IPV4, IPV6;
	// 현재 선택된 네트워크 어댑터의 MAC 주소 반환 함수(NI Layer 참조)
	unsigned char* macaddr = m_NILayer->SetAdapter(m_ComboxAdapter.GetCurSel(), OUTER);
	// MAC 주소를 반환받지 못한 경우
	if (macaddr == nullptr) {
		// MAC 주소에 기본 텍스트 설정
		MAC = DEFAULT_EDIT_TEXT;
	}
	// MAC 주소를 반환 받은 경우
	else {
		// MAC 주소 설정
		MAC.Format(_T("%hhx:%hhx:%hhx:%hhx:%hhx:%hhx"), macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
		// 출발지 MAC 주소를 선택한 MAC 주소로 채움
		m_EtherLayer->SetSourceAddress(macaddr, OUTER);
		// 선택한 어댑터의 IP주소를 가져옴
		m_NILayer->GetIPAddress(IPV4, IPV6, OUTER, TRUE);
	}
	// MAC 주소를 UI에 표시
	m_editSrcHwAddr.SetWindowTextW(MAC);
	// IPv4 주소를 UI에 표시
	m_SrcIPADDRESS.SetWindowTextW(IPV4);
}


void CARPDlg::GetADDRINFO(CString& MAC, CString& IP, unsigned char* srcip, CEdit &hw, CIPAddressCtrl &ip)
{
	// MAC 주소를 가져옴
	hw.GetWindowTextW(MAC);
	// IP 주소를 가져옴
	ip.GetWindowTextW(IP);
	ip.GetAddress(srcip[0], srcip[1], srcip[2], srcip[3]);
}

void CARPDlg::SetVisible(CComboBox& adapt, CIPAddressCtrl& srcip, bool isON)
{
	adapt.EnableWindow(isON);
	srcip.EnableWindow(isON);
}

void CARPDlg::SetADDR(CString& MAC, CString& IP, unsigned char* srcip, int iosel)
{
	// ARP 계층의 MAC 주소와 IP 주소 설정
	m_ARPLayer->setmyAddr(MAC, IP, iosel);
	// IP 계층의 출발지 주소 설정
	m_IPLayer->SetSourceAddress(srcip, iosel);
	// IP 계층의 목적지 주소 설정
	m_IPLayer->SetDestinAddress(srcip, iosel);
}

// 네트워크 어댑터 정보 확정 버튼
void CARPDlg::OnBnClickedButtonSelect()
{
	// MAC과 IP 주소를 저장할 변수
	CString MAC[2], IP[2];

	// 출발지 IP 주소를 저장할 변수
	unsigned char srcip[2][IP_ADDR_SIZE] = { 0, };
	
	// 네트워크 어댑터 선택 콤보 박스가 활성화되어 있는 경우 실행
	if (m_ComboxAdapter.IsWindowEnabled()) {
		// 내부 네트워크 정보를 가져옴
		GetADDRINFO(MAC[INNER], IP[INNER], srcip[INNER], m_editSrcHwAddr2, m_SrcIPADDRESS2);
		// 외부 네트워크 정보를 가져옴
		GetADDRINFO(MAC[OUTER], IP[OUTER], srcip[OUTER], m_editSrcHwAddr, m_SrcIPADDRESS);
		// 내부 네트워크 어댑터 선택 변화를 처리하는 함수 호출
		OnCbnSelchangeComboAdapter();
		// 외부 네트워크 어댑터 선택 변화를 처리하는 함수 호출
		OnCbnSelchangeComboAdapter2();

		// 외부 네트워크 정보가 잘설정되어 있는 경우 실행
		if (MAC[OUTER] != DEFAULT_EDIT_TEXT && IP[OUTER] != "0.0.0.0") {
			// 내부 네트워크 어댑터 선택 콤보 박스와 IP 주소 입력 컨트롤을 비활성화
			SetVisible(m_ComboxAdapter, m_SrcIPADDRESS, FALSE);
			// 외부 네트워크 어댑터 선택 콤보 박스와 IP 주소 입력 컨트롤을 비활성화
			SetVisible(m_ComboxAdapter2, m_SrcIPADDRESS2, FALSE);
			// 목적지 IP 주소 입력 컨트롤을 활성화
			m_DstIPADDRESS.EnableWindow(TRUE);
			// NI Layer 참조 -> 패킷 수신 활성화
			m_NILayer->Receiveflip();
			// 내부 네트워크 정보를 설정
			SetADDR(MAC[INNER], IP[INNER], srcip[INNER], INNER);
			// 외부 네트워크 정보를 설정
			SetADDR(MAC[OUTER], IP[OUTER], srcip[OUTER], OUTER);
			// 버튼의 텍스트를 "ReSelect"로 변경
			CDialog::SetDlgItemTextW(IDC_BUTTON_SELECT, _T("ReSelect"));
			// 타이머를 1초로 설정
			SetTimer(1, 1000, NULL);

			// 내부 네트워크 NI Layer의 Receive Thread 시작
			AfxBeginThread(m_NILayer->ThreadFunction_RECEIVE, m_NILayer);
			// 외부 네트워크 NI Layer의 Receive Thread 시작
			AfxBeginThread(m_NILayer->ThreadFunction_RECEIVE2, m_NILayer);

			// GARP 요청 전송
			OnBnClickedButtonGArpSend();
		}
		else {
			AfxMessageBox(_T("Select other Adapter"));
		}
	}
	else {
		// 내부 네트워크 어댑터 선택 콤보 박스와 IP 주소 입력 컨트롤을 활성화
		SetVisible(m_ComboxAdapter, m_SrcIPADDRESS, TRUE);
		// 외부 네트워크 어댑터 선택 콤보 박스와 IP 주소 입력 컨트롤을 활성화
		SetVisible(m_ComboxAdapter2, m_SrcIPADDRESS2, TRUE);
		// 목적지 IP 주소 입력 컨트롤을 비활성화
		m_DstIPADDRESS.EnableWindow(FALSE);
		// NI Layer 참조 -> 패킷 수신 비활성화
		m_NILayer->Receiveflip();
		// 버튼의 텍스트를 "Select"로 변경
		CDialog::SetDlgItemTextW(IDC_BUTTON_SELECT, _T("Select"));
		// 타이머를 종료
		KillTimer(1);
	}
}

// ARP 요청 보내는 함수
void CARPDlg::OnBnClickedButtonSendArp()
{
	// 출발지 IP와 목적지 IP 주소를 저장할 변수
	unsigned char srcip[2][IP_ADDR_SIZE] = {0,}, dstip[IP_ADDR_SIZE] = {0,};
	// 내부 네트워크의 IP 주소를 가져옴
	m_SrcIPADDRESS2.GetAddress(srcip[INNER][0], srcip[INNER][1], srcip[INNER][2], srcip[INNER][3]);
	// 외부 네트워크의 IP 주소를 가져옴
	m_SrcIPADDRESS.GetAddress(srcip[OUTER][0], srcip[OUTER][1], srcip[OUTER][2], srcip[OUTER][3]);
	// 목적지 IP 주소를 가져옴
	m_DstIPADDRESS.GetAddress(dstip[0], dstip[1], dstip[2], dstip[3]);

	// 목적지 IP 주소 입력 컨트롤이 활성화되어 있고, 네트워크 어댑터 선택 콤보 박스가 비활성화되어 있는 경우 실행
	// (== Select 버튼이 눌렸을 때)
	if (m_DstIPADDRESS.IsWindowEnabled() && !m_ComboxAdapter.IsWindowEnabled()) {

		// IP Layer 참조

		// 내부 네트워크의 출발지 IP 주소를 설정
		m_IPLayer->SetSourceAddress(srcip[INNER], INNER);
		// 외부 네트워크의 출발지 IP 주소를 설정
		m_IPLayer->SetSourceAddress(srcip[OUTER], OUTER);
		// 내부 네트워크의 목적지 IP 주소를 설정
		m_IPLayer->SetDestinAddress(dstip, INNER);
		// 외부 네트워크의 목적지 IP 주소를 설정
		m_IPLayer->SetDestinAddress(dstip, OUTER);
	
		// 목적지 IP 주소가 유효한지 확인 
		int check = 0;
		for (int i = 0; i < IP_ADDR_SIZE; i++) {
			check += dstip[i];
		}
		if (check == 0 || check == 255 * 4) {
			AfxMessageBox(_T("Fail : Invalid Address"));
			return;
		}
		// 내부 네트워크의 IP Layer로 ARP 패킷을 전송
		mp_UnderLayer->Send((unsigned char*)"dummy Data", 11, INNER);
		// 외부 네트워크의 IP Layer로 ARP 패킷을 전송
		mp_UnderLayer->Send((unsigned char*)"dummy Data", 11, OUTER);
	}
	else {
		// 어댑터 설정 경고 메시지를 출력
		AfxMessageBox(_T("Fail : Set Adapter first"));
		return;
	}
}


// GARP 요청 함수
void CARPDlg::OnBnClickedButtonGArpSend()
{
	CString sgarpaddr;
	unsigned char dstip[2][IP_ADDR_SIZE] = { 0, };
	char *a = "dummy";

	// GARP 전송 상태를 설정
	m_ARPLayer->isGARP(TRUE);
	// 외부 네트워크 주소를 변수에 저장
	m_SrcIPADDRESS.GetAddress(dstip[OUTER][0], dstip[OUTER][1], dstip[OUTER][2], dstip[OUTER][3]);
	// 외부 네트워크의 목적지 IP 주소를 설정
	m_IPLayer->SetDestinAddress(dstip[OUTER], OUTER);
	// 외부 네트워크의 출발지 IP 주소를 설정
	m_IPLayer->SetSourceAddress(dstip[OUTER], OUTER);

	// 외부 네트워크로 GARP 패킷을 전송
	mp_UnderLayer->Send((unsigned char*)a, 6, OUTER);

	// 내부 네트워크 주소를 변수에 저장
	m_SrcIPADDRESS2.GetAddress(dstip[INNER][0], dstip[INNER][1], dstip[INNER][2], dstip[INNER][3]);
	// 내부 네트워크의 목적지 IP 주소를 설정
	m_IPLayer->SetDestinAddress(dstip[INNER], INNER);
	// 내부 네트워크의 출발지 IP 주소를 설정
	m_IPLayer->SetSourceAddress(dstip[INNER], INNER);
	// 내부 네트워크로 GARP 패킷을 전송
	mp_UnderLayer->Send((unsigned char*)a, 6, INNER);
	
	// GARP 전송 상태를 해제
	m_ARPLayer->isGARP(FALSE);
}

// proxy ARP "Add"버튼이 클릭되었을 때 호출됨
void CARPDlg::OnBnClickedButtonAddRoutingTableEntry()
{
	mRoutingTabledlg.ShowWindow(SW_SHOW);
}

void CARPDlg::OnBnClickedButtonAddProxyEntry()
{
	// TODO: 삭제 요함
}

// Routing table에 항목 추가 함수
void CARPDlg::AddRoutingTable(const int _index, CString ip1, CString ip2, CString ip3, CString mFlag, CString mInterface)
{
	// MAC 주소와 IP 주소를 저장할 변수
	UCHAR mac[ENET_ADDR_SIZE] = { 0, };
	UCHAR _ip[ENET_ADDR_SIZE] = { 0, };
	CString deviceName, IP, ADDR;
	CString count;

	// 문자열로 주어진 IP 주소를 바이트 배열로 변환
	StrToaddr(ARP_IP_TYPE, _ip, ip1);
	StrToaddr(ARP_IP_TYPE, _ip, ip2);
	StrToaddr(ARP_IP_TYPE, _ip, ip3);

	// 라우팅 테이블의 현재 항목 수를 가져옴
	int nListIndex = m_ListStaticRoutingTable.GetItemCount();
	// 항목 수를 문자열로 변환
	count.Format(_T("%d"), nListIndex + 1);

	// Dlg Layer의 Proxy Table에 등록
	m_ListStaticRoutingTable.InsertItem(nListIndex, deviceName);
	
	// IP 주소 1
	m_ListStaticRoutingTable.SetItemText(nListIndex, 0, ip1);
	// IP 주소 2
	m_ListStaticRoutingTable.SetItemText(nListIndex, 1, ip2);
	// IP 주소 3
	m_ListStaticRoutingTable.SetItemText(nListIndex, 2, ip3);
	// flag
	m_ListStaticRoutingTable.SetItemText(nListIndex, 3, mFlag);
	// 인터페이스
	m_ListStaticRoutingTable.SetItemText(nListIndex, 4, mInterface);
	// 항목 번호
	m_ListStaticRoutingTable.SetItemText(nListIndex, 5, count);
}

// Routing table의 항목을 삭제하는 함수
void CARPDlg::OnBnClickedButtonDelRoutingTableEntry()
{   
	// 현재 선택된 항목의 인덱스를 가져옴
	int nListIndex = m_ListStaticRoutingTable.GetSelectionMark();
	// 선택된 항목을 삭제
	m_ListStaticRoutingTable.DeleteItem(nListIndex);
}

// 외부 네트워크 어댑터 선택 콤보 박스의 선택이 변경되었을 때 호출되는 함수
void CARPDlg::OnCbnSelchangeComboAdapter2()
{
	CString MAC, IPV4, IPV6;
	// 선택된 어댑터의 MAC 주소를 가져옴
	unsigned char* macaddr = m_NILayer->SetAdapter(m_ComboxAdapter2.GetCurSel(), INNER);
	if (macaddr == nullptr) {
		MAC = DEFAULT_EDIT_TEXT;
	}
	else {
		// MAC 주소를 문자열로 변환
		MAC.Format(_T("%hhx:%hhx:%hhx:%hhx:%hhx:%hhx"), macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
		// 출발지 MAC 주소를 설정
		m_EtherLayer->SetSourceAddress(macaddr, INNER);
		// IP 주소를 가져옴
		m_NILayer->GetIPAddress(IPV4, IPV6, INNER, TRUE);
	}
	// MAC 주소를 텍스트 박스에 출력
	m_editSrcHwAddr2.SetWindowTextW(MAC);
	// IPv4 주소를 텍스트 박스에 출력
	m_SrcIPADDRESS2.SetWindowTextW(IPV4);
}
