#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISStatisticTablesForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISStatisticTablesForm(QWidget *parent = 0);
	virtual ~ISStatisticTablesForm();

protected:
	void DetailsClicked();
};
//-----------------------------------------------------------------------------
