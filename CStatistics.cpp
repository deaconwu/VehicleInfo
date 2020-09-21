// CStatistics.cpp: 实现文件
//

#include "pch.h"
#include "VehicleInfo.h"
#include "CStatistics.h"
#include "afxdialogex.h"
#include "CInfoRecord.h"
#include "WeekDay.h"

// CStatistics 对话框

IMPLEMENT_DYNAMIC(CStatistics, CDialogEx)

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
	
	(CEdit*)GetDlgItem(IDC_EDIT_VEHICLESUM)->EnableWindow(false);
	(CEdit*)GetDlgItem(IDC_EDIT_VEHICLEONLINE)->EnableWindow(false);

	//节能减排
	(CEdit*)GetDlgItem(IDC_EDIT_MILEAGESUN)->EnableWindow(false);
	(CEdit*)GetDlgItem(IDC_EDIT_CARBONEMISSION)->EnableWindow(false);
	(CEdit*)GetDlgItem(IDC_EDIT_FUELTHRIFT)->EnableWindow(false);
	(CEdit*)GetDlgItem(IDC_EDIT_POWERCONSUME)->EnableWindow(false);
	
	//当日车辆
	(CEdit*)GetDlgItem(IDC_EDIT_VEHICLEONLINETODAY)->EnableWindow(false);
	(CEdit*)GetDlgItem(IDC_EDIT_VEHICLERECHARGETODAY)->EnableWindow(false);
	(CEdit*)GetDlgItem(IDC_EDIT_VEHICLEFAULTTODAY)->EnableWindow(false);
	(CEdit*)GetDlgItem(IDC_EDIT_VEHICLEOFFLINETODAY)->EnableWindow(false);

	//上周车辆
	(CEdit*)GetDlgItem(IDC_EDIT_VEHICLEONLINELASTWEEK)->EnableWindow(false);
	(CEdit*)GetDlgItem(IDC_EDIT_VEHICLERECHARGELASTWEEK)->EnableWindow(false);
	(CEdit*)GetDlgItem(IDC_EDIT_VEHICLEFAULTLASTWEEK)->EnableWindow(false);
	(CEdit*)GetDlgItem(IDC_EDIT_VEHICLEOFFLINELASTWEEK)->EnableWindow(false);
	
	return TRUE;
}

