#include "StdAfx.h"
#include "ISTaskForm.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISSystem.h"
#include "ISControls.h"
#include "ISCore.h"
#include "ISGui.h"
#include "ISMetaData.h"
#include "ISIntegralSystem.h"
#include "ISQuery.h"
#include "ISInputDialog.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISTaskForm::ISTaskForm(QWidget *parent) : ISParagraphBaseForm(parent)
{
	TaskListForm = nullptr;
	ActionGroup = new QActionGroup(this);

	MainLayout = new QVBoxLayout();
	setLayout(MainLayout);

	ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	MainLayout->addWidget(ToolBar);

	MainLayout->addWidget(ISControls::CreateHorizontalLine(this));

	CreateTempWidget();
	
	QAction *ActionCreate = new QAction(ToolBar);
	ActionCreate->setText(LOCALIZATION("Task.CreateTask"));
	ActionCreate->setToolTip(LOCALIZATION("Task.CreateTask"));
	ActionCreate->setIcon(BUFFER_ICONS("Add"));
	connect(ActionCreate, &QAction::triggered, this, &ISTaskForm::CreateTask);
	ToolBar->addAction(ActionCreate);
	ISSystem::SetFontWidgetUnderline(ToolBar->widgetForAction(ActionCreate), true);

	QAction *ActionSearchByNumber = new QAction(ToolBar);
	ActionSearchByNumber->setText(LOCALIZATION("Task.SearchByNumber"));
	ActionSearchByNumber->setToolTip(LOCALIZATION("Task.SearchByNumber"));
	ActionSearchByNumber->setIcon(BUFFER_ICONS("Search"));
	connect(ActionSearchByNumber, &QAction::triggered, this, &ISTaskForm::SearchByNumber);
	ToolBar->addAction(ActionSearchByNumber);

	ToolBar->addSeparator();

	CreateActionFilter(LOCALIZATION("Task.Filter.All"), BUFFER_ICONS("Task.Filter.All"), "ISTaskAllListForm");
	CreateActionFilter(LOCALIZATION("Task.Filter.My"), BUFFER_ICONS("Task.Filters.My"), "ISTaskMyListForm");
	CreateActionFilter(LOCALIZATION("Task.Filter.From"), BUFFER_ICONS("Task.Filters.From"), "ISTaskFromListForm");
	CreateActionFilter(LOCALIZATION("Task.Filter.Favorite"), BUFFER_ICONS("Task.Filters.Favorite"), "ISTaskFavoriteListForm");
	CreateActionFilter(LOCALIZATION("Task.Filter.Deadline"), BUFFER_ICONS("Task.Filters.Deadline"), "ISTaskTodayListForm");
	CreateActionFilter(LOCALIZATION("Task.Filter.Overdue"), BUFFER_ICONS("Task.Filter.Overdue"), "ISTaskOverdueListForm");
}
//-----------------------------------------------------------------------------
ISTaskForm::~ISTaskForm()
{

}
//-----------------------------------------------------------------------------
void ISTaskForm::Invoke()
{
	ISParagraphBaseForm::Invoke();
}
//-----------------------------------------------------------------------------
void ISTaskForm::CreateTempWidget()
{
	QVBoxLayout *Layout = new QVBoxLayout();
	Layout->setContentsMargins(LAYOUT_MARGINS_NULL);
	
	TempWidget = new QWidget(this);
	TempWidget->setLayout(Layout);
	MainLayout->addWidget(TempWidget);

	Layout->addStretch();

	QHBoxLayout *LayoutLabel = new QHBoxLayout();
	Layout->addLayout(LayoutLabel);

	LayoutLabel->addStretch();

	QLabel *LabelIcon = new QLabel(this);
	LabelIcon->setPixmap(BUFFER_ICONS("Arrow.Up").pixmap(SIZE_32_32));
	LayoutLabel->addWidget(LabelIcon);

	QLabel *LabelText = new QLabel(this);
	LabelText->setText(LOCALIZATION("Task.LabelCentral"));
	LabelText->setFont(FONT_TAHOMA_12_BOLD);
	LabelText->setStyleSheet(STYLE_SHEET("QLabel.Color.Gray"));
	LabelText->setAlignment(Qt::AlignCenter);
	LayoutLabel->addWidget(LabelText);

	LayoutLabel->addStretch();

	Layout->addStretch();
}
//-----------------------------------------------------------------------------
QAction* ISTaskForm::CreateActionFilter(const QString &Text, const QIcon &Icon, const QString &ListFormName)
{
	QAction *ActionFilter = new QAction(ToolBar);
	ActionFilter->setText(Text);
	ActionFilter->setIcon(Icon);
	ActionFilter->setData(ListFormName);
	ActionFilter->setCheckable(true);
	connect(ActionFilter, &QAction::triggered, this, &ISTaskForm::FilterClicked);
	ToolBar->addAction(ActionFilter);

	ActionGroup->addAction(ActionFilter);
	return ActionFilter;
}
//-----------------------------------------------------------------------------
void ISTaskForm::FilterClicked()
{
	QString ListFormName = dynamic_cast<QAction*>(sender())->data().toString();

	if (CurrentListForm == ListFormName)
	{
		return;
	}

	if (TempWidget)
	{
		delete TempWidget;
		TempWidget = nullptr;
	}

	if (TaskListForm)
	{
		delete TaskListForm;
		TaskListForm = nullptr;
	}

	int ObjectType = QMetaType::type((ListFormName + "*").toLocal8Bit().constData());
	IS_ASSERT(ObjectType, QString("Class for task list form is NULL. ClassName: %1").arg(ListFormName));

	const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
	IS_ASSERT(MetaObject, "Error opening subsystem widget.");

	TaskListForm = dynamic_cast<ISTaskBaseListForm*>(MetaObject->newInstance(Q_ARG(QWidget *, this)));
	IS_ASSERT(TaskListForm, QString("Error instance task list form . ListForm: %1").arg(ListFormName));
	connect(TaskListForm, &ISTaskBaseListForm::AddFormFromTab, this, &ISTaskForm::CreateObjectForm);
	MainLayout->addWidget(TaskListForm);
	QTimer::singleShot(WAIT_LOAD_DATA_LIST_FORM, Qt::PreciseTimer, TaskListForm, &ISTaskBaseListForm::LoadData);

	CurrentListForm = ListFormName;

	for (QAction *Action : ActionGroup->actions())
	{
		Action->setFont(FONT_APPLICATION);
		Action->setChecked(false);

		if (Action == sender())
		{
			Action->setFont(FONT_APPLICATION_BOLD);
			Action->setChecked(true);
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskForm::CreateTask()
{
	ISIntegralSystem::ShowTaskObjectForm(ISNamespace::OFT_New);
}
//-----------------------------------------------------------------------------
void ISTaskForm::SearchByNumber()
{
	int TaskID = ISInputDialog::GetInteger(this, LOCALIZATION("Search"), LOCALIZATION("InputTheTaskNumber") + ":", 0, INTEGER_MAXIMUM).toInt();
	if (TaskID)
	{
		if (ISCore::TaskCheckExist(TaskID))
		{
			ISUuid StatusUID = ISCore::TaskGetStatusUID(TaskID);
			if (StatusUID == CONST_UID_TASK_STATUS_NEW)
			{
				ISIntegralSystem::ShowTaskObjectForm(ISNamespace::OFT_Edit, TaskID);
			}
			else
			{
				ISIntegralSystem::ShowTaskViewForm(TaskID);
			}
		}
		else
		{
			ISMessageBox::ShowInformation(this, LOCALIZATION("Message.Information.TaskWithNumberNotFound").arg(TaskID));
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskForm::CreateObjectForm(QWidget *TaskObjectForm)
{
	ISIntegralSystem::ShowTaskObjectForm(TaskObjectForm);
}
//-----------------------------------------------------------------------------
