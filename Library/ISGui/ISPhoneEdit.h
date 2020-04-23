#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISPhoneBaseEdit.h"
//-----------------------------------------------------------------------------
class ISPhoneEdit : public ISPhoneBaseEdit
{
	Q_OBJECT

signals:
	void Called();

public:
	Q_INVOKABLE ISPhoneEdit(QWidget *parent);
	virtual ~ISPhoneEdit();

protected:
	void Call() override;
};
//-----------------------------------------------------------------------------
