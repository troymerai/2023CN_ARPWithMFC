
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
	// 참조할 계층 추가
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

	// 계층 정의
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
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS_DST, &CARPDlg::OnIpnFieldchangedIpaddressDst)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CONTROL, &CARPDlg::OnLvnItemchangedListControl)
	ON_EN_CHANGE(IDC_EDIT_HW_ADDR, &CARPDlg::OnEnChangeEditHwAddr)
	ON_BN_CLICKED(IDC_BUTTON_G_ARP_SEND, &CARPDlg::OnBnClickedButtonGArpSend)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CONTROL_PROXY, &CARPDlg::OnLvnItemchangedListControlProxy)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS_SRC, &CARPDlg::OnIpnFieldchangedIpaddressSrc)
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


	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	InitFn();
	SetTable();
	SetComboBox();
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

void CARPDlg::SetTable()
{
	CRect rt;
	m_ListARPTable.GetWindowRect(&rt);

	m_ListARPTable.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ListARPTable.InsertColumn(1, _T("IP Address"), LVCFMT_CENTER, int(rt.Width() * 0.35));
	m_ListARPTable.InsertColumn(2, _T("MAC Address"), LVCFMT_CENTER, int(rt.Width() * 0.4));
	m_ListARPTable.InsertColumn(3, _T("Status"), LVCFMT_CENTER, int(rt.Width() * 0.25));
}

void CARPDlg::SetComboBox()
{
	m_NILayer->SetAdapterComboBox(m_ComboxAdapter);
}


void CARPDlg::InitFn()
{
	//--------------------------------------------------------------------------------------
	// 
	//
	//Proxy ARP Cache Set
	//
	// 
	//--------------------------------------------------------------------------------------
	m_ctrlListControlProxy.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_ctrlListControlProxy.InsertColumn(0, _T("Device"));
	m_ctrlListControlProxy.SetColumnWidth(0, 70);
	m_ctrlListControlProxy.InsertColumn(1, _T("IP Address"));
	m_ctrlListControlProxy.SetColumnWidth(1, 120);
	m_ctrlListControlProxy.InsertColumn(2, _T("Ethernet Address"));
	m_ctrlListControlProxy.SetColumnWidth(2, 140);
	//--------------------------------------------------------------------------------------
	// 
	//
	//Device Add Dlg Set
	//
	// 
	//--------------------------------------------------------------------------------------
	mDeviceAddDlg.Create(IDD_DIALOG_DEVICE_ADD, this);
	mDeviceAddDlg.ShowWindow(SW_HIDE);
}

void CARPDlg::AddProxyArpCache(TCHAR* _Device, TCHAR* _ip, TCHAR* _Ethernet)
{
	if (_ip == NULL) { return; }

	int nListIndex = m_ctrlListControlProxy.GetItemCount();

	m_ctrlListControlProxy.InsertItem(nListIndex, _Device);

	m_ctrlListControlProxy.SetItemText(nListIndex, 1, _ip);
	m_ctrlListControlProxy.SetItemText(nListIndex, 2, _Ethernet);
	m_ctrlListControlProxy.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
	m_ctrlListControlProxy.SendMessage(WM_VSCROLL, SB_BOTTOM);	// Scroll to bottom
}

void CARPDlg::OnBnClickedButtonAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//mDeviceAddDlg.InitDeviceAddDlg(0);
	mDeviceAddDlg.ShowWindow(SW_SHOW);
}


void CARPDlg::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos = m_ctrlListControlProxy.GetFirstSelectedItemPosition();
	int nItem = m_ctrlListControlProxy.GetNextSelectedItem(pos);
	m_ctrlListControlProxy.DeleteItem(nItem);
}


void CARPDlg::OnBnClickedButtonItemDel()
{
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
	m_ARPLayer->clearTable();
	m_ListARPTable.DeleteAllItems();
}


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

