#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceMetaForm.h"
#include "ISListWidget.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISInternalDirectoriesSubSystem : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISInternalDirectoriesSubSystem(QWidget *parent = 0);
	virtual ~ISInternalDirectoriesSubSystem();

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
