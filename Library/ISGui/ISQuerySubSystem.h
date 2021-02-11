#pragma once
#ifndef _ISQUERYSUBSYSTEM_H_INCLUDED
#define _ISQUERYSUBSYSTEM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceMetaForm.h"
#include "ISListWidget.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISQuerySubSystem : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISQuerySubSystem(QWidget *parent);
	virtual ~ISQuerySubSystem();

	void LoadData() override;

private:
	void ItemClicked(QListWidgetItem *ListWidgetItem);

private:
	ISListWidget *ListWidgetQuery;
	ISTextEdit *TextEdit;
};
//-----------------------------------------------------------------------------
#endif