void CStatistics::OnQueryStatis()
{
	STCIRCLEQUEUE circleQue[20000];
	long vehicleNum = CInfoRecord::GetInstance()->GetQueInfo(circleQue);
	CString csStr;

	SYSTEMTIME st;
	GetLocalTime(&st);

	//本周是今年的第几周
	int nWeekCur = WeekIndex(st.wYear, st.wMonth, st.wDay);
	int nWeekLast = nWeekCur - 1;
	if (nWeekCur == 1)
		nWeekLast = 53;	//上一年的最后一周

	//接入车辆数
	csStr = _T("");
	csStr.Format(_T("%lu"), vehicleNum);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLESUM))->SetWindowTextW(csStr);

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
		long rear = circleQue[i].rear;

		bool bToday = false;
		if ((st.wYear%100) == circleQue[i].pElem[rear - 1].F8_0[0]
			&& st.wMonth == circleQue[i].pElem[rear - 1].F8_0[1]
			&& st.wDay == circleQue[i].pElem[rear - 1].F8_0[2])
		{
			bToday = true;
		}

		if (circleQue[i].pElem[rear - 1].F1_0 == 1)
		{
			onLineNum += 1;
			if (bToday)
				onLineNumToday += 1;
		}

		if (circleQue[i].pElem[rear - 1].F1_0 == 2)
		{
			if (bToday)
				offLineNumToday += 1;
		}

		if (circleQue[i].pElem[rear - 1].F1_1 == 1 || circleQue[i].pElem[rear - 1].F1_1 == 2)
		{
			if (bToday)
				rechargeNumToday += 1;
		}

		if (circleQue[i].pElem[rear - 1].F7_0 > 0)
		{
			if (bToday)
				faultNumToday += 1;
		}

		uint32_t maxValue = 0;
		bool bOnlineTag = false;
		bool bOfflineTag = false;
		bool bRechargeTag = false;
		bool bfaultTag = false;
		for (long j = rear - 1; j >= 0; j--)
		{
			if (circleQue[i].pElem[j].F1_4> maxValue)
				maxValue = circleQue[i].pElem[j].F1_4;

			uint16_t year = circleQue[i].pElem[rear - 1].F8_0[0] % 100 + 2000;
			uint16_t month = circleQue[i].pElem[rear - 1].F8_0[1];
			uint16_t day = circleQue[i].pElem[rear - 1].F8_0[2];
			int nWeek = WeekIndex(st.wYear, st.wMonth, st.wDay);
			
			if (nWeek == nWeekLast)
			{
				if (circleQue[i].pElem[j].F1_0 == 1 && !bOnlineTag)
				{
					onLineNumLastWeek += 1;
					bOnlineTag = true;
				}

				if (circleQue[i].pElem[j].F1_0 == 2 && !bOfflineTag)
				{
					offLineNumLastWeek += 1;
					bOfflineTag = true;
				}

				if ((circleQue[i].pElem[j].F1_1 == 1 || circleQue[i].pElem[j].F1_1 == 2) && !bRechargeTag)
				{
					rechargeNumLastWeek += 1;
					bRechargeTag = true;
				}

				if (circleQue[i].pElem[rear - 1].F7_0 > 0 && !bfaultTag)
				{
					faultNumLastWeek += 1;
					bfaultTag = true;
				}
			}
		}
		mileageSum += maxValue;
	}

	//在线车辆数
	csStr = _T("");
	csStr.Format(_T("%lu"), onLineNum);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEONLINE))->SetWindowTextW(csStr);

	//累计行驶里程
	csStr = _T("");
	csStr.Format(_T("%.5f"), (long double)mileageSum/10/10000);
	//csStr.Append(_T(" 万公里"));
	((CEdit*)GetDlgItem(IDC_EDIT_MILEAGESUN))->SetWindowTextW(csStr);

	//累计碳减排
	csStr = _T("");
	csStr.Format(_T("%.4f"), (0.0031 / 6) * ((long double)mileageSum / 10 / 10000));
	//csStr.Append(_T(" 万升"));
	((CEdit*)GetDlgItem(IDC_EDIT_CARBONEMISSION))->SetWindowTextW(csStr);

	//累计节油量
	csStr = _T("");
	csStr.Format(_T("%.2f"), (1.75/6) * ((long double)mileageSum / 10 /10000));
	//csStr.Append(_T(" 万升"));
	((CEdit*)GetDlgItem(IDC_EDIT_FUELTHRIFT))->SetWindowTextW(csStr);

	//累计耗电量
	csStr = _T("");
	csStr.Format(_T("%.2f"), (5.68 / 6) * ((long double)mileageSum / 10 / 10000));
	//csStr.Append(_T(" 万千瓦时"));
	((CEdit*)GetDlgItem(IDC_EDIT_POWERCONSUME))->SetWindowTextW(csStr);

	//今日在线车辆数
	csStr = _T("");
	csStr.Format(_T("%lu"), onLineNumToday);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEONLINETODAY))->SetWindowTextW(csStr);

	//今日充电车辆数
	csStr = _T("");
	csStr.Format(_T("%lu"), rechargeNumToday);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLERECHARGETODAY))->SetWindowTextW(csStr);

	//今日故障车辆数
	csStr = _T("");
	csStr.Format(_T("%lu"), faultNumToday);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEFAULTTODAY))->SetWindowTextW(csStr);

	//今日离线车辆数
	csStr = _T("");
	csStr.Format(_T("%lu"), offLineNumToday);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEOFFLINETODAY))->SetWindowTextW(csStr);

	//上周在线车辆数
	csStr = _T("");
	csStr.Format(_T("%lu"), onLineNumLastWeek);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEONLINELASTWEEK))->SetWindowTextW(csStr);

	//上周充电车辆数
	csStr = _T("");
	csStr.Format(_T("%lu"), rechargeNumLastWeek);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLERECHARGELASTWEEK))->SetWindowTextW(csStr);

	//上周故障车辆数
	csStr = _T("");
	csStr.Format(_T("%lu"), faultNumLastWeek);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEFAULTLASTWEEK))->SetWindowTextW(csStr);

	//上周离线车辆数
	csStr = _T("");
	csStr.Format(_T("%lu"), offLineNumLastWeek);
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLEOFFLINELASTWEEK))->SetWindowTextW(csStr);
}

BEGIN_MESSAGE_MAP(CStatistics, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CStatistics::OnBnClickedBtnRefresh)
END_MESSAGE_MAP()


// CStatistics 消息处理程序


void CStatistics::OnBnClickedBtnRefresh()
{
	// TODO: 在此添加控件通知处理程序代码
	OnQueryStatis();
}
