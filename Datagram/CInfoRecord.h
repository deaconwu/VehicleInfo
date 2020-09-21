#ifndef __CINFORECORD_H__
#define __CINFORECORD_H__
#include "InfoType.h"
#include "CInfoSocket.h"

#define UM_STATISTIC WM_USER+1
#define UM_ALERT WM_USER+2

class CInfoRecord //: public CSingleton<CInfoRecord>
{
public:
	CInfoRecord();
	~CInfoRecord() {}

	static CInfoRecord* GetInstance()
	{
		if (NULL == m_pInstance)
			m_pInstance = new CInfoRecord;

		return m_pInstance;
	}

	bool GetLockFlag()
	{
		return m_bLockFlag;
	}

	void SetLockFlag(bool bFlag)
	{
		m_bLockFlag = bFlag;
	}

	void OnRealTimeRecv(HWND hWnd);

	void WriteVin();
	long FindVinPos(uint8_t pVin[]);
	long InsertVinAndSort(uint8_t pVin[]);

	bool QueryLatestInfo(uint8_t pVin[], STRECVDATA &stData);

	long GetQueInfo(STCIRCLEQUEUE circleQue[]);

	//¼�복����Ϣ����Ϣ����1~7
	void RecordInfo(long pos, STRECVDATA stRecv);

	//¼�복����Ϣ8
	long RecordInfoType8(long pos, const char* pRecv);

	//¼�복����Ϣ9
	long RecordInfoType9(long pos, const char* pRecv);

	void OnStatistic();

private:
	void SortVin();

	uint8_t m_chVin[200000][VIN_LENGTH + 1]; //ÿ����vin��

	//ÿ������ʷ��¼
	STCIRCLEQUEUE m_circleQue[20000];	//������ʽ��ţ��������ݡ�����������ݡ�����λ�á���ֵ���ݡ��������ݡ�ʱ������
	STRECVDATATYPE8 m_dataType8[20000];	//��ѹ����
	STRECVDATATYPE9 m_dataType9[20000];	//�¶�����

	long m_vehicleNum;
	bool m_bLockFlag;	//¼����vin���漰��������λ�������ѯ����Ҫ�ȴ���ѯ����ټ���¼������

	static CInfoRecord* m_pInstance;
};

#endif