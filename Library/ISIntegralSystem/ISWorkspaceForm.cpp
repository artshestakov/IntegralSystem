#include "StdAfx.h"
#include "ISWorkspaceForm.h"
#include "EXDefines.h"
#include "ISMemoryObjects.h"
#include "ISMetaSystemsEntity.h"
#include "ISUserRoleEntity.h"
#include "ISSystemsPanel.h"
#include "ISProtocol.h"
#include "ISSystem.h"
#include "ISAssert.h"
#include "ISListBaseForm.h"
#include "ISMetaData.h"
#include "ISObjectFormBase.h"
#include "ISGui.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISWorkspaceForm::ISWorkspaceForm(QWidget *parent) : ISParagraphBaseForm(parent)
{
	CentralForm = nullptr;

	ISMemoryObjects::GetInstance().SetWorkspaceForm(this);

	Layout = new QVBoxLayout();
	Layout->setContentsMargins(LAYOUT_MARGINS_NULL);
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
	connect(ObjectForm, &ISObjectFormBase::SaveAndCreate, this, &ISWorkspaceForm::CreateDuplicate);

	//Изменение наименования заголовка вкладки формы объекта
	connect(ObjectForm, &ISObjectFormBase::windowTitleChanged, [=](const QString &WindowTitle)
	{
		TabWidget->setTabText(TabWidget->indexOf(ObjectForm), WindowTitle);
	});

	//Изменение иконки заголовка вкладки формы объекта
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

	//Заполнение систем в виджете
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

	//Если у пользователя нет доступа ни к одной из систем
	if (!ISMetaSystemsEntity::GetInstance().GetSystems().count())
	{
		QLabel *Label = new QLabel(TabWidget);
		Label->setText(LOCALIZATION("NotAccessSystems"));
		Label->setFont(FONT_TAHOMA_12_BOLD);
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

	ISSystem::SetWaitGlobalCursor(true);

	ISMetaSubSystem *MetaSubSystem = ISMetaSystemsEntity::GetInstance().GetSubSystem(SubSystemUID);

	if (CentralForm) //Если центральная форма была создана - удалить её
	{
		delete CentralForm;
		CentralForm = nullptr;
	}

	QString TableName = MetaSubSystem->GetTableName();
	QString ClassName = MetaSubSystem->GetClassName();

	TabWidget->tabBar()->setTabIcon(0, IconSubSystem);
	TabWidget->tabBar()->setTabText(0, MetaSubSystem->GetLocalName());
	TabWidget->tabBar()->setCurrentIndex(0);

	if (TableName.length()) //Открытие таблицы
	{
		ISProtocol::OpenSubSystem(TableName, ISMetaData::GetInstanse().GetMetaTable(TableName)->GetLocalListName());

		ISListBaseForm *ListBaseForm = new ISListBaseForm(TableName, this);
		CentralForm = ListBaseForm;
	}
	else if (ClassName.length()) //Открытие класса (виджета)
	{
		ISProtocol::OpenSubSystem(QString(), MetaSubSystem->GetLocalName());

		int ObjectType = QMetaType::type((ClassName + "*").toLocal8Bit().constData());
		IS_ASSERT(ObjectType, QString("Class for SybSystem is NULL. ClassName: %1").arg(ClassName));

		const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
		IS_ASSERT(MetaObject, "Error opening subsystem widget.");

		CentralForm = dynamic_cast<ISInterfaceMetaForm*>(MetaObject->newInstance(Q_ARG(QWidget *, this)));
		IS_ASSERT(CentralForm, QString("Error instance subsystem. ClassName: %1").arg(ClassName));
	}

	CentralForm->SetUID(SubSystemUID);
	connect(CentralForm, &ISListBaseForm::AddFormFromTab, this, &ISWorkspaceForm::AddObjectForm);
	TabWidget->GetMainTab()->layout()->addWidget(CentralForm);

	ISSystem::SetWaitGlobalCursor(false);
	ISSystem::RepaintWidget(CentralForm);

	QTimer::singleShot(WAIT_LOAD_DATA_LIST_FORM, Qt::PreciseTimer, CentralForm, &ISInterfaceMetaForm::LoadData);
}
//-----------------------------------------------------------------------------
void ISWorkspaceForm::CreateDuplicate(PMetaClassTable *MetaTable)
{
	AddObjectForm(ISGui::CreateObjectForm(ISNamespace::OFT_New, MetaTable->GetName()));
}
//-----------------------------------------------------------------------------
