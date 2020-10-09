// CAlertRank.cpp: 实现文件
//

#include "pch.h"
#include "VehicleInfo.h"
#include "CAlertRank.h"
#include "CInfoRecord.h"
#include "CAlertStats.h"
#include "UserMessage.h"
#include "afxdialogex.h"

// CAlertRank 对话框

IMPLEMENT_DYNAMIC(CAlertRank, CDialogEx)

static STCIRCLEQUEUE g_circleQue[MAX_VEHICLENUM] = {};
static uint8_t g_chVin[MAX_VEHICLENUM][VIN_LENGTH + 1];
static long g_vehicleNum = 0;

static long FindVinPos(uint8_t pVin[])
{
	long left = 0;
	long right = g_vehicleNum - 1;
	long mid = -1;

	while (left <= right)
	{
		mid = (left + right) / 2;
		if (memcmp((char*)pVin, (char*)g_chVin[mid], VIN_LENGTH) < 0)
		{
			right = mid - 1;
		}
		else if (memcmp((char*)pVin, (char*)g_chVin[mid], VIN_LENGTH) > 0)
		{
			left = mid + 1;
		}
		else
		{
			return mid;
		}
	}

	return -1;
}

CAlertRank::CAlertRank(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ALERTRANK, pParent)
{
	m_pDlgAlertCategory = NULL;
}

CAlertRank::~CAlertRank()
{
}

BOOL CAlertRank::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_cbRankTypeChoice.InsertString(0, _T("所有报警"));
	m_cbRankTypeChoice.InsertString(1, _T("温度差异报警"));
	m_cbRankTypeChoice.InsertString(2, _T("电池高温报警"));
	m_cbRankTypeChoice.InsertString(3, _T("车载储能装置类型过压报警"));
	m_cbRankTypeChoice.InsertString(4, _T("车载储能装置类型欠压报警"));
	m_cbRankTypeChoice.InsertString(5, _T("SOC低报警"));
	m_cbRankTypeChoice.InsertString(6, _T("单体电池过压报警"));
	m_cbRankTypeChoice.InsertString(7, _T("单体电池欠压报警"));
	m_cbRankTypeChoice.InsertString(8, _T("SOC过高报警"));
	m_cbRankTypeChoice.InsertString(9, _T("SOC跳变报警"));
	m_cbRankTypeChoice.InsertString(10, _T("可充电储能系统不匹配报警"));
	m_cbRankTypeChoice.InsertString(11, _T("电池单体一致性差报警"));
	m_cbRankTypeChoice.InsertString(12, _T("绝缘报警"));
	m_cbRankTypeChoice.InsertString(13, _T("DC-DC温度报警"));
	m_cbRankTypeChoice.InsertString(14, _T("制动系统报警"));
	m_cbRankTypeChoice.InsertString(15, _T("DC-DC状态报警"));
	m_cbRankTypeChoice.InsertString(16, _T("驱动电池控制器温度报警"));
	m_cbRankTypeChoice.InsertString(17, _T("高压互锁状态报警"));
	m_cbRankTypeChoice.InsertString(18, _T("驱动电机温度报警"));
	m_cbRankTypeChoice.InsertString(19, _T("车载储能装置类型过充报警"));
	m_cbRankTypeChoice.SetCurSel(0);

	m_listRank.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_listRank.InsertColumn(1, _T("名次"), LVCFMT_CENTER, 50);
	m_listRank.InsertColumn(2, _T("Vin码"), LVCFMT_CENTER, 150);
	m_listRank.InsertColumn(3, _T("报警次数"), LVCFMT_CENTER, 120);

	CString csStr;
	for (int i = 1; i <= ALERTTIMES_REANK_NUM; i++)
	{
		csStr.Format(_T("%u"), i);
		m_listRank.InsertItem(i-1, csStr);
		m_listRank.SetItemText(i-1, 0, csStr);
		m_listRank.SetItemState(i, ~LVIS_SELECTED, LVIS_SELECTED);
	}

	return TRUE;
}

void CAlertRank::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_RANKTYPE, m_cbRankTypeChoice);
	DDX_Control(pDX, IDC_LIST_RANK, m_listRank);
}


BEGIN_MESSAGE_MAP(CAlertRank, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_RANKTYPE, &CAlertRank::OnCbnSelchangeComboRanktype)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RANK, &CAlertRank::OnLvnItemchangedListRank)
	ON_MESSAGE(UM_ALERTRANK, &CAlertRank::OnAlertStatsRank)
END_MESSAGE_MAP()


// CAlertRank 消息处理程序


void CAlertRank::OnCbnSelchangeComboRanktype()
{
	// TODO: 在此添加控件通知处理程序代码
	if (NULL != m_pDlgAlertCategory)
	{
		delete m_pDlgAlertCategory;
		m_pDlgAlertCategory = NULL;
	}

	int iIndex = m_cbRankTypeChoice.GetCurSel();

	CAlertStats::GetInstance()->NotifyType(iIndex);
}

void CAlertRank::RankLaunch()
{
	CAlertStats::GetInstance()->OnLaunchAlertRank(this->m_hWnd, -1);
}

LRESULT CAlertRank::OnAlertStatsRank(WPARAM wParam, LPARAM lParam)
{
	STMSGALERTRANKSEQ* pMsg = (STMSGALERTRANKSEQ*)wParam;
	
	for (int i = 0; i < ALERTTIMES_REANK_NUM; i++)
	{
		m_listRank.SetItemText(i, 1, _T(""));
		m_listRank.SetItemText(i, 2, _T(""));

		if (i < pMsg->iNum)
		{
			CString csStr(pMsg->stNode[i].chVin);
			m_listRank.SetItemText(i, 1, csStr);

			csStr.Format(_T("%u"), pMsg->stNode[i].iAlertTimes);
			m_listRank.SetItemText(i, 2, csStr);
		}
	}

	return 0;
}

void CAlertRank::OnLvnItemchangedListRank(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	CString strVin = L"";
	for (int i = 0; i < m_listRank.GetItemCount(); i++)
	{
		if (m_listRank.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			//str.Format(_T("选中了第%d行"), i);
			//AfxMessageBox(str);
			//m_listRank.SetCheck(i, FALSE);
			m_listRank.SetItemState(i, 0, LVIS_SELECTED | LVIS_FOCUSED);
			strVin = m_listRank.GetItemText(i, 1);
			if (strVin.IsEmpty())
				return;

			char chVin[VIN_LENGTH + 1] = {};

			int iLength = WideCharToMultiByte(CP_ACP, 0, strVin, -1, NULL, 0, NULL, NULL);
			WideCharToMultiByte(CP_ACP, 0, strVin, -1, chVin, iLength, NULL, NULL);

			if (NULL != m_pDlgAlertCategory)
			{
				delete m_pDlgAlertCategory;
				m_pDlgAlertCategory = NULL;
			}

			m_pDlgAlertCategory = new CAlertCategory;
			m_pDlgAlertCategory->Create(IDD_ALERTCATEGORY, this);
			m_pDlgAlertCategory->ShowWindow(SW_SHOW);

			CString csStr(chVin);
			m_pDlgAlertCategory->SetTitle(csStr);

			m_pDlgAlertCategory->OnLauch(chVin);

			return;
		}
	}
}

void CAlertRank::DestroySubDlg()
{
	if (NULL != m_pDlgAlertCategory)
	{
		CAlertStats::GetInstance()->OnStopAlertCategory();
		delete m_pDlgAlertCategory;
		m_pDlgAlertCategory = NULL;
	}

	CAlertStats::GetInstance()->OnStopAlertRank();
}
