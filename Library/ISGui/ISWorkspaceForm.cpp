#include "ISWorkspaceForm.h"
#include "ISDefinesGui.h"
#include "ISMemoryObjects.h"
#include "ISMetaSystemsEntity.h"
#include "ISUserRoleEntity.h"
#include "ISSystemsPanel.h"
#include "ISProtocol.h"
#include "ISAssert.h"
#include "ISListBaseForm.h"
#include "ISMetaData.h"
#include "ISObjectFormBase.h"
#include "ISGui.h"
#include "ISLocalization.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISWorkspaceForm::ISWorkspaceForm(QWidget *parent) : ISParagraphBaseForm(parent)
{
	CentralForm = nullptr;

	ISMemoryObjects::GetInstance().SetWorkspaceForm(this);

	Layout = new QVBoxLayout();
	Layout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	Layout->setSpacing(0);
	setLayout(Layout);

	CreateSystems();
	CreateTabWidget();
}
//-----------------------------------------------------------------------------
ISWorkspaceForm::~ISWorkspaceForm()
{

}
//-----------------------------------------------------------------------------
void ISWorkspaceForm::Invoke()
{
	ISParagraphBaseForm::Invoke();
}
//-----------------------------------------------------------------------------
void ISWorkspaceForm::AddObjectForm(QWidget *ObjectFormWidget)
{
	ISObjectFormBase *ObjectForm = dynamic_cast<ISObjectFormBase*>(ObjectFormWidget);
	connect(ObjectForm, &ISObjectFormBase::CloseTab, TabWidget, &ISTabWidgetMain::RemoveActionTab);

	//��������� ������������ ��������� ������� ����� �������
	connect(ObjectForm, &ISObjectFormBase::windowTitleChanged, [=](const QString &WindowTitle)
	{
		TabWidget->setTabText(TabWidget->indexOf(ObjectForm), WindowTitle);
	});

	//��������� ������ ��������� ������� ����� �������
	connect(ObjectForm, &ISObjectFormBase::windowIconChanged, [=](const QIcon &WindowIcon)
	{
		TabWidget->setTabIcon(TabWidget->indexOf(ObjectForm), WindowIcon);
	});

	connect(ObjectForm, &ISObjectFormBase::CurrentObjectTab, [=]
	{
		TabWidget->setCurrentWidget(ObjectFormWidget);
	});
	
	int TabIndex = TabWidget->addTab(ObjectForm, ObjectForm->windowIcon(), ObjectForm->windowTitle());
	TabWidget->setCurrentWidget(ObjectForm);
	ObjectForm->SetCurrentIndexTab(TabIndex);
}
//-----------------------------------------------------------------------------
void ISWorkspaceForm::CreateSystems()
{
	ISSystemsPanel *SystemsPanel = new ISSystemsPanel(this);
	connect(SystemsPanel, &ISSystemsPanel::ClickedSubSystem, this, &ISWorkspaceForm::ClickedSubSystem);
	Layout->addWidget(SystemsPanel);

	//���������� ������ � �������
	for (ISMetaSystem *MetaSystem : ISMetaSystemsEntity::GetInstance().GetSystems())
	{
		SystemsPanel->AddSystem(MetaSystem);
	}
}
//-----------------------------------------------------------------------------
void ISWorkspaceForm::CreateTabWidget()
{
	TabWidget = new ISTabWidgetMain(this);
	TabWidget->setSizePolicy(QSizePolicy::Ignored, TabWidget->sizePolicy().verticalPolicy());
	connect(TabWidget, &ISTabWidgetMain::Duplicate, this, &ISWorkspaceForm::AddObjectForm);
	Layout->addWidget(TabWidget);

	//���� � ������������ ��� ������� �� � ����� �� ������
	if (!ISMetaSystemsEntity::GetInstance().GetSystems().count())
	{
		QLabel *Label = new QLabel(TabWidget);
		Label->setText(LANG("NotAccessSystems"));
		Label->setFont(ISDefines::Gui::FONT_TAHOMA_12_BOLD);
		dynamic_cast<QVBoxLayout*>(TabWidget->GetMainTab()->layout())->addWidget(Label, 0, Qt::AlignCenter);
	}
}
//-----------------------------------------------------------------------------
void ISWorkspaceForm::ClickedSubSystem(const QString &SubSystemUID, const QIcon &IconSubSystem)
{
	if (SubSystemUID == CurrentSubSystemUID)
	{
		TabWidget->setCurrentIndex(0);
		return;
	}
	else
	{
		CurrentSubSystemUID = SubSystemUID;
	}

	ISGui::SetWaitGlobalCursor(true);

	ISMetaSubSystem *MetaSubSystem = ISMetaSystemsEntity::GetInstance().GetSubSystem(SubSystemUID);

	if (CentralForm) //���� ����������� ����� ���� ������� - ������� �
	{
		delete CentralForm;
		CentralForm = nullptr;
	}

	TabWidget->tabBar()->setTabIcon(0, IconSubSystem);
	TabWidget->tabBar()->setTabText(0, MetaSubSystem->LocalName);
	TabWidget->tabBar()->setCurrentIndex(0);

	if (!MetaSubSystem->TableName.isEmpty()) //�������� �������
	{
		ISProtocol::OpenSubSystem(MetaSubSystem->TableName, ISMetaData::Instance().GetMetaTable(MetaSubSystem->TableName)->LocalListName);
		CentralForm = new ISListBaseForm(MetaSubSystem->TableName, this);
	}
	else if (!MetaSubSystem->ClassName.isEmpty()) //�������� ������ (�������)
	{
		ISProtocol::OpenSubSystem(QString(), MetaSubSystem->LocalName);
		CentralForm = ISAlgorithm::CreatePointer<ISInterfaceMetaForm *>(MetaSubSystem->ClassName, Q_ARG(QWidget *, this));
	}

	CentralForm->SetUID(SubSystemUID);
	connect(CentralForm, &ISListBaseForm::AddFormFromTab, this, &ISWorkspaceForm::AddObjectForm);
	TabWidget->GetMainTab()->layout()->addWidget(CentralForm);

	ISGui::SetWaitGlobalCursor(false);
	ISGui::RepaintWidget(CentralForm);
	QTimer::singleShot(WAIT_LOAD_DATA_LIST_FORM, Qt::PreciseTimer, CentralForm, &ISInterfaceMetaForm::LoadData);
}
//-----------------------------------------------------------------------------
