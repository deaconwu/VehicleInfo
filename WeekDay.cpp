#include "WeekDay.h"

int days[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

bool CheckLeapYear(int year)
{
	return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int WeekIndex(int y, int m, int d)
{
	//今年的第几天
	int daySum = days[m-1] + d - 1;
	if (m > 2)
	{
		if (CheckLeapYear(y))
			daySum += 1;
	}

	//周几
	int yearTmp = y;
	if (m == 1 || m == 2) //把一月和二月换算成上一年的十三月和十四月  
	{
		m += 12;
		yearTmp--;
	}
	int iWeekIndex = (d + 2 * m + 3 * (m + 1) / 5 + yearTmp + yearTmp / 4 - yearTmp / 100 + yearTmp / 400) % 7;	//0:周一

	//本年1月1日是周几
	int year1st = y - 1;
	int month1st = 13;
	int day1st = 1;
	int iWeekFirstDay = (day1st + 2 * month1st + 3 * (month1st + 1) / 5 + year1st + year1st / 4 - year1st / 100 + year1st / 400) % 7;

	//本周是今年的第几周
	int nweek = (iWeekFirstDay + daySum) / 7 + 1;

	return nweek;
}