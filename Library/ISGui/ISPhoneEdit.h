#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISPhoneBaseEdit.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISPhoneEdit : public ISPhoneBaseEdit
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
