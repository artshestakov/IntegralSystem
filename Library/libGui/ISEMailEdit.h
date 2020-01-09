#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISEMailEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISEMailEdit(QWidget *parent);
	virtual ~ISEMailEdit();

protected:
	void Send();
};
//-----------------------------------------------------------------------------
