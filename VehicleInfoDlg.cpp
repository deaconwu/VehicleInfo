
// VehicleInfoDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "VehicleInfo.h"
#include "VehicleInfoDlg.h"
#include "afxdialogex.h"
#include "CAlertStats.h"
#include "CInfoRecord.h"
#include "CHistoryRecord.h"
#include "UserMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVehicleInfoDlg 对话框
CVehicleInfoDlg::CVehicleInfoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VEHICLEINFO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVehicleInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
}

BEGIN_MESSAGE_MAP(CVehicleInfoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CVehicleInfoDlg::OnTcnSelchangeTab1)
	ON_MESSAGE(UM_ALERT, &CVehicleInfoDlg::OnRealAlert)
	ON_MESSAGE(UM_STOPRECV, &CVehicleInfoDlg::OnRealStopRecv)
	ON_MESSAGE(UM_STOPPARSE, &CVehicleInfoDlg::OnRealStopParse)
	ON_MESSAGE(UM_HISTORY, &CVehicleInfoDlg::OnHistoryRecord)
	ON_BN_CLICKED(IDC_BTN_CONNECT, &CVehicleInfoDlg::OnBnClickedBtnConnect)
	ON_BN_CLICKED(IDC_BTN_DISCONNECT, &CVehicleInfoDlg::OnBnClickedBtnDisconnect)
END_MESSAGE_MAP()

LRESULT CVehicleInfoDlg::OnRealAlert(WPARAM wParam, LPARAM lParam)
{
	STALERTDATAPOST* pAlertPost = (STALERTDATAPOST*)wParam;
	CString vinStr(pAlertPost->chVin);
	m_alertData.UpdateAlert(pAlertPost->F7_0, vinStr);

	return 0;
}

LRESULT CVehicleInfoDlg::OnRealStopRecv(WPARAM wParam, LPARAM lParam)
{
	CInfoRecord::GetInstance()->OnClearDataGram();

	return 0;
}

LRESULT CVehicleInfoDlg::OnRealStopParse(WPARAM wParam, LPARAM lParam)
{
	CInfoRecord::GetInstance()->OnReset();

	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS))->EnableWindow(true);
	((CEdit*)GetDlgItem(IDC_EDIT_PORT))->EnableWindow(true);
	((CButton*)GetDlgItem(IDC_BTN_DISCONNECT))->EnableWindow(false);
	((CButton*)GetDlgItem(IDC_BTN_CONNECT))->EnableWindow(true);

	return 0;
}

LRESULT CVehicleInfoDlg::OnHistoryRecord(WPARAM wParam, LPARAM lParam)
{
	WORD* pDayOfWeek = (WORD*)wParam;
	CHistoryRecord::GetInstance()->OnRecord(*pDayOfWeek);

	return 0;
}

// CVehicleInfoDlg 消息处理程序

BOOL CVehicleInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	//SetIcon(m_hIcon, TRUE);			// 设置大图标
	//SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetWindowText(_T("车辆信息"));

 	m_query.Create(IDD_QUERY, &m_tab);
	m_statistics.Create(IDD_STATISTICS, &m_tab);
	m_alertData.Create(IDD_ALERTDATA, &m_tab);
	m_alertRank.Create(IDD_ALERTRANK, &m_tab);

	CRect tabRect;
	m_tab.GetClientRect(tabRect);
	tabRect.left += 1;
	tabRect.right -= 1;
	tabRect.top += 22;
	tabRect.bottom -= 1;
	m_query.MoveWindow(&tabRect);
	m_statistics.MoveWindow(&tabRect);
	m_alertData.MoveWindow(&tabRect);
	m_alertRank.MoveWindow(&tabRect);

	m_tab.InsertItem(0, _T("车辆查询"));
	m_tab.InsertItem(1, _T("基础统计"));
	m_tab.InsertItem(2, _T("实时报警"));
	m_tab.InsertItem(3, _T("报警排名"));

	m_query.ShowWindow(SW_SHOW);
	m_statistics.ShowWindow(SW_HIDE);
	m_alertData.ShowWindow(SW_HIDE);
	m_alertRank.ShowWindow(SW_HIDE);

	m_tab.SetCurSel(0);

	((CButton*)GetDlgItem(IDC_BTN_CONNECT))->EnableWindow(true);
	((CButton*)GetDlgItem(IDC_BTN_DISCONNECT))->EnableWindow(false);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
/*
void CVehicleInfoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		//SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}*/

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
//HCURSOR CVehicleInfoDlg::OnQueryDragIcon()
//{
//	return static_cast<HCURSOR>(m_hIcon);
//}

void CVehicleInfoDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	switch (m_tab.GetCurSel())
	{
	case 0:
		m_query.ShowWindow(SW_SHOW);
		m_statistics.ShowWindow(SW_HIDE);
		m_alertData.ShowWindow(SW_HIDE);
		m_alertRank.ShowWindow(SW_HIDE);
		m_alertRank.DestroySubDlg();
		break;
	case 1:
		m_query.ShowWindow(SW_HIDE);
		m_statistics.ShowWindow(SW_SHOW);
		m_alertData.ShowWindow(SW_HIDE);
		m_alertRank.ShowWindow(SW_HIDE);
		m_alertRank.DestroySubDlg();
		m_statistics.OnQueryStatis();
		break;
	case 2:
		m_statistics.ShowWindow(SW_HIDE);
		m_query.ShowWindow(SW_HIDE);
		m_alertData.ShowWindow(SW_SHOW);
		m_alertRank.ShowWindow(SW_HIDE);
		m_alertRank.DestroySubDlg();
		break;
	case 3:
		m_statistics.ShowWindow(SW_HIDE);
		m_query.ShowWindow(SW_HIDE);
		m_alertData.ShowWindow(SW_HIDE);
		m_alertRank.ShowWindow(SW_SHOW);
		//m_alertRank.RankSort();
		m_alertRank.RankLaunch();
		break;
	default:
		break;
	}
}


void CVehicleInfoDlg::OnBnClickedBtnConnect()
{
	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	CIPAddressCtrl* pIPAddr = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS);
	pIPAddr->GetAddress(serAddr.sin_addr.S_un.S_un_b.s_b1, serAddr.sin_addr.S_un.S_un_b.s_b2, serAddr.sin_addr.S_un.S_un_b.s_b3, serAddr.sin_addr.S_un.S_un_b.s_b4);

	CString portStr;
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_PORT);
	pEdit->GetWindowText(portStr);
	int iPort = _ttoi(portStr);
	serAddr.sin_port = htons(iPort);

	if (!CInfoRecord::GetInstance()->OnRealTimeRecv(this->m_hWnd, serAddr))
	{
		MessageBox(_T("连接失败"), _T("提示"), MB_OK | MB_ICONERROR);
		return;
	}

	((CButton*)GetDlgItem(IDC_BTN_CONNECT))->EnableWindow(false);
	((CButton*)GetDlgItem(IDC_BTN_DISCONNECT))->EnableWindow(true);
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_PORT))->EnableWindow(false);
}


void CVehicleInfoDlg::OnBnClickedBtnDisconnect()
{
 	CAlertStats::GetInstance()->OnStopAlertRank();
 	CAlertStats::GetInstance()->OnStopAlertCategory();
 	CInfoRecord::GetInstance()->OnStopRecv();
}
