#ifndef __CINFORECORD_H__
#define __CINFORECORD_H__
#include "InfoType.h"
#include "CInfoSocket.h"

class CInfoRecord //: public CSingleton<CInfoRecord>
{
public:
	CInfoRecord();
	~CInfoRecord()
	{
		if (NULL != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

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

	long GetDatagramNum()
	{
		return m_datagramNum;
	}

	bool OnRealTimeRecv(HWND hWnd, sockaddr_in serAddr);
	bool OnStopRecv();

	void OnClearDataGram();
	void OnReset();
	void OnClose();

	long FindVinPos(uint8_t pVin[]);

	long InsertVinAndSort(uint8_t pVin[]);

	bool QueryLatestInfo(uint8_t pVin[], STRECVDATA &stData);

	long GetQueInfo(STCIRCLEQUEUE circleQue[]);

	void GetVinInfo(uint8_t chVin[][VIN_LENGTH + 1]);

	//¼�복����Ϣ����Ϣ����1~7
	void RecordInfo(long pos, STRECVDATA& stRecv);

	//¼�복����Ϣ8
	long RecordInfoType8(long pos, const char* pRecv);

	//¼�복����Ϣ9
	long RecordInfoType9(long pos, const char* pRecv);

	void ReadVin();

	void WriteVin();

	long GetTodayJoinCount();

	long GetAppendNum() { return m_vehicleNumAppend; }

	long GetVehicleNumSum() { return m_vehicleNumSum; }

	void OnMsgParse(char recvData[], int recvSize);

private:
	uint8_t m_chVin[MAX_VEHICLENUM][VIN_LENGTH + 1]; //ÿ����vin��
	uint8_t m_bTodayJoin[MAX_VEHICLENUM];	//���ս���ĳ�

	//ÿ������ʷ��¼
	STCIRCLEQUEUE m_circleQue[MAX_VEHICLENUM];	//������ʽ��ţ��������ݡ�����������ݡ�����λ�á���ֵ���ݡ��������ݡ�ʱ������
	STRECVDATATYPE8 m_dataType8[MAX_VEHICLENUM];	//��ѹ����
	STRECVDATATYPE9 m_dataType9[MAX_VEHICLENUM];	//�¶�����

	long m_vehicleNumSum;	//�ܼƳ�����
	long m_vehicleNumAppend;	//����������
	long m_datagramNum;	//���ս��ձ�����
	bool m_bLockFlag;	//¼����vin���漰��������λ�������ѯ����Ҫ�ȴ���ѯ����ټ���¼������

	HANDLE m_hThreadRecv;	//���߳̽�����Ϣ����ֳ����ɱ���
	HANDLE m_hThreadParse;	//���߳̽�������

	HWND m_hWnd;

	PSTDATABUFFGRAM m_pDataGramPre;
	int m_iPreSize;

	static CInfoRecord* m_pInstance;
};

#endif