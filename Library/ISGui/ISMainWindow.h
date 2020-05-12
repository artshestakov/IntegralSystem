#pragma once
#ifndef _ISMAINWINDOW_H_INCLUDED
#define _ISMAINWINDOW_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISDesktopForm.h"
#include "ISMenuBar.h"
#include "ISStatusBar.h"
#include "ISCalendarForm.h"
#include "ISChatForm.h"
#include "ISWorkspaceForm.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISMainWindow : public ISInterfaceForm
{
	Q_OBJECT

	Q_PROPERTY(ISUuid CurrentParagraphUID READ GetCurrentParagraphUID WRITE SetCurrentParagraphUID)

public:
	ISMainWindow(QWidget *parent = 0);
	virtual ~ISMainWindow();
	
	void SetCurrentParagraphUID(const ISUuid &current_paragraph_uid);
	ISUuid GetCurrentParagraphUID() const;

protected:
	void closeEvent(QCloseEvent *CloseEvent);
	void AfterShowEvent() override;

	void CreateMenuBar(); //Создание главного тулбара
	void CreateInformationMessage(); //Создание информационного сообщения
	void CreateStackWidget(); //Создание начальной страницы и рабочей области
	void CreateStatusBar(); //Создание статус-бара

	void ParagraphClicked(const ISUuid &ParagraphUID); //Событие нажатия на кнопку-параграф
	
	void IncomingCall(const QVariantMap &VariantMap); //Сигнал о входящем звонке

	void OpenHistoryObject(const QString &TableName, int ObjectID);
	void OpenFavoritesObject(const QString &TableName, int ObjectID);
	void EscapeClicked() override;
	void InitializePlugin();
	void ChangeUser(); //Смена пользователя
	void ActivateWorkspace(); //Активация рабочей области
	void ShowFavoritesForm(); //Открыть форму избранного
	void ShowHistoryForm(); //Открыть форму истории
	void ShowDeviceSettings(); //Открыть форму настройки устройств
	void ShowChangePasswordForm(); //Открыть форму смены пароля
	void ShowNoteForm(); //Открыть форму "Блокнот"
	void ShowAddressBook(); //Открыть адресную книгу
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
	ISChatForm *ChatForm; //Виджет чата
	ISStatusBar *StatusBar;
	
	QString CurrentParagraphUID;
};
//-----------------------------------------------------------------------------
#endif