void CARPDlg::updateTable()
{
	m_ARPLayer->updateTable();
	std::vector<CARPLayer::ARP_NODE> table = m_ARPLayer->getTable();
	CTime cur = CTime::GetCurrentTime();

	for (int i = 0; i < table.size(); i++) {
		CString dstip, dstmac;
		addrToStr(ARP_IP_TYPE, dstip, table[i].prot_addr);
		addrToStr(ARP_MAC_TYPE, dstmac, table[i].hard_addr);
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

// 네트워크 어뎁터 선택 UI
void CARPDlg::OnCbnSelchangeComboAdapter()
{
	// MAC 주소, IPv4, IPv6 주소를 저장할 CString 변수 선언
	CString MAC, IPV4, IPV6;
	// 현재 선택된 네트워크 어댑터의 MAC 주소 반환 함수(NI Layer 참조)
	unsigned char* macaddr = m_NILayer->SetAdapter(m_ComboxAdapter.GetCurSel());
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
		m_EtherLayer->SetSourceAddress(macaddr);
		// 선택한 어댑터의 IP주소를 가져옴
		m_NILayer->GetIPAddress(IPV4, IPV6);
	}
	// MAC 주소를 UI에 표시
	m_editSrcHwAddr.SetWindowTextW(MAC);
	// IPv4 주소를 UI에 표시
	m_SrcIPADDRESS.SetWindowTextW(IPV4);
}

// 네트워크 어댑터 정보 확정 버튼
void CARPDlg::OnBnClickedButtonSelect()
{
	// MAC 주소와 IP 주소를 저장할 CString 변수 선언
	CString MAC, IP;
	// 현재 UI(app계층)에서 MAC 주소와 IP 주소를 가져옴
	m_editSrcHwAddr.GetWindowTextW(MAC);
	m_SrcIPADDRESS.GetWindowTextW(IP);
	
	// 네트워크 어댑터 선택 콤보 박스가 활성화되어 있는 경우
	if (m_ComboxAdapter.IsWindowEnabled()) {

		// MAC 주소와 IP 주소가 유효한 경우
		if (MAC != DEFAULT_EDIT_TEXT && IP != "0.0.0.0") {

			// 네트워크 어댑터 선택 콤보 박스와 IP 주소 입력 필드를 비활성화 
			// 대상 IP 주소 입력 필드를 활성화
			m_ComboxAdapter.EnableWindow(FALSE);
			m_SrcIPADDRESS.EnableWindow(FALSE);
			m_DstIPADDRESS.EnableWindow(TRUE);

			// NI Layer에서 패킷 수신 상태 변경 (가능<-불가능)
			m_NILayer->Receiveflip();
			
			// 자신의 MAC 주소와 IP 주소 설정
			m_ARPLayer->setmyAddr(MAC, IP);

			// 버튼의 텍스트를 ReSelect로 변경
			CDialog::SetDlgItemTextW(IDC_BUTTON_SELECT, _T("ReSelect"));

			// 1초마다 타이머 실행
			SetTimer(1, 1000, NULL);

			// 데이터 수신을 위한 스레드 실행
			AfxBeginThread(m_NILayer->ThreadFunction_RECEIVE, m_NILayer);
		}
		// MAC 주소나 IP 주소가 유효하지 않은 경우
		else {
			// 다른 어댑터를 선택하라는 메시지 출력
			AfxMessageBox(_T("Select other Adapter"));
		}
	}
	// 네트워크 어댑터 선택 콤보 박스가 비활성화되어 있는 경우 
	else {
		// 대상 IP 주소 입력 필드를 비활성화 
		m_DstIPADDRESS.EnableWindow(FALSE);
		// IP 주소 입력 필드와 네트워크 어댑터 선택 콤보 박스를 활성화
		m_SrcIPADDRESS.EnableWindow(TRUE);
		m_ComboxAdapter.EnableWindow(TRUE);

		// 버튼의 텍스트를 Select로 변경
		CDialog::SetDlgItemTextW(IDC_BUTTON_SELECT, _T("Select"));

		// 타이머 종료
		KillTimer(1);

		// NI Layer에서 패킷 수신 상태 변경 (가능->불가능)
		m_NILayer->Receiveflip();
	}
}

// ARP 요청 보내는 버튼
void CARPDlg::OnBnClickedButtonSendArp()
{
	// 출발지 IP 주소와 목적지 IP 주소를 저장할 배열 선언
	unsigned char srcip[IP_ADDR_SIZE] = {0,}, dstip[IP_ADDR_SIZE] = {0,};

	// 현재 UI(app 계층)에서 소스 IP 주소와 목적지 IP 주소를 가져옴
	m_SrcIPADDRESS.GetAddress(srcip[0], srcip[1], srcip[2], srcip[3]);
	m_DstIPADDRESS.GetAddress(dstip[0], dstip[1], dstip[2], dstip[3]);

	// 목적지 IP 주소 입력 필드가 활성화 
	// 네트워크 어댑터 선택 콤보 박스가 비활성화면 (== 테스트 조건)
	if (m_DstIPADDRESS.IsWindowEnabled() && !m_ComboxAdapter.IsWindowEnabled()) {
		
		// 출발지 IP 주소와 목적지 IP 주소 설정 
		// IP Layer에 전달
		m_IPLayer->SetSourceAddress(srcip);
		m_IPLayer->SetDestinAddress(dstip);

		// 소스 IP 주소와 목적지 IP 주소가 같다면 
		if (memcmp(srcip, dstip, IP_ADDR_SIZE)==0) {

			// 오류 메시지를 표시하고 함수 종료
			AfxMessageBox(_T("Fail : Invalid Address"));
			return;
		}


		// 목적지 IP 주소의 각 바이트를 합한 값이 0이거나 255 * 4이면 
		int check = 0;
		for (int i = 0; i < IP_ADDR_SIZE; i++) {
			check += dstip[i];
		}

		if (check == 0 || check == 255 * 4) {
			
			// 오류 메시지를 표시하고 함수를 종료
			AfxMessageBox(_T("Fail : Invalid Address"));
			return;
		}

		// 하위 레이어(여기서는 IP Layer)로 ARP 요청 전달
		mp_UnderLayer->Send((unsigned char*)"ARP Request", 11);
	}
	// 네트워크 어댑터가 설정되지 않았다면
	else {
		// 오류 메시지를 표시하고 함수 종료
		AfxMessageBox(_T("Fail : Set Adapter first"));
		return;
	}
}


// 목적지 IP 주소 넣는 UI
void CARPDlg::OnIpnFieldchangedIpaddressDst(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 초기값 0
	*pResult = 0;
}


void CARPDlg::OnLvnItemchangedListControl(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

// GARP용 MAC 주소 넣는 곳
void CARPDlg::OnEnChangeEditHwAddr()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// __super::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//20231112 GARP modify
	//CString strMacAddress; 지역 변수 대신 멤버 변수 사용
	GetDlgItemText(IDC_EDIT_HW_ADDR, m_strMacAddress);
}

// GARP 요청 날리는 곳
void CARPDlg::OnBnClickedButtonGArpSend()
{


	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 20231112 GARP modify
	// MAC 주소를 바이트 배열로 변환
	unsigned char mac[MAC_ADDR_SIZE];
	sscanf_s(CW2A(m_strMacAddress.GetString()), "%02x:%02x:%02x:%02x:%02x:%02x", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);

	//////////////////////////////////////////////////////////////////////////////
	// ARP 시나리오를 먼저 진행하면 빼도 되나? 모르겠음
	// 
	// 출발지 IP 주소와 목적지 IP 주소를 저장할 배열 선언
	unsigned char srcip[IP_ADDR_SIZE] = { 0, }, dstip[IP_ADDR_SIZE] = { 0, };

	// 현재 UI(app 계층)에서 소스 IP 주소와 목적지 IP 주소를 가져옴
	m_SrcIPADDRESS.GetAddress(srcip[0], srcip[1], srcip[2], srcip[3]);
	m_DstIPADDRESS.GetAddress(dstip[0], dstip[1], dstip[2], dstip[3]);

	// 출발지 IP 주소와 목적지 IP 주소 설정 
	// IP Layer에 전달
	m_IPLayer->SetSourceAddress(srcip);
	m_IPLayer->SetDestinAddress(dstip);

	///////////////////////////////////////////////////////////////////////////////

	// MAC 주소 설정
	m_EtherLayer->SetSourceAddress(mac);

	// GARP 요청 전달
	mp_UnderLayer->Send((unsigned char*)"GARP Request", 12);
}


void CARPDlg::OnLvnItemchangedListControlProxy(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CARPDlg::OnIpnFieldchangedIpaddressSrc(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}
