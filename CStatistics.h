#pragma once


// CStatistics 对话框

class CStatistics : public CDialogEx
{
	DECLARE_DYNAMIC(CStatistics)

public:
	CStatistics(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CStatistics();

	void OnQueryStatis();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STATISTICS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnRefresh();
};
