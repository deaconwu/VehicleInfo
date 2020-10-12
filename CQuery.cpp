// CQuery.cpp: 实现文件
//

#include "pch.h"
#include "VehicleInfo.h"
#include "CQuery.h"
#include "afxdialogex.h"
#include "CInfoRecord.h"
// #include "CHistoryRecord.h"
#include "CAlertData.h"


// CQuery 对话框

IMPLEMENT_DYNAMIC(CQuery, CDialogEx)

CQuery::CQuery(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_QUERY, pParent)
{
	m_alertData = 0;
}

CQuery::~CQuery()
{
}

void CQuery::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CQuery::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//((CEdit*)GetDlgItem(IDC_EDIT_VINCODE))->SetLimitText(VIN_LENGTH);
	((CEdit*)GetDlgItem(IDC_EDIT_GATHERTIME))->EnableWindow(false);

	//整车
	((CEdit*)GetDlgItem(IDC_EDIT_VEHICLESTATE))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_RECHARGESTATE))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_RUNMODE))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_SPEED))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_MILEAGE))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_MILEAGETODAY))->EnableWindow(false);

	//驱动电机
	((CEdit*)GetDlgItem(IDC_EDIT_MOTORSTATE))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_MOTORROTATESPEED))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_MOTORTORQUE))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_MOTORTEMP))->EnableWindow(false);

	//车辆位置
	((CEdit*)GetDlgItem(IDC_EDIT_LONGTITUDE))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_LATITUDE))->EnableWindow(false);

	//极值
	((CEdit*)GetDlgItem(IDC_EDIT_MAXVOLTNO))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_VOLTMAXVALUE))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_MINVOLTNO))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_VOLTMINVALUE))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_MAXTEMPPROPNO))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_MAXTEMPVALUE))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_MINTEMPPROPNO))->EnableWindow(false);
	((CEdit*)GetDlgItem(IDC_EDIT_MINTEMPVALUE))->EnableWindow(false);

	((CButton*)GetDlgItem(IDC_BTN_ALERTDATA))->EnableWindow(false);
	((CButton*)GetDlgItem(IDC_BTN_MAPLOCATION))->EnableWindow(false);
	((CButton*)GetDlgItem(IDC_BTN_DIAGNOSIS))->EnableWindow(false);

	return TRUE;
}


BEGIN_MESSAGE_MAP(CQuery, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_QUERY, &CQuery::OnBnClickedBtnQuery)
	ON_BN_CLICKED(IDC_BTN_ALERTDATA, &CQuery::OnBnClickedBtnAlertdata)
END_MESSAGE_MAP()


// CQuery 消息处理程序


