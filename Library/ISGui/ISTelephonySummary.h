#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class ISTelephonySummary
{
public:
	static int GetCountIncoming(const ISRangeStruct &RangeStruct); //�������� ����� ���������� �������� ������� �� ���������
	static int GetCountOutcoming(const ISRangeStruct &RangeStruct); //�������� ����� ���������� ��������� ������� �� ���������
	static QString GetDurationAll(const ISRangeStruct &RangeStruct); //�������� ����� ������������ ���� ������� �� ���������
	static QString GetDurationIncoming(const ISRangeStruct &RangeStruct); //�������� ����� ������������ ���� �������� ������� �� ���������
	static QString GetDurationOutcoming(const ISRangeStruct &RangeStruct); //�������� ����� ������������ ���� ��������� ������� �� ���������
	static QString GetAverageIncoming(const ISRangeStruct &RangeStruct); //�������� ������� ������������ �������� ������� �� ���������
	static QString GetAverageOutcoming(const ISRangeStruct &RangeStruct); //�������� ������� ������������ ��������� ������� �� ���������
};
//-----------------------------------------------------------------------------
