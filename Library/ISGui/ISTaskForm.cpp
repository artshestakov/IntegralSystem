#include "ISTaskForm.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISControls.h"
#include "ISCore.h"
#include "ISGui.h"
#include "ISInputDialog.h"
#include "ISMessageBox.h"
#include "ISAlgorithm.h"
#include "ISTaskSearchByTextForm.h"
//-----------------------------------------------------------------------------
ISTaskForm::ISTaskForm(QWidget *parent)
	: ISParagraphBaseForm(parent),
	ActionGroup(new QActionGroup(this))
{
	MainLayout = new QVBoxLayout();
	MainLayout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_5_PX);
	setLayout(MainLayout);

	ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	MainLayout->addWidget(ToolBar);

	QAction *ActionCreate = new QAction(BUFFER_ICONS("Add"), LANG("Task.CreateTask"), ToolBar);
	connect(ActionCreate, &QAction::triggered, this, &ISTaskForm::CreateTask);
	ToolBar->addAction(ActionCreate);
	ISGui::SetFontWidgetUnderline(ToolBar->widgetForAction(ActionCreate), true);

	QAction *ActionSearchByNumber = new QAction(BUFFER_ICONS("Search"), LANG("Task.SearchByNumber"), ToolBar);
	ActionSearchByNumber->setToolTip(LANG("Task.SearchByNumber.ToolTip"));
	connect(ActionSearchByNumber, &QAction::triggered, this, &ISTaskForm::SearchByNumber);
	ToolBar->addAction(ActionSearchByNumber);

	QAction *ActionSearchByText = new QAction(BUFFER_ICONS("Search"), LANG("Task.SearchByText"), ToolBar);
	connect(ActionSearchByText, &QAction::triggered, this, &ISTaskForm::SearchByText);
	ToolBar->addAction(ActionSearchByText);

	ToolBar->addSeparator();

	CreateActionFilter(LANG("Task.Filter.All"), BUFFER_ICONS("Task.Filter.All"), QString());
	CreateActionFilter(LANG("Task.Filter.My"), BUFFER_ICONS("Task.Filter.My"), "task_executor = currentuserid()");
	CreateActionFilter(LANG("Task.Filter.Open"), BUFFER_ICONS("Task.Filter.Open"), "task_executor = currentuserid() AND task_status = get_status_id_by_uid('{2FD27958-6EC9-4E78-88F4-0A2BFDFE4716}')");
	CreateActionFilter(LANG("Task.Filter.InWork"), BUFFER_ICONS("Task.Filter.InWork"), "task_executor = currentuserid() AND task_status = get_status_id_by_uid('{72BEB1A3-0491-4932-AAA0-5725DA41A0A8}')");
	CreateActionFilter(LANG("Task.Filter.Done"), BUFFER_ICONS("Task.Filter.Done"), "task_executor = currentuserid() AND task_status = get_status_id_by_uid('{20F3D6A5-1A90-4228-9C39-8F05DFDED9FE}')");
	CreateActionFilter(LANG("Task.Filter.NotDone"), BUFFER_ICONS("Task.Filter.NotDone"), "task_executor = currentuserid() AND task_status = get_status_id_by_uid('{D3F65984-117E-4AFE-81E0-1198533767F8}')");
	CreateActionFilter(LANG("Task.Filter.Close"), BUFFER_ICONS("Task.Filter.Close"), "task_executor = currentuserid() AND task_status = get_status_id_by_uid('{1617EC4F-50F6-4912-8143-4D702E1F5F8F}')");

	MainLayout->addWidget(ISControls::CreateHorizontalLine(this));

	TaskListForm = new ISTaskListForm(this);
	connect(TaskListForm, &ISTaskListForm::AddFormFromTab, [=](QWidget *ObjectForm) { ISGui::ShowObjectForm(ObjectForm); });
	MainLayout->addWidget(TaskListForm);
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
void ISTaskForm::CreateActionFilter(const QString &Text, const QIcon &Icon, const QString &SqlFilter)
{
	QAction *ActionFilter = ToolBar->addAction(Icon, Text, this, &ISTaskForm::FilterClicked);
	ActionFilter->setProperty("SqlFilter", SqlFilter);
	ActionFilter->setCheckable(true);
	ToolBar->addAction(ActionFilter);
	ActionGroup->addAction(ActionFilter);
}
//-----------------------------------------------------------------------------
void ISTaskForm::FilterClicked()
{
	QString SqlFilter = sender()->property("SqlFilter").toString();
	if (SqlFilter.isEmpty())
	{
		TaskListForm->GetQueryModel()->ClearClassFilter();
	}
	else
	{
		TaskListForm->GetQueryModel()->SetClassFilter(SqlFilter);
	}
	TaskListForm->LoadData();

	for (QAction *Action : ActionGroup->actions())
	{
		Action->setFont(ISDefines::Gui::FONT_APPLICATION);
		Action->setChecked(false);
		if (Action == sender())
		{
			Action->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
			Action->setChecked(true);
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskForm::CreateTask()
{
	ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::OFT_New, "_Task"));
}
//-----------------------------------------------------------------------------
void ISTaskForm::SearchByNumber()
{
	bool Ok = true;
	while (true)
	{
		int TaskID = ISInputDialog::GetInteger(Ok, LANG("Task.Search"), LANG("Task.InputTaskNumber") + ':', 0, INT_MAX);
		if (Ok && TaskID > 0)
		{
			if (ISCore::TaskCheckExist(TaskID))
			{
				ISGui::ShowTaskViewForm(TaskID);
				break;
			}
			else
			{
				ISMessageBox::ShowInformation(this, LANG("Message.Warning.TaskWithNumberNotFound").arg(TaskID));
			}
		}
		else
		{
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskForm::SearchByText()
{
	(new ISTaskSearchByTextForm())->show();
}
//-----------------------------------------------------------------------------
