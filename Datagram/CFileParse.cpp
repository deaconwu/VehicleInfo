#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include "CFileParse.h"
#include "CInfoSocket.h"

//char strFileData[PARSE_FILE_SIZE + 1] = {};
char strTitle[] = "Vin��,�ɼ�ʱ��,����״̬,���״̬,����ģʽ,����,�ۼ����,�ܵ�ѹ,�ܵ���,SOC,DC/DC״̬,��λ,��Ե����,����̤���г�ֵ,�ƶ�̤��״̬,�������״̬,��������������¶�,�������ת��,�������ת��,��������¶�,��������������ѹ,ֱ��������ֱ��ĸ�ߵ���,����,γ��,���㷽��,��ߵ�ѹ�����,��ص����ѹ���ֵ,��͵�ѹ�����,��ص����ѹ���ֵ,����¶�̽���,����¶�ֵ,����¶�̽���,����¶�ֵ,��������";

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

		//��ʼ����
		latestOffset += 2;

		//�����ʶ
		uint8_t iCmdFlag = strFileData[latestOffset];
		latestOffset += 1;

		//Ӧ���ʶ
		uint8_t iRespFlag = strFileData[latestOffset];
		latestOffset += 1;

		//vin��
		char chVin[VIN_LENGTH + 1] = {};
		memcpy(chVin, &strFileData[latestOffset], VIN_LENGTH);
		latestOffset += 17;

		//���ݵ�Ԫ���ܷ�ʽ
		latestOffset += 1;

		//���ݵ�Ԫ����
		uchar = strFileData[latestOffset];
		uint16_t iDataLen = uchar * 256;
		uchar = strFileData[latestOffset + 1];
		iDataLen += uchar;
		latestOffset += 2;

		STRECVDATA infoData;
		memset(&infoData, 0, sizeof(infoData));

		/*******���ݵ�Ԫ��ʽ ʵʱ��Ϣ�ϱ�*******/

		//���ݲɼ�ʱ��(������ʱ����)
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

				// 0__����״̬
				uchar = strFileData[latestOffset];
				infoData.F1_0 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 1__���״̬
				uchar = strFileData[latestOffset];
				infoData.F1_1 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 2__����ģʽ
				uchar = strFileData[latestOffset];
				infoData.F1_2 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 3__����
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F1_3 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 5__�ۼ����
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

				// 9__�ܵ�ѹ
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F1_5 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 11__�ܵ���
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

				// 14__DC/DC״̬
				uchar = strFileData[latestOffset];
				infoData.F1_8 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 15__��λ
				uchar = strFileData[latestOffset];
				infoData.F1_9 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 16__��Ե����
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F1_10 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 18__����̤���г�ֵ
				uchar = strFileData[latestOffset];
				infoData.F1_11 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 19__�ƶ�̤��״̬
				uchar = strFileData[latestOffset];
				infoData.F1_12 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
			}
			else if (infoType == 2)
			{
				//���������������������������
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 0__�������״̬
				uchar = strFileData[latestOffset];
				infoData.F2_0 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 1__��������������¶�
				uchar = strFileData[latestOffset];
				infoData.F2_1 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 2__�������ת��
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F2_2 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 4__�������ת��
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F2_3 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 6__��������¶�
				uchar = strFileData[latestOffset];
				infoData.F2_4 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 7__��������������ѹ
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F2_5 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 9__ֱ��������ֱ��ĸ�ߵ���
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
				//ȼ�ϵ�ص�ѹ
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//ȼ�ϵ�ص���
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//ȼ��������
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//ȼ�ϵ���¶�̽������
				uchar = strFileData[latestOffset];
				uint16_t iProbNum = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				iProbNum += uchar;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//̽���¶�ֵ
				for (uint16_t i = 0; i < iProbNum; i++)
				{
					leftOffset -= 1;
					if (leftOffset <= 0)
						break;
					latestOffset += 1;
				}

				if (leftOffset <= 0)
					break;

				//��ϵͳ������¶�
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//��ϵͳ������¶�̽�����
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//�������Ũ��
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//�������Ũ�ȴ���������
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//�������ѹ��
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//�������ѹ������������
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//��ѹDC/DC״̬
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
			}
			else if (infoType == 4)
			{
				//������״̬
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//����ת��
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//ȼ��������
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;
			}
			else if (infoType == 5)
			{
				//������λ״̬
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 0__����
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

				// 4__γ��
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
				//������ߵ�ѹ�����ϵͳ��
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 0__��ߵ�ѹ�����
				uchar = strFileData[latestOffset];
				infoData.F6_0 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 2__��ص����ѹ���ֵ
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F6_1 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//������͵�ѹ�����ϵͳ��
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 4__��͵�ѹ�����
				uchar = strFileData[latestOffset];
				infoData.F6_2 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 6__��ص����ѹ���ֵ
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				infoData.F6_3 = ushortsw;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//��������¶���ϵͳ��
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 8__����¶�̽���
				uchar = strFileData[latestOffset];
				infoData.F6_4 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 9__����¶�ֵ
				uchar = strFileData[latestOffset];
				infoData.F6_5 = uchar;;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//��������¶���ϵͳ��
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 10__����¶�̽���
				uchar = strFileData[latestOffset];
				infoData.F6_6 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 11__����¶�ֵ
				uchar = strFileData[latestOffset];
				infoData.F6_7 = uchar;
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
			}
			else if (infoType == 7)
			{
				//������߱����ȼ�
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//ͨ�ñ�����־
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

				//�ɳ�索��װ�ù�������
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

				//���������������
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

				//��������������
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

				//������������
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
				//�ɳ�索����ϵͳ����(Ŀǰֻ��1��)
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//�ɳ�索����ϵͳ��
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//�ɳ�索��װ�õ�ѹ
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//�ɳ�索��װ�õ���
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//����������
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//��֡��ʼ������;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//��֡����������
				uint8_t iCellNum = strFileData[latestOffset];
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//�����ص�ѹ�б�
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
				//�ɳ�索����ϵͳ����(Ŀǰֻ��1��);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//�ɳ�索����ϵͳ��
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//�ɳ�索���¶�̽�����
				uchar = strFileData[latestOffset];
				uint16_t iProbNum = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				iProbNum += uchar;
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//�ɳ�索����ϵͳ���¶�̽���⵽���¶�ֵ
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

		//��ʼ����
		fprintf(fpWrite, "%c", strFileData[latestOffset]);
		latestOffset += 1;

		fprintf(fpWrite, "%c\n", strFileData[latestOffset]);
		latestOffset += 1;

		//�����ʶ
		fprintf(fpWrite, "%u\n", strFileData[latestOffset]);
		latestOffset += 1;

		//Ӧ���ʶ
		uchar = strFileData[latestOffset];
		fprintf(fpWrite, "%u\n", uchar);
		latestOffset += 1;

		//vin��
		char vin[18] = {};
		memcpy(vin, &strFileData[latestOffset], 17);
		fprintf(fpWrite, "%s\n", vin);
		latestOffset += 17;

		//���ݵ�Ԫ���ܷ�ʽ
		uchar = strFileData[latestOffset];
		fprintf(fpWrite, "%u\n", uchar);
		latestOffset += 1;

		//���ݵ�Ԫ����
		uchar = strFileData[latestOffset];
		uint16_t iDataLen = uchar * 256;
		uchar = strFileData[latestOffset + 1];
		iDataLen += uchar;
		fprintf(fpWrite, "%u\n", iDataLen);
		latestOffset += 2;

		/*******���ݵ�Ԫ��ʽ*******/

		  /*ʵʱ��Ϣ�ϱ�*/
			//���ݲɼ�ʱ��(������ʱ����)
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
				// 0__����״̬
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 1__���״̬
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 2__����ģʽ
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 3__����
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 5__�ۼ����
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

				// 9__�ܵ�ѹ
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 11__�ܵ���
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

				// 14__DC/DC״̬
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 15__��λ
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 16__��Ե����
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 18__����̤���г�ֵ
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 19__�ƶ�̤��״̬
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u\n", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
			}
			else if (infoType == 2)
			{
				//���������������������������
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 0__�������״̬
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 1__��������������¶�
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 2__�������ת��
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 4__�������ת��
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 6__��������¶�
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 7__��������������ѹ
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				// 9__ֱ��������ֱ��ĸ�ߵ���
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
				//ȼ�ϵ�ص�ѹ
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//ȼ�ϵ�ص���
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//ȼ��������
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//ȼ�ϵ���¶�̽������
				uchar = strFileData[latestOffset];
				uint16_t iProbNum = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				iProbNum += uchar;
				fprintf(fpWrite, "%u ", iProbNum);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//̽���¶�ֵ
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

				//��ϵͳ������¶�
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//��ϵͳ������¶�̽�����
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//�������Ũ��
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//�������Ũ�ȴ���������
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//�������ѹ��
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//�������ѹ������������
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//��ѹDC/DC״̬
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u\n", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
			}
			else if (infoType == 4)
			{
				//������״̬
				uchar = strFileData[latestOffset];
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//����ת��
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//ȼ��������
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
				//������λ״̬
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 0__����
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

				// 4__γ��
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
				//������ߵ�ѹ�����ϵͳ��
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 0__��ߵ�ѹ�����
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 2__��ص����ѹ���ֵ
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//������͵�ѹ�����ϵͳ��
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 4__��͵�ѹ�����
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 6__��ص����ѹ���ֵ
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//��������¶���ϵͳ��
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 8__����¶�̽���
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//��������¶���ϵͳ��
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 9__����¶�ֵ
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 10__����¶�̽���
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				// 11__����¶�ֵ
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u\n", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;
			}
			else if (infoType == 7)
			{
				//������߱����ȼ�
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//ͨ�ñ�����־
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

				//�ɳ�索��װ�ù�������
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

				//���������������
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

				//��������������
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

				//������������
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
				//�ɳ�索����ϵͳ����(Ŀǰֻ��1��)
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//�ɳ�索����ϵͳ��
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//�ɳ�索��װ�õ�ѹ
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//�ɳ�索��װ�õ���
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//����������
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//��֡��ʼ������
				uchar = strFileData[latestOffset];
				ushortsw = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				ushortsw += uchar;
				fprintf(fpWrite, "%u ", ushortsw);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//��֡����������
				uint8_t iCellNum = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", iCellNum);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//�����ص�ѹ�б�
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
				//�ɳ�索����ϵͳ����(Ŀǰֻ��1��)
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//�ɳ�索����ϵͳ��
				uchar = strFileData[latestOffset];
				fprintf(fpWrite, "%u ", uchar);
				leftOffset -= 1;
				if (leftOffset <= 0)
					break;
				latestOffset += 1;

				//�ɳ�索���¶�̽�����
				uchar = strFileData[latestOffset];
				uint16_t iProbNum = uchar * 256;
				uchar = strFileData[latestOffset + 1];
				iProbNum += uchar;
				fprintf(fpWrite, "%u ", iProbNum);
				leftOffset -= 2;
				if (leftOffset <= 0)
					break;
				latestOffset += 2;

				//�ɳ�索����ϵͳ���¶�̽���⵽���¶�ֵ
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