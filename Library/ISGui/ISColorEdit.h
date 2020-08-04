#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISFieldEditBase.h"
//-----------------------------------------------------------------------------
class ISColorEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISColorEdit(QWidget *parent = 0);
	virtual ~ISColorEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;

public slots:
	void SetReadOnly(bool read_only);

protected:
	void SelectColor();

private:
	QWidget *WidgetColor;
};
//-----------------------------------------------------------------------------