void CQuery::OnBnClickedBtnQuery()
{
	// TODO: 在此添加控件通知处理程序代码
	CString vinStr;
	TCHAR tchVin[VIN_LENGTH+1] = {};
	((CEdit*)GetDlgItem(IDC_EDIT_VINCODE))->GetWindowText(tchVin, VIN_LENGTH+1);

	char chVin[VIN_LENGTH + 1] = {};

	int iLength = WideCharToMultiByte(CP_ACP, 0, (LPCWCH)&tchVin, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, (LPCWCH)&tchVin, -1, chVin, iLength, NULL, NULL);

	STRECVDATA stData;
	memset(&stData, 0, sizeof(stData));

	bool bRet = CInfoRecord::GetInstance()->QueryLatestInfo((uint8_t*)chVin, stData);
	if (!bRet)
	{
		((CButton*)GetDlgItem(IDC_BTN_ALERTDATA))->EnableWindow(false);
		MessageBox(_T("该车辆不存在"), _T("提示"), MB_OK | MB_ICONERROR);
		return;
	}

	CString csStr;

	//采集时间
	csStr.Format(_T("%02d-%02d-%02d %02d:%02d:%02d"), stData.F8_0[0], stData.F8_0[1], stData.F8_0[2], stData.F8_0[3], stData.F8_0[4], stData.F8_0[5]);
	((CEdit*)GetDlgItem(IDC_EDIT_GATHERTIME))->SetWindowText(csStr);

	//车辆状态
	if (stData.F1_0 == 1)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_VEHICLESTATE))->SetWindowText(_T("启动"));
	}
	else if (stData.F1_0 == 2)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_VEHICLESTATE))->SetWindowText(_T("熄火"));
	}
	else if (stData.F1_0 == 3)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_VEHICLESTATE))->SetWindowText(_T("其它"));
	}
	else if (stData.F1_0 == 254)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_VEHICLESTATE))->SetWindowText(_T("异常"));
	}
	else if (stData.F1_0 == 255)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_VEHICLESTATE))->SetWindowText(_T("无效"));
	}
	else
	{
		((CEdit*)GetDlgItem(IDC_EDIT_VEHICLESTATE))->SetWindowText(_T(""));
	}

	//充电状态
	if (stData.F1_1 == 1)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_RECHARGESTATE))->SetWindowText(_T("停车充电"));
	}
	else if (stData.F1_1 == 2)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_RECHARGESTATE))->SetWindowText(_T("行驶充电"));
	}
	else if (stData.F1_1 == 3)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_RECHARGESTATE))->SetWindowText(_T("未充电"));
	}
	else if (stData.F1_1 == 4)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_RECHARGESTATE))->SetWindowText(_T("充电完成"));
	}
	else if (stData.F1_1 == 254)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_RECHARGESTATE))->SetWindowText(_T("异常"));
	}
	else if (stData.F1_1 == 255)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_RECHARGESTATE))->SetWindowText(_T("无效"));
	}
	else
	{
		((CEdit*)GetDlgItem(IDC_EDIT_RECHARGESTATE))->SetWindowText(_T(""));
	}

	//运行模式
	if (stData.F1_2 == 1)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_RUNMODE))->SetWindowText(_T("纯电"));
	}
	else if (stData.F1_2 == 2)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_RUNMODE))->SetWindowText(_T("混动"));
	}
	else if (stData.F1_2 == 3)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_RUNMODE))->SetWindowText(_T("燃油"));
	}
	else if (stData.F1_2 == 254)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_RUNMODE))->SetWindowText(_T("异常"));
	}
	else if (stData.F1_2 == 255)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_RUNMODE))->SetWindowText(_T("无效"));
	}
	else
	{
		((CEdit*)GetDlgItem(IDC_EDIT_RUNMODE))->SetWindowText(_T(""));
	}

	//车速
	csStr = _T("");
	csStr.Format(_T("%u"), stData.F1_3);
	((CEdit*)GetDlgItem(IDC_EDIT_SPEED))->SetWindowText(csStr);

	//累计里程
	csStr = _T("");
	csStr.Format(_T("%u"), stData.F1_4);
	((CEdit*)GetDlgItem(IDC_EDIT_MILEAGE))->SetWindowText(csStr);

	//今日里程
// 	unsigned int iLatestMileage = CHistoryRecord::GetInstance()->GetLatestMileage((uint8_t*)chVin);
// 	if (stData.F1_4 >= iLatestMileage)
// 	{
// 		csStr = _T("");
// 		csStr.Format(_T("%u"), stData.F1_4-iLatestMileage);
// 		((CEdit*)GetDlgItem(IDC_EDIT_MILEAGETODAY))->SetWindowText(csStr);
// 	}

	//电机状态
	if (stData.F2_0 == 1)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_MOTORSTATE))->SetWindowText(_T("耗电"));
	}
	else if (stData.F2_0 == 2)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_MOTORSTATE))->SetWindowText(_T("发电"));
	}
	else if (stData.F2_0 == 3)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_MOTORSTATE))->SetWindowText(_T("关闭"));
	}
	else if (stData.F2_0 == 4)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_MOTORSTATE))->SetWindowText(_T("准备"));
	}
	else if (stData.F2_0 == 254)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_MOTORSTATE))->SetWindowText(_T("异常"));
	}
	else if (stData.F2_0 == 255)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_MOTORSTATE))->SetWindowText(_T("无效"));
	}

	//电机转速
	csStr.Format(_T("%u"), stData.F2_2);
	((CEdit*)GetDlgItem(IDC_EDIT_MOTORROTATESPEED))->SetWindowText(csStr);

	//电机转矩
	csStr.Format(_T("%u"), stData.F2_3);
	((CEdit*)GetDlgItem(IDC_EDIT_MOTORTORQUE))->SetWindowText(csStr);

	//电机温度
	csStr.Format(_T("%u"), stData.F2_4);
	((CEdit*)GetDlgItem(IDC_EDIT_MOTORTEMP))->SetWindowText(csStr);

	//经度
	double dLongitude = (double)stData.F5_0 / 1000000;
	csStr.Format(_T("%f"), dLongitude);
	((CEdit*)GetDlgItem(IDC_EDIT_LONGTITUDE))->SetWindowText(csStr);
	
	//纬度
	double dLatitude = (double)stData.F5_1 / 1000000;
	csStr.Format(_T("%f"), dLatitude);
	((CEdit*)GetDlgItem(IDC_EDIT_LATITUDE))->SetWindowText(csStr);

	//最高电压单体号
	csStr.Format(_T("%d"), stData.F6_0 / 256);
	((CEdit*)GetDlgItem(IDC_EDIT_MAXVOLTNO))->SetWindowText(csStr);

	//单体电压最高值
	csStr.Format(_T("%u"), stData.F6_1);
	((CEdit*)GetDlgItem(IDC_EDIT_VOLTMAXVALUE))->SetWindowText(csStr);

	//最低电压单体号
	csStr.Format(_T("%u"), stData.F6_2 / 256);
	((CEdit*)GetDlgItem(IDC_EDIT_MINVOLTNO))->SetWindowText(csStr);

	//单体电压最低值
	csStr.Format(_T("%u"), stData.F6_3);
	((CEdit*)GetDlgItem(IDC_EDIT_VOLTMINVALUE))->SetWindowText(csStr);

	//最高温度探针号
	csStr.Format(_T("%u"), stData.F6_4);
	((CEdit*)GetDlgItem(IDC_EDIT_MAXTEMPPROPNO))->SetWindowText(csStr);

	//最高温度值
	csStr.Format(_T("%u"), stData.F6_5);
	((CEdit*)GetDlgItem(IDC_EDIT_MAXTEMPVALUE))->SetWindowText(csStr);

	//最低温度探针号
	csStr.Format(_T("%u"), stData.F6_6);
	((CEdit*)GetDlgItem(IDC_EDIT_MINTEMPPROPNO))->SetWindowText(csStr);

	//最低温度值
	csStr.Format(_T("%u"), stData.F6_7);
	((CEdit*)GetDlgItem(IDC_EDIT_MINTEMPVALUE))->SetWindowText(csStr);

	//报警数据
	m_alertData = stData.F7_0;

	((CButton*)GetDlgItem(IDC_BTN_ALERTDATA))->EnableWindow(true);
}

