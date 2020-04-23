#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISDateTimeEdit.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISDateEdit : public ISDateTimeEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDateEdit(QWidget *parent);
	virtual ~ISDateEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;

	void SetMinimumDate(const QDate &Date);
	void SetMaximumDate(const QDate &Date);
};
//-----------------------------------------------------------------------------
