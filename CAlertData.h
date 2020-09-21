#pragma once


// CAlertData 对话框

class CAlertData : public CDialogEx
{
	DECLARE_DYNAMIC(CAlertData)

public:
	CAlertData(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAlertData();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ALERTDATA };
#endif

	void UpdateAlert(unsigned int iF7_0, CString cstrVin);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	int m_alertData;
};
