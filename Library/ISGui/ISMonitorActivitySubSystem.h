#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceMetaForm.h"
#include "ISScrollArea.h"
#include "ISMonitorUserWidget.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISMonitorActivitySubSystem : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISMonitorActivitySubSystem(QWidget *parent = 0);
	virtual ~ISMonitorActivitySubSystem();

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
