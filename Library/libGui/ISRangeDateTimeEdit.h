#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISRangeEditBase.h"
#include "ISDateTimeEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISRangeDateTimeEdit : public ISRangeEditBase
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
