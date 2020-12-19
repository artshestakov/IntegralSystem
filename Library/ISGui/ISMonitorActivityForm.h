#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceMetaForm.h"
#include "ISScrollArea.h"
#include "ISMonitorUserWidget.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISMonitorActivityForm : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISMonitorActivityForm(QWidget *parent = 0);
	virtual ~ISMonitorActivityForm();

	void LoadData() override;

protected:
	void ShowUserCard(); //Показать карточку пользователя
	void ShowProtocol(); //Просмотр протокола

private:
	QLabel *LabelCount;
	ISScrollArea *ScrollArea;
	std::vector<ISMonitorUserWidget*> VectorUsers;
};
//-----------------------------------------------------------------------------
