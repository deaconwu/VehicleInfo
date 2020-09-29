#pragma once


// CAlertCategory 对话框

class CAlertCategory : public CDialogEx
{
	DECLARE_DYNAMIC(CAlertCategory)

public:
	CAlertCategory(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAlertCategory();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ALERTCATEGORY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listCategory;
};
