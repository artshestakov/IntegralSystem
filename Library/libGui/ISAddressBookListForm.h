#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISAddressBookListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	ISAddressBookListForm(QWidget *parent = 0);
	virtual ~ISAddressBookListForm();

protected:
	void CreateObjectForm(QWidget *ObjectFormBase);
};
//-----------------------------------------------------------------------------
