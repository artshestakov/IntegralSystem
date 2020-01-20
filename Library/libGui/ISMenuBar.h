#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
#include "ISMainMenu.h"
#include "ISButtonNotify.h"
#include "ISParagraphButton.h"
#include "ISUuid.h"
#include "ISButtonMainMenu.h"
//-----------------------------------------------------------------------------
class ISMenuBar : public QWidget
{
	Q_OBJECT

signals:
	void CreateRecords();
	void ExternalTools();
	void ParagraphClicked(const ISUuid &ParagraphUID);
	void Lock();
	void ChangeUser();
	void Exit();
	void Favorites();
	void History();
	void DeviceSettings();
	void ChangePassword();
	void CreateLogToday();
	void Debug();
	void UserStatus();
	void Settings();
	void Notebook();
	void Calculator();
	void AddressBook();
	void HelpSystem();
	void AboutApplication();
	void FeedbackForm();
	void AboutQt();
	void License();

public:
	ISMenuBar(QWidget *parent = 0);
	virtual ~ISMenuBar();

	ISParagraphButton* GetParagraphButton(const ISUuid &ParagraphUID);
	void ButtonParagraphClicked(const ISUuid &ClickedParagraphUID);

	ISButtonNotify* GetbuttonNotify(); //Получить указатель на кнопку уведомлений

protected:
	void CreateFastAccessMenu(); //Создание быстрого доступа
	void CreateMenuFile(); //Создание меню "Файл"
	void CreateMenuService(); //Создание меню "Сервис"
	void CreateMenuAddition(); //Создание меню "Утилиты"
	void CreateMenuHelp(); //Создание меню "Справка"
	
	void CreateButtonsPanel(); //Создание панели с кнопками

	QToolButton* CreateButton(const QString &ToolTip, const QString &IconName);
	void ShowMainMenu();
	void HideMainMenu();

	void CheckVisibleMunuItems(); //Проверка отображения пунктов главного меню
	void InstallFontMenuItems(); //Установка шрифта для всех элементов главного меню
	void CreateParagraphs(); //Создание параграфов
	void ButtonParagraphClicked();

private:
	QHBoxLayout *MainLayout;
	QHBoxLayout *LayoutParagraphs;
	QHBoxLayout *LayoutButtons;

	ISButtonMainMenu *ButtonMenu;
	ISMainMenu *MainMenu;
	ISButtonNotify *ButtonNotify;
	QAbstractButton *CurrentButton;

	QMap<ISUuid, ISParagraphButton*> ParagraphButtons;
};
//-----------------------------------------------------------------------------
