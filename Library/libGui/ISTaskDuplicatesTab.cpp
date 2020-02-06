#include "ISTaskDuplicatesTab.h"
#include "ISQuery.h"
#include "ISBuffer.h"
#include "ISGui.h"
#include "ISConstants.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_DUPLICATES = PREPARE_QUERY("SELECT tsdp_duplicate, task_name "
											 "FROM _taskduplicate "
											 "LEFT JOIN _task ON tsdp_duplicate = task_id "
											 "WHERE tsdp_original = :TaskID");
//-----------------------------------------------------------------------------
ISTaskDuplicatesTab::ISTaskDuplicatesTab(int task_id, QTabWidget *parent) : ISTaskTabBase(task_id, parent)
{
	ListWidget = new ISListWidget(this);
	ListWidget->setAlternatingRowColors(true);
	ListWidget->setFrameShape(QFrame::NoFrame);
	ListWidget->setCursor(CURSOR_POINTING_HAND);
	connect(ListWidget, &ISListWidget::itemDoubleClicked, this, &ISTaskDuplicatesTab::ItemDoubleClicked);
	layout()->addWidget(ListWidget);
}
//-----------------------------------------------------------------------------
ISTaskDuplicatesTab::~ISTaskDuplicatesTab()
{

}
//-----------------------------------------------------------------------------
void ISTaskDuplicatesTab::Update()
{
	ListWidget->Clear();
	ISQuery qSelectDuplicates(QS_DUPLICATES);
	qSelectDuplicates.BindValue(":TaskID", GetTaskID());
	if (qSelectDuplicates.Execute())
	{
		SetCount(qSelectDuplicates.GetCountResultRows());
		while (qSelectDuplicates.Next())
		{
			int DuplicateID = qSelectDuplicates.ReadColumn("tsdp_duplicate").toInt();
			QString TaskName = qSelectDuplicates.ReadColumn("task_name").toString();

			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
			ListWidgetItem->setText(QString("#%1 %2").arg(DuplicateID).arg(TaskName));
			ListWidgetItem->setIcon(BUFFER_ICONS("Task"));
			ListWidgetItem->setData(Qt::UserRole, DuplicateID);
			ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));
		}
	}

	Change();
}
//-----------------------------------------------------------------------------
void ISTaskDuplicatesTab::ItemDoubleClicked(QListWidgetItem *ListWidgetItem)
{
	ISGui::ShowTaskViewForm(ListWidgetItem->data(Qt::UserRole).toInt());
}
//-----------------------------------------------------------------------------
