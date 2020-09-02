#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISAboutForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISAboutForm();
	virtual ~ISAboutForm();

protected:
	void CreateCommonTab(); //Общие
	void CreateContactsTab(); //Контакты
	void CreateModuleTab(); //Текущий модуль
	void CreateDescriptionTab(); //Описание
	void CreateLicenseTab(); //Лицензионное соглашение
	void CreateOtherTab(); //Прочее

	void AddLabel(QWidget *parent, const QString &LabelText, const QString &Text);

private:
	QVBoxLayout *LayoutRight;
	QTabWidget *TabWidget;
};
//-----------------------------------------------------------------------------
