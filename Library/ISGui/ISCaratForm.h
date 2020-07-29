#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceMetaForm.h"
//-----------------------------------------------------------------------------
class ISCaratForm : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCaratForm(QWidget *parent = 0);
	virtual ~ISCaratForm();

	void LoadData() override;

protected:
	void CoreChecked(const QVariant &value);
};
//-----------------------------------------------------------------------------
