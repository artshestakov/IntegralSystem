#pragma once
#ifndef _ISSERVERLOGSSUBSYSTEM_H_INCLUDED
#define _ISSERVERLOGSSUBSYSTEM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceMetaForm.h"
#include "ISBaseTableView.h"
#include "ISTcpModels.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISServerLogsSubSystem : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISServerLogsSubSystem(QWidget *parent = 0);
	virtual ~ISServerLogsSubSystem();

	void LoadData() override;

private:
	void ItemDoubleClicked(QTreeWidgetItem *TreeWidgetItem, int Column);

private:
	QTreeWidget *TreeWidget;
	QGroupBox *GroupBox;
	QTextEdit *TextEdit;
};
//-----------------------------------------------------------------------------
#endif
