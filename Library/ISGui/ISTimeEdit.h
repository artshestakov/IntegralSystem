#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISDateTimeEdit.h"
//-----------------------------------------------------------------------------
class ISTimeEdit : public ISDateTimeEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTimeEdit(QWidget *parent);
	virtual ~ISTimeEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
};
//-----------------------------------------------------------------------------
