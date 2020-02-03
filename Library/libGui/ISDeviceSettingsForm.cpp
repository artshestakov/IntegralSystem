#include "StdAfx.h"
#include "ISDeviceSettingsForm.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISControls.h"
#include "ISGui.h"
#include "ISDeviceUserObjectForm.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
#include "ISDeviceEntity.h"
//-----------------------------------------------------------------------------
static QString QS_DEVICE_USER = PREPARE_QUERY("SELECT dvus_id, dvus_uid, dvus_name "
											  "FROM _deviceuser "
											  "WHERE NOT dvus_isdeleted "
											  "AND dvus_user = currentuserid() "
											  "ORDER BY dvus_name");
//-----------------------------------------------------------------------------
static QString QD_DEVICE_USER = PREPARE_QUERY("DELETE FROM _deviceuser WHERE dvus_id = :DeviceUserID");
//-----------------------------------------------------------------------------
ISDeviceSettingsForm::ISDeviceSettingsForm(QWidget *parent) : ISInterfaceForm(parent)
{
	setWindowTitle(LANG("Device.Settings"));
	setWindowIcon(BUFFER_ICONS("Device.Settings"));
	GetMainLayout()->setContentsMargins(MARGINS_LAYOUT_10_PX);

	QToolBar *ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	GetMainLayout()->addWidget(ToolBar);

	ActionCreate = new QAction(ToolBar);
	ActionCreate->setText(LANG("Add"));
	ActionCreate->setIcon(BUFFER_ICONS("Add"));
	connect(ActionCreate, &QAction::triggered, this, &ISDeviceSettingsForm::Create);
	ToolBar->addAction(ActionCreate);

	ActionEdit = new QAction(ToolBar);
	ActionEdit->setEnabled(false);
	ActionEdit->setText(LANG("Edit"));
	ActionEdit->setIcon(BUFFER_ICONS("Edit"));
	connect(ActionEdit, &QAction::triggered, this, &ISDeviceSettingsForm::Edit);
	ToolBar->addAction(ActionEdit);

	ActionDelete = new QAction(ToolBar);
	ActionDelete->setEnabled(false);
	ActionDelete->setText(LANG("Delete"));
	ActionDelete->setIcon(BUFFER_ICONS("Delete"));
	connect(ActionDelete, &QAction::triggered, this, &ISDeviceSettingsForm::Delete);
	ToolBar->addAction(ActionDelete);

	ActionUpdate = new QAction(ToolBar);
	ActionUpdate->setText(LANG("Update"));
	ActionUpdate->setIcon(BUFFER_ICONS("Update"));
	connect(ActionUpdate, &QAction::triggered, this, &ISDeviceSettingsForm::Update);
	ToolBar->addAction(ActionUpdate);

	QVBoxLayout *GroupBoxLayout = new QVBoxLayout();
	GroupBoxLayout->setContentsMargins(MARGINS_LAYOUT_5_PX);

	QGroupBox *GroupBox = new QGroupBox(this);
	GroupBox->setTitle(LANG("Device.My"));
	GroupBox->setLayout(GroupBoxLayout);
	GetMainLayout()->addWidget(GroupBox);

	ListWidget = new ISListWidget(GroupBox);
	ListWidget->setFrameShape(QFrame::NoFrame);
	connect(ListWidget, &ISListWidget::itemSelectionChanged, this, &ISDeviceSettingsForm::ItemSelectionChanged);
	connect(ListWidget, &ISListWidget::itemDoubleClicked, this, &ISDeviceSettingsForm::ItemDoubleClicked);
	GroupBox->layout()->addWidget(ListWidget);

	Reload();
}
//-----------------------------------------------------------------------------
ISDeviceSettingsForm::~ISDeviceSettingsForm()
{

}
//-----------------------------------------------------------------------------
void ISDeviceSettingsForm::ItemSelectionChanged()
{
	ActionEdit->setEnabled(true);
	ActionDelete->setEnabled(true);
}
//-----------------------------------------------------------------------------
void ISDeviceSettingsForm::ItemDoubleClicked(QListWidgetItem *ListWidgetItem)
{
	Edit();
}
//-----------------------------------------------------------------------------
void ISDeviceSettingsForm::Reload()
{
	ListWidget->Clear();
	ISQuery qSelect(QS_DEVICE_USER);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			int DeviceUserID = qSelect.ReadColumn("dvus_id").toInt();
			ISUuid DeviceUserUID = qSelect.ReadColumn("dvus_uid");
			QString DeviceUserName = qSelect.ReadColumn("dvus_name").toString();

			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
			ListWidgetItem->setText(DeviceUserName);
			ListWidgetItem->setData(Qt::UserRole, DeviceUserID);
			ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));

			ISDeviceObjectBase *DeviceObjectBase = ISDeviceEntity::GetInstance().GetDevice(DeviceUserUID);

			if (DeviceObjectBase->IsConnected()) //Если устройство подключено
			{
				if (!DeviceObjectBase->IsOpen())
				{
					DeviceObjectBase->Initialize();
				}

				if (DeviceObjectBase->IsValid())
				{
					ListWidgetItem->setIcon(BUFFER_ICONS("Apply.Green"));
				}
				else
				{
					ListWidgetItem->setIcon(BUFFER_ICONS("Importance.High"));
					ListWidgetItem->setToolTip(LANG("Device.Error") + ": " + DeviceObjectBase->GetErrorText());
				}
			}
			else //Устройство не подключено
			{
				ListWidgetItem->setIcon(BUFFER_ICONS("Device.Disconnect"));
				ListWidgetItem->setText(ListWidgetItem->text() + " (" + LANG("Device.NotConnected") + ')');
			}
		}
	}

	ActionEdit->setEnabled(ListWidget->count());
	ActionDelete->setEnabled(ListWidget->count());
}
//-----------------------------------------------------------------------------
void ISDeviceSettingsForm::Create()
{
	ISDeviceUserObjectForm *DeviceUserObjectForm = dynamic_cast<ISDeviceUserObjectForm*>(ISGui::CreateObjectForm(ISNamespace::OFT_New, "_DeviceUser"));
	connect(DeviceUserObjectForm, &ISDeviceUserObjectForm::UpdateList, this, &ISDeviceSettingsForm::Reload);
	DeviceUserObjectForm->show();
}
//-----------------------------------------------------------------------------
void ISDeviceSettingsForm::Edit()
{
	ISDeviceUserObjectForm *DeviceUserObjectForm = dynamic_cast<ISDeviceUserObjectForm*>(ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "_DeviceUser", ListWidget->currentItem()->data(Qt::UserRole).toInt()));
	connect(DeviceUserObjectForm, &ISDeviceUserObjectForm::UpdateList, this, &ISDeviceSettingsForm::Reload);
	DeviceUserObjectForm->show();
}
//-----------------------------------------------------------------------------
void ISDeviceSettingsForm::Delete()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteDeviceUser")))
	{
		ISQuery qDelete(QD_DEVICE_USER);
		qDelete.BindValue(":DeviceUserID", ListWidget->currentItem()->data(Qt::UserRole));
		if (qDelete.Execute())
		{
			Reload();
		}
	}
}
//-----------------------------------------------------------------------------
void ISDeviceSettingsForm::Update()
{
	Reload();
}
//-----------------------------------------------------------------------------
