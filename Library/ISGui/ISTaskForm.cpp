#include "ISTaskForm.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISSystem.h"
#include "ISControls.h"
#include "ISCore.h"
#include "ISGui.h"
#include "ISInputDialog.h"
#include "ISMessageBox.h"
#include "ISAlgorithm.h"
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
	ActionCreate->setText(LANG("Task.CreateTask"));
	ActionCreate->setToolTip(LANG("Task.CreateTask"));
	ActionCreate->setIcon(BUFFER_ICONS("Add"));
	connect(ActionCreate, &QAction::triggered, this, &ISTaskForm::CreateTask);
	ToolBar->addAction(ActionCreate);
	ISGui::SetFontWidgetUnderline(ToolBar->widgetForAction(ActionCreate), true);

	QAction *ActionSearchByNumber = new QAction(ToolBar);
	ActionSearchByNumber->setText(LANG("Task.SearchByNumber"));
	ActionSearchByNumber->setToolTip(LANG("Task.SearchByNumber"));
	ActionSearchByNumber->setIcon(BUFFER_ICONS("Search"));
	connect(ActionSearchByNumber, &QAction::triggered, this, &ISTaskForm::SearchByNumber);
	ToolBar->addAction(ActionSearchByNumber);

	ToolBar->addSeparator();

	CreateActionFilter(LANG("Task.Filter.All"), BUFFER_ICONS("Task.Filter.All"), "ISTaskAllListForm");
	CreateActionFilter(LANG("Task.Filter.My"), BUFFER_ICONS("Task.Filters.My"), "ISTaskMyListForm");
	CreateActionFilter(LANG("Task.Filter.From"), BUFFER_ICONS("Task.Filters.From"), "ISTaskFromListForm");
	CreateActionFilter(LANG("Task.Filter.Favorite"), BUFFER_ICONS("Task.Filters.Favorite"), "ISTaskFavoriteListForm");
	CreateActionFilter(LANG("Task.Filter.Deadline"), BUFFER_ICONS("Task.Filters.Deadline"), "ISTaskTodayListForm");
	CreateActionFilter(LANG("Task.Filter.Overdue"), BUFFER_ICONS("Task.Filter.Overdue"), "ISTaskOverdueListForm");
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

	TaskListForm = ISAlgorithm::CreatePointer<ISTaskBaseListForm *>(ListFormName, Q_ARG(QWidget *, this));
	connect(TaskListForm, &ISTaskBaseListForm::AddFormFromTab, [=](QWidget *ObjectForm) { ISGui::ShowObjectForm(ObjectForm); });
	MainLayout->addWidget(TaskListForm);
	QTimer::singleShot(WAIT_LOAD_DATA_LIST_FORM, Qt::PreciseTimer, TaskListForm, &ISTaskBaseListForm::LoadData);

	CurrentListForm = ListFormName;

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
	ISGui::ShowTaskObjectForm(ISNamespace::OFT_New);
}
//-----------------------------------------------------------------------------
void ISTaskForm::SearchByNumber()
{
	while (true)
	{
		int TaskID = ISInputDialog::GetInteger(LANG("Search"), LANG("InputTheTaskNumber") + ':', 0, INT_MAX);
		if (TaskID != NPOS)
		{
			if (ISCore::TaskCheckExist(TaskID))
			{
				ISCore::TaskGetStatusUID(TaskID) == CONST_UID_TASK_STATUS_NEW ? ISGui::ShowTaskObjectForm(ISNamespace::OFT_Edit, TaskID) : ISGui::ShowTaskViewForm(TaskID);
				break;
			}
			else
			{
				ISMessageBox::ShowInformation(this, LANG("Message.Information.TaskWithNumberNotFound").arg(TaskID));
			}
		}
		else
		{
			break;
		}
	}
}
//-----------------------------------------------------------------------------