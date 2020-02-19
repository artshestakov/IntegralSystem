#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISMainMenu.h"
#include "ISButtons.h"
#include "ISParagraphButton.h"
#include "ISUuid.h"
#include "ISButtons.h"
//-----------------------------------------------------------------------------
class ISMenuBar : public QWidget
{
	Q_OBJECT

signals:
	void CreateRecords();
	void ExternalTools();
	void ParagraphClicked(const ISUuid &ParagraphUID);
	void Lock();
	void ChangeUser();
	void Exit();
	void Favorites();
	void History();
	void DeviceSettings();
	void ChangePassword();
	void CreateLogToday();
	void UserStatus();
	void Settings();
	void Notebook();
	void Calculator();
	void AddressBook();
	void HelpSystem();
	void AboutApplication();
	void AboutQt();

public:
	ISMenuBar(QWidget *parent = 0);
	virtual ~ISMenuBar();

	ISParagraphButton* GetParagraphButton(const ISUuid &ParagraphUID);
	void ButtonParagraphClicked(const ISUuid &ClickedParagraphUID);

	ISButtonNotify* GetbuttonNotify(); //�������� ��������� �� ������ �����������

protected:
	void CreateFastAccessMenu(); //�������� �������� �������
	void CreateMenuFile(); //�������� ���� "����"
	void CreateMenuService(); //�������� ���� "������"
	void CreateMenuAddition(); //�������� ���� "�������"
	void CreateMenuHelp(); //�������� ���� "�������"
	
	void CreateButtonsPanel(); //�������� ������ � ��������

	QToolButton* CreateButton(const QString &ToolTip, const QString &IconName);
	void ShowMainMenu();
	void HideMainMenu();

	void CheckVisibleMunuItems(); //�������� ����������� ������� �������� ����
	void InstallFontMenuItems(); //��������� ������ ��� ���� ��������� �������� ����
	void CreateParagraphs(); //�������� ����������
	void ButtonParagraphClicked();

private:
	QHBoxLayout *MainLayout;
	QHBoxLayout *LayoutParagraphs;
	QHBoxLayout *LayoutButtons;

	ISButtonMainMenu *ButtonMenu;
	ISMainMenu *MainMenu;
	ISButtonNotify *ButtonNotify;
	QAbstractButton *CurrentButton;

	QMap<ISUuid, ISParagraphButton*> ParagraphButtons;
};
//-----------------------------------------------------------------------------
