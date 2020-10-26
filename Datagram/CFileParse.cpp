#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include "CFileParse.h"
#include "CInfoSocket.h"

//char strFileData[PARSE_FILE_SIZE + 1] = {};
char strTitle[] = "Vin码,采集时间,车辆状态,充电状态,运行模式,车速,累计里程,总电压,总电流,SOC,DC/DC状态,挡位,绝缘电阻,加速踏板行程值,制动踏板状态,驱动电机状态,驱动电机控制器温度,驱动电机转速,驱动电机转矩,驱动电机温度,电机控制器输入电压,直流控制器直流母线电流,经度,纬度,推算方向,最高电压单体号,电池单体电压最高值,最低电压单体号,电池单体电压最低值,最高温度探针号,最高温度值,最低温度探针号,最低温度值,报警数据";

CFileParse* CFileParse::m_pInstance = NULL;

DWORD WINAPI OnDatagramParseThread(LPVOID lparam);

CFileParse::CFileParse() : m_hThreadParse(NULL)
{

}

void CFileParse::OnLauchParse()
{
	if (NULL != m_hThreadParse)
	{
		CloseHandle(m_hThreadParse);
		m_hThreadParse = NULL;
	}

	DWORD dwThreadId;
	m_hThreadParse = CreateThread(NULL, NULL, OnDatagramParseThread, NULL, 0, &dwThreadId);
}

