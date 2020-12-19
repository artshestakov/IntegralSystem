#include "ISMonitorActivityForm.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISTcpQuery.h"
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

			ISMonitorUserWidget *MonitorUserWidget = new ISMonitorUserWidget(
				ClientMap["ID"].toUInt(), ClientMap["FIO"].toString(),
				ISGui::ByteArrayToPixmap(QByteArray::fromBase64(ClientMap["Photo"].toByteArray())).scaled(ISDefines::Gui::SIZE_32_32),
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
		ProtocolBaseListForm->GetQueryModel()->SetClassFilter("prtc_user = :UserID");
		ProtocolBaseListForm->GetQueryModel()->AddCondition(":UserID", MonitorUserWidget->property("UserID"));
		ProtocolBaseListForm->LoadData();
		ProtocolBaseListForm->showMaximized();
	}
}
//-----------------------------------------------------------------------------
