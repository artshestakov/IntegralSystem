#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceMetaForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISCaratForm : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCaratForm(QWidget *parent = 0);
	virtual ~ISCaratForm();

	void LoadData();

protected:
	void CoreChecked(const QVariant &value);
};
//-----------------------------------------------------------------------------
