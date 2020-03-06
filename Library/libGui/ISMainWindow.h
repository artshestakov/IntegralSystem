#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISDesktopForm.h"
#include "ISMenuBar.h"
#include "ISStatusBar.h"
#include "ISCalendarForm.h"
#include "ISChatForm.h"
#include "ISWorkspaceForm.h"
#include "ISPhoneForm.h"
//-----------------------------------------------------------------------------
class ISMainWindow : public ISInterfaceForm
{
	Q_OBJECT

	Q_PROPERTY(bool CloseEvent READ GetCloseEventFlag WRITE SetCloseEventFlag)
	Q_PROPERTY(ISUuid CurrentParagraphUID READ GetCurrentParagraphUID WRITE SetCurrentParagraphUID)

signals:
	void Unlocked();

public:
	ISMainWindow(QWidget *parent = 0);
	virtual ~ISMainWindow();
	
	void SetCloseEventFlag(bool close_event);
	bool GetCloseEventFlag() const;

	void SetCurrentParagraphUID(const ISUuid &current_paragraph_uid);
	ISUuid GetCurrentParagraphUID() const;

	void LockApplication(); //Блокировка программы

protected:
	void closeEvent(QCloseEvent *e);
	void AfterShowEvent() override;

	void CreateMenuBar(); //Создание главного тулбара
	void CreateInformationMessage(); //Создание информационного сообщения
	void CreateStackWidget(); //Создание начальной страницы и рабочей области
	void CreateStatusBar(); //Создание статус-бара

	void ShowCreateRecords(); //Показать форму настроек быстрого создания записей
	void ShowExternalTools(); //Показать форму настроек быстрого доступа
	void ParagraphClicked(const ISUuid &ParagraphUID); //Событие нажатия на кнопку-параграф
	
	void TerminateMe(const QVariantMap &VariantMap); //Сигнал об отключении пользователя
	void UpdateAviable(const QVariantMap &VariantMap); //Сигнал о наличии обновления
	void IncomingCall(const QVariantMap &VariantMap); //Сигнал о входящем звонке
	void AlreadyConnected(const QVariantMap &VariantMap); //Сигнал об аналогичном подключении

	void BeforeClose(); //Действие перед завершением программы
	void OpenHistoryObject(const QString &TableName, int ObjectID);
	void OpenFavoritesObject(const QString &TableName, int ObjectID);
	void EscapeClicked() override;
	void InitializePlugin();
	void LockClicked(); //Блокировка программы
	void ChangeUser(); //Смена пользователя
	void ActivateWorkspace(); //Активация рабочей области
	void ShowFavoritesForm(); //Открыть форму избранного
	void ShowHistoryForm(); //Открыть форму истории
	void ShowDeviceSettings(); //Открыть форму настройки устройств
	void ShowChangePasswordForm(); //Открыть форму смены пароля
	void ShowNoteForm(); //Открыть форму "Блокнот"
	void ShowCalculator(); //Открыть калькулятор
	void ShowAddressBook(); //Открыть адресную книгу
	void UserStatusChange(); //Изменение текущего статуса пользователя
	void ShowSettingsForm(); //Открыть форму "Настройки"
	void ShowHelpSystem(); //Открыть справочную систему
	void ShowAboutForm(); //Открыть форму "О программе"
	void ShowAboutQt(); //Открыть форму "О Qt"

	void MakeCall();

private:
	ISMenuBar *MenuBar;

	QMap<ISUuid, int> Paragraphs;
	QStackedWidget *StackedWidget; //Стек-главных виджетов
	ISDesktopForm *WidgetDesktop; //Виджет рабочего стола
	ISWorkspaceForm *WorkspaceForm; //Виджет рабочей области
	ISCalendarForm *CalendarForm; //Виджет календаря
	ISChatForm *ChatForm; //Виджет чата
	ISPhoneForm *PhoneForm;
	ISStatusBar *StatusBar;
	
	QString CurrentParagraphUID;
	bool CloseEvent;
};
//-----------------------------------------------------------------------------
