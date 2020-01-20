#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceMetaForm.h"
#include "ISListWidget.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISInternalDirectoriesForm : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISInternalDirectoriesForm(QWidget *parent = 0);
	virtual ~ISInternalDirectoriesForm();

	void LoadData() override;

protected:
	void ItemSelectionChanged();

private:
	ISListBaseForm *ListBaseForm;

	QHBoxLayout *Layout;
	ISListWidget *ListWidget;
	QLabel *Label;
};
//-----------------------------------------------------------------------------
