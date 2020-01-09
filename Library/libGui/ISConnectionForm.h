#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceDialogForm.h"
#include "ISIPAddressEdit.h"
#include "ISIntegerEdit.h"
#include "ISDatabaseEdit.h"
#include "ISCheckEdit.h"
#include "ISPasswordEdit.h"
#include "ISPushButton.h"
#include "ISLanguageEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISConnectionForm : public ISInterfaceDialogForm
{
	Q_OBJECT

signals:
	void AutiInputChanged(const QString &Login, const QString &Password);

public:
	ISConnectionForm(QWidget *parent = 0);
	virtual ~ISConnectionForm();

protected:
	void AfterShowEvent() override;
	void EnterClicked() override;
	void SaveSettings(); //Программное сохранение настроек
	bool CheckFields(); //Проверка на заполнение полей
	void SaveDatabaseName(); //Сохранение названия базы в temp

private:
	ISIPAddressEdit *EditServer;
	ISIntegerEdit *EditPort;
	ISDatabaseEdit *EditDatabase;
	ISLanguageEdit *EditLanguage;
	QGroupBox *GroupBoxInput;
	ISLineEdit *EditLogin;
	ISPasswordEdit *EditPassword;
	ISCheckEdit *CheckAutoboot;
	ISPushButton *ButtonSave;
};
//-----------------------------------------------------------------------------
