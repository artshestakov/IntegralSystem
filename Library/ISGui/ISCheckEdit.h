#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISFieldEditBase.h"
//-----------------------------------------------------------------------------
class ISCheckEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCheckEdit(QWidget *parent = 0);
	virtual ~ISCheckEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;

	void SetText(const QString &Text);
	
public slots:
	void SetReadOnly(bool ReadOnly);

protected:
	QCheckBox* GetCheckBox();

private:
	QCheckBox *CheckBox;
};
//-----------------------------------------------------------------------------
