#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISRangeEditBase.h"
#include "ISFieldEdits.h"
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
