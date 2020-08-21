#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceMetaForm.h"
#include "ISScrollArea.h"
#include "ISMonitorUserWidget.h"
#include "ISCheckEdit.h"
//-----------------------------------------------------------------------------
class ISMonitorActivityForm : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISMonitorActivityForm(QWidget *parent = 0);
	virtual ~ISMonitorActivityForm();

	void LoadData() override;

protected:
	void CheckEditChanged(const QVariant &value);
	void Search(const QVariant &value);
	void ShowUserCard(); //Показать карточку пользователя
	void ShowProtocol(); //Просмотр протокола

private:
	ISCheckEdit *CheckEdit;
	QLabel *LabelCount;
	ISScrollArea *ScrollArea;
	std::vector<ISMonitorUserWidget*> VectorUsers;
};
//-----------------------------------------------------------------------------
