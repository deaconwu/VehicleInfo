// CAlertCategory.cpp: 实现文件
//

#include "pch.h"
#include "VehicleInfo.h"
#include "CAlertCategory.h"
#include "CAlertStats.h"
#include "UserMessage.h"
#include "afxdialogex.h"

// CAlertCategory 对话框

IMPLEMENT_DYNAMIC(CAlertCategory, CDialogEx)

CString g_strAlertType[20] = { _T("所有报警"), _T("温度差异报警"), _T("电池高温报警"), _T("车载储能装置类型过压报警"),
							  _T("车载储能装置类型欠压报警"), _T("SOC低报警"), _T("单体电池过压报警"), _T("单体电池欠压报警"),
							  _T("SOC过高报警"), _T("SOC跳变报警"), _T("可充电储能系统不匹配报警"), _T("电池单体一致性差报警"),
							  _T("绝缘报警"), _T("DC-DC温度报警"), _T("制动系统报警"), _T("DC-DC状态报警"), _T("驱动电池控制器温度报警"),
							  _T("高压互锁状态报警"), _T("驱动电机温度报警"), _T("车载储能装置类型过充报警") };

CAlertCategory::CAlertCategory(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ALERTCATEGORY, pParent)
{

}

CAlertCategory::~CAlertCategory()
{
	CAlertStats::GetInstance()->OnStopAlertCategory();
}

void CAlertCategory::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CATEGORY, m_listCategory);
}

BOOL CAlertCategory::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(_T("车辆报警分类统计"));

	m_listCategory.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_listCategory.InsertColumn(1, _T("类别"), LVCFMT_CENTER, 230);
	m_listCategory.InsertColumn(2, _T("次数"), LVCFMT_CENTER, 100);
	m_listCategory.InsertColumn(3, _T("排名"), LVCFMT_CENTER, 50);

	for (int i = 0; i < 20; i++)
	{
		m_listCategory.InsertItem(i, g_strAlertType[i]);
		m_listCategory.SetItemText(i, 0, g_strAlertType[i]);
		m_listCategory.SetItemText(i, 1, _T("0"));
		m_listCategory.SetItemText(i, 2, _T("0"));
	}

	return TRUE;
}

void CAlertCategory::SetTitle(CString csStr)
{
	((CStatic*)GetDlgItem(IDC_TITLE))->SetWindowText(_T("车辆Vin码: ")+csStr);
}

void CAlertCategory::OnLauch(char chVin[])
{
	CAlertStats::GetInstance()->OnLaunchAlertCategory(this->m_hWnd, (uint8_t*)chVin);
}

void CAlertCategory::SetTypeTimesRank(int iType, uint32_t iTimes, uint32_t iRank)
{
	CString csStr;
	csStr.Format(_T("%u"), iTimes);
	m_listCategory.SetItemText(iType+1, 1, csStr);
	csStr.Format(_T("%u"), iRank);
	m_listCategory.SetItemText(iType+1, 2, csStr);
}

BEGIN_MESSAGE_MAP(CAlertCategory, CDialogEx)
	ON_MESSAGE(UM_ALERTCATEGORY, &CAlertCategory::OnAlertStatsCategory)
END_MESSAGE_MAP()

// CAlertCategory 消息处理程序


LRESULT CAlertCategory::OnAlertStatsCategory(WPARAM wParam, LPARAM lParam)
{
	STMSGALERTCATEGORY* pMsg = (STMSGALERTCATEGORY*)wParam;

	for (int i = 0; i < ALERT_CATEGORY_NUM; i++)
	{
		SetTypeTimesRank(i-1, pMsg->iAlertTimes[i], pMsg->iRank[i]);
	}

	return 0;
}