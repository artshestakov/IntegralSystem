#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISFieldEditBase.h"
//-----------------------------------------------------------------------------
class ISSexEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISSexEdit(QWidget *parent = 0);
	virtual ~ISSexEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;

	void SetFont(const QFont &Font);
	void SetReadOnly(bool read_only);

private:
	void ButtonClicked(QAbstractButton *AbstractButton);

private:
	QWidget *Widget;
	QButtonGroup *ButtonGroup;
	int CurrentID;
};
//-----------------------------------------------------------------------------
