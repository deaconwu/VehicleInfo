#pragma once
#include "CInfoRecord.h"

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
	void RankSort(int iType=-1);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cbRankTypeChoice;
	CListCtrl m_listRank;
	afx_msg void OnCbnSelchangeComboRanktype();

private:
	bool CheckAlertFlag(uint32_t iData, int iType);

	void OnAlertCategory(long iVinPos);
	
public:
	afx_msg void OnLvnItemchangedListRank(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
};
