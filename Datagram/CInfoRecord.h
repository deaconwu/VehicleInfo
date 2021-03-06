#ifndef __CINFORECORD_H__
#define __CINFORECORD_H__
#include "InfoType.h"
#include "CInfoSocket.h"

class CInfoRecord //: public CSingleton<CInfoRecord>
{
public:
	~CInfoRecord()
	{
// 		if (NULL != m_pInstance)
// 		{
// 			delete m_pInstance;
// 			m_pInstance = NULL;
// 		}
	}

	static CInfoRecord* GetInstance();

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

	bool QueryLatestInfo(uint8_t pVin[], STRECVDATATYPE1TO7 &stData);

	long GetQueInfo(STCIRCLEQUEUE circleQue[]);

	void GetVinInfo(uint8_t chVin[][VIN_LENGTH + 1]);

	//录入车辆信息，信息类型1~7
	void RecordInfo(long pos, STRECVDATATYPE1TO7& stRecv);

	//录入车辆信息8
	long RecordInfoType8(long pos, const char* pRecv);

	//录入车辆信息9
	long RecordInfoType9(long pos, const char* pRecv);

	void ReadVin();

	void WriteVin();

	long GetTodayJoinCount();

	long GetAppendNum() { return m_vehicleNumAppend; }

	long GetVehicleNumSum() { return m_vehicleNumSum; }

	void OnMsgParse(char recvData[], int recvSize);

private:
	CInfoRecord();

	uint8_t m_chVin[MAX_VEHICLENUM][VIN_LENGTH + 1]; //每辆车vin码
	uint8_t m_bTodayJoin[MAX_VEHICLENUM];	//今日接入的车

	//每辆车历史记录
	STCIRCLEQUEUE m_circleQue[MAX_VEHICLENUM];	//队列形式存放，整车数据、驱动电机数据、车辆位置、极值数据、报警数据、时间数据
	STRECVDATATYPE8 m_dataType8[MAX_VEHICLENUM];	//电压数据
	STRECVDATATYPE9 m_dataType9[MAX_VEHICLENUM];	//温度数据

	long m_vehicleNumSum;	//总计车辆数
	long m_vehicleNumAppend;	//新增车辆数
	long m_datagramNum;	//今日接收报文数
	bool m_bLockFlag;	//录入新vin码涉及到排序移位，点击查询，需要等待查询完成再继续录入排序

	HANDLE m_hThreadRecv;	//子线程接收消息，拆分成若干报文
	HANDLE m_hThreadParse;	//子线程解析报文

	HWND m_hWnd;

	PSTDATABUFFGRAM m_pDataGramPre;
	int m_iPreSize;

	uint8_t* m_pVinArr;
	STRECVDATATYPE1TO7* m_pInfoTypeElem;
	long* m_pVehicleNum;

	static CInfoRecord* m_pInstance;
};

#endif