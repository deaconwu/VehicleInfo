#pragma once
#include "InfoType.h"

// CAlertCategory 对话框

class CAlertCategory : public CDialogEx
{
	DECLARE_DYNAMIC(CAlertCategory)

public:
	CAlertCategory(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAlertCategory();

	void SetTitle(CString csStr);
	void SetTypeTimesRank(int iType, uint32_t iTimes, uint32_t iRank);
	void OnLauch(char chVin[]);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ALERTCATEGORY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	LRESULT OnAlertStatsCategory(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
private:
	CListCtrl m_listCategory;
};
