#pragma once
#ifndef _ISDOUBLEEDIT_H_INCLUDED
#define _ISDOUBLEEDIT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISLineEdit.h"
#include "ISEdits.h"
//-----------------------------------------------------------------------------
class ISDoubleEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDoubleEdit(QWidget *parent = 0);
	virtual ~ISDoubleEdit();

	void SetValue(const QVariant &value) override;

protected:
	void TextChanged(const QString &Text) override;
};
//-----------------------------------------------------------------------------
#endif
