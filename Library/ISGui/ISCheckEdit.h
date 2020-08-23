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

	void SetReadOnly(bool read_only) override;
	void SetText(const QString &Text);
	void SetCheckableStrikeOut(bool StrikeOut); //≈сли включить это свойство, то при активном состо€нии текст будет зачеркнутым

protected:
	QCheckBox* GetCheckBox();

private:
	void CheckableStrikeOut();

private:
	QCheckBox *CheckBox;
};
//-----------------------------------------------------------------------------
#endif