void CQuery::OnBnClickedBtnAlertdata()
{
	CString strContent;
	if (m_alertData == 0)
	{
		strContent = _T("正常！");
	}
	else
	{
		if (m_alertData % 2 > 0)
			strContent.Append(_T("温度差异报警 "));

		if ((m_alertData/2) % 2 > 0)
			strContent.Append(_T("电池高温报警 "));

		if ((m_alertData / 4) % 2 > 0)
			strContent.Append(_T("车载储能装置类型过压报警 "));

		if ((m_alertData / 8) % 2 > 0)
			strContent.Append(_T("车载储能装置类型欠压报警 "));

		if ((m_alertData / 16) % 2 > 0)
			strContent.Append(_T("SOC低报警 "));

		if ((m_alertData / 32) % 2 > 0)
			strContent.Append(_T("单体电池过压报警 "));

		if ((m_alertData / 64) % 2 > 0)
			strContent.Append(_T("单体电池欠压报警 "));

		if ((m_alertData / 128) % 2 > 0)
			strContent.Append(_T("SOC过高报警 "));

		if ((m_alertData / 256) % 2 > 0)
			strContent.Append(_T("SOC跳变报警 "));

		if ((m_alertData / 512) % 2 > 0)
			strContent.Append(_T("可充电储能系统不匹配报警 "));

		if ((m_alertData / 1024) % 2 > 0)
			strContent.Append(_T("电池单体一致性差报警 "));

		if ((m_alertData / 2048) % 2 > 0)
			strContent.Append(_T("绝缘报警 "));

		if ((m_alertData / 4096) % 2 > 0)
			strContent.Append(_T("DC-DC温度报警 "));

		if ((m_alertData / 8192) % 2 > 0)
			strContent.Append(_T("制动系统报警 "));

		if ((m_alertData / 16384) % 2 > 0)
			strContent.Append(_T("DC-DC状态报警 "));

		if ((m_alertData / 32768) % 2 > 0)
			strContent.Append(_T("驱动电池控制器温度报警 "));

		if ((m_alertData / 65536) % 2 > 0)
			strContent.Append(_T("高压互锁状态报警 "));

		if ((m_alertData / 131072) % 2 > 0)
			strContent.Append(_T("驱动电机温度报警 "));

		if ((m_alertData / 262144) % 2 > 0)
			strContent.Append(_T("车载储能装置类型过充报警 "));
	}

	MessageBox(strContent, _T("报警数据"), MB_OK | MB_ICONINFORMATION);
}
