#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISLoginEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISLoginEdit(QWidget *parent);
	virtual ~ISLoginEdit();

	bool IsValid() const override;

protected:
	void LoginChanged();

private:
	bool Valid;
};
//-----------------------------------------------------------------------------
