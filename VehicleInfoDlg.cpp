﻿
// VehicleInfoDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "VehicleInfo.h"
#include "VehicleInfoDlg.h"
#include "afxdialogex.h"
#include "CInfoRecord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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
	ON_MESSAGE(UM_STATISTIC, &CVehicleInfoDlg::OnUpdateStatistic)
	ON_MESSAGE(UM_ALERT, &CVehicleInfoDlg::OnRealAlert)
END_MESSAGE_MAP()

LRESULT CVehicleInfoDlg::OnUpdateStatistic(WPARAM wParam, LPARAM lParam)
{
	CInfoRecord::GetInstance()->OnStatistic();
	return 0;
}

LRESULT CVehicleInfoDlg::OnRealAlert(WPARAM wParam, LPARAM lParam)
{
	uint32_t iF7_0 = wParam;
	uint8_t* pVinStr = (uint8_t*)lParam;
	m_alertData.UpdateAlert(iF7_0, pVinStr);

	return 0;
}

// CVehicleInfoDlg 消息处理程序

BOOL CVehicleInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CInfoRecord::GetInstance()->OnRealTimeRecv(this->m_hWnd);

	m_query.Create(IDD_QUERY, &m_tab);
	m_statistics.Create(IDD_STATISTICS, &m_tab);
	m_alertData.Create(IDD_ALERTDATA, &m_tab);

	CRect tabRect;
	m_tab.GetClientRect(tabRect);
	tabRect.left += 1;
	tabRect.right -= 1;
	tabRect.top += 22;
	tabRect.bottom -= 1;
	m_query.MoveWindow(&tabRect);
	m_statistics.MoveWindow(&tabRect);
	m_alertData.MoveWindow(&tabRect);

	m_tab.InsertItem(0, _T("车辆查询"));
	m_tab.InsertItem(1, _T("基础统计"));
	m_tab.InsertItem(2, _T("实时报警"));

	m_query.ShowWindow(SW_SHOW);
	m_statistics.ShowWindow(SW_HIDE);
	m_alertData.ShowWindow(SW_HIDE);

	m_tab.SetCurSel(0);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CVehicleInfoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CVehicleInfoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CVehicleInfoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CVehicleInfoDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	switch (m_tab.GetCurSel())
	{
	case 0:
		m_statistics.ShowWindow(SW_HIDE);
		m_alertData.ShowWindow(SW_HIDE);
		m_query.ShowWindow(SW_SHOW);
		break;
	case 1:
		m_statistics.ShowWindow(SW_SHOW);
		m_query.ShowWindow(SW_HIDE);
		m_alertData.ShowWindow(SW_HIDE);
		m_statistics.OnQueryStatis();
		break;
	case 2:
		m_statistics.ShowWindow(SW_HIDE);
		m_query.ShowWindow(SW_HIDE);
		m_alertData.ShowWindow(SW_SHOW);
	default:
		break;
	}
}
