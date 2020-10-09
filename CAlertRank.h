#pragma once
//#include "CInfoRecord.h"
#include "CAlertCategory.h"

// CAlertRank 对话框
class CAlertRank : public CDialogEx
{
	DECLARE_DYNAMIC(CAlertRank)

public:
	CAlertRank(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAlertRank();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ALERTRANK };
#endif

	void RankLaunch();

	void DestroySubDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	LRESULT OnAlertStatsRank(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cbRankTypeChoice;
	CListCtrl m_listRank;
	afx_msg void OnCbnSelchangeComboRanktype();;
	
public:
	afx_msg void OnLvnItemchangedListRank(NMHDR *pNMHDR, LRESULT *pResult);

private:
	CAlertCategory* m_pDlgAlertCategory;
};