DWORD WINAPI OnDatagramParseThread(LPVOID lparam)
{
	if (!CInfoSocket::GetInstance()->CheckClose())
	{
		return 0;
	}

	LONGLONG iLineNum = 1;
	uint32_t iSheetIndex = 1;

	FILE *fpRead = fopen("datagram.dat", "r");
	if (NULL == fpRead)
	{
		return 0;
	}

	FILE *fpWrite = fopen("datagram_parse1.csv", "wb+");
	if (NULL == fpWrite)
	{
		return 0;
	}

	fseek(fpRead, 0L, SEEK_END);
	long fileLen = ftell(fpRead);
	fseek(fpRead, 0L, SEEK_SET);

	char* strFileData = (char*)malloc(fileLen + 1);
	fread(strFileData, 1, fileLen, fpRead);
	fclose(fpRead);

	fprintf(fpWrite, "%s\n", strTitle);

	long latestOffset = 0;
	uint8_t uchar;
	uint16_t ushortsw;
	uint32_t uintsw;

	while (latestOffset < fileLen)
	{
		if (!CInfoSocket::GetInstance()->CheckClose())
		{
			fclose(fpWrite);
			break;
		}

		if (memcmp(&strFileData[latestOffset], "##", 2) != 0)
		{
			latestOffset += 1;
			continue;
		}

		if (memcmp(&strFileData[latestOffset], "###", 3) == 0)
		{
			latestOffset += 1;
			continue;
		}

		//起始符号
		latestOffset += 2;

		//命令标识
		uint8_t iCmdFlag = strFileData[latestOffset];
		latestOffset += 1;

		//应答标识
		uint8_t iRespFlag = strFileData[latestOffset];
		latestOffset += 1;

		//vin码
		char chVin[VIN_LENGTH + 1] = {};
		memcpy(chVin, &strFileData[latestOffset], VIN_LENGTH);
		latestOffset += 17;

		//数据单元加密方式
		latestOffset += 1;

		//数据单元长度
		uchar = strFileData[latestOffset];
		uint16_t iDataLen = uchar * 256;
		uchar = strFileData[latestOffset + 1];
		iDataLen += uchar;
		latestOffset += 2;

		STRECVDATA infoData;
		memset(&infoData, 0, sizeof(infoData));

		/*******数据单元格式 实时信息上报*******/

		//数据采集时间(年月日时分秒)
		memcpy(infoData.F8_0, &strFileData[latestOffset], sizeof(infoData.F8_0));
		latestOffset += 6;

		long curOffset = latestOffset;
		long leftOffset = iDataLen - 6;

		bool bSetFlag[10] = {};

		while (latestOffset < (iDataLen - 6) + curOffset)
		{
			if (leftOffset <= 1)
			{
				break;
			}

			uint8_t infoType = strFileData[latestOffset];
			leftOffset -= 1;
			if (leftOffset <= 0)
				break;
			latestOffset += 1;

			if (infoType == 1)
			{
				bSetFlag[1] = true;

				// 0__车辆状态
				uchar = strFileData[latestOffset];
				infoData.F1_0 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 1__充电状态
				uchar = strFileData[latestOffset];
				infoData.F1_1 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 2__运行模式
				uchar = strFileData[latestOffset];
				infoData.F1_2 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 3__车速
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F1_3 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 5__累计里程
				uchar = strFileData[latestOffset];
				uintsw = uchar * 16777216;
				uchar = strFileData[latestOffset + 1];
				uintsw += uchar * 65536;
				uchar = strFileData[latestOffset + 2];
				uintsw += uchar * 256;
				uchar = strFileData[latestOffset + 3];
				uintsw += uchar;
				infoData.F1_4 = uintsw;
				leftOffset -= 4;
				if (leftOffset <= 0)
					break;
				latestOffset += 4;

				// 9__总电压
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F1_5 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 11__总电流
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F1_6 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 13__SOC
				uchar = strFileData[latestOffset];
				infoData.F1_7 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 14__DC/DC状态
				uchar = strFileData[latestOffset];
				infoData.F1_8 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 15__挡位
				uchar = strFileData[latestOffset];
				infoData.F1_9 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 16__绝缘电阻
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F1_10 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 18__加速踏板行程值
				uchar = strFileData[latestOffset];
				infoData.F1_11 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 19__制动踏板状态
				uchar = strFileData[latestOffset];
				infoData.F1_12 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
			}
			else if (infoType == 2)
			{
				//跳过驱动电机个数、驱动电机序号
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 0__驱动电机状态
				uchar = strFileData[latestOffset];
				infoData.F2_0 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 1__驱动电机控制器温度
				uchar = strFileData[latestOffset];
				infoData.F2_1 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 2__驱动电机转速
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F2_2 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 4__驱动电机转矩
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F2_3 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 6__驱动电机温度
				uchar = strFileData[latestOffset];
				infoData.F2_4 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 7__电机控制器输入电压
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F2_5 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 9__直流控制器直流母线电流
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F2_6 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;
			}
			else if (infoType == 3)
			{
				//燃料电池电压
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//燃料电池电流
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//燃料消耗率
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//燃料电池温度探针总数
				uchar = strFileData[latestOffset];
				uint16_t iProbNum = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				iProbNum += uchar;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//探针温度值
				for (uint16_t i = 0; i < iProbNum; i++)
				{
					leftOffset -= 1;
					if (leftOffset <= 0)
						break;
					latestOffset += 1;
				}

				if (leftOffset <= 0)
					break;

				//氢系统中最高温度
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//氢系统中最高温度探针代号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//氢气最高浓度
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//氢气最高浓度传感器代号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//氢气最高压力
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//氢气最高压力传感器代号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//高压DC/DC状态
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
			}
			else if (infoType == 4)
			{
				//发动机状态
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//曲轴转速
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//燃料消耗率
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;
			}
			else if (infoType == 5)
			{
				//跳过定位状态
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 0__经度
				uchar = strFileData[latestOffset];
				uintsw = uchar * 16777216;
				uchar = strFileData[latestOffset + 1];
				uintsw += uchar * 65536;
				uchar = strFileData[latestOffset + 2];
				uintsw += uchar * 256;
				uchar = strFileData[latestOffset + 3];
				uintsw += uchar;
				infoData.F5_0 = uintsw;
				leftOffset -= 4;
				if (leftOffset <= 0)
					break;
				latestOffset += 4;

				// 4__纬度
				uchar = strFileData[latestOffset];
				uintsw = uchar * 16777216;
				uchar = strFileData[latestOffset + 1];
				uintsw += uchar * 65536;
				uchar = strFileData[latestOffset + 2];
				uintsw += uchar * 256;
				uchar = strFileData[latestOffset + 3];
				uintsw += uchar;
				infoData.F5_1 = uintsw;
				leftOffset -= 4;
				if (leftOffset <= 0)
					break;
				latestOffset += 4;
			}
			else if (infoType == 6)
			{
				//跳过最高电压电池子系统号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 0__最高电压单体号
				uchar = strFileData[latestOffset];
				infoData.F6_0 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 2__电池单体电压最高值
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F6_1 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//跳过最低电压电池子系统号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 4__最低电压单体号
				uchar = strFileData[latestOffset];
				infoData.F6_2 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 6__电池单体电压最低值
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F6_3 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//跳过最高温度子系统号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 8__最高温度探针号
				uchar = strFileData[latestOffset];
				infoData.F6_4 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 9__最高温度值
				uchar = strFileData[latestOffset];
				infoData.F6_5 = uchar;;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//跳过最低温度子系统号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 10__最低温度探针号
				uchar = strFileData[latestOffset];
				infoData.F6_6 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 11__最低温度值
				uchar = strFileData[latestOffset];
				infoData.F6_7 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
			}
			else if (infoType == 7)
			{
				//跳过最高报警等级
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//通用报警标志
				uchar = strFileData[latestOffset];
				uintsw = uchar * 16777216;
				uchar = strFileData[latestOffset + 1];
				uintsw += uchar * 65536;
				uchar = strFileData[latestOffset + 2];
				uintsw += uchar * 256;
				uchar = strFileData[latestOffset + 3];
				uintsw += uchar;
				infoData.F7_0 = uintsw;
				leftOffset -= 4;
				if (leftOffset <= 0)
					break;
				latestOffset += 4;

				//可充电储能装置故障总数
				uchar = strFileData[latestOffset];
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
				if (uchar > 0)
				{
					leftOffset -= uchar * 4;
					if (leftOffset <= 0)
						break;
					latestOffset += uchar * 4;
				}

				//驱动电机故障总数
				uchar = strFileData[latestOffset];
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
				if (uchar > 0)
				{
					leftOffset -= uchar * 4;
					if (leftOffset <= 0)
						break;
					latestOffset += uchar * 4;
				}

				//发动机故障总数
				uchar = strFileData[latestOffset];
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
				if (uchar > 0)
				{
					leftOffset -= uchar * 4;
					if (leftOffset <= 0)
						break;
					latestOffset += uchar * 4;
				}

				//其它故障总数
				uchar = strFileData[latestOffset];
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
				if (uchar > 0)
				{
					leftOffset -= uchar * 4;
					if (leftOffset <= 0)
						break;
					latestOffset += uchar * 4;
				}
			}
			else if (infoType == 8)
			{
				//可充电储能子系统个数(目前只有1个)
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//可充电储能子系统号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//可充电储能装置电压
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//可充电储能装置电流
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//单体电池总数
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//本帧起始电池序号;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//本帧单体电池总数
				uint8_t iCellNum = strFileData[latestOffset];
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//单体电池电压列表
				for (uint8_t i = 0; i < iCellNum; i++)
				{
					leftOffset -= 2;
					if (leftOffset <= 0)
						break;
					latestOffset += 2;
				}
			}
			else if (infoType == 9)
			{
				//可充电储能子系统个数(目前只有1个);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//可充电储能子系统号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//可充电储能温度探针个数
				uchar = strFileData[latestOffset];
				uint16_t iProbNum = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				iProbNum += uchar;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//可充电储能子系统各温度探针检测到的温度值
				for (uint16_t j = 0; j < iProbNum; j++)
				{
					if (latestOffset > fileLen)
						break;

					leftOffset -= 1;
					if (leftOffset <= 0)
						break;
					latestOffset += 1;
				}
			}
			else
			{
				latestOffset += leftOffset;
				break;
			}
		}

		if ((iCmdFlag == 2 || iCmdFlag == 3) && iRespFlag == 254 && bSetFlag[1])
		{
			if (iLineNum >= MAXLNE_PER_SHEET)
			{
				fclose(fpWrite);

				iLineNum = 1;
				iSheetIndex += 1;

				char strFileName[40] = {};
				sprintf(strFileName, "datagram_parse%d.csv", iSheetIndex);
				fpWrite = fopen(strFileName, "wb+");
				if (NULL == fpWrite)
				{
					return 0;
				}

				fprintf(fpWrite, "%s\n", strTitle);
			}
			
			fprintf(fpWrite, "%s,%d-%d-%d %d:%d:%d,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,",
				chVin,
				infoData.F8_0[0], infoData.F8_0[1], infoData.F8_0[2], infoData.F8_0[3], infoData.F8_0[4], infoData.F8_0[5],
				infoData.F1_0, infoData.F1_1, infoData.F1_2, infoData.F1_3, infoData.F1_4, infoData.F1_5, infoData.F1_6,
				infoData.F1_7, infoData.F1_8, infoData.F1_9, infoData.F1_10, infoData.F1_11, infoData.F1_12);

			fprintf(fpWrite, "%u,%u,%u,%u,%u,%u,%u,",
				infoData.F2_0, infoData.F2_1, infoData.F2_2, infoData.F2_3, infoData.F2_4, infoData.F2_5, infoData.F2_6);

			fprintf(fpWrite, "%u,%u,%u,",
				infoData.F5_0, infoData.F5_1, infoData.F5_2);

			fprintf(fpWrite, "%u,%u,%u,%u,%u,%u,%u,%u,",
				infoData.F6_0, infoData.F6_1, infoData.F6_2, infoData.F6_3, infoData.F6_4, infoData.F6_5, infoData.F6_6, infoData.F6_7);

			fprintf(fpWrite, "%u",
				infoData.F7_0);

			iLineNum += 1;

			if (iLineNum < MAXLNE_PER_SHEET)
				fprintf(fpWrite, "\n");
		}
	}

	fclose(fpWrite);
	free(strFileData);

	return 0;
}

