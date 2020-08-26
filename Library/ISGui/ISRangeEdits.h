#pragma once
#ifndef _ISRANGEEDITS_H_INCLUDED
#define _ISRANGEEDITS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISRangeEditBase.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISRangeDateTimeEdit : public ISRangeEditBase
{
	Q_OBJECT

public:
	ISRangeDateTimeEdit(QWidget *parent = 0, const QString &FieldName = QString());
	virtual ~ISRangeDateTimeEdit();

	virtual void SetValue(const ISRangeStruct &range) override; //Изменить значение
	virtual ISRangeStruct GetValue() const override; //Получить значение

private:
	ISDateTimeEdit *BeginDateTimeEdit;
	ISDateTimeEdit *EndDateTimeEdit;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISRangeDateEdit : public ISRangeEditBase
{
	Q_OBJECT

public:
	ISRangeDateEdit(QWidget *parent = 0, const QString &FieldName = QString());
	virtual ~ISRangeDateEdit();

	void SetValue(const ISRangeStruct &range) override;
	ISRangeStruct GetValue() const override;

protected:
	void PeriodChanged(const QVariant &value);

private:
	ISDateEdit *BeginDateEdit;
	ISDateEdit *EndDateEdit;
};
//-----------------------------------------------------------------------------
#endif
