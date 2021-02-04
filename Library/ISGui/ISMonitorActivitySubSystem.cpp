#include "ISMonitorActivitySubSystem.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISTcpQuery.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISFlowLayout.h"
#include "ISGui.h"
#include "ISProtocolSubSystem.h"
#include "ISDialogsCommon.h"
#include "ISUserRoleEntity.h"
#include "ISSettings.h"
//-----------------------------------------------------------------------------
ISMonitorActivitySubSystem::ISMonitorActivitySubSystem(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutTitle);

	ISPushButton *ButtonUpdate = new ISPushButton(this);
	ButtonUpdate->setText(LANG("Update"));
	ButtonUpdate->setIcon(BUFFER_ICONS("Update"));
	connect(ButtonUpdate, &ISPushButton::clicked, this, &ISMonitorActivitySubSystem::LoadData);
	LayoutTitle->addWidget(ButtonUpdate);

	LayoutTitle->addStretch();

	QHBoxLayout *Layout = new QHBoxLayout();
	GetMainLayout()->addLayout(Layout);

	ScrollArea = new ISScrollArea(this);
	ScrollArea->widget()->setLayout(new ISFlowLayout());
	ScrollArea->widget()->layout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	Layout->addWidget(ScrollArea);

	QAction *ActionUpdate = new QAction(this);
	ActionUpdate->setShortcut(Qt::Key_F5);
	connect(ActionUpdate, &QAction::triggered, this, &ISMonitorActivitySubSystem::LoadData);
	addAction(ActionUpdate);
}
//-----------------------------------------------------------------------------
ISMonitorActivitySubSystem::~ISMonitorActivitySubSystem()
{
	
}
//-----------------------------------------------------------------------------
void ISMonitorActivitySubSystem::LoadData()
{
	while (!VectorUsers.empty())
	{
		delete ISAlgorithm::VectorTakeBack(VectorUsers);
	}

	QSize SizeWidget;
	ISTcpQuery qGetClients(API_GET_CLIENTS);
	if (qGetClients.Execute())
	{
		QVariantList Clients = qGetClients.GetAnswer()["Clients"].toList();
		for (const QVariant &Variant : Clients)
		{
			QVariantMap ClientMap = Variant.toMap();

			ISMonitorUserWidget *MonitorUserWidget = new ISMonitorUserWidget(ClientMap, ScrollArea);
			connect(MonitorUserWidget, &ISMonitorUserWidget::ShowUserCard, this, &ISMonitorActivitySubSystem::ShowUserCard);
			connect(MonitorUserWidget, &ISMonitorUserWidget::ShowProtocol, this, &ISMonitorActivitySubSystem::ShowProtocol);
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
	else
	{
		ISMessageBox::ShowCritical(this, qGetClients.GetErrorString());
	}

	SizeWidget.setHeight(SizeWidget.height() + 10);
	SizeWidget.setWidth(SizeWidget.width() + 10);
	for (ISMonitorUserWidget *MonitorUserWidget : VectorUsers)
	{
		MonitorUserWidget->setMinimumSize(SizeWidget);
	}
}
//-----------------------------------------------------------------------------
void ISMonitorActivitySubSystem::ShowUserCard()
{
	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::ObjectFormType::Edit, "_Users", sender()->property("UserID").toInt());
	connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISMonitorActivitySubSystem::LoadData);
	ISGui::ShowObjectForm(ObjectFormBase);
}
//-----------------------------------------------------------------------------
void ISMonitorActivitySubSystem::ShowProtocol()
{
	ISProtocolSubSystem *ProtocolBaseListForm = new ISProtocolSubSystem();
	ProtocolBaseListForm->setWindowTitle(LANG("ProtocolUser") + ": " + sender()->property("UserName").toString());
	ProtocolBaseListForm->setWindowIcon(BUFFER_ICONS("Protocol"));
	ProtocolBaseListForm->GetTcpQuery()->AddFilter("User", sender()->property("UserID"));
	ProtocolBaseListForm->showMaximized();
	QTimer::singleShot(WAIT_LOAD_DATA_LIST_FORM, ProtocolBaseListForm, &ISProtocolSubSystem::LoadData);
}
//-----------------------------------------------------------------------------
