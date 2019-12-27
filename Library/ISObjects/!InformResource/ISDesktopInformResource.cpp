#include "StdAfx.h"
#include "ISDesktopInformResource.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISQuery.h"
#include "ISMetaUser.h"
#include "ISGui.h"
#include "ISMetaData.h"
#include "ISUserRoleEntity.h"
#include "ISBuffer.h"
#include "ISPushButton.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
static QString QI_CALL_HISTORY = PREPARE_QUERY("INSERT INTO callhistory(clhs_organization, clhs_phone, clhs_result) "
											   "VALUES(:Organization, :Phone, (SELECT clrs_id FROM callresult WHERE clrs_uid = '{5A92BB6B-9D51-4189-8BCE-787792964C99}')) "
											   "RETURNING clhs_id");
//-----------------------------------------------------------------------------
static QString QS_DEMO_ORGANIZATION = PREPARE_QUERY("SELECT dacc_organization FROM demoaccesses WHERE dacc_id = :DemoID");
//-----------------------------------------------------------------------------
static QString QS_DESKTOP_DAY_NOTE = PREPARE_QUERY("SELECT ddnt_note "
												   "FROM desktopdaynote "
												   "WHERE ddnt_user = :UserID "
												   "AND ddnt_date = :Date");
//-----------------------------------------------------------------------------
static QString QI_DESKTOP_DAY_NOTE = PREPARE_QUERY("INSERT INTO desktopdaynote(ddnt_date, ddnt_note) "
												   "VALUES(:Date, :Note)");
//-----------------------------------------------------------------------------
static QString QU_DESKTOP_DAY_NOTE = PREPARE_QUERY("UPDATE desktopdaynote SET "
												   "ddnt_note = :Note "
												   "WHERE ddnt_user = :UserID "
												   "AND ddnt_date = :Date");
