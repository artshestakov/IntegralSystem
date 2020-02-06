#include "ISTaskFilesTab.h"
#include "ISQuery.h"
#include "ISTaskFileWidget.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_FILES = PREPARE_QUERY("SELECT tfls_creationdate, userfullname(tfls_user), tfls_filename, tfls_size, tfls_icon, tfls_note "
										"FROM _taskfile "
										"WHERE tfls_task = :TaskID "
										"ORDER BY tfls_id");
//-----------------------------------------------------------------------------
ISTaskFilesTab::ISTaskFilesTab(int task_id, QTabWidget *parent) : ISTaskTabBase(task_id, parent)
{
	ListWidget = new ISListWidget(this);
	ListWidget->setFrameShape(QFrame::NoFrame);
	ListWidget->setAlternatingRowColors(true);
	layout()->addWidget(ListWidget);
}
//-----------------------------------------------------------------------------
ISTaskFilesTab::~ISTaskFilesTab()
{

}
//-----------------------------------------------------------------------------
void ISTaskFilesTab::Update()
{
	ListWidget->Clear();
	ISQuery qSelectFiles(QS_FILES);
	qSelectFiles.BindValue(":TaskID", GetTaskID());
	if (qSelectFiles.Execute())
	{
		SetCount(qSelectFiles.GetCountResultRows());
		while (qSelectFiles.Next())
		{
			QDateTime DateTime = qSelectFiles.ReadColumn("tfls_creationdate").toDateTime();
			QString UserName = qSelectFiles.ReadColumn("userfullname").toString();
			QString FileName = qSelectFiles.ReadColumn("tfls_filename").toString();
			QString FileSize = qSelectFiles.ReadColumn("tfls_size").toString();
			QByteArray FileIcon = qSelectFiles.ReadColumn("tfls_icon").toByteArray();
			QString FileNote = qSelectFiles.ReadColumn("tfls_note").toString();

			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);
			ISTaskFileWidget *TaskFileWidget = new ISTaskFileWidget(DateTime, UserName, FileName, FileSize, FileIcon, FileNote, ListWidget);
			ListWidget->setItemWidget(ListWidgetItem, TaskFileWidget);
			TaskFileWidget->adjustSize();
			ListWidgetItem->setSizeHint(TaskFileWidget->sizeHint());
		}
	}

	Change();
}
//-----------------------------------------------------------------------------
