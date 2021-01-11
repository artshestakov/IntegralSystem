#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISAboutDialog : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISAboutDialog();
	virtual ~ISAboutDialog();

private:
	void CreateCommonTab(); //Общие
	void CreateContactsTab(); //Контакты
	void CreateModuleTab(); //Текущий модуль
	void CreateDescriptionTab(); //Описание
	void CreateLicenseTab(); //Лицензионное соглашение
	void CreateOtherTab(); //Прочее

	void AddLabel(QWidget *parent, const QString &LabelText, const QString &Text);
	void Feedback();

private:
	QVBoxLayout *LayoutRight;
	QTabWidget *TabWidget;
};
//-----------------------------------------------------------------------------
