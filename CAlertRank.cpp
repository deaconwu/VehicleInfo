﻿// CAlertRank.cpp: 实现文件
//

#include "pch.h"
#include "VehicleInfo.h"
#include "CAlertRank.h"
#include "afxdialogex.h"

// CAlertRank 对话框

IMPLEMENT_DYNAMIC(CAlertRank, CDialogEx)

CAlertRank::CAlertRank(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ALERTRANK, pParent)
{

}

CAlertRank::~CAlertRank()
{
}

BOOL CAlertRank::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_cbRankTypeChoice.InsertString(0, _T("所有报警"));
	m_cbRankTypeChoice.InsertString(1, _T("温度差异报警"));
	m_cbRankTypeChoice.InsertString(2, _T("电池高温报警"));
	m_cbRankTypeChoice.InsertString(3, _T("车载储能装置类型过压报警"));
	m_cbRankTypeChoice.InsertString(4, _T("车载储能装置类型欠压报警"));
	m_cbRankTypeChoice.InsertString(5, _T("SOC低报警"));
	m_cbRankTypeChoice.InsertString(6, _T("单体电池过压报警"));
	m_cbRankTypeChoice.InsertString(7, _T("单体电池欠压报警"));
	m_cbRankTypeChoice.InsertString(8, _T("SOC过高报警"));
	m_cbRankTypeChoice.InsertString(9, _T("SOC跳变报警"));
	m_cbRankTypeChoice.InsertString(10, _T("可充电储能系统不匹配报警"));
	m_cbRankTypeChoice.InsertString(11, _T("电池单体一致性差报警"));
	m_cbRankTypeChoice.InsertString(12, _T("绝缘报警"));
	m_cbRankTypeChoice.InsertString(13, _T("DC-DC温度报警"));
	m_cbRankTypeChoice.InsertString(14, _T("制动系统报警"));
	m_cbRankTypeChoice.InsertString(15, _T("DC-DC状态报警"));
	m_cbRankTypeChoice.InsertString(16, _T("驱动电池控制器温度报警"));
	m_cbRankTypeChoice.InsertString(17, _T("高压互锁状态报警"));
	m_cbRankTypeChoice.InsertString(18, _T("驱动电机温度报警"));
	m_cbRankTypeChoice.InsertString(19, _T("车载储能装置类型过充报警"));
	m_cbRankTypeChoice.SetCurSel(0);

	m_listRank.SetExtendedStyle(LVS_EX_GRIDLINES);
	m_listRank.InsertColumn(1, _T("名次"), LVCFMT_CENTER, 50);
	m_listRank.InsertColumn(2, _T("Vin码"), LVCFMT_CENTER, 150);
	m_listRank.InsertColumn(3, _T("报警次数"), LVCFMT_CENTER, 120);

	CString csStr;
	for (int i = 1; i <= ALERTTIMES_REANK_NUM; i++)
	{
		csStr.Format(_T("%u"), i);
		m_listRank.InsertItem(i-1, csStr);
		m_listRank.SetItemText(i-1, 0, csStr);
	}

	//RankSort(-1);

	return TRUE;
}

void CAlertRank::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_RANKTYPE, m_cbRankTypeChoice);
	DDX_Control(pDX, IDC_LIST_RANK, m_listRank);
}


BEGIN_MESSAGE_MAP(CAlertRank, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_RANKTYPE, &CAlertRank::OnCbnSelchangeComboRanktype)
END_MESSAGE_MAP()


// CAlertRank 消息处理程序


void CAlertRank::OnCbnSelchangeComboRanktype()
{
	// TODO: 在此添加控件通知处理程序代码
	int iIndex = m_cbRankTypeChoice.GetCurSel();
	RankSort(iIndex - 1);
}

bool CAlertRank::CheckAlertFlag(uint32_t iData, int iType)
{
	uint8_t iFlag = iData;

	switch (iType)
	{
	case 0:
	{
		iFlag = iData % 2;
		break;
	}
	case 1:
	{
		iFlag = (iData / 2) % 2;
		break;
	}
	case 2:
	{
		iFlag = (iData / 4) % 2;
		break;
	}
	case 3:
	{
		iFlag = (iData / 8) % 2;
		break;
	}
	case 4:
	{
		iFlag = (iData / 16) % 2;
		break;
	}
	case 5:
	{
		iFlag = (iData / 32) % 2;
		break;
	}
	case 6:
	{
		iFlag = (iData / 64) % 2;
		break;
	}
	case 7:
	{
		iFlag = (iData / 128) % 2;
		break;
	}
	case 8:
	{
		iFlag = (iData / 256) % 2;
		break;
	}
	case 9:
	{
		iFlag = (iData / 512) % 2;
		break;
	}
	case 10:
	{
		iFlag = (iData / 1024) % 2;
		break;
	}
	case 11:
	{
		iFlag = (iData / 2048) % 2;
		break;
	}
	case 12:
	{
		iFlag = (iData / 4096) % 2;
		break;
	}
	case 13:
	{
		iFlag = (iData / 8192) % 2;
		break;
	}
	case 14:
	{
		iFlag = (iData / 16384) % 2;
		break;
	}
	case 15:
	{
		iFlag = (iData / 32768) % 2;
		break;
	}
	case 16:
	{
		iFlag = (iData / 65536) % 2;
		break;
	}
	case 17:
	{
		iFlag = (iData / 131072) % 2;
		break;
	}
	case 18:
	{
		iFlag = (iData / 262144) % 2;
		break;
	}
	default:
		break;
	}

	return (iFlag > 0);
}

