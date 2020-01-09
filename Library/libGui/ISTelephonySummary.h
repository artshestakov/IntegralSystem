#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISTelephonySummary : public QObject
{
	Q_OBJECT

public:
	ISTelephonySummary();
	virtual ~ISTelephonySummary();

	static int GetCountIncoming(const ISRangeStruct &RangeStruct); //�������� ����� ���������� �������� ������� �� ���������
	static int GetCountOutcoming(const ISRangeStruct &RangeStruct); //�������� ����� ���������� ��������� ������� �� ���������
	static QString GetDurationAll(const ISRangeStruct &RangeStruct); //�������� ����� ������������ ���� ������� �� ���������
	static QString GetDurationIncoming(const ISRangeStruct &RangeStruct); //�������� ����� ������������ ���� �������� ������� �� ���������
	static QString GetDurationOutcoming(const ISRangeStruct &RangeStruct); //�������� ����� ������������ ���� ��������� ������� �� ���������
	static QString GetAverageIncoming(const ISRangeStruct &RangeStruct); //�������� ������� ������������ �������� ������� �� ���������
	static QString GetAverageOutcoming(const ISRangeStruct &RangeStruct); //�������� ������� ������������ ��������� ������� �� ���������
};
//-----------------------------------------------------------------------------
