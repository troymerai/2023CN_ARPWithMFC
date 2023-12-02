// CDeviceAdd.cpp: 구현 파일
//

////////////////////////////////////////////////////
//
// Proxy ARP cache Table에 추가
//
//
////////////////////////////////////////////////////


#include "pch.h"
#include "ARP.h"
#include "afxdialogex.h"
#include "CDeviceAdd.h"


// CDeviceAdd 대화 상자


// CDeviceAdd 클래스를 동적으로 생성하는 매크로
IMPLEMENT_DYNAMIC(CDeviceAdd, CDialogEx)


// CDeviceAdd 클래스의 생성자
CDeviceAdd::CDeviceAdd(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_DEVICE_ADD, pParent)
{

}

// 소멸자
CDeviceAdd::~CDeviceAdd()
{
}

// 데이터를 교환하는 함수
// 컨트롤 변수에 대한 동기화를 수행
void CDeviceAdd::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_COMBO_DEVICE_LIST, m_DeviceList);
	DDX_Control(pDX, IDC_EDIT_DEVICE_ETHERNET_ADDRESS, m_editDeviceEthernetAddr);
	DDX_Control(pDX, IDC_IPADDRESS_DEVICE_ADD, m_IPADDRESS_DEVICE_ADD);
	CDialogEx::DoDataExchange(pDX);
}

// 메시지 맵을 시작하는 매크로
BEGIN_MESSAGE_MAP(CDeviceAdd, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDeviceAdd::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDeviceAdd::OnBnClickedCancel)
	ON_CBN_CLOSEUP(IDC_COMBO_DEVICE_LIST, &CDeviceAdd::OnCbnCloseupComboDeviceList)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICE_LIST, &CDeviceAdd::OnCbnSelchangeComboDeviceList)
END_MESSAGE_MAP()


// CDeviceAdd 메시지 처리기


// 대화 상자가 초기화되면 호출되는 함수
BOOL CDeviceAdd::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetAdapterList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


// 대화 상자를 초기화하는 함수
void CDeviceAdd::InitDeviceAddDlg(int nTemp)	//nTemp는 사용은 안하지만 추가해놨습니다	
{
	SetAdapterList();
	
	if (nTemp >= 0) {
		m_DeviceList.DeleteString(nTemp);
		m_DeviceList.InsertString(nTemp, _T("Default"));
		m_DeviceList.SetCurSel(nTemp);
	}

	m_IPADDRESS_DEVICE_ADD.SetWindowTextW(_T(""));
	m_editDeviceEthernetAddr.SetWindowTextW(_T(""));
}

// 어댑터 목록을 설정하는 함수
void CDeviceAdd::SetAdapterList()
{
	m_DeviceList.ResetContent();
	theApp.MainDlg->m_NILayer->SetAdapterComboBox(m_DeviceList);
}

// UCHAR를 CString으로 변환하는 함수
void CDeviceAdd::UctoS(UCHAR* src, CString& dst)
{
	dst.Format(_T("%02x:%02x:%02x:%02x:%02x:%02x"),
		&src[0], &src[1], &src[2],
		&src[3], &src[4], &src[5]);
}

// 확인 버튼을 클릭했을 때 호출되는 함수
void CDeviceAdd::OnBnClickedOk()
{
	CString mMAC;
	UCHAR ip[IP_ADDR_SIZE] = { 0, };
	UCHAR mac[ENET_ADDR_SIZE] = { 0, };
	
	// IP와 MAC 주소를 가져옴
	m_IPADDRESS_DEVICE_ADD.GetAddress(ip[0], ip[1], ip[2], ip[3]);
	m_editDeviceEthernetAddr.GetWindowTextW(mMAC);
	StrToaddr(ARP_ENET_TYPE, mac, mMAC);

	// Proxy ARP 캐시에 추가
	theApp.MainDlg->AddProxyArpCache(m_DeviceList.GetCurSel(), ip, mac);
	
	CDialogEx::OnOK();
}

// 취소 버튼을 클릭했을 때 호출되는 함수
void CDeviceAdd::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


void CDeviceAdd::OnCbnCloseupComboDeviceList()
{
	//int nIndex = m_DeviceList.GetCurSel();
	//memcpy(m_ucSrcAddr, theApp.MainDlg->m_Network->SetAdapter(nIndex), 6);

	//UctoS(m_ucSrcAddr, m_unSrcAddr);
	//m_editDeviceEthernetAddr.SetWindowTextW(m_unSrcAddr);
}


void CDeviceAdd::OnCbnSelchangeComboDeviceList()
{
	//CString IPv4, IPv6;
	//theApp.MainDlg->m_NILayer->GetIPAddress(IPv4, IPv6, m_DeviceList.GetCurSel());
	//m_IPADDRESS_DEVICE_ADD.SetWindowTextW(IPv4);
}
