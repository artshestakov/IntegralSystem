#pragma once
#ifndef _ISMAINWINDOW_H_INCLUDED
#define _ISMAINWINDOW_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISDesktopForm.h"
#include "ISMenuBar.h"
#include "ISCalendarForm.h"
#include "ISWorkspaceForm.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISMainWindow : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISMainWindow(QWidget *parent = 0);
	virtual ~ISMainWindow();
	
private:
	void closeEvent(QCloseEvent *CloseEvent);
	void AfterShowEvent() override;

	void CreateMenuBar(); //Создание главного тулбара
	void CreateInformationMessage(); //Создание информационного сообщения
	void CreateStackWidget(); //Создание начальной страницы и рабочей области

	void ParagraphClicked(const ISUuid &ParagraphUID); //Событие нажатия на кнопку-параграф

	void EscapeClicked() override;
	void InitializePlugin();
	void ChangeUser(); //Смена пользователя
	void ActivateWorkspace(); //Активация рабочей области
	void ShowFavoritesForm(); //Открыть форму избранного
	void ShowHistoryForm(); //Открыть форму истории
	void ShowChangePasswordForm(); //Открыть форму смены пароля
	void ShowSettingsForm(); //Открыть форму "Настройки"
	void ShowAboutForm(); //Открыть форму "О программе"
	void ShowAboutQt(); //Открыть форму "О Qt"

private:
	ISMenuBar *MenuBar;

	std::map<ISUuid, int> Paragraphs;
	QStackedWidget *StackedWidget; //Стек-главных виджетов
	ISDesktopForm *WidgetDesktop; //Виджет рабочего стола
	ISWorkspaceForm *WorkspaceForm; //Виджет рабочей области
	ISCalendarForm *CalendarForm; //Виджет календаря
	
	QString CurrentParagraphUID;
	bool ExitConfirm;
};
//-----------------------------------------------------------------------------
#endif
