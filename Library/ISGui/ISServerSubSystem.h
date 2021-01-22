#pragma once
#ifndef _ISSERVERSUBSYSTEM_H_INCLUDED
#define _ISSERVERSUBSYSTEM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceMetaForm.h"
#include "ISBaseTableView.h"
#include "ISTcpModels.h"
//-----------------------------------------------------------------------------
class ISServerSubSystem : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISServerSubSystem(QWidget *parent = 0);
	virtual ~ISServerSubSystem();

	void LoadData() override;

private:
	void CreateTabLog(); //������� "�������� �������"
	void TabLogDateChanged(const QVariant &Date);

private:
	QTabWidget *TabWidget;
	ISBaseTableView *TableView;
	ISLogModel *LogModel;
	QLabel *LabelBottom;
};
//-----------------------------------------------------------------------------
#endif
