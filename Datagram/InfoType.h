#ifndef __INFOTYPE_H__
#define __INFOTYPE_H__

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define BUFFER_SIZE 65537
#define VIN_LENGTH  17
#define QUEUE_SIZE  1000   //���г���
#define CELLNUM_PER_FRAM 200 //��֡�������
#define ALERTTIMES_REANK_NUM 20
#define MAX_VEHICLENUM 200000

#pragma pack (1)
// typedef struct RecData
// {
//     uint8_t chStarCode[2];  //��ʼ��##
//     uint8_t chCmdFlag;
//     uint8_t chRspFlag;
//     uint8_t chVin[VIN_LENGTH];
//     uint8_t chEncodeWay;
//     uint16_t chUnitLen;
// } STRECVDATA;


/**********��Ϣ����1~7***********/
typedef struct RecvData
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
} STRECVDATA;
/****************************/


/**********��Ϣ����8����ѹ����***********/
/*
typedef struct
{
	uint16_t F8_1_4_0;      //��֡��ʼ������
	uint8_t F8_1_4_1;       //��֡����������
	uint16_t* pF8_1_4_2;    //�����ص�ѹ
} RecvDataType8_1_4;	

typedef struct
{
	uint8_t F8_1_0;       //�ɳ�索����ϵͳ��
	uint16_t F8_1_1;      //�ɳ�索��װ�õ�ѹ
	uint16_t F8_1_2;      //�ɳ�索��װ�õ���
	uint16_t F8_1_3;      //����������,1֡���200��
	RecvDataType8_1_4* pF8_1_4;	//��֡���
} RecvDataType8_1;
*/

typedef struct
{
	uint8_t F8_1_0;       //�ɳ�索����ϵͳ��
	uint16_t F8_1_1;      //�ɳ�索��װ�õ�ѹ
	uint16_t F8_1_2;      //�ɳ�索��װ�õ���
	uint16_t F8_1_3;      //����������
	uint16_t F8_1_4;      //��֡��ʼ������
	uint8_t F8_1_5;       //��֡����������
	uint16_t* pF8_1_6;    //�����ص�ѹ
} RecvDataType8_1;

typedef struct RecvDataType8
{
	uint8_t F8_0;		  //�ɳ�索����ϵͳ����
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
	uint8_t F9_0;        //�ɳ�索����ϵͳ����
	RecvDataType9_1 *pF9_1;  //�ɳ�索����ϵͳ�¶���Ϣ�б�
} STRECVDATATYPE9;
/**************************************/


/**********ѭ�����У���¼ĳ��������ʷ����**********/

//��¼��Ϣ����1~7
typedef struct CircleQueue
{
	STRECVDATA* pElem;
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

/***********************************************/
#pragma pack()
#endif