DWORD WINAPI OnDatagramParseThread1(LPVOID lparam)
{
	if (!CInfoSocket::GetInstance()->CheckClose())
	{
		return 0;
	}

	FILE *fpRead = fopen("datagram.dat", "r");
	if (NULL == fpRead)
	{
		return 0;
	}

	FILE *fpWrite = fopen("datagram_parse.dat", "wb+");
	if (NULL == fpWrite)
	{
		return 0;
	}

	fseek(fpRead, 0L, SEEK_END);
	long fileLen = ftell(fpRead);
	fseek(fpRead, 0L, SEEK_SET);
	
	char* strFileData = (char*)malloc(fileLen + 1);
	fread(strFileData, 1, fileLen, fpRead);
	fclose(fpRead);

	long latestOffset = 0;
	uint8_t uchar;
	uint16_t ushortsw;
	uint32_t uintsw;

	while (latestOffset < fileLen)
	{
		if (!CInfoSocket::GetInstance()->CheckClose())
		{
			fclose(fpWrite);
			break;
		}

		if (memcmp(&strFileData[latestOffset], "##", 2) != 0)
		{
			latestOffset += 1;
			continue;
		}

		if (memcmp(&strFileData[latestOffset], "###", 3) == 0)
		{
			latestOffset += 1;
			continue;
		}

		//起始符号
		fprintf(fpWrite, "%c", strFileData[latestOffset]);
		latestOffset += 1;

		fprintf(fpWrite, "%c\n", strFileData[latestOffset]);
		latestOffset += 1;

		//命令标识
		fprintf(fpWrite, "%u\n", strFileData[latestOffset]);
		latestOffset += 1;

		//应答标识
		uchar = strFileData[latestOffset];
		fprintf(fpWrite, "%u\n", uchar);
		latestOffset += 1;

		//vin码
		char vin[18] = {};
		memcpy(vin, &strFileData[latestOffset], 17);
		fprintf(fpWrite, "%s\n", vin);
		latestOffset += 17;

		//数据单元加密方式
		uchar = strFileData[latestOffset];
		fprintf(fpWrite, "%u\n", uchar);
		latestOffset += 1;

		//数据单元长度
		uchar = strFileData[latestOffset];
		uint16_t iDataLen = uchar * 256;
		uchar = strFileData[latestOffset + 1];
		iDataLen += uchar;
		fprintf(fpWrite, "%u\n", iDataLen);
		latestOffset += 2;

		/*******数据单元格式*******/

		  /*实时信息上报*/
			//数据采集时间(年月日时分秒)
		uint8_t uchTime[6] = {};
		memcpy(uchTime, &strFileData[latestOffset], 6);
		fprintf(fpWrite, "%02u-%02u-%02u %02u:%02u:%02u\n", uchTime[0], uchTime[1], uchTime[2], uchTime[3], uchTime[4], uchTime[5]);
		latestOffset += 6;

		long curOffset = latestOffset;
		long leftOffset = iDataLen - 6;

		while (latestOffset < (iDataLen - 6) + curOffset)
		{
			if (!CInfoSocket::GetInstance()->CheckClose())
			{
				break;
			}

// 			if (memcmp(&strFileData[latestOffset], "##", 2) == 0)
// 			{
// 				break;
// 			}

			if (leftOffset <= 1)
			{
				fprintf(fpWrite, "checkcode %u \n", strFileData[latestOffset]);
				break;
			}

			uint8_t infoType = strFileData[latestOffset];
			fprintf(fpWrite, "infoType_%u ", infoType);
			leftOffset -= 1;
			if (leftOffset <= 0)
				break;
			latestOffset += 1;

			if (infoType == 1)
			{
				// 0__车辆状态
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 1__充电状态
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 2__运行模式
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 3__车速
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 5__累计里程
				uchar = strFileData[latestOffset];
				uintsw = uchar * 16777216;
				uchar = strFileData[latestOffset + 1];
				uintsw += uchar * 65536;
				uchar = strFileData[latestOffset + 2];
				uintsw += uchar * 256;
				uchar = strFileData[latestOffset + 3];
				uintsw += uchar;
				fprintf(fpWrite, "%u ", uintsw);
				leftOffset -= 4;
				if (leftOffset <= 0)
					break;
				latestOffset += 4;

				// 9__总电压
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 11__总电流
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 13__SOC
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 14__DC/DC状态
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 15__挡位
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 16__绝缘电阻
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 18__加速踏板行程值
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 19__制动踏板状态
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u\n", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
			}
			else if (infoType == 2)
			{
				//跳过驱动电机个数、驱动电机序号
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 0__驱动电机状态
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 1__驱动电机控制器温度
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 2__驱动电机转速
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 4__驱动电机转矩
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 6__驱动电机温度
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 7__电机控制器输入电压
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 9__直流控制器直流母线电流
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u\n", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;
			}
			else if (infoType == 3)
			{
				//燃料电池电压
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//燃料电池电流
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//燃料消耗率
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//燃料电池温度探针总数
				uchar = strFileData[latestOffset];
				uint16_t iProbNum = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				iProbNum += uchar;
				fprintf(fpWrite, "%u ", iProbNum);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//探针温度值
				if (iProbNum > 0)
					fprintf(fpWrite, "[ ");
				for (uint16_t i=0; i<iProbNum; i++)
				{
					uchar = strFileData[latestOffset];
					fprintf(fpWrite, "%u ", uchar);
					leftOffset -= 1;
					if (leftOffset <= 0)
						break;
					latestOffset += 1;
				}
				if (iProbNum > 0)
					fprintf(fpWrite, "] ");

				if (leftOffset <= 0)
					break;

				//氢系统中最高温度
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//氢系统中最高温度探针代号
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//氢气最高浓度
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//氢气最高浓度传感器代号
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//氢气最高压力
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//氢气最高压力传感器代号
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//高压DC/DC状态
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u\n", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
			}
			else if (infoType == 4)
			{
				//发动机状态
				uchar = strFileData[latestOffset];
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//曲轴转速
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//燃料消耗率
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u\n", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;
			}
			else if (infoType == 5)
			{
				//跳过定位状态
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 0__经度
				uchar = strFileData[latestOffset];
				uintsw = uchar * 16777216;
				uchar = strFileData[latestOffset + 1];
				uintsw += uchar * 65536;
				uchar = strFileData[latestOffset + 2];
				uintsw += uchar * 256;
				uchar = strFileData[latestOffset + 3];
				uintsw += uchar;
				fprintf(fpWrite, "%u ", uintsw);
				leftOffset -= 4;
				if (leftOffset <= 0)
					break;
				latestOffset += 4;

				// 4__纬度
				uchar = strFileData[latestOffset];
				uintsw = uchar * 16777216;
				uchar = strFileData[latestOffset + 1];
				uintsw += uchar * 65536;
				uchar = strFileData[latestOffset + 2];
				uintsw += uchar * 256;
				uchar = strFileData[latestOffset + 3];
				uintsw += uchar;
				fprintf(fpWrite, "%u\n", uintsw);
				leftOffset -= 4;
				if (leftOffset <= 0)
					break;
				latestOffset += 4;
			}
			else if (infoType == 6)
			{
				//跳过最高电压电池子系统号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 0__最高电压单体号
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 2__电池单体电压最高值
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//跳过最低电压电池子系统号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 4__最低电压单体号
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 6__电池单体电压最低值
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//跳过最高温度子系统号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 8__最高温度探针号
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//跳过最低温度子系统号
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 9__最高温度值
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 10__最低温度探针号
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 11__最低温度值
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u\n", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
			}
			else if (infoType == 7)
			{
				//跳过最高报警等级
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//通用报警标志
				uchar = strFileData[latestOffset];
				uintsw = uchar * 16777216;
				uchar = strFileData[latestOffset + 1];
				uintsw += uchar * 65536;
				uchar = strFileData[latestOffset + 2];
				uintsw += uchar * 256;
				uchar = strFileData[latestOffset + 3];
				uintsw += uchar;
				fprintf(fpWrite, "%u ", uintsw);
				leftOffset -= 4;
				if (leftOffset <= 0)
					break;
				latestOffset += 4;

				//可充电储能装置故障总数
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
				if (uchar > 0)
				{
					leftOffset -= uchar * 4;
					if (leftOffset <= 0)
						break;
					latestOffset += uchar * 4;
				}

				//驱动电机故障总数
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
				if (uchar > 0)
				{
					leftOffset -= uchar * 4;
					if (leftOffset <= 0)
						break;
					latestOffset += uchar * 4;
				}

				//发动机故障总数
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
				if (uchar > 0)
				{
					leftOffset -= uchar * 4;
					if (leftOffset <= 0)
						break;
					latestOffset += uchar * 4;
				}

				//其它故障总数
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u\n", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
				if (uchar > 0)
				{
					leftOffset -= uchar * 4;
					if (leftOffset <= 0)
						break;
					latestOffset += uchar * 4;
				}
			}
			else if (infoType == 8)
			{
				//可充电储能子系统个数(目前只有1个)
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//可充电储能子系统号
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//可充电储能装置电压
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//可充电储能装置电流
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//单体电池总数
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//本帧起始电池序号
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//本帧单体电池总数
				uint8_t iCellNum = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", iCellNum);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//单体电池电压列表
				fprintf(fpWrite, "[ ");
				for (uint8_t i = 0; i < iCellNum; i++)
				{
					uchar = strFileData[latestOffset];
					ushortsw = uchar * 256;
					uchar = strFileData[latestOffset + 1];
					ushortsw += uchar;
					fprintf(fpWrite, "%u ", ushortsw);
					leftOffset -= 2;
					if (leftOffset <= 0)
						break;
					latestOffset += 2;
				}
				fprintf(fpWrite, " ]");

				fprintf(fpWrite, "\n");
			}
			else if (infoType == 9)
			{
				//可充电储能子系统个数(目前只有1个)
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//可充电储能子系统号
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//可充电储能温度探针个数
				uchar = strFileData[latestOffset];
				uint16_t iProbNum = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				iProbNum += uchar;
				fprintf(fpWrite, "%u ", iProbNum);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//可充电储能子系统各温度探针检测到的温度值
				fprintf(fpWrite, "[ ");
				for (uint16_t j = 0; j < iProbNum; j++)
				{
					if (latestOffset > fileLen)
						break;

					uchar = strFileData[latestOffset];
					fprintf(fpWrite, "%u ", uchar);
					leftOffset -= 1;
					if (leftOffset <= 0)
						break;
					latestOffset += 1;
				}
				fprintf(fpWrite, " ]");

				fprintf(fpWrite, "\n");
			}
			else
			{
				latestOffset += leftOffset;
				break;
			}
		}

		fprintf(fpWrite, "\n\n");
	}

	fclose(fpWrite);
	free(strFileData);

	return 0;
}