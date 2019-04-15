#include "StdAfx.h"
#include "ISMonitorActivityForm.h"
#include "EXDefines.h"
#include "EXConstants.h"
#include "ISQuery.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISFlowLayout.h"
#include "ISSystem.h"
#include "ISUserOnlineForm.h"
#include "ISGui.h"
#include "ISProtocolListForm.h"
#include "ISLineEdit.h"
#include "ISUserOnlineDetailsForm.h"
#include "ISMessageBox.h"
#include "ISNotifySender.h"
#include "ISDatabase.h"
#include "ISMetaUser.h"
#include "ISProtocol.h"
#include "ISUserRoleEntity.h"
#include "ISSettings.h"
#include "ISNotifyRecipient.h"
#include "ISQueryPool.h"
#include "ISInputDialog.h"
//-----------------------------------------------------------------------------
static QString QS_USERS = PREPARE_QUERY("SELECT useronline(usrs_login), usrs_id, userfullname(usrs_id) "
										"FROM _users "
										"WHERE usrs_uid != :PostgresUID "
										"AND useronline(usrs_login) IN(:Online) "
										"ORDER BY userfullname(usrs_id)");
//-----------------------------------------------------------------------------
static QString QD_SCREENSHOT = PREPARE_QUERY("DELETE FROM _screenshots WHERE scrn_id = :ScreenshotID");
//-----------------------------------------------------------------------------
ISMonitorActivityForm::ISMonitorActivityForm(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_10_PX);

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutTitle);

	ISPushButton *ButtonUpdate = new ISPushButton(this);
	ButtonUpdate->setText(LOCALIZATION("Update"));
	ButtonUpdate->setIcon(BUFFER_ICONS("Update"));
	connect(ButtonUpdate, &ISPushButton::clicked, this, &ISMonitorActivityForm::LoadData);
	LayoutTitle->addWidget(ButtonUpdate);

	ISPushButton *ButtonTerminateAll = new ISPushButton(this);
	ButtonTerminateAll->setText(LOCALIZATION("MonitorActivity.TerminateAll"));
	ButtonTerminateAll->setIcon(BUFFER_ICONS("TerminateAll"));
	connect(ButtonTerminateAll, &ISPushButton::clicked, this, &ISMonitorActivityForm::TerminateAll);
	LayoutTitle->addWidget(ButtonTerminateAll);

	ISLineEdit *EditSearch = new ISLineEdit(this);
	EditSearch->SetPlaceholderText(LOCALIZATION("Search"));
	EditSearch->setFixedWidth(200);
	connect(EditSearch, &ISLineEdit::ValueChange, this, &ISMonitorActivityForm::Search);
	LayoutTitle->addWidget(EditSearch);

	CheckEdit = new ISCheckEdit(this);
	CheckEdit->SetText(LOCALIZATION("MonitorActivity.HideOffline"));
	CheckEdit->SetValue(SETTING_BOOL(CONST_UID_SETTING_OTHER_HIDE_OFFLINE_MONITOR_ACTIVITY));
	connect(CheckEdit, &ISCheckEdit::ValueChange, this, &ISMonitorActivityForm::CheckEditChanged);
	LayoutTitle->addWidget(CheckEdit);

	LayoutTitle->addStretch();

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	ScrollArea = new ISScrollArea(this);
	ScrollArea->widget()->setLayout(new ISFlowLayout());
	ScrollArea->widget()->layout()->setContentsMargins(LAYOUT_MARGINS_NULL);
	Layout->addWidget(ScrollArea);

	QAction *ActionUpdate = new QAction(this);
	ActionUpdate->setShortcut(Qt::Key_F5);
	connect(ActionUpdate, &QAction::triggered, this, &ISMonitorActivityForm::LoadData);
	addAction(ActionUpdate);

	connect(&ISNotifyRecipient::GetInstance(), &ISNotifyRecipient::ScreenshotCreated, this, &ISMonitorActivityForm::ScreenshotCreated);
}
//-----------------------------------------------------------------------------
ISMonitorActivityForm::~ISMonitorActivityForm()
{
	ISSystem::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISMonitorActivityForm::LoadData()
{
	ISSystem::SetWaitGlobalCursor(true);

	while (VectorUsers.count())
	{
		delete VectorUsers.takeFirst();
	}

	QSize SizeWidget;
	QString QueryText = QS_USERS;
	if (CheckEdit->GetValue().toBool())
	{
		QueryText = QueryText.replace(":Online", "true");
	}
	else
	{
		QueryText = QueryText.replace(":Online", "true, false");
	}
	
	ISQuery qSelect(QueryText);
	qSelect.BindValue(":PostgresUID", CONST_UID_USER_POSTGRES);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			bool IsOnline = qSelect.ReadColumn("useronline").toBool();
			int UserID = qSelect.ReadColumn("usrs_id").toInt();
			QString UserFullName = qSelect.ReadColumn("userfullname").toString();

			ISMonitorUserWidget *MonitorUserWidget = new ISMonitorUserWidget(IsOnline, UserID, UserFullName, ScrollArea);
			connect(MonitorUserWidget, &ISMonitorUserWidget::ShowUserCard, this, &ISMonitorActivityForm::ShowUserCard);
			connect(MonitorUserWidget, &ISMonitorUserWidget::ShowActivity, this, &ISMonitorActivityForm::ShowActivity);
			connect(MonitorUserWidget, &ISMonitorUserWidget::ShowProtocol, this, &ISMonitorActivityForm::ShowProtocol);
			connect(MonitorUserWidget, &ISMonitorUserWidget::ShowDetails, this, &ISMonitorActivityForm::ShowDetails);
			connect(MonitorUserWidget, &ISMonitorUserWidget::EndSession, this, &ISMonitorActivityForm::TerminateUser);
			connect(MonitorUserWidget, &ISMonitorUserWidget::GetScreenshot, this, &ISMonitorActivityForm::GetScreenshot);
			connect(MonitorUserWidget, &ISMonitorUserWidget::SendNotify, this, &ISMonitorActivityForm::SendNotify);
			ScrollArea->widget()->layout()->addWidget(MonitorUserWidget);
			MonitorUserWidget->adjustSize();

			QSize CurrentSize = MonitorUserWidget->size();
			if (CurrentSize.width() > SizeWidget.width())
			{
				SizeWidget.setWidth(CurrentSize.width());
			}

			if (CurrentSize.height() > SizeWidget.height())
			{
				SizeWidget.setHeight(CurrentSize.height());
			}

			VectorUsers.append(MonitorUserWidget);
		}
	}

	SizeWidget.setHeight(SizeWidget.height() + 10);
	SizeWidget.setWidth(SizeWidget.width() + 10);
	for (ISMonitorUserWidget *MonitorUserWidget : VectorUsers)
	{
		MonitorUserWidget->setMinimumSize(SizeWidget);
	}

	ISSystem::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISMonitorActivityForm::CheckEditChanged(const QVariant &value)
{
	ISSettings::GetInstance().SaveValue(CONST_UID_SETTING_OTHER_HIDE_OFFLINE_MONITOR_ACTIVITY, value);
	LoadData();
}
//-----------------------------------------------------------------------------
void ISMonitorActivityForm::Search(const QVariant &value)
{
	for (ISMonitorUserWidget *MonitorUserWidget : VectorUsers)
	{
		if (value.isValid())
		{
			if (MonitorUserWidget->GetUserName().toLower().contains(value.toString().toLower()))
			{
				MonitorUserWidget->setVisible(true);
			}
			else
			{
				MonitorUserWidget->setVisible(false);
			}
		}
		else
		{
			MonitorUserWidget->setVisible(true);
		}
	}
}
//-----------------------------------------------------------------------------
void ISMonitorActivityForm::ShowUserCard()
{
	ISMonitorUserWidget *MonitorUserWidget = dynamic_cast<ISMonitorUserWidget*>(sender());
	if (MonitorUserWidget)
	{
		ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "_Users", MonitorUserWidget->GetUserID())->show();
	}
}
//-----------------------------------------------------------------------------
void ISMonitorActivityForm::ShowActivity()
{
	if (!ISUserRoleEntity::GetInstance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_SHOW_ACTIVITY_USER))
	{
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.NotAccess.Special.ShowActivityUser"));
		return;
	}

	ISMonitorUserWidget *MonitorUserWidget = dynamic_cast<ISMonitorUserWidget*>(sender());
	if (MonitorUserWidget)
	{
		ISSystem::SetWaitGlobalCursor(true);
		ISUserOnlineForm *UserOnlineForm = new ISUserOnlineForm(MonitorUserWidget->GetUserID(), MonitorUserWidget->GetUserName());
		UserOnlineForm->showMaximized();
		ISSystem::SetWaitGlobalCursor(false);

		ISProtocol::Insert(true, CONST_UID_PROTOCOL_SHOW_ACTIVITY_USER, QString(), QString(), QVariant(), MonitorUserWidget->GetUserName());
	}
}
//-----------------------------------------------------------------------------
void ISMonitorActivityForm::ShowProtocol()
{
	ISMonitorUserWidget *MonitorUserWidget = dynamic_cast<ISMonitorUserWidget*>(sender());
	if (MonitorUserWidget)
	{
		ISProtocolListForm *ProtocolBaseListForm = new ISProtocolListForm();
		ProtocolBaseListForm->setWindowTitle(LOCALIZATION("ProtocolUser") + ": " + MonitorUserWidget->GetUserName());
		ProtocolBaseListForm->setWindowIcon(BUFFER_ICONS("Protocol"));
		ProtocolBaseListForm->GetQueryModel()->SetClassFilter("prtc_user = :UserID");
		ProtocolBaseListForm->GetQueryModel()->AddCondition(":UserID", MonitorUserWidget->GetUserID());
		ProtocolBaseListForm->LoadData();
		ProtocolBaseListForm->showMaximized();
	}
}
//-----------------------------------------------------------------------------
void ISMonitorActivityForm::ShowDetails()
{
	ISMonitorUserWidget *MonitorUserWidget = dynamic_cast<ISMonitorUserWidget*>(sender());
	if (MonitorUserWidget)
	{
		ISSystem::SetWaitGlobalCursor(true);
		ISUserOnlineDetailsForm UserOnlineDetailsForm(MonitorUserWidget->GetUserID());
		ISSystem::SetWaitGlobalCursor(false);
		UserOnlineDetailsForm.Exec();
	}
}
//-----------------------------------------------------------------------------
void ISMonitorActivityForm::TerminateUser()
{
	if (!ISUserRoleEntity::GetInstance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_TERMINATE_USER))
	{
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.NotAccess.Special.TerminateUser"));
		return;
	}

	ISMonitorUserWidget *MonitorUserWidget = dynamic_cast<ISMonitorUserWidget*>(sender());
	if (MonitorUserWidget)
	{
		if (ISDatabase::GetInstance().IsUserOnline(MonitorUserWidget->GetUserID()))
		{
			if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.TerminateUser").arg(MonitorUserWidget->GetUserName())))
			{
				ISNotifySender::GetInstance().SendToUser(CONST_UID_NOTIFY_TERMINATE_USER, MonitorUserWidget->GetUserID(), QVariant(), QString(), false);
				ISProtocol::Insert(true, CONST_UID_PROTOCOL_TERMINATE_USER, QString(), QString(), QVariant(), MonitorUserWidget->GetUserName());
			}
		}
		else
		{
			ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.UserIsNotOnline").arg(MonitorUserWidget->GetUserName()));
		}
	}
}
//-----------------------------------------------------------------------------
void ISMonitorActivityForm::GetScreenshot()
{
	ISMonitorUserWidget *MonitorUserWidget = dynamic_cast<ISMonitorUserWidget*>(sender());
	if (MonitorUserWidget)
	{
		if (MonitorUserWidget->GetUserID() == CURRENT_USER_ID)
		{
			ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.NotGetScreenshotCurrentUser"));
			return;
		}

		if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.GetScreenshot")))
		{
			ISNotifySender::GetInstance().SendToUser(CONST_UID_NOTIFY_SCREENSHOT_CREATE, MonitorUserWidget->GetUserID(), CURRENT_USER_ID, QString(), false);
			ISSystem::SetWaitGlobalCursor(true);
		}
	}
}
//-----------------------------------------------------------------------------
void ISMonitorActivityForm::SendNotify()
{
	ISMonitorUserWidget *MonitorUserWidget = dynamic_cast<ISMonitorUserWidget*>(sender());
	if (MonitorUserWidget)
	{
		QVariant NotifyText = ISInputDialog::GetText(this, LOCALIZATION("Notify"), LOCALIZATION("NotifyText") + ":");
		if (NotifyText.isValid())
		{
			ISNotifySender::GetInstance().SendToUser(CONST_UID_NOTIFY_USER_MESSAGE, MonitorUserWidget->GetUserID(), QVariant(), LOCALIZATION("NotifyUserMessage").arg(CURRENT_USER_FULL_NAME).arg(NotifyText.toString()), true);
		}
	}
}
//-----------------------------------------------------------------------------
void ISMonitorActivityForm::TerminateAll()
{
	if (!ISUserRoleEntity::GetInstance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_TERMINATE_ALL_USERS))
	{
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.NotAccess.Special.TerminateAllUsers"));
		return;
	}

	if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.TerminateAllUser")))
	{
		int TerminatedCount = 0;
		for (ISMonitorUserWidget *MonitorUserWidget : VectorUsers)
		{
			if (MonitorUserWidget->GetUserID() == CURRENT_USER_ID)
			{
				continue;
			}

			if (ISDatabase::GetInstance().IsUserOnline(MonitorUserWidget->GetUserID()))
			{
				ISNotifySender::GetInstance().SendToUser(CONST_UID_NOTIFY_TERMINATE_USER, MonitorUserWidget->GetUserID(), QVariant(), QString(), false);

				TerminatedCount++;
			}
		}

		if (TerminatedCount)
		{
			ISProtocol::Insert(true, CONST_UID_PROTOCOL_TERMIANTE_ALL_USERS, QString(), QString(), QVariant());
			ISMessageBox::ShowInformation(this, LOCALIZATION("Message.Information.TermiantedUsers").arg(TerminatedCount));
		}
	}
}
//-----------------------------------------------------------------------------
void ISMonitorActivityForm::ScreenshotCreated(const QVariantMap &VariantMap)
{
	int ScreenshotID = VariantMap.value("Payload").toInt();
	QVariant ValueDB = ISDatabase::GetInstance().GetValue("_Screenshots", "Screenshot", ScreenshotID);
	ISSystem::SetWaitGlobalCursor(false);
	if (ValueDB.isValid())
	{
		ISImageViewerForm *ImageViewerForm = ISGui::ShowImageForm(ValueDB.toByteArray());
		ImageViewerForm->setProperty("ScreenshotID", ScreenshotID);
		connect(ImageViewerForm, &ISImageViewerForm::destroyed, this, &ISMonitorActivityForm::CloseScreenshot);
	}
}
//-----------------------------------------------------------------------------
void ISMonitorActivityForm::CloseScreenshot(QObject *Object)
{
	QVariantMap VariantMap;
	VariantMap.insert(":ScreenshotID", Object->property("ScreenshotID"));
	ISQueryPool::GetInstance().AddQuery(QD_SCREENSHOT, VariantMap);
}
//-----------------------------------------------------------------------------
