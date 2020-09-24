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

	void FetchVinCode(long pos, uint8_t chVin[])
	{
		memset(chVin, 0, (VIN_LENGTH+1) * sizeof(uint8_t));
		memcpy(chVin, m_chVin[pos], VIN_LENGTH * sizeof(uint8_t));
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

	uint8_t m_chVin[MAX_VEHICLENUM][VIN_LENGTH + 1]; //ÿ����vin��

	//ÿ������ʷ��¼
	STCIRCLEQUEUE m_circleQue[MAX_VEHICLENUM];	//������ʽ��ţ��������ݡ�����������ݡ�����λ�á���ֵ���ݡ��������ݡ�ʱ������
	STRECVDATATYPE8 m_dataType8[MAX_VEHICLENUM];	//��ѹ����
	STRECVDATATYPE9 m_dataType9[MAX_VEHICLENUM];	//�¶�����

	long m_vehicleNum;
	bool m_bLockFlag;	//¼����vin���漰��������λ�������ѯ����Ҫ�ȴ���ѯ����ټ���¼������

	static CInfoRecord* m_pInstance;
};

#endif