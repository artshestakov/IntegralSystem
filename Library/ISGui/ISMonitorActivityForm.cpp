#include "ISMonitorActivityForm.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISQuery.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISFlowLayout.h"
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
										"ORDER BY userfullname(usrs_id)");
//-----------------------------------------------------------------------------
ISMonitorActivityForm::ISMonitorActivityForm(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutTitle);

	ISPushButton *ButtonUpdate = new ISPushButton(this);
	ButtonUpdate->setText(LANG("Update"));
	ButtonUpdate->setIcon(BUFFER_ICONS("Update"));
	connect(ButtonUpdate, &ISPushButton::clicked, this, &ISMonitorActivityForm::LoadData);
	LayoutTitle->addWidget(ButtonUpdate);

	ISLineEdit *EditSearch = new ISLineEdit(this);
	EditSearch->SetPlaceholderText(LANG("Search"));
	EditSearch->setFixedWidth(200);
	connect(EditSearch, &ISLineEdit::ValueChange, this, &ISMonitorActivityForm::Search);
	LayoutTitle->addWidget(EditSearch);

	CheckEdit = new ISCheckEdit(this);
	CheckEdit->SetText(LANG("MonitorActivity.HideOffline"));
	CheckEdit->SetValue(SETTING_BOOL(CONST_UID_SETTING_OTHER_HIDE_OFFLINE_MONITOR_ACTIVITY));
	connect(CheckEdit, &ISCheckEdit::ValueChange, this, &ISMonitorActivityForm::CheckEditChanged);
	LayoutTitle->addWidget(CheckEdit);

	LayoutTitle->addStretch();

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	ScrollArea = new ISScrollArea(this);
	ScrollArea->widget()->setLayout(new ISFlowLayout());
	ScrollArea->widget()->layout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	Layout->addWidget(ScrollArea);

	QAction *ActionUpdate = new QAction(this);
	ActionUpdate->setShortcut(Qt::Key_F5);
	connect(ActionUpdate, &QAction::triggered, this, &ISMonitorActivityForm::LoadData);
	addAction(ActionUpdate);
}
//-----------------------------------------------------------------------------
ISMonitorActivityForm::~ISMonitorActivityForm()
{
	
}
//-----------------------------------------------------------------------------
void ISMonitorActivityForm::LoadData()
{
	ISGui::SetWaitGlobalCursor(true);
	while (!VectorUsers.isEmpty())
	{
		delete VectorUsers.takeFirst();
	}

	QSize SizeWidget;
	ISQuery qSelect(QS_USERS);
	qSelect.BindValue(":PostgresUID", CONST_UID_USER_POSTGRES);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			bool IsOnline = qSelect.ReadColumn("useronline").toBool();
			if (CheckEdit->GetValue().toBool() && !IsOnline)
			{
				continue;
			}

			int UserID = qSelect.ReadColumn("usrs_id").toInt();
			QString UserFullName = qSelect.ReadColumn("userfullname").toString();

			ISMonitorUserWidget *MonitorUserWidget = new ISMonitorUserWidget(IsOnline, UserID, UserFullName, ScrollArea);
			connect(MonitorUserWidget, &ISMonitorUserWidget::ShowUserCard, this, &ISMonitorActivityForm::ShowUserCard);
			connect(MonitorUserWidget, &ISMonitorUserWidget::ShowProtocol, this, &ISMonitorActivityForm::ShowProtocol);
			connect(MonitorUserWidget, &ISMonitorUserWidget::ShowDetails, this, &ISMonitorActivityForm::ShowDetails);
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
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISMonitorActivityForm::CheckEditChanged(const QVariant &value)
{
	ISSettings::Instance().SaveValue(CONST_UID_SETTING_OTHER_HIDE_OFFLINE_MONITOR_ACTIVITY, value);
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
void ISMonitorActivityForm::ShowProtocol()
{
	ISMonitorUserWidget *MonitorUserWidget = dynamic_cast<ISMonitorUserWidget*>(sender());
	if (MonitorUserWidget)
	{
		ISProtocolListForm *ProtocolBaseListForm = new ISProtocolListForm();
		ProtocolBaseListForm->setWindowTitle(LANG("ProtocolUser") + ": " + MonitorUserWidget->GetUserName());
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
		ISGui::SetWaitGlobalCursor(true);
		ISUserOnlineDetailsForm UserOnlineDetailsForm(MonitorUserWidget->GetUserID());
		ISGui::SetWaitGlobalCursor(false);
		UserOnlineDetailsForm.Exec();
	}
}
//-----------------------------------------------------------------------------
void ISMonitorActivityForm::SendNotify()
{
	ISMonitorUserWidget *MonitorUserWidget = dynamic_cast<ISMonitorUserWidget*>(sender());
	if (MonitorUserWidget)
	{
		QVariant NotifyText = ISInputDialog::GetText(LANG("Notify"), LANG("NotifyText") + ':');
		if (NotifyText.isValid())
		{
			//ISNotifySender::GetInstance().SendToUser(CONST_UID_NOTIFY_USER_MESSAGE, MonitorUserWidget->GetUserID(), QVariant(), LANG("NotifyUserMessage").arg(ISMetaUser::Instance().UserData->FullName).arg(NotifyText.toString()), true);
		}
	}
}
//-----------------------------------------------------------------------------
