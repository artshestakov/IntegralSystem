#include "ISTaskViewForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
#include "ISGui.h"
#include "ISControls.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
static QString QS_TASK = PREPARE_QUERY("SELECT "
									   "task_name, "
									   "task_description, "
									   "userfullname(task_executor) AS task_executor, "
									   "tstp_name AS task_type, "
									   "tsst_name AS task_status, "
									   "tspr_name AS task_priority, "
									   "userfullname(task_owner) AS task_owner, "
									   "task_important "
									   "FROM _task "
									   "LEFT JOIN _tasktype ON tstp_id = task_type "
									   "LEFT JOIN _taskstatus ON tsst_id = task_status "
									   "LEFT JOIN _taskpriority ON tspr_id = task_priority "
									   "WHERE task_id = :TaskID");
//-----------------------------------------------------------------------------
ISTaskViewForm::ISTaskViewForm(int task_id, QWidget *parent)
	: ISInterfaceForm(parent),
	TaskID(task_id)
{
	setWindowIcon(BUFFER_ICONS("Task"));
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	//Запрашиваем информацию по задаче
	ISQuery qSelect(QS_TASK);
	qSelect.BindValue(":TaskID", TaskID);
	if (!qSelect.ExecuteFirst()) //Ошибка запроса
	{
		ISMessageBox::ShowCritical(nullptr, qSelect.GetErrorString());
		return;
	}

	QString TaskName = qSelect.ReadColumn("task_name").toString();
	QString TaskDescription = qSelect.ReadColumn("task_description").toString();
	QString TaskExecutor = qSelect.ReadColumn("task_executor").toString();
	QString TaskType = qSelect.ReadColumn("task_type").toString();
	QString TaskStatus = qSelect.ReadColumn("task_status").toString();
	QString TaskPriority = qSelect.ReadColumn("task_priority").toString();
	QString TaskOwner = qSelect.ReadColumn("task_owner").toString();
	bool TaskImportant = qSelect.ReadColumn("task_important").toBool();

	setWindowTitle(LANG("Task.ViewFormTitle").arg(TaskID).arg(TaskName));

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutTitle);

	QLabel *LabelName = new QLabel(QString("#%1: %2").arg(TaskID).arg(TaskName), this);
	LabelName->setFont(ISDefines::Gui::FONT_TAHOMA_12_BOLD);
	LabelName->setStyleSheet(STYLE_SHEET("QLabel.Color.Gray"));
	LayoutTitle->addWidget(LabelName);

	LayoutTitle->addStretch();

	if (TaskImportant)
	{
		QLabel *LabelImportantIcon = new QLabel(this);
		LabelImportantIcon->setPixmap(BUFFER_ICONS("Task.Important.Checked").pixmap(ISDefines::Gui::SIZE_32_32));
		LayoutTitle->addWidget(LabelImportantIcon);

		QLabel *LabelImportantText = new QLabel(LANG("Task.ThisIsImportantTask"), this);
		ISGui::SetFontWidgetUnderline(LabelImportantText, true);
		LayoutTitle->addWidget(LabelImportantText);
	}

	LayoutHorizontal = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutHorizontal);

	LayoutLeft = new QVBoxLayout();
	LayoutHorizontal->addLayout(LayoutLeft);

	QGroupBox *GroupBoxDescription = new QGroupBox(LANG("Task.Description"), this);
	GroupBoxDescription->setLayout(new QVBoxLayout());
	LayoutLeft->addWidget(GroupBoxDescription);

	QLabel *LabelDescription = new QLabel(TaskDescription.isEmpty() ? LANG("Task.Description.Empty") : TaskDescription, GroupBoxDescription);
	LabelDescription->setWordWrap(true);
	LabelDescription->setTextInteractionFlags(TaskDescription.isEmpty() ? Qt::LinksAccessibleByMouse : Qt::TextSelectableByMouse);
	GroupBoxDescription->layout()->addWidget(LabelDescription);

	QGroupBox *GroupBoxFiles = new QGroupBox(LANG("Task.Files"), this);
	GroupBoxFiles->setLayout(new QVBoxLayout());
	LayoutLeft->addWidget(GroupBoxFiles);

	QGroupBox *GroupBoxLinkTask = new QGroupBox(LANG("Task.LinkTask"), this);
	GroupBoxLinkTask->setLayout(new QVBoxLayout());
	LayoutLeft->addWidget(GroupBoxLinkTask);

	QGroupBox *GroupBoxComments = new QGroupBox(LANG("Task.Comments"), this);
	GroupBoxComments->setLayout(new QVBoxLayout());
	LayoutLeft->addWidget(GroupBoxComments);

	ISListWidget *ListWidgetComments = new ISListWidget(GroupBoxComments);
	ListWidgetComments->setAlternatingRowColors(true);
	GroupBoxComments->layout()->addWidget(ListWidgetComments);

	LayoutLeft->addStretch();

	LayoutRight = new QVBoxLayout();

	QGroupBox *GroupBoxDetails = new QGroupBox(LANG("Task.Details"), this);
	GroupBoxDetails->setSizePolicy(QSizePolicy::Maximum, GroupBoxDetails->sizePolicy().verticalPolicy());
	GroupBoxDetails->setLayout(LayoutRight);
	LayoutHorizontal->addWidget(GroupBoxDetails);

	LayoutRight->addWidget(new QLabel(LANG("Task.Right.Executor") + ':', GroupBoxDetails));

	QLabel *LabelExecutor = new QLabel(TaskExecutor.isEmpty() ? LANG("Task.Right.Executor.Empty") : TaskExecutor, GroupBoxDetails);
	ISGui::SetFontWidgetBold(LabelExecutor, true);
	LayoutRight->addWidget(LabelExecutor);

	LayoutRight->addWidget(ISControls::CreateHorizontalLine(GroupBoxDetails));

	LayoutRight->addWidget(new QLabel(LANG("Task.Right.Type") + ':', GroupBoxDetails));

	QLabel *LabelType = new QLabel(TaskType.isEmpty() ? LANG("Task.Right.Type.Empty") : TaskType, GroupBoxDetails);
	ISGui::SetFontWidgetBold(LabelType, true);
	LayoutRight->addWidget(LabelType);

	LayoutRight->addWidget(ISControls::CreateHorizontalLine(GroupBoxDetails));

	LayoutRight->addWidget(new QLabel(LANG("Task.Right.Priority") + ':', GroupBoxDetails));

	QLabel *LabelPriority = new QLabel(TaskPriority, GroupBoxDetails);
	ISGui::SetFontWidgetBold(LabelPriority, true);
	LayoutRight->addWidget(LabelPriority);

	LayoutRight->addWidget(ISControls::CreateHorizontalLine(GroupBoxDetails));

	LayoutRight->addStretch();
}
//-----------------------------------------------------------------------------
ISTaskViewForm::~ISTaskViewForm()
{

}
//-----------------------------------------------------------------------------
