#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISFieldEditBase.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISRangeEditBase : public QWidget
{
	Q_OBJECT

public:
	ISRangeEditBase(QWidget *parent = 0, const QString &FieldName = QString());
	virtual ~ISRangeEditBase();

	virtual void SetValue(const ISRangeStruct &range) = 0; //Изменить значение
	virtual ISRangeStruct GetValue() const = 0; //Получить значение

protected:
	void AddFields(const QString &BeginString, ISFieldEditBase *begin_field_edit, const QString &EndString, ISFieldEditBase *end_field_edit);
	void AddWidgetToLeft(QWidget *Widget);
	void AddWidgetToRight(QWidget *Widget);

private:
	QHBoxLayout *LayoutLeft;
	QHBoxLayout *LayoutBegin;
	QLabel *LabelBegin;
	ISFieldEditBase *BeginFieldEdit;

	QHBoxLayout *LayoutRight;
	QHBoxLayout *LayoutEnd;
	QLabel *LabelEnd;
	ISFieldEditBase *EndFieldEdit;
};
//-----------------------------------------------------------------------------
