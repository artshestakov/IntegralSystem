#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISPhoneBaseEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISPhoneEdit : public ISPhoneBaseEdit
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
