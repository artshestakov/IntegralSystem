#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISInterfaceForm.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISPatriotClientsBirthdayForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISPatriotClientsBirthdayForm(QWidget *parent = 0);
	virtual ~ISPatriotClientsBirthdayForm();

	void AddClient(int ClientID);

private:
	ISListWidget *ListWidget;
};
//-----------------------------------------------------------------------------
