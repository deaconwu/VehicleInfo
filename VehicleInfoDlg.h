
// VehicleInfoDlg.h: 头文件
//

#pragma once

#include "CQuery.h"
#include "CStatistics.h"
#include "CAlertData.h"
#include "CAlertRank.h"

// CVehicleInfoDlg 对话框
class CVehicleInfoDlg : public CDialogEx
{
// 构造
public:
	CVehicleInfoDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VEHICLEINFO_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	LRESULT OnRealAlert(WPARAM wParam, LPARAM lParam);
	LRESULT OnRealStopRecv(WPARAM wParam, LPARAM lParam);
	LRESULT OnRealStopParse(WPARAM wParam, LPARAM lParam);
	LRESULT OnHistoryRecord(WPARAM wParam, LPARAM lParam);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();

	//afx_msg void OnPaint();
	//afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_tab;
	//CRealTimeRecv m_realTimeRecv;
	CQuery m_query;
	CStatistics m_statistics;
	CAlertData m_alertData;
	CAlertRank m_alertRank;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnConnect();
	afx_msg void OnBnClickedBtnDisconnect();
};
