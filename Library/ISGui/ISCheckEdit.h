#pragma once
#ifndef _ISCHECKEDIT_H_INCLUDED
#define _ISCHECKEDIT_H_INCLUDED
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
	void SetReadOnly(bool read_only) override;

protected:
	QCheckBox* GetCheckBox();

private:
	QCheckBox *CheckBox;
};
//-----------------------------------------------------------------------------
#endif
