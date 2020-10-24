#include "ISWorkspaceForm.h"
#include "ISDefinesGui.h"
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
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISWorkspaceForm::ISWorkspaceForm(QWidget *parent)
	: ISParagraphBaseForm(parent),
	CentralForm(nullptr)
{
	Layout = new QVBoxLayout();
	Layout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	Layout->setSpacing(0);
	setLayout(Layout);

	ISSystemsPanel *SystemsPanel = new ISSystemsPanel(this);
	connect(SystemsPanel, &ISSystemsPanel::ClickedSubSystem, this, &ISWorkspaceForm::ClickedSubSystem);
	Layout->addWidget(SystemsPanel);

	//Заполнение систем в виджете
	for (ISMetaSystem *MetaSystem : ISMetaSystemsEntity::Instance().GetSystems())
	{
		SystemsPanel->AddSystem(MetaSystem);
	}

	TabWidget = new ISTabWidgetMain(this);
	TabWidget->setSizePolicy(QSizePolicy::Ignored, TabWidget->sizePolicy().verticalPolicy());
	connect(TabWidget, &ISTabWidgetMain::Duplicate, this, &ISWorkspaceForm::AddObjectForm);
	Layout->addWidget(TabWidget);

	//Если у пользователя нет доступа ни к одной из систем
	if (ISMetaSystemsEntity::Instance().GetSystems().empty())
	{
		QLabel *Label = new QLabel(TabWidget);
		Label->setText(LANG("NotAccessSystems"));
		Label->setFont(ISDefines::Gui::FONT_TAHOMA_12_BOLD);
		Label->setAlignment(Qt::AlignCenter);
		TabWidget->addTab(Label, QString());
	}
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
void ISWorkspaceForm::AddObjectForm(QWidget *ObjectForm)
{
	connect(ObjectForm, &ISObjectFormBase::windowTitleChanged, [=](const QString &WindowTitle)
	{
		TabWidget->setTabText(TabWidget->indexOf(ObjectForm), WindowTitle);
	});
	connect(ObjectForm, &ISObjectFormBase::windowIconChanged, [=](const QIcon &WindowIcon)
	{
		TabWidget->setTabIcon(TabWidget->indexOf(ObjectForm), WindowIcon);
	});
	connect(dynamic_cast<ISObjectFormBase*>(ObjectForm), &ISObjectFormBase::CurrentObjectTab, [=]
	{
		TabWidget->setCurrentWidget(ObjectForm);
	});
	TabWidget->addTab(ObjectForm, ObjectForm->windowIcon(), ObjectForm->windowTitle());
	TabWidget->setCurrentWidget(ObjectForm);
}
//-----------------------------------------------------------------------------
void ISWorkspaceForm::ClickedSubSystem(const QString &SubSystemUID, const QIcon &IconSubSystem)
{
	if (SubSystemUID == CurrentSubSystemUID)
	{
		TabWidget->setCurrentIndex(0);
		return;
	}
	CurrentSubSystemUID = SubSystemUID;
	ISLOGGER_I(__CLASS__, "Open subsystem " + CurrentSubSystemUID);

	ISGui::SetWaitGlobalCursor(true);
	ISMetaSubSystem *MetaSubSystem = ISMetaSystemsEntity::Instance().GetSubSystem(SubSystemUID);
	POINTER_DELETE(CentralForm);
	if (!MetaSubSystem->TableName.isEmpty()) //Открытие таблицы
	{
		ISProtocol::OpenSubSystem(MetaSubSystem->TableName, ISMetaData::Instance().GetMetaTable(MetaSubSystem->TableName)->LocalListName);
		CentralForm = new ISListBaseForm(MetaSubSystem->TableName, this);
	}
	else if (!MetaSubSystem->ClassName.isEmpty()) //Открытие класса (виджета)
	{
		ISProtocol::OpenSubSystem(QString(), MetaSubSystem->LocalName);
		CentralForm = ISAlgorithm::CreatePointer<ISInterfaceMetaForm *>(MetaSubSystem->ClassName, Q_ARG(QWidget *, this));
	}
	connect(CentralForm, &ISListBaseForm::AddFormFromTab, this, &ISWorkspaceForm::AddObjectForm);
	TabWidget->insertTab(0, CentralForm, IconSubSystem, MetaSubSystem->LocalName);
	TabWidget->setCurrentIndex(0);
	ISGui::SetWaitGlobalCursor(false);
	QTimer::singleShot(WAIT_LOAD_DATA_LIST_FORM, Qt::PreciseTimer, CentralForm, &ISInterfaceMetaForm::LoadData);
}
//-----------------------------------------------------------------------------
