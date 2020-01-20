#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISPatriotClientsBirthdayForm : public ISInterfaceForm
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
