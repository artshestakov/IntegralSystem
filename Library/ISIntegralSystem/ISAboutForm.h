#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISAboutForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISAboutForm(QWidget *parent = 0);
	virtual ~ISAboutForm();

protected:
	void CreateCommonTab(); //Общие
	void CreateContactsTab(); //Контакты
	void CreateModuleTab(); //Текущий модуль
	void CreateDescriptionTab(); //Описание
	void CreateLicenseTab(); //Лицензионное соглашение
	void CreateShortcuts(); //Комбинации клавиш
	void CreateOtherTab(); //Прочее

	void AddLabel(QWidget *parent, const QString &LabelText, const QString &Text);
	void SaveAuthorInfo(); //Сохранить информацию о разработчике

private:
	QVBoxLayout *LayoutRight;
	QTabWidget *TabWidget;
};
//-----------------------------------------------------------------------------
