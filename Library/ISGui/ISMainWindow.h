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
	void ParagraphClicked(const ISUuid &ParagraphUID); //������� ������� �� ������-��������
	void RollUp();
	void ChangeUser(); //����� ������������
	void ActivateWorkspace(); //��������� ������� �������
	void ShowFavoritesForm(); //������� ����� ����������
	void ShowHistoryForm(); //������� ����� �������
	void ShowChangePasswordForm(); //������� ����� ����� ������
	void ShowSettingsForm(); //������� ����� "���������"
	void ShowAboutForm(); //������� ����� "� ���������"
	void ShowAboutQt(); //������� ����� "� Qt"
	void Reconnect(); //����� ����� ���������������

private:
	ISMenuBar *MenuBar;

	std::map<ISUuid, int> Paragraphs;
	QStackedWidget *StackedWidget; //����-������� ��������
	ISDesktopParagraph *WidgetDesktop; //������ �������� �����
	ISWorkspaceParagraph *WorkspaceForm; //������ ������� �������
	ISCalendarParagraph *CalendarForm; //������ ���������
	
	QString CurrentParagraphUID;
	bool ExitConfirm; //���� ������������� ������
	bool ExistCheckModifie; //���� �������� ������������� ��������
	QPropertyAnimation *PropertyAnimation;
};
//-----------------------------------------------------------------------------
#endif