//-----------------------------------------------------------------------------
ISDesktopInformResource::ISDesktopInformResource(QWidget *parent) : QWidget(parent)
{
	MainLayout = new QVBoxLayout();
	MainLayout->setContentsMargins(LAYOUT_MARGINS_NULL);
	setLayout(MainLayout);

	LayoutCentral = new QHBoxLayout();
	MainLayout->addLayout(LayoutCentral);

	CreateLeftPanel();
	CreateCentralPanel();
	CreateRightPanel();
}
//-----------------------------------------------------------------------------
ISDesktopInformResource::~ISDesktopInformResource()
{

}
//-----------------------------------------------------------------------------
void ISDesktopInformResource::showEvent(QShowEvent *e)
{
	QWidget::showEvent(e);
	Reload();
}
//-----------------------------------------------------------------------------
void ISDesktopInformResource::CreateLeftPanel()
{
	QVBoxLayout *LayoutLeft = new QVBoxLayout();
	LayoutCentral->addLayout(LayoutLeft);
	
	QGroupBox *GroupBoxDemoAccess = new QGroupBox(this);
	GroupBoxDemoAccess->setTitle(LOCALIZATION("DemoAccessesInWord"));
	GroupBoxDemoAccess->setLayout(new QVBoxLayout());
	LayoutLeft->addWidget(GroupBoxDemoAccess);

	ListViewDemoAccesses = new ISListViewWidget(ISMetaData::GetInstanse().GetMetaQuery("UserDemoAccesses"), GroupBoxDemoAccess);
	connect(ListViewDemoAccesses, &ISListViewWidget::DoubleClickedSignal, this, &ISDesktopInformResource::DoubleClickedDemo);
	GroupBoxDemoAccess->layout()->addWidget(ListViewDemoAccesses);

	QGroupBox *GroupBoxOrganizations = new QGroupBox(this);
	GroupBoxOrganizations->setTitle(LOCALIZATION("OrganizationsInWork"));
	GroupBoxOrganizations->setLayout(new QVBoxLayout());
	LayoutLeft->addWidget(GroupBoxOrganizations);

	ListViewOrganizations = new ISListViewWidget(ISMetaData::GetInstanse().GetMetaQuery("UserOrganizations"), GroupBoxOrganizations);
	connect(ListViewOrganizations, &ISListViewWidget::DoubleClickedSignal, this, &ISDesktopInformResource::DoubleClickedOganization);
	GroupBoxOrganizations->layout()->addWidget(ListViewOrganizations);
}
//-----------------------------------------------------------------------------
void ISDesktopInformResource::CreateCentralPanel()
{
	QVBoxLayout *LayoutCenter = new QVBoxLayout();
	LayoutCentral->addLayout(LayoutCenter);

	UserEdit = new ISUserEdit(this);
	UserEdit->SetValue(CURRENT_USER_ID);
	UserEdit->HideFirstItem();
	UserEdit->setVisible(ISUserRoleEntity::GetInstance().CheckAccessSpecial("{D67DE9D1-0092-406D-910C-0E11ED820561}"));
	connect(UserEdit, &ISUserEdit::DataChanged, this, &ISDesktopInformResource::Reload);
	LayoutCenter->addWidget(UserEdit, 0, Qt::AlignHCenter);

	CalendarWidget = new ISCalendarWidget(this);
	CalendarWidget->setSizePolicy(CalendarWidget->sizePolicy().horizontalPolicy(), QSizePolicy::Maximum);
	connect(CalendarWidget, &ISCalendarWidget::selectionChanged, this, &ISDesktopInformResource::Reload);
	LayoutCenter->addWidget(CalendarWidget, 0, Qt::AlignHCenter);

	TextEdit = new ISTextEdit(this);
	TextEdit->SetPlaceholderText(LOCALIZATION("InputNoteFromDay"));
	TextEdit->SetVisibleClear(false);
	TextEdit->setSizePolicy(QSizePolicy::Maximum, TextEdit->sizePolicy().verticalPolicy());
	LayoutCenter->addWidget(TextEdit);

	ISPushButton *ButtonSaveNote = new ISPushButton(this);
	ButtonSaveNote->setText(LOCALIZATION("Save"));
	ButtonSaveNote->setIcon(BUFFER_ICONS("Save"));
	connect(ButtonSaveNote, &ISPushButton::clicked, this, &ISDesktopInformResource::UpdateDayNote);
	LayoutCenter->addWidget(ButtonSaveNote, 0, Qt::AlignLeft);
}
//-----------------------------------------------------------------------------
void ISDesktopInformResource::CreateRightPanel()
{
	QVBoxLayout *LayoutGroupBox = new QVBoxLayout();
	LayoutGroupBox->setContentsMargins(LAYOUT_MARGINS_5_PX);

	QGroupBox *GroupBoxCallHistory = new QGroupBox(this);
	GroupBoxCallHistory->setTitle(LOCALIZATION("CallHistory"));
	GroupBoxCallHistory->setLayout(LayoutGroupBox);
	LayoutCentral->addWidget(GroupBoxCallHistory);

	ListViewAllCalls = new ISListViewWidget(ISMetaData::GetInstanse().GetMetaQuery("CallHistoryAll"), GroupBoxCallHistory);
	ListViewAllCalls->SetVisibleHorizontalHeader(false);
	connect(ListViewAllCalls, &ISListViewWidget::DoubleClickedSignal, this, &ISDesktopInformResource::DoubleClickedCallOrganization);
	LayoutGroupBox->addWidget(ListViewAllCalls);

	ListViewOrganizationCalls = new ISListViewWidget(ISMetaData::GetInstanse().GetMetaQuery("CallHistoryOrganization"), GroupBoxCallHistory);
	ListViewOrganizationCalls->AddCondition(":Date", DATE_TODAY);
	LayoutGroupBox->addWidget(ListViewOrganizationCalls);
}
//-----------------------------------------------------------------------------
void ISDesktopInformResource::Reload()
{
	CurrentUserID = UserEdit->GetValue().toInt();

	ListViewDemoAccesses->AddCondition(":UserID", CurrentUserID);
	ListViewDemoAccesses->AddCondition(":AgreementDateTime", CalendarWidget->selectedDate());
	ListViewDemoAccesses->LoadData();

	ListViewOrganizations->AddCondition(":Date", CalendarWidget->selectedDate());
	ListViewOrganizations->AddCondition(":UserID", CurrentUserID);
	ListViewOrganizations->LoadData();

	ListViewAllCalls->AddCondition(":UserID", CurrentUserID);
	ListViewAllCalls->AddCondition(":Date", CalendarWidget->selectedDate());
	ListViewAllCalls->LoadData();

	ListViewOrganizationCalls->AddCondition(":OrganizationID", 0);
	ListViewOrganizationCalls->AddCondition(":Date", CalendarWidget->selectedDate());
	ListViewOrganizationCalls->LoadData();

	ISQuery qSelectNote(QS_DESKTOP_DAY_NOTE);
	qSelectNote.BindValue(":UserID", CurrentUserID);
	qSelectNote.BindValue(":Date", CalendarWidget->selectedDate());
	if (qSelectNote.ExecuteFirst())
	{
		TextEdit->SetValue(qSelectNote.ReadColumn("ddnt_note"));
	}
	else
	{
		TextEdit->Clear();
	}
}
//-----------------------------------------------------------------------------
void ISDesktopInformResource::UpdateDayNote()
{
	ISSystem::SetWaitGlobalCursor(true);

	ISQuery qSelectNote(QS_DESKTOP_DAY_NOTE);
	qSelectNote.BindValue(":UserID", CurrentUserID);
	qSelectNote.BindValue(":Date", CalendarWidget->selectedDate());
	if (qSelectNote.ExecuteFirst())
	{
		ISQuery qUpdateNote(QU_DESKTOP_DAY_NOTE);
		qUpdateNote.BindValue(":Date", CalendarWidget->selectedDate());
		qUpdateNote.BindValue(":UserID", CurrentUserID);
		qUpdateNote.BindValue(":Note", TextEdit->GetValue());
		qUpdateNote.Execute();
	}
	else
	{
		ISQuery qInsertNote(QI_DESKTOP_DAY_NOTE);
		qInsertNote.BindValue(":Date", CalendarWidget->selectedDate());
		qInsertNote.BindValue(":Note", TextEdit->GetValue());
		qInsertNote.Execute();
	}

	ISSystem::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISDesktopInformResource::DoubleClickedDemo(const QSqlRecord &SqlRecord)
{
	ISQuery qSelect(QS_DEMO_ORGANIZATION);
	qSelect.BindValue(":DemoID", SqlRecord.value("DemoID"));
	if (qSelect.ExecuteFirst())
	{
		ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "Organizations", qSelect.ReadColumn("dacc_organization").toInt())->show();
	}
}
//-----------------------------------------------------------------------------
void ISDesktopInformResource::DoubleClickedOganization(const QSqlRecord &SqlRecord)
{
	ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "Organizations", SqlRecord.value("ID").toInt())->showMaximized();
}
//-----------------------------------------------------------------------------
void ISDesktopInformResource::DoubleClickedCallOrganization(const QSqlRecord &SqlRecord)
{
	ListViewOrganizationCalls->AddCondition(":OrganizationID", SqlRecord.value("ID"));
	ListViewOrganizationCalls->AddCondition(":Date", CalendarWidget->selectedDate());
	ListViewOrganizationCalls->LoadData();
}
//-----------------------------------------------------------------------------
	