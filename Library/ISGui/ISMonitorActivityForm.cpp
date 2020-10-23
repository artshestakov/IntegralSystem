#include "ISMonitorActivityForm.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISQuery.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISFlowLayout.h"
#include "ISGui.h"
#include "ISProtocolListForm.h"
#include "ISMessageBox.h"
#include "ISDatabase.h"
#include "ISMetaUser.h"
#include "ISProtocol.h"
#include "ISUserRoleEntity.h"
#include "ISSettings.h"
#include "ISQueryPool.h"
#include "ISInputDialog.h"
//-----------------------------------------------------------------------------
static QString QS_USERS = PREPARE_QUERY("SELECT usrs_id, usrs_fio, usrs_photo "
										"FROM _users "
										"WHERE usrs_uid != :PostgresUID "
										"ORDER BY usrs_fio");
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
	while (!VectorUsers.empty())
	{
		delete ISAlgorithm::VectorTakeBack(VectorUsers);
	}

	QSize SizeWidget;
	ISQuery qSelect(QS_USERS);
	qSelect.BindValue(":PostgresUID", CONST_UID_USER_POSTGRES);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			bool IsOnline = false;
			if (CheckEdit->GetValue().toBool() && !IsOnline)
			{
				continue;
			}

			ISMonitorUserWidget *MonitorUserWidget = new ISMonitorUserWidget(IsOnline,
				qSelect.ReadColumn("usrs_id").toInt(),
				qSelect.ReadColumn("usrs_fio").toString(),
				ISGui::ByteArrayToPixmap(qSelect.ReadColumn("usrs_photo").toByteArray()).scaled(ISDefines::Gui::SIZE_32_32),
				ScrollArea);
			connect(MonitorUserWidget, &ISMonitorUserWidget::ShowUserCard, this, &ISMonitorActivityForm::ShowUserCard);
			connect(MonitorUserWidget, &ISMonitorUserWidget::ShowProtocol, this, &ISMonitorActivityForm::ShowProtocol);
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
			VectorUsers.emplace_back(MonitorUserWidget);
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
	ISSettings::Instance().SetValue(CONST_UID_SETTING_OTHER_HIDE_OFFLINE_MONITOR_ACTIVITY, value);
	LoadData();
}
//-----------------------------------------------------------------------------
void ISMonitorActivityForm::Search(const QVariant &value)
{
	for (ISMonitorUserWidget *MonitorUserWidget : VectorUsers)
	{
		if (value.isValid())
		{
			MonitorUserWidget->setVisible(MonitorUserWidget->property("UserName").toString().toLower().contains(value.toString().toLower()));
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
		ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "_Users", MonitorUserWidget->property("UserID").toInt());
		connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISMonitorActivityForm::LoadData);
		ISGui::ShowObjectForm(ObjectFormBase);
	}
}
//-----------------------------------------------------------------------------
void ISMonitorActivityForm::ShowProtocol()
{
	ISMonitorUserWidget *MonitorUserWidget = dynamic_cast<ISMonitorUserWidget*>(sender());
	if (MonitorUserWidget)
	{
		ISProtocolListForm *ProtocolBaseListForm = new ISProtocolListForm();
		ProtocolBaseListForm->setWindowTitle(LANG("ProtocolUser") + ": " + MonitorUserWidget->property("UserName").toString());
		ProtocolBaseListForm->setWindowIcon(BUFFER_ICONS("Protocol"));
		ProtocolBaseListForm->GetQueryModel()->SetClassFilter("prtc_creationuser = :UserID");
		ProtocolBaseListForm->GetQueryModel()->AddCondition(":UserID", MonitorUserWidget->property("UserID"));
		ProtocolBaseListForm->LoadData();
		ProtocolBaseListForm->showMaximized();
	}
}
//-----------------------------------------------------------------------------
