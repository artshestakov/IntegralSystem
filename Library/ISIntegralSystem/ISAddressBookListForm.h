#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISAddressBookListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	ISAddressBookListForm(QWidget *parent = 0);
	virtual ~ISAddressBookListForm();

protected:
	void CreateObjectForm(QWidget *ObjectFormBase);
};
//-----------------------------------------------------------------------------
