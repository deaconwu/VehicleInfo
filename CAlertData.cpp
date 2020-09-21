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

	return TRUE;
}


BEGIN_MESSAGE_MAP(CAlertData, CDialogEx)
END_MESSAGE_MAP()


// CAlertData 消息处理程序
