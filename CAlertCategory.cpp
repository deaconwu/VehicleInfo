// CAlertCategory.cpp: 实现文件
//

#include "pch.h"
#include "VehicleInfo.h"
#include "CAlertCategory.h"
#include "afxdialogex.h"


// CAlertCategory 对话框

IMPLEMENT_DYNAMIC(CAlertCategory, CDialogEx)

CAlertCategory::CAlertCategory(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ALERTCATEGORY, pParent)
{

}

CAlertCategory::~CAlertCategory()
{
}

void CAlertCategory::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CATEGORY, m_listCategory);
}

BOOL CAlertCategory::OnInitDialog()
{
	CDialogEx::OnInitDialog();
}

BEGIN_MESSAGE_MAP(CAlertCategory, CDialogEx)
END_MESSAGE_MAP()


// CAlertCategory 消息处理程序
