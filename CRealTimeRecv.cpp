// CRealTimeRecv.cpp: 实现文件
//

#include "pch.h"
#include "VehicleInfo.h"
#include "CRealTimeRecv.h"
#include "afxdialogex.h"


// CRealTimeRecv 对话框

IMPLEMENT_DYNAMIC(CRealTimeRecv, CDialogEx)

CRealTimeRecv::CRealTimeRecv(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REALTIMERECV, pParent)
{

}

CRealTimeRecv::~CRealTimeRecv()
{
}

void CRealTimeRecv::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRealTimeRecv, CDialogEx)
END_MESSAGE_MAP()


// CRealTimeRecv 消息处理程序
