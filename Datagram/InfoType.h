#ifndef __INFOTYPE_H__
#define __INFOTYPE_H__

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define BUFFER_SIZE 65537
#define VIN_LENGTH  17
#define QUEUE_SIZE  10000   //���г���
#define CELLNUM_PER_FRAM 200 //��֡�������
#define ALERTTIMES_REANK_NUM 20
#define ALERT_CATEGORY_NUM 20
#define MAX_VEHICLENUM 200000
#define NUM_MSGRECV_PERLOOP 3 //ÿ��ѭ��������Ϣ��
#define PARSE_FILE_SIZE 1073741824
#define MAXLNE_PER_SHEET 1048576

#pragma pack (1)

/**********��Ϣ����1~7***********/
typedef struct RecvDataType1To7
{
	//��������
	uint8_t F1_0;       // 0__����״̬
	uint8_t F1_1;       // 1__���״̬
	uint8_t F1_2;       // 2__����ģʽ
	uint16_t F1_3;      // 3__����
	uint32_t F1_4;      // 5__�ۼ����
	uint16_t F1_5;      // 9__�ܵ�ѹ
	uint16_t F1_6;      // 11__�ܵ���
	uint8_t F1_7;       // 13__SOC
	uint8_t F1_8;       // 14__DC/DC״̬
	uint8_t F1_9;       // 15__��λ
	uint16_t F1_10;     // 16__��Ե����
	uint8_t F1_11;      // 18__����̤���г�ֵ
	uint8_t F1_12;      // 19__�ƶ�̤��״̬

	//�����������
	uint8_t F2_0;       // 0__�������״̬
	uint8_t F2_1;       // 1__��������������¶�
	uint16_t F2_2;      // 2__�������ת��
	uint16_t F2_3;      // 4__�������ת��
	uint8_t F2_4;       // 6__��������¶�
	uint16_t F2_5;      // 7__��������������ѹ
	uint16_t F2_6;      // 9__ֱ��������ֱ��ĸ�ߵ���

	//λ������
	uint32_t F5_0;      // 0__����
	uint32_t F5_1;      // 4__γ��
	uint8_t F5_2;       // 8__���㷽��

	//��ֵ����
	uint16_t F6_0;      // 0__��ߵ�ѹ�����
	uint16_t F6_1;      // 2__��ص����ѹ���ֵ
	uint16_t F6_2;      // 4__��͵�ѹ�����
	uint16_t F6_3;      // 6__��ص����ѹ���ֵ
	uint8_t F6_4;       // 8__����¶�̽���
	uint8_t F6_5;       // 9__����¶�ֵ
	uint8_t F6_6;       // 10__����¶�̽���
	uint8_t F6_7;       // 11__����¶�ֵ

	//��������
	uint32_t F7_0;

	//ʱ������
	uint8_t F8_0[6];    //������ʱ����
} STRECVDATATYPE1TO7;
/****************************/


/**********��Ϣ����8����ѹ����***********/
typedef struct RecvDataType8_1_4
{
	uint16_t F8_1_4_1;      //��֡��ʼ������
	uint8_t F8_1_4_2;       //��֡����������
	uint16_t* pF8_1_4_3;    //�����ص�ѹ
	struct RecvDataType8_1_4* pNext;
} RecvDataType8_1_4;

typedef struct
{
	uint8_t F8_1_0;       //�ɳ�索����ϵͳ��
	uint16_t F8_1_1;      //�ɳ�索��װ�õ�ѹ
	uint16_t F8_1_2;      //�ɳ�索��װ�õ���
	uint16_t F8_1_3;      //����������
	RecvDataType8_1_4* pF8_1_4;	//��ص�ѹ�б�
} RecvDataType8_1;

typedef struct RecvDataType8
{
	uint8_t F8_0;		  //�ɳ�索����ϵͳ����(Ŀǰֻ��1��)
	RecvDataType8_1* pF8_1;    //�ɳ�索����ϵͳ��ѹ��Ϣ�б�
} STRECVDATATYPE8;
/**************************************/


/**********��Ϣ����9���¶�����***********/
typedef struct
{
	uint8_t F9_1_0;       //�ɳ�索����ϵͳ��
	uint16_t F9_1_1;      //�ɳ�索���¶�̽�����
	uint8_t* pF9_1_2;      //�ɳ�索����ϵͳ���¶�̽���⵽���¶�ֵ
} RecvDataType9_1;

typedef struct RecvDataType9
{
	uint8_t F9_0;        //�ɳ�索����ϵͳ����(Ŀǰֻ��1��)
	RecvDataType9_1 *pF9_1;  //�ɳ�索����ϵͳ�¶���Ϣ�б�
} STRECVDATATYPE9;
/**************************************/


/**********ѭ�����У���¼ĳ��������ʷ����**********/

//��¼��Ϣ����1~7
typedef struct CircleQueue
{
	STRECVDATATYPE1TO7* pElem;
	uint32_t front;
	uint32_t rear;
} STCIRCLEQUEUE;

//��¼��Ϣ����8
typedef struct CircleQueueType8
{
	STRECVDATATYPE8* pElem;
	uint32_t front;
	uint32_t rear;
} STCIRCLEQUEUETYPE8;

//��¼��Ϣ����9
typedef struct CircleQueueType9
{
	STRECVDATATYPE9* pElem;
	uint32_t front;
	uint32_t rear;
} STCIRCLEQUEUETYPE9;

typedef struct AlertDataPost
{
	uint8_t chVin[VIN_LENGTH];
	uint32_t F7_0;
} STALERTDATAPOST;

typedef struct AlertDataLink
{
	uint8_t chVin[VIN_LENGTH+1];
	uint32_t iAlertTimes;
	struct AlertDataLink* pPre;
	struct AlertDataLink* pNext;
} STALERTDATALINK, *PSTALERTDATALINK;

typedef struct AlertRankSeq
{
	uint8_t chVin[VIN_LENGTH + 1];
	uint32_t iAlertTimes;
} STALERTRANKSEQ;

typedef struct MsgAlertRankSeq
{
	uint8_t iNum;
	STALERTRANKSEQ stNode[ALERTTIMES_REANK_NUM];	//�䳤�����ALERTTIMES_REANK_NUM
} STMSGALERTRANKSEQ;

typedef struct MsgAlertCategory
{
	uint32_t iAlertTimes[ALERT_CATEGORY_NUM];
	uint32_t iRank[ALERT_CATEGORY_NUM];
} STMSGALERTCATEGORY;

typedef struct DayVehicle
{
	long lVehicleNum;
	uint16_t wDayOfWeek;
} STDAYVEHICLE;

typedef struct DataOneWeek
{
	long long lMileageSum;
	unsigned long iFaultNum;
} STDATAONEWEEK;

typedef struct DataBuffGram
{
	int recvSize;
	char recvData[20000];
	struct DataBuffGram* pNext;
} STDATABUFFGRAM, *PSTDATABUFFGRAM;

typedef struct DataGramQueue
{
	PSTDATABUFFGRAM front;
	PSTDATABUFFGRAM rear;
} STDATAGRAMQUEUE, *PSTDATAGRAMQUEUE;

/***********************************************/
#pragma pack()
#endif
