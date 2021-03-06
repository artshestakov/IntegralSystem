#include "ISTaskParagraph.h"
#include "ISDefinesGui.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISControls.h"
#include "ISCore.h"
#include "ISGui.h"
#include "ISInputDialog.h"
#include "ISMessageBox.h"
#include "ISAlgorithm.h"
#include "ISTaskSearchByTextForm.h"
#include "ISTcpQuery.h"
#include "ISUserRoleEntity.h"
//-----------------------------------------------------------------------------
ISTaskParagraph::ISTaskParagraph(QWidget *parent)
	: ISParagraphBaseForm(parent),
	ActionGroup(new QActionGroup(this)),
	TaskListForm(nullptr)
{
	MainLayout = new QVBoxLayout();
	MainLayout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_5_PX);
	setLayout(MainLayout);

	ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	MainLayout->addWidget(ToolBar);

	QAction *ActionCreate = ToolBar->addAction(BUFFER_ICONS("Add"), LANG("Task.CreateTask"), this, &ISTaskParagraph::CreateTask);
	dynamic_cast<QToolButton*>(ToolBar->widgetForAction(ActionCreate))->setPopupMode(QToolButton::MenuButtonPopup);
	ISGui::SetFontWidgetUnderline(ToolBar->widgetForAction(ActionCreate), true);
	ActionCreate->setMenu(new QMenu(ToolBar->widgetForAction(ActionCreate)));
	ActionCreate->menu()->addAction(BUFFER_ICONS("Search"), LANG("Task.SearchByNumber"), this, &ISTaskParagraph::SearchByNumber);
	ActionCreate->menu()->addAction(BUFFER_ICONS("Search"), LANG("Task.SearchByText"), this, &ISTaskParagraph::SearchByText);

	ToolBar->addSeparator();

	//���� ��������� ������������� ��� ������
	if (ISUserRoleEntity::Instance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_TASK_SHOW_ALL))
	{
		CreateActionFilter(LANG("Task.Filter.All"), BUFFER_ICONS("Task.Filter.All"), QString());
	}

	CreateActionFilter(LANG("Task.Filter.My"), BUFFER_ICONS("Task.Filter.My"), "task_executor = " + QString::number(CURRENT_USER_ID));
	CreateActionFilter(LANG("Task.Filter.Open"), BUFFER_ICONS("Task.Status.Open"), "task_executor = " + QString::number(CURRENT_USER_ID) + " AND task_status = get_task_status_id_by_uid('{2FD27958-6EC9-4E78-88F4-0A2BFDFE4716}')");
	CreateActionFilter(LANG("Task.Filter.InWork"), BUFFER_ICONS("Task.Status.InWork"), "task_executor = " + QString::number(CURRENT_USER_ID) + " AND task_status = get_task_status_id_by_uid('{72BEB1A3-0491-4932-AAA0-5725DA41A0A8}')");
	CreateActionFilter(LANG("Task.Filter.Done"), BUFFER_ICONS("Task.Status.Done"), "task_executor = " + QString::number(CURRENT_USER_ID) + " AND task_status = get_task_status_id_by_uid('{20F3D6A5-1A90-4228-9C39-8F05DFDED9FE}')");
	CreateActionFilter(LANG("Task.Filter.NotDone"), BUFFER_ICONS("Task.Status.NotDone"), "task_executor = " + QString::number(CURRENT_USER_ID) + " AND task_status = get_task_status_id_by_uid('{D3F65984-117E-4AFE-81E0-1198533767F8}')");
	CreateActionFilter(LANG("Task.Filter.Close"), BUFFER_ICONS("Task.Status.Close"), "task_executor = " + QString::number(CURRENT_USER_ID) + " AND task_status = get_task_status_id_by_uid('{1617EC4F-50F6-4912-8143-4D702E1F5F8F}')");
	CreateActionFilter(LANG("Task.Filter.Today"), BUFFER_ICONS("Task.Filter.Today"), "task_executor = " + QString::number(CURRENT_USER_ID) + " AND task_status IN(get_task_status_id_by_uid('{2FD27958-6EC9-4E78-88F4-0A2BFDFE4716}'), get_task_status_id_by_uid('{72BEB1A3-0491-4932-AAA0-5725DA41A0A8}')) AND task_deadline = CURRENT_DATE");
	CreateActionFilter(LANG("Task.Filter.Tomorrow"), BUFFER_ICONS("Task.Filter.Tomorrow"), "task_executor = " + QString::number(CURRENT_USER_ID) + " AND task_status IN(get_task_status_id_by_uid('{2FD27958-6EC9-4E78-88F4-0A2BFDFE4716}'), get_task_status_id_by_uid('{72BEB1A3-0491-4932-AAA0-5725DA41A0A8}')) AND task_deadline = CURRENT_DATE + 1");
	CreateActionFilter(LANG("Task.Filter.Expired"), BUFFER_ICONS("Task.Filter.Expired"), "task_executor = " + QString::number(CURRENT_USER_ID) + " AND task_status IN(get_task_status_id_by_uid('{2FD27958-6EC9-4E78-88F4-0A2BFDFE4716}'), get_task_status_id_by_uid('{72BEB1A3-0491-4932-AAA0-5725DA41A0A8}')) AND task_deadline < CURRENT_DATE");
	MainLayout->addWidget(ISControls::CreateHorizontalLine(this));

	CreateTempWidget();
}
//-----------------------------------------------------------------------------
ISTaskParagraph::~ISTaskParagraph()
{

}
//-----------------------------------------------------------------------------
void ISTaskParagraph::Invoke()
{
	ISParagraphBaseForm::Invoke();
}
//-----------------------------------------------------------------------------
void ISTaskParagraph::CreateTempWidget()
{
	QVBoxLayout *Layout = new QVBoxLayout();
	Layout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	TempWidget = new QWidget(this);
	TempWidget->setLayout(Layout);
	MainLayout->addWidget(TempWidget);

	Layout->addStretch();

	QHBoxLayout *LayoutLabel = new QHBoxLayout();
	Layout->addLayout(LayoutLabel);

	LayoutLabel->addStretch();

	QLabel *LabelIcon = new QLabel(this);
	LabelIcon->setPixmap(BUFFER_ICONS("Arrow.Up").pixmap(ISDefines::Gui::SIZE_32_32));
	LayoutLabel->addWidget(LabelIcon);

	QLabel *LabelText = new QLabel(this);
	LabelText->setText(LANG("Task.LabelCentral"));
	LabelText->setFont(ISDefines::Gui::FONT_TAHOMA_12_BOLD);
	LabelText->setStyleSheet(BUFFER_STYLE_SHEET("QLabel.Color.Gray"));
	LabelText->setAlignment(Qt::AlignCenter);
	LayoutLabel->addWidget(LabelText);

	LayoutLabel->addStretch();
	Layout->addStretch();
}
//-----------------------------------------------------------------------------
void ISTaskParagraph::CreateActionFilter(const QString &Text, const QIcon &Icon, const QString &SqlFilter)
{
	QAction *ActionFilter = ToolBar->addAction(Icon, Text, this, &ISTaskParagraph::FilterClicked);
	ActionFilter->setProperty("SqlFilter", SqlFilter);
	ActionFilter->setCheckable(true);
	ToolBar->addAction(ActionFilter);
	ActionGroup->addAction(ActionFilter);
}
//-----------------------------------------------------------------------------
void ISTaskParagraph::FilterClicked()
{
	if (!TaskListForm)
	{
		delete TempWidget;
		TaskListForm = new ISTaskListForm(this);
		connect(TaskListForm, &ISTaskListForm::AddFormFromTab, [=](QWidget *ObjectForm) { ISGui::ShowObjectForm(ObjectForm); });
		MainLayout->addWidget(TaskListForm);
	}

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
void ISTaskParagraph::CreateTask()
{
	ISGui::ShowObjectForm(ISGui::CreateObjectForm(ISNamespace::OFT_New, "_Task"));
}
//-----------------------------------------------------------------------------
void ISTaskParagraph::SearchByNumber()
{
	bool Ok = true;
	while (true)
	{
		unsigned int TaskID = ISInputDialog::GetUInteger(Ok, LANG("Task.Search"), LANG("Task.InputTaskNumber") + ':', INT_MAX);
		if (Ok && TaskID > 0)
		{
			ISTcpQuery qTaskSearchID(API_SEARCH_TASK_ID);
			qTaskSearchID.BindValue("ID", TaskID);
			if (qTaskSearchID.Execute())
			{
				bool Result = qTaskSearchID.GetAnswer()["Result"].toBool();
				if (Result)
				{
					ISGui::ShowTaskViewForm(TaskID);
					break;
				}
				else
				{
					ISMessageBox::ShowWarning(this, LANG("Message.Warning.TaskWithNumberNotFound").arg(TaskID));
				}
			}
			else
			{
				ISMessageBox::ShowWarning(this, qTaskSearchID.GetErrorString());
			}
		}
		else
		{
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskParagraph::SearchByText()
{
	(new ISTaskSearchByTextForm())->show();
}
//-----------------------------------------------------------------------------
