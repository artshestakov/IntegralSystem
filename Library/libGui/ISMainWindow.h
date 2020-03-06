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

	void LockApplication(); //���������� ���������

protected:
	void closeEvent(QCloseEvent *e);
	void AfterShowEvent() override;

	void CreateMenuBar(); //�������� �������� �������
	void CreateInformationMessage(); //�������� ��������������� ���������
	void CreateStackWidget(); //�������� ��������� �������� � ������� �������
	void CreateStatusBar(); //�������� ������-����

	void ShowCreateRecords(); //�������� ����� �������� �������� �������� �������
	void ShowExternalTools(); //�������� ����� �������� �������� �������
	void ParagraphClicked(const ISUuid &ParagraphUID); //������� ������� �� ������-��������
	
	void TerminateMe(const QVariantMap &VariantMap); //������ �� ���������� ������������
	void UpdateAviable(const QVariantMap &VariantMap); //������ � ������� ����������
	void IncomingCall(const QVariantMap &VariantMap); //������ � �������� ������
	void AlreadyConnected(const QVariantMap &VariantMap); //������ �� ����������� �����������

	void BeforeClose(); //�������� ����� ����������� ���������
	void OpenHistoryObject(const QString &TableName, int ObjectID);
	void OpenFavoritesObject(const QString &TableName, int ObjectID);
	void EscapeClicked() override;
	void InitializePlugin();
	void LockClicked(); //���������� ���������
	void ChangeUser(); //����� ������������
	void ActivateWorkspace(); //��������� ������� �������
	void ShowFavoritesForm(); //������� ����� ����������
	void ShowHistoryForm(); //������� ����� �������
	void ShowDeviceSettings(); //������� ����� ��������� ���������
	void ShowChangePasswordForm(); //������� ����� ����� ������
	void ShowNoteForm(); //������� ����� "�������"
	void ShowCalculator(); //������� �����������
	void ShowAddressBook(); //������� �������� �����
	void UserStatusChange(); //��������� �������� ������� ������������
	void ShowSettingsForm(); //������� ����� "���������"
	void ShowHelpSystem(); //������� ���������� �������
	void ShowAboutForm(); //������� ����� "� ���������"
	void ShowAboutQt(); //������� ����� "� Qt"

	void MakeCall();

private:
	ISMenuBar *MenuBar;

	QMap<ISUuid, int> Paragraphs;
	QStackedWidget *StackedWidget; //����-������� ��������
	ISDesktopForm *WidgetDesktop; //������ �������� �����
	ISWorkspaceForm *WorkspaceForm; //������ ������� �������
	ISCalendarForm *CalendarForm; //������ ���������
	ISChatForm *ChatForm; //������ ����
	ISPhoneForm *PhoneForm;
	ISStatusBar *StatusBar;
	
	QString CurrentParagraphUID;
	bool CloseEvent;
};
//-----------------------------------------------------------------------------
