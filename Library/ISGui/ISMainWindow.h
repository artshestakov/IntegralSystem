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
	
protected:
	void AfterShowEvent() override;
	void EscapeClicked() override;

private:
	void closeEvent(QCloseEvent *CloseEvent);
	

	void CreateInformationMessage(); //�������� ��������������� ���������
	void CreateStackWidget(); //�������� ��������� �������� � ������� �������

	void ParagraphClicked(const ISUuid &ParagraphUID); //������� ������� �� ������-��������

	void RollUp();
	void InitializePlugin();
	void ChangeUser(); //����� ������������
	void ActivateWorkspace(); //��������� ������� �������
	void ShowFavoritesForm(); //������� ����� ����������
	void ShowHistoryForm(); //������� ����� �������
	void ShowChangePasswordForm(); //������� ����� ����� ������
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
	
	QString CurrentParagraphUID;
	bool ExitConfirm;
	QPropertyAnimation *PropertyAnimation;
};
//-----------------------------------------------------------------------------
#endif
