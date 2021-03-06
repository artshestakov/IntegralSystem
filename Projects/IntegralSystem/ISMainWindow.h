#pragma once
#ifndef _ISMAINWINDOW_H_INCLUDED
#define _ISMAINWINDOW_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISParagraphs.h"
#include "ISMenuBar.h"
//-----------------------------------------------------------------------------
class ISMainWindow : public ISInterfaceForm
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
    void ParagraphClicked(const QString &ParagraphUID); //������� ������� �� ������-��������
    void RollUp();
    void ChangeUser(); //����� ������������
    void ActivateWorkspace(); //��������� ������� �������
    void ShowFavoritesForm(); //������� ����� ����������
    void ShowHistoryForm(); //������� ����� �������
    void ShowDatabaseSettings(); //������� ��������� ��
    void ShowChangePasswordForm(); //������� ����� ����� ������
    void ShowSettingsForm(); //������� ����� "���������"
    void ShowAboutForm(); //������� ����� "� ���������"
    void Reconnect(); //����� ����� ���������������

private:
    ISMenuBar *MenuBar;

    std::map<QString, int> Paragraphs;
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
