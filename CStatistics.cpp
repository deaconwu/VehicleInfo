// CStatistics.cpp: 实现文件
//

#include "pch.h"
#include "VehicleInfo.h"
#include "CStatistics.h"
#include "afxdialogex.h"
#include "CInfoRecord.h"
// #include "WeekDay.h"

// CStatistics 对话框

IMPLEMENT_DYNAMIC(CStatistics, CDialogEx)

static STCIRCLEQUEUE g_circleQue[MAX_VEHICLENUM] = {};

extern long long g_lRecvSizeSum;

CStatistics::CStatistics(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_STATISTICS, pParent)
{

}

CStatistics::~CStatistics()
{
}

void CStatistics::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CStatistics::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	((CEdit*)GetDlgItem(IDC_EDIT_DATAGRAMSUM))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_DATASIZESUM))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLESUM))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEONLINE))->EnableWindow(false);

	//节能减排
	((CEdit*)GetDlgItem(IDC_EDIT_MILEAGESUN))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_CARBONEMISSION))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_FUELTHRIFT))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_POWERCONSUME))->EnableWindow(false);
	
	//当日车辆
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEONLINETODAY))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLERECHARGETODAY))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEFAULTTODAY))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEOFFLINETODAY))->EnableWindow(false);

	//上周车辆
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEONLINELASTWEEK))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLERECHARGELASTWEEK))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEFAULTLASTWEEK))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEOFFLINELASTWEEK))->EnableWindow(false);

	((CEdit*)GetDlgItem(IDC_EDIT_MEMTOTAL))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_MEMFREE))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_VMEMTOTAL))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_VMEMFREE))->EnableWindow(false);
	
	return TRUE;
}

void CStatistics::OnQueryStatis()
{
	if (CInfoSocket::GetInstance()->CheckClose())
	{
		MessageBox(_T("未连接"), _T("提示"), MB_OK | MB_ICONERROR);
		return;
	}

	memset(g_circleQue, 0, sizeof(g_circleQue));
	long vehicleNum = CInfoRecord::GetInstance()->GetQueInfo(g_circleQue);
	long datagramNum = CInfoRecord::GetInstance()->GetDatagramNum();
	CString csStr;

	SYSTEMTIME st;
	GetLocalTime(&st);

	//本周是今年的第几周
// 	int nWeekCur = WeekIndex(st.wYear, st.wMonth, st.wDay);
// 	int nWeekLast = nWeekCur - 1;
// 	if (nWeekCur == 1)
// 		nWeekLast = 53;	//上一年的最后一周

	//报文数
	csStr = _T("");
	csStr.Format(_T("%lu"), datagramNum);
	((CEdit*)GetDlgItem(IDC_EDIT_DATAGRAMSUM))->SetWindowText(csStr);

	//字节数
	csStr = _T("");
	csStr.Format(_T("%lu"), g_lRecvSizeSum);
	((CEdit*)GetDlgItem(IDC_EDIT_DATASIZESUM))->SetWindowText(csStr);

	//接入车辆数
	csStr = _T("");
	csStr.Format(_T("%lu"), vehicleNum);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLESUM))->SetWindowText(csStr);

	//总计
	unsigned long onLineNum = 0;
	unsigned long long mileageSum = 0;

	//今日
	unsigned long onLineNumToday = 0;
	unsigned long offLineNumToday = 0;
	unsigned long rechargeNumToday = 0;
	unsigned long faultNumToday = 0;

	//上周
	unsigned long onLineNumLastWeek = 0;
	unsigned long offLineNumLastWeek = 0;
	unsigned long rechargeNumLastWeek = 0;
	unsigned long faultNumLastWeek = 0;
	unsigned long noneLocateNumLastWeek = 0;

	for (long i = 0; i < vehicleNum; i++)
	{
		if (!g_circleQue[i].pElem)
			continue;

		long rear = g_circleQue[i].rear;
		if (rear == 0)
			continue;

		bool bToday = false;
		if ((st.wYear % 100) == g_circleQue[i].pElem[rear - 1].F8_0[0]
			&& st.wMonth == g_circleQue[i].pElem[rear - 1].F8_0[1]
			&& st.wDay == g_circleQue[i].pElem[rear - 1].F8_0[2])
		{
			bToday = true;
		}

		if (g_circleQue[i].pElem[rear - 1].F1_0 == 1)
		{
			onLineNum += 1;
			if (bToday)
				onLineNumToday += 1;
		}

		if (g_circleQue[i].pElem[rear - 1].F1_0 == 2)
		{
			if (bToday)
				offLineNumToday += 1;
		}

		if (g_circleQue[i].pElem[rear - 1].F1_1 == 1 || g_circleQue[i].pElem[rear - 1].F1_1 == 2)
		{
			if (bToday)
				rechargeNumToday += 1;
		}

		if (g_circleQue[i].pElem[rear - 1].F7_0 > 0)
		{
			if (bToday)
				faultNumToday += 1;
		}

		mileageSum += g_circleQue[i].pElem[rear - 1].F1_4;
		/*
		uint32_t maxValue = 0;
		bool bOnlineTag = false;
		bool bOfflineTag = false;
		bool bRechargeTag = false;
		bool bfaultTag = false;
		for (long j = rear - 1; j >= 0; j--)
		{
			if (g_circleQue[i].pElem[j].F1_4> maxValue)
				maxValue = g_circleQue[i].pElem[j].F1_4;

			uint16_t year = g_circleQue[i].pElem[rear - 1].F8_0[0] % 100 + 2000;
			uint16_t month = g_circleQue[i].pElem[rear - 1].F8_0[1];
			uint16_t day = g_circleQue[i].pElem[rear - 1].F8_0[2];
			int nWeek = WeekIndex(st.wYear, st.wMonth, st.wDay);

			if (nWeek == nWeekLast)
			{
				if (g_circleQue[i].pElem[j].F1_0 == 1 && !bOnlineTag)
				{
					onLineNumLastWeek += 1;
					bOnlineTag = true;
				}

				if (g_circleQue[i].pElem[j].F1_0 == 2 && !bOfflineTag)
				{
					offLineNumLastWeek += 1;
					bOfflineTag = true;
				}

				if ((g_circleQue[i].pElem[j].F1_1 == 1 || g_circleQue[i].pElem[j].F1_1 == 2) && !bRechargeTag)
				{
					rechargeNumLastWeek += 1;
					bRechargeTag = true;
				}

				if (g_circleQue[i].pElem[rear - 1].F7_0 > 0 && !bfaultTag)
				{
					faultNumLastWeek += 1;
					bfaultTag = true;
				}
			}
		}
		mileageSum += maxValue;*/
	}

	//在线车辆数
	csStr = _T("");
	csStr.Format(_T("%lu"), onLineNum);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEONLINE))->SetWindowText(csStr);

	//累计行驶里程
	csStr = _T("");
	csStr.Format(_T("%.5f"), (long double)mileageSum / 10 / 10000);
	//csStr.Append(_T(" 万公里"));
	((CEdit*)GetDlgItem(IDC_EDIT_MILEAGESUN))->SetWindowText(csStr);

	//累计碳减排
	csStr = _T("");
	csStr.Format(_T("%.4f"), (0.0031 / 6) * ((long double)mileageSum / 10 / 10000));
	//csStr.Append(_T(" 万升"));
	((CEdit*)GetDlgItem(IDC_EDIT_CARBONEMISSION))->SetWindowText(csStr);

	//累计节油量
	csStr = _T("");
	csStr.Format(_T("%.2f"), (1.75 / 6) * ((long double)mileageSum / 10 / 10000));
	//csStr.Append(_T(" 万升"));
	((CEdit*)GetDlgItem(IDC_EDIT_FUELTHRIFT))->SetWindowText(csStr);

	//累计耗电量
	csStr = _T("");
	csStr.Format(_T("%.2f"), (5.68 / 6) * ((long double)mileageSum / 10 / 10000));
	//csStr.Append(_T(" 万千瓦时"));
	((CEdit*)GetDlgItem(IDC_EDIT_POWERCONSUME))->SetWindowText(csStr);

	//今日在线车辆数
	csStr = _T("");
	csStr.Format(_T("%lu"), onLineNumToday);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEONLINETODAY))->SetWindowText(csStr);

	//今日充电车辆数
	csStr = _T("");
	csStr.Format(_T("%lu"), rechargeNumToday);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLERECHARGETODAY))->SetWindowText(csStr);

	//今日故障车辆数
	csStr = _T("");
	csStr.Format(_T("%lu"), faultNumToday);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEFAULTTODAY))->SetWindowText(csStr);

	//今日离线车辆数
	csStr = _T("");
	csStr.Format(_T("%lu"), offLineNumToday);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEOFFLINETODAY))->SetWindowText(csStr);

	//上周在线车辆数
	csStr = _T("");
	csStr.Format(_T("%lu"), onLineNumLastWeek);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEONLINELASTWEEK))->SetWindowText(csStr);

	//上周充电车辆数
	csStr = _T("");
	csStr.Format(_T("%lu"), rechargeNumLastWeek);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLERECHARGELASTWEEK))->SetWindowText(csStr);

	//上周故障车辆数
	csStr = _T("");
	csStr.Format(_T("%lu"), faultNumLastWeek);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEFAULTLASTWEEK))->SetWindowText(csStr);

	//上周离线车辆数
	csStr = _T("");
	csStr.Format(_T("%lu"), offLineNumLastWeek);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEOFFLINELASTWEEK))->SetWindowText(csStr);
}

