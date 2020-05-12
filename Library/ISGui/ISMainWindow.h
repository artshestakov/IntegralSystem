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

	void CreateMenuBar(); //�������� �������� �������
	void CreateInformationMessage(); //�������� ��������������� ���������
	void CreateStackWidget(); //�������� ��������� �������� � ������� �������
	void CreateStatusBar(); //�������� ������-����

	void ParagraphClicked(const ISUuid &ParagraphUID); //������� ������� �� ������-��������
	
	void IncomingCall(const QVariantMap &VariantMap); //������ � �������� ������

	void OpenHistoryObject(const QString &TableName, int ObjectID);
	void OpenFavoritesObject(const QString &TableName, int ObjectID);
	void EscapeClicked() override;
	void InitializePlugin();
	void ChangeUser(); //����� ������������
	void ActivateWorkspace(); //��������� ������� �������
	void ShowFavoritesForm(); //������� ����� ����������
	void ShowHistoryForm(); //������� ����� �������
	void ShowDeviceSettings(); //������� ����� ��������� ���������
	void ShowChangePasswordForm(); //������� ����� ����� ������
	void ShowNoteForm(); //������� ����� "�������"
	void ShowAddressBook(); //������� �������� �����
	void ShowSettingsForm(); //������� ����� "���������"
	void ShowAboutForm(); //������� ����� "� ���������"
	void ShowAboutQt(); //������� ����� "� Qt"

private:
	ISMenuBar *MenuBar;

	std::map<ISUuid, int> Paragraphs;
	QStackedWidget *StackedWidget; //����-������� ��������
	ISDesktopForm *WidgetDesktop; //������ �������� �����
	ISWorkspaceForm *WorkspaceForm; //������ ������� �������
	ISCalendarForm *CalendarForm; //������ ���������
	ISChatForm *ChatForm; //������ ����
	ISStatusBar *StatusBar;
	
	QString CurrentParagraphUID;
};
//-----------------------------------------------------------------------------
#endif
