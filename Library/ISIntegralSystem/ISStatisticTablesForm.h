#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISInterfaceForm.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISStatisticTablesForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISStatisticTablesForm(QWidget *parent = 0);
	virtual ~ISStatisticTablesForm();

protected:
	void DetailsClicked();
};
//-----------------------------------------------------------------------------
