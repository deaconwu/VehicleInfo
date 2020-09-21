#pragma once


// CRealTimeRecv 对话框

class CRealTimeRecv : public CDialogEx
{
	DECLARE_DYNAMIC(CRealTimeRecv)

public:
	CRealTimeRecv(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CRealTimeRecv();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REALTIMERECV };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
