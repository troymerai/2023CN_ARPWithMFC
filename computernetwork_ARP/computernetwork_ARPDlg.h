
// computernetwork_ARPDlg.h: 헤더 파일
//

#pragma once


// CcomputernetworkARPDlg 대화 상자
class CcomputernetworkARPDlg : public CDialogEx
{
// 생성입니다.
public:
	CcomputernetworkARPDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COMPUTERNETWORK_ARP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedMfcshelllist1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnCbnSelchangeComboboxex1();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedBntListAdd();
	afx_msg void OnBnClickedBntArpDel();
	afx_msg void OnBnClickedBntListAllDel();
	afx_msg void OnIpnFieldchangedIpaddress1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLbnSelchangeList8();
	afx_msg void OnBnClickedBntArpAdd();
	afx_msg void OnLbnSelchangeList();
	afx_msg void OnLbnSelchangeListArp();
	CListCtrl m_list_arp_cache;
	afx_msg void OnBnClickedBntListItemDel();
	CIPAddressCtrl m_ip_address;
	afx_msg void OnBnClickedBntSend();
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
};
