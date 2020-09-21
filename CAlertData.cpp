// CAlertData.cpp: 实现文件
//

#include "pch.h"
#include "VehicleInfo.h"
#include "CAlertData.h"
#include "afxdialogex.h"


// CAlertData 对话框

IMPLEMENT_DYNAMIC(CAlertData, CDialogEx)

CAlertData::CAlertData(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ALERTDATA, pParent), m_alertData(0)
{

}

CAlertData::~CAlertData()
{
}

void CAlertData::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CAlertData::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	this->SetWindowText(_T("报警数据"));

	((CEdit*)GetDlgItem(IDC_EDIT_ALERTVIN))->EnableWindow(false);
	((CStatic*)GetDlgItem(IDC_STATIC0))->EnableWindow(false);
	((CStatic*)GetDlgItem(IDC_STATIC1))->EnableWindow(false);
	((CStatic*)GetDlgItem(IDC_STATIC2))->EnableWindow(false);
	((CStatic*)GetDlgItem(IDC_STATIC3))->EnableWindow(false);
	((CStatic*)GetDlgItem(IDC_STATIC4))->EnableWindow(false);
	((CStatic*)GetDlgItem(IDC_STATIC5))->EnableWindow(false);
	((CStatic*)GetDlgItem(IDC_STATIC6))->EnableWindow(false);
	((CStatic*)GetDlgItem(IDC_STATIC7))->EnableWindow(false);
	((CStatic*)GetDlgItem(IDC_STATIC8))->EnableWindow(false);
	((CStatic*)GetDlgItem(IDC_STATIC9))->EnableWindow(false);
	((CStatic*)GetDlgItem(IDC_STATIC10))->EnableWindow(false);
	((CStatic*)GetDlgItem(IDC_STATIC11))->EnableWindow(false);
	((CStatic*)GetDlgItem(IDC_STATIC12))->EnableWindow(false);
	((CStatic*)GetDlgItem(IDC_STATIC13))->EnableWindow(false);
	((CStatic*)GetDlgItem(IDC_STATIC14))->EnableWindow(false);
	((CStatic*)GetDlgItem(IDC_STATIC15))->EnableWindow(false);
	((CStatic*)GetDlgItem(IDC_STATIC16))->EnableWindow(false);
	((CStatic*)GetDlgItem(IDC_STATIC17))->EnableWindow(false);
	((CStatic*)GetDlgItem(IDC_STATIC18))->EnableWindow(false);

	return TRUE;
}

void CAlertData::UpdateAlert(unsigned int iF7_0, CString cstrVin)
{
	//CString cstrVin(pVinStr);
	((CEdit*)GetDlgItem(IDC_EDIT_ALERTVIN))->SetWindowTextW(cstrVin);

	bool bFlag = iF7_0 % 2 > 0 ? true : false;
	((CStatic*)GetDlgItem(IDC_STATIC0))->EnableWindow(bFlag);

	bFlag = (iF7_0 / 2) % 2 > 0 ? true : false;
	((CStatic*)GetDlgItem(IDC_STATIC1))->EnableWindow(bFlag);

	bFlag = (iF7_0 / 4) % 2 > 0 ? true : false;
	((CStatic*)GetDlgItem(IDC_STATIC2))->EnableWindow(bFlag);

	bFlag = (iF7_0 / 8) % 2 > 0 ? true : false;
	((CStatic*)GetDlgItem(IDC_STATIC3))->EnableWindow(bFlag);

	bFlag = (iF7_0 / 16) % 2 > 0 ? true : false;
	((CStatic*)GetDlgItem(IDC_STATIC4))->EnableWindow(bFlag);

	bFlag = (iF7_0 / 32) % 2 > 0 ? true : false;
	((CStatic*)GetDlgItem(IDC_STATIC5))->EnableWindow(bFlag);

	bFlag = (iF7_0 / 64) % 2 > 0 ? true : false;
	((CStatic*)GetDlgItem(IDC_STATIC6))->EnableWindow(bFlag);

	bFlag = (iF7_0 / 128) % 2 > 0 ? true : false;
	((CStatic*)GetDlgItem(IDC_STATIC7))->EnableWindow(bFlag);

	bFlag = (iF7_0 / 256) % 2 > 0 ? true : false;
	((CStatic*)GetDlgItem(IDC_STATIC8))->EnableWindow(bFlag);

	bFlag = (iF7_0 / 512) % 2 > 0 ? true : false;
	((CStatic*)GetDlgItem(IDC_STATIC9))->EnableWindow(bFlag);

	bFlag = (iF7_0 / 1024) % 2 > 0 ? true : false;
	((CStatic*)GetDlgItem(IDC_STATIC10))->EnableWindow(bFlag);

	bFlag = (iF7_0 / 2048) % 2 > 0 ? true : false;
	((CStatic*)GetDlgItem(IDC_STATIC11))->EnableWindow(bFlag);

	bFlag = (iF7_0 / 4096) % 2 > 0 ? true : false;
	((CStatic*)GetDlgItem(IDC_STATIC12))->EnableWindow(bFlag);

	bFlag = (iF7_0 / 8192) % 2 > 0 ? true : false;
	((CStatic*)GetDlgItem(IDC_STATIC13))->EnableWindow(bFlag);

	bFlag = (iF7_0 / 16384) % 2 > 0 ? true : false;
	((CStatic*)GetDlgItem(IDC_STATIC14))->EnableWindow(bFlag);

	bFlag = (iF7_0 / 32768) % 2 > 0 ? true : false;
	((CStatic*)GetDlgItem(IDC_STATIC15))->EnableWindow(bFlag);

	bFlag = (iF7_0 / 65536) % 2 > 0 ? true : false;
	((CStatic*)GetDlgItem(IDC_STATIC16))->EnableWindow(bFlag);

	bFlag = (iF7_0 / 131072) % 2 > 0 ? true : false;
	((CStatic*)GetDlgItem(IDC_STATIC17))->EnableWindow(bFlag);

	bFlag = (iF7_0 / 262144) % 2 > 0 ? true : false;
	((CStatic*)GetDlgItem(IDC_STATIC18))->EnableWindow(bFlag);
}


BEGIN_MESSAGE_MAP(CAlertData, CDialogEx)
END_MESSAGE_MAP()


// CAlertData 消息处理程序