BEGIN_MESSAGE_MAP(CStatistics, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CStatistics::OnBnClickedBtnRefresh)
END_MESSAGE_MAP()


// CStatistics 消息处理程序


void CStatistics::OnBnClickedBtnRefresh()
{
	// TODO: 在此添加控件通知处理程序代码
	OnQueryStatis();

	MEMORYSTATUS mem;//定义一个内存状态变量
	SIZE_T MemTotal, MemFree, VMemTotal, VMemFree;//存储内存状态信息

	GlobalMemoryStatus(&mem); //存储内存状态信息
	MemTotal = mem.dwTotalPhys;//得到总内存大小（单位b）
	MemFree = mem.dwAvailPhys;//得到剩余内存大小（单位b）
	VMemTotal = mem.dwTotalVirtual;//得到总虚拟内存大小（单位b）
	VMemFree = mem.dwAvailVirtual;//得到剩余虚拟内存内存大小（单位b）

	CString csStr;
	csStr = _T("");
	csStr.Format(_T("%lu"), MemTotal);
	((CEdit*)GetDlgItem(IDC_EDIT_MEMTOTAL))->SetWindowText(csStr);

	csStr = _T("");
	csStr.Format(_T("%lu"), MemFree);
	((CEdit*)GetDlgItem(IDC_EDIT_MEMFREE))->SetWindowText(csStr);

	csStr = _T("");
	csStr.Format(_T("%lu"), VMemTotal);
	((CEdit*)GetDlgItem(IDC_EDIT_VMEMTOTAL))->SetWindowText(csStr);

	csStr = _T("");
	csStr.Format(_T("%lu"), VMemFree);
	((CEdit*)GetDlgItem(IDC_EDIT_VMEMFREE))->SetWindowText(csStr);
}
