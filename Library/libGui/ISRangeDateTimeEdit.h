#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISRangeEditBase.h"
#include "ISDateTimeEdit.h"
//-----------------------------------------------------------------------------
class ISRangeDateTimeEdit : public ISRangeEditBase
{
	Q_OBJECT

public:
	ISRangeDateTimeEdit(QWidget *parent = 0, const QString &FieldName = QString());
	virtual ~ISRangeDateTimeEdit();

	virtual void SetValue(const ISRangeStruct &range) override; //�������� ��������
	virtual ISRangeStruct GetValue() const override; //�������� ��������

private:
	ISDateTimeEdit *BeginDateTimeEdit;
	ISDateTimeEdit *EndDateTimeEdit;
};
//-----------------------------------------------------------------------------