void CAlertRank::RankSort(int iType)
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	STCIRCLEQUEUE circleQue[20000];
	long vehicleNum = CInfoRecord::GetInstance()->GetQueInfo(circleQue);

	PSTALERTDATALINK pNode = NULL;
	PSTALERTDATALINK pLast = NULL;
	uint8_t iRankNum = 0;

	for (long i = 0; i < vehicleNum; i++)
	{
		uint32_t iAlertTimes = 0;

		long rear = circleQue[i].rear;
		for (long j = rear - 1; j >= 0; j--)
		{
			if ((st.wYear % 100) == circleQue[i].pElem[j].F8_0[0]
				&& st.wMonth == circleQue[i].pElem[j].F8_0[1]
				&& st.wDay == circleQue[i].pElem[j].F8_0[2])
			{
				//))
				if (circleQue[i].pElem[j].F7_0 > 0 && iType<0)
				{
					iAlertTimes += 1;
				}
				else if (CheckAlertFlag(circleQue[i].pElem[j].F7_0, iType))
				{
					iAlertTimes += 1;
				}
			}
			else
			{
				break;
			}
		}

		if (iAlertTimes > 0)	//与链表每个节点比较
		{
			if (pNode == NULL)
			{
				//链表为空
				pNode = (PSTALERTDATALINK)malloc(sizeof(STALERTDATALINK));
				if (NULL == pNode)
					return;

				pNode->pPre = NULL;
				pNode->pNext = NULL;
				pLast = pNode;

				pLast->iAlertTimes = iAlertTimes;
				CInfoRecord::GetInstance()->FetchVinCode(i, pLast->chVin);
				iRankNum += 1;
			}
			else if (iRankNum < ALERTTIMES_REANK_NUM)
			{
				PSTALERTDATALINK pPrev = pLast;
				while (pPrev != NULL)
				{
					if (iAlertTimes <= pPrev->iAlertTimes)
					{
						PSTALERTDATALINK pNew = (PSTALERTDATALINK)malloc(sizeof(STALERTDATALINK));
						pNew->iAlertTimes = iAlertTimes;
						CInfoRecord::GetInstance()->FetchVinCode(i, pNew->chVin);
						pNew->pNext = NULL;

						if (pPrev->pNext != NULL)
						{
							pPrev->pNext->pPre = pNew;
							pNew->pNext = pPrev->pNext;
						}
						else
						{
							//新结点插入链尾
							pLast = pNew;
						}

						pNew->pPre = pPrev;
						pPrev->pNext = pNew;

						iRankNum += 1;
						break;
					}

					pPrev = pPrev->pPre;
				}

				if (pPrev == NULL)
				{
					//排名最大
					PSTALERTDATALINK pNew = (PSTALERTDATALINK)malloc(sizeof(STALERTDATALINK));
					pNew->iAlertTimes = iAlertTimes;
					CInfoRecord::GetInstance()->FetchVinCode(i, pNew->chVin);
					pNew->pPre = NULL;
					pNew->pNext = pNode;
					pNode->pPre = pNew;
					pNode = pNew;
					iRankNum += 1;
				}
			}
			else
			{
				if (iAlertTimes <= pLast->iAlertTimes)	//比不过最小的，排不上名
					continue;

				PSTALERTDATALINK pPrev = pLast;
				while (pPrev != NULL)
				{
					if (iAlertTimes <= pPrev->iAlertTimes)
					{
						PSTALERTDATALINK pNew = (PSTALERTDATALINK)malloc(sizeof(STALERTDATALINK));
						pNew->iAlertTimes = iAlertTimes;
						CInfoRecord::GetInstance()->FetchVinCode(i, pNew->chVin);
						pNew->pNext = NULL;

						if (pPrev->pNext != NULL)
						{
							pPrev->pNext->pPre = pNew;
							pNew->pNext = pPrev->pNext;
						}
						else
						{
							//新结点插入链尾
							pLast = pNew;
						}

						pNew->pPre = pPrev;
						pPrev->pNext = pNew;

						pLast = pLast->pPre;
						free(pLast->pNext);
						pLast->pNext = NULL;

						break;
					}

					pPrev = pPrev->pPre;
				}

				if (pPrev == NULL)
				{
					//排名最大
					PSTALERTDATALINK pNew = (PSTALERTDATALINK)malloc(sizeof(STALERTDATALINK));
					pNew->iAlertTimes = iAlertTimes;
					CInfoRecord::GetInstance()->FetchVinCode(i, pNew->chVin);
					pNew->pPre = NULL;
					pNew->pNext = pNode;
					pNode->pPre = pNew;
					pNode = pNew;
					
					pLast = pLast->pPre;
					free(pLast->pNext);
					pLast->pNext = NULL;
				}
			}
		}
	}

	for (int i = 0; i < ALERTTIMES_REANK_NUM; i++)
	{
		m_listRank.SetItemText(i, 1, _T(""));
		m_listRank.SetItemText(i, 2, _T(""));
	}

	uint8_t iLineNum = 0;

	while (pNode != NULL && pNode->iAlertTimes>0)
	{
		CString csStr(pNode->chVin);
		m_listRank.SetItemText(iLineNum, 1, csStr);

		csStr.Format(_T("%u"), pNode->iAlertTimes);
		m_listRank.SetItemText(iLineNum, 2, csStr);

		PSTALERTDATALINK pDel = pNode;
		pNode = pNode->pNext;
		free(pDel);

		iLineNum++;
	}
}