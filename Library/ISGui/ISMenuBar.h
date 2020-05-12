#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISMainMenu.h"
#include "ISButtons.h"
#include "ISUuid.h"
#include "ISButtons.h"
//-----------------------------------------------------------------------------
class ISMenuBar : public QWidget
{
	Q_OBJECT

signals:
	void ParagraphClicked(const ISUuid &ParagraphUID);
	void ChangeUser();
	void Exit();
	void Favorites();
	void History();
	void DeviceSettings();
	void ChangePassword();
	void Settings();
	void HelpSystem();
	void AboutApplication();
	void AboutQt();

public:
	ISMenuBar(QWidget *parent = 0);
	virtual ~ISMenuBar();

	ISParagraphButton* GetParagraphButton(const ISUuid &ParagraphUID);
	void ButtonParagraphClicked(const ISUuid &ClickedParagraphUID);

	ISButtonNotify* GetbuttonNotify(); //Получить указатель на кнопку уведомлений

protected:
	void CreateMenuFile(); //Создание меню "Файл"
	void CreateMenuService(); //Создание меню "Сервис"
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

	QMap<ISUuid, ISParagraphButton*> ParagraphButtons;
};
//-----------------------------------------------------------------------------
