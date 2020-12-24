#pragma once
#ifndef _ISMAINWINDOW_H_INCLUDED
#define _ISMAINWINDOW_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISDesktopParagraph.h"
#include "ISMenuBar.h"
#include "ISCalendarParagraph.h"
#include "ISWorkspaceParagraph.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISMainWindow : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISMainWindow(QWidget *parent = 0);
	virtual ~ISMainWindow();
	
protected:
	void closeEvent(QCloseEvent *CloseEvent);
	void AfterShowEvent() override;
	void EscapeClicked() override;

private:
	void ParagraphClicked(const ISUuid &ParagraphUID); //Событие нажатия на кнопку-параграф
	void RollUp();
	void ChangeUser(); //Смена пользователя
	void ActivateWorkspace(); //Активация рабочей области
	void ShowFavoritesForm(); //Открыть форму избранного
	void ShowHistoryForm(); //Открыть форму истории
	void ShowChangePasswordForm(); //Открыть форму смены пароля
	void ShowSettingsForm(); //Открыть форму "Настройки"
	void ShowAboutForm(); //Открыть форму "О программе"
	void ShowAboutQt(); //Открыть форму "О Qt"
	void Reconnect(); //Вызов формы переподключения

private:
	ISMenuBar *MenuBar;

	std::map<ISUuid, int> Paragraphs;
	QStackedWidget *StackedWidget; //Стек-главных виджетов
	ISDesktopParagraph *WidgetDesktop; //Виджет рабочего стола
	ISWorkspaceParagraph *WorkspaceForm; //Виджет рабочей области
	ISCalendarParagraph *CalendarForm; //Виджет календаря
	
	QString CurrentParagraphUID;
	bool ExitConfirm; //Флаг подтверждения выхода
	bool ExistCheckModifie; //Флаг проверки несохраненных объектов
	QPropertyAnimation *PropertyAnimation;
};
//-----------------------------------------------------------------------------
#endif
