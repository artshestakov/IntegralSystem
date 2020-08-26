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
	ActionGroup(new QActionGroup(this)),
	TaskListForm(nullptr)
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

	CreateActionFilter(LANG("Task.Filter.All"), BUFFER_ICONS("Task.Filter.All"), "ISTaskListForm");

	MainLayout->addWidget(ISControls::CreateHorizontalLine(this));

	CreateTempWidget();
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
	if (ListFormName == CurrentListForm)
	{
		return;
	}
	CurrentListForm = ListFormName;
	POINTER_DELETE(TempWidget);
	if (!TaskListForm)
	{
		TaskListForm = new ISTaskListForm(this);
		connect(TaskListForm, &ISTaskListForm::AddFormFromTab, [=](QWidget *ObjectForm) { ISGui::ShowObjectForm(ObjectForm); });
		MainLayout->addWidget(TaskListForm);
		QTimer::singleShot(WAIT_LOAD_DATA_LIST_FORM, Qt::PreciseTimer, TaskListForm, &ISTaskListForm::LoadData);
	}

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
