#include "ISTaskInfoWidget.h"
#include "ISDefinesGui.h"
#include "ISDefinesCore.h"
#include "ISLocalization.h"
#include "ISStyleSheet.h"
#include "ISQuery.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
static QString QS_TASK = PREPARE_QUERY("SELECT "
									   "tsst_name, "
									   "tsst_stylesheet, "
									   "tspr_name, "
									   "task_deadline, "
									   "userfullname(task_owner) as owner, "
									   "userfullname(task_executor) as executor, "
									   "task_datetime, "
									   "task_resolution "
									   "FROM _task "
									   "LEFT JOIN _taskstatus ON tsst_id = task_status "
									   "LEFT JOIN _taskpriority ON tspr_id = task_priority "
									   "WHERE task_id = :TaskID");
//-----------------------------------------------------------------------------
ISTaskInfoWidget::ISTaskInfoWidget(int task_id, QWidget *parent) : QWidget(parent)
{
	TaskID = task_id;

	FormLayout = new QFormLayout();
	setLayout(FormLayout);

	LabelStatus = AddLabel(LANG("Task.Status") + ':');
	LabelStatus->setFont(ISDefines::Gui::FONT_TAHOMA_10);
	LabelStatus->setSizePolicy(QSizePolicy::Maximum, LabelStatus->sizePolicy().verticalPolicy());

	LabelNumber = AddLabel(LANG("Task.Number") + ':');
	LabelPriority = AddLabel(LANG("Task.Priority") + ':');
	LabelDeadline = AddLabel(LANG("Task.Deadline") + ':');
	LabelOwner = AddLabel(LANG("Task.Owner") + ':');
	LabelExecutor = AddLabel(LANG("Task.Executor") + ':');
	LabelDateTime = AddLabel(LANG("Task.DateTime") + ':');

	LabelResolution = AddLabel(LANG("Task.Resolution") + ':');
	LabelResolution->setWordWrap(true);
}
//-----------------------------------------------------------------------------
ISTaskInfoWidget::~ISTaskInfoWidget()
{

}
//-----------------------------------------------------------------------------
void ISTaskInfoWidget::Update()
{
	ISQuery qSelectTask(QS_TASK);
	qSelectTask.BindValue(":TaskID", TaskID);
	if (qSelectTask.ExecuteFirst())
	{
		QString TaskNumber = QString::number(TaskID);
		QString TaskStatus = qSelectTask.ReadColumn("tsst_name").toString();
		QString TaskStatusStyleSheet = qSelectTask.ReadColumn("tsst_stylesheet").toString();
		QString TaskPriority = qSelectTask.ReadColumn("tspr_name").toString();
		QString TaskDeadline = qSelectTask.ReadColumn("task_deadline").toDate().toString(DATE_FORMAT_V2);
		QString TaskOwner = qSelectTask.ReadColumn("owner").toString();
		QString TaskExecutor = qSelectTask.ReadColumn("executor").toString();
		QString TaskDateTime = qSelectTask.ReadColumn("task_datetime").toDateTime().toString(DATE_TIME_FORMAT_V2);
		QString TaskResolution = qSelectTask.ReadColumn("task_resolution").toString();

		LabelNumber->setText(TaskNumber);
		LabelStatus->setText(TaskStatus);

		if (TaskStatusStyleSheet.length())
		{
			LabelStatus->setStyleSheet(STYLE_SHEET(TaskStatusStyleSheet));
		}

		LabelPriority->setText(TaskPriority);

		if (TaskDeadline.length())
		{
			LabelDeadline->setText(TaskDeadline);
		}
		else
		{
			LabelDeadline->setText("-");
		}

		LabelOwner->setText(TaskOwner);
		LabelExecutor->setText(TaskExecutor);
		LabelDateTime->setText(TaskDateTime);

		
		if (TaskResolution.length())
		{
			LabelResolution->setText(TaskResolution);
		}
		else
		{
			LabelResolution->setText("-");
		}
	}
}
//-----------------------------------------------------------------------------
QLabel* ISTaskInfoWidget::AddLabel(const QString &RowText)
{
	QLabel *LabelRow = new QLabel(this);
	LabelRow->setText(RowText);
	LabelRow->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);

	QLabel *Label = new QLabel(this);

	FormLayout->addRow(LabelRow, Label);

	return Label;
}
//-----------------------------------------------------------------------------
