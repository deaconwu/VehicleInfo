#ifndef __INFOTYPE_H__
#define __INFOTYPE_H__

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define BUFFER_SIZE 65537
#define VIN_LENGTH  17
#define QUEUE_SIZE  1000   //队列长度
#define CELLNUM_PER_FRAM 200 //本帧最大电池数
#define ALERTTIMES_REANK_NUM 20
#define MAX_VEHICLENUM 200000

#pragma pack (1)
// typedef struct RecData
// {
//     uint8_t chStarCode[2];  //起始符##
//     uint8_t chCmdFlag;
//     uint8_t chRspFlag;
//     uint8_t chVin[VIN_LENGTH];
//     uint8_t chEncodeWay;
//     uint16_t chUnitLen;
// } STRECVDATA;


/**********信息类型1~7***********/
typedef struct RecvData
{
	//整车数据
	uint8_t F1_0;       // 0__车辆状态
	uint8_t F1_1;       // 1__充电状态
	uint8_t F1_2;       // 2__运行模式
	uint16_t F1_3;      // 3__车速
	uint32_t F1_4;      // 5__累计里程
	uint16_t F1_5;      // 9__总电压
	uint16_t F1_6;      // 11__总电流
	uint8_t F1_7;       // 13__SOC
	uint8_t F1_8;       // 14__DC/DC状态
	uint8_t F1_9;       // 15__挡位
	uint16_t F1_10;     // 16__绝缘电阻
	uint8_t F1_11;      // 18__加速踏板行程值
	uint8_t F1_12;      // 19__制动踏板状态

	//驱动电机数据
	uint8_t F2_0;       // 0__驱动电机状态
	uint8_t F2_1;       // 1__驱动电机控制器温度
	uint16_t F2_2;      // 2__驱动电机转速
	uint16_t F2_3;      // 4__驱动电机转矩
	uint8_t F2_4;       // 6__驱动电机温度
	uint16_t F2_5;      // 7__电机控制器输入电压
	uint16_t F2_6;      // 9__直流控制器直流母线电流

	//位置数据
	uint32_t F5_0;      // 0__经度
	uint32_t F5_1;      // 4__纬度
	uint8_t F5_2;       // 8__推算方向

	//极值数据
	uint16_t F6_0;      // 0__最高电压单体号
	uint16_t F6_1;      // 2__电池单体电压最高值
	uint16_t F6_2;      // 4__最低电压单体号
	uint16_t F6_3;      // 6__电池单体电压最低值
	uint8_t F6_4;       // 8__最高温度探针号
	uint8_t F6_5;       // 9__最高温度值
	uint8_t F6_6;       // 10__最低温度探针号
	uint8_t F6_7;       // 11__最低温度值

	//报警数据
	uint32_t F7_0;

	//时间数据
	uint8_t F8_0[6];    //年月日时分秒
} STRECVDATA;
/****************************/


/**********信息类型8：电压数据***********/
/*
typedef struct
{
	uint16_t F8_1_4_0;      //本帧起始电池序号
	uint8_t F8_1_4_1;       //本帧单体电池总数
	uint16_t* pF8_1_4_2;    //单体电池电压
} RecvDataType8_1_4;	

typedef struct
{
	uint8_t F8_1_0;       //可充电储能子系统号
	uint16_t F8_1_1;      //可充电储能装置电压
	uint16_t F8_1_2;      //可充电储能装置电流
	uint16_t F8_1_3;      //单体电池总数,1帧最多200个
	RecvDataType8_1_4* pF8_1_4;	//本帧电池
} RecvDataType8_1;
*/

typedef struct
{
	uint8_t F8_1_0;       //可充电储能子系统号
	uint16_t F8_1_1;      //可充电储能装置电压
	uint16_t F8_1_2;      //可充电储能装置电流
	uint16_t F8_1_3;      //单体电池总数
	uint16_t F8_1_4;      //本帧起始电池序号
	uint8_t F8_1_5;       //本帧单体电池总数
	uint16_t* pF8_1_6;    //单体电池电压
} RecvDataType8_1;

typedef struct RecvDataType8
{
	uint8_t F8_0;		  //可充电储能子系统个数
	RecvDataType8_1* pF8_1;    //可充电储能子系统电压信息列表
} STRECVDATATYPE8;

/**************************************/


/**********信息类型9：温度数据***********/
typedef struct
{
	uint8_t F9_1_0;       //可充电储能子系统号
	uint16_t F9_1_1;      //可充电储能温度探针个数
	uint8_t* pF9_1_2;      //可充电储能子系统各温度探针检测到的温度值
} RecvDataType9_1;

typedef struct RecvDataType9
{
	uint8_t F9_0;        //可充电储能子系统个数
	RecvDataType9_1 *pF9_1;  //可充电储能子系统温度信息列表
} STRECVDATATYPE9;
/**************************************/


/**********循环队列，记录某辆车的历史数据**********/

//记录信息类型1~7
typedef struct CircleQueue
{
	STRECVDATA* pElem;
	uint32_t front;
	uint32_t rear;
} STCIRCLEQUEUE;

//记录信息类型8
typedef struct CircleQueueType8
{
	STRECVDATATYPE8* pElem;
	uint32_t front;
	uint32_t rear;
} STCIRCLEQUEUETYPE8;

//记录信息类型9
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
