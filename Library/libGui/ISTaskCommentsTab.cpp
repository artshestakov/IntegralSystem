#include "ISTaskCommentsTab.h"
#include "ISDefinesGui.h"
#include "ISQuery.h"
#include "ISTaskCommentWidget.h"
#include "ISControls.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_COMMENTS = PREPARE_QUERY("SELECT tcom_creationdate, userfullname(tcom_user), tcom_comment "
										   "FROM _taskcomment "
										   "WHERE NOT tcom_isdeleted "
										   "AND tcom_task = :TaskID "
										   "ORDER BY tcom_id");
//-----------------------------------------------------------------------------
ISTaskCommentsTab::ISTaskCommentsTab(int task_id, QTabWidget *parent) : ISTaskTabBase(task_id, parent)
{
	LayoutComments = new QVBoxLayout();
	LayoutComments->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	LayoutComments->addStretch();

	ScrollAreaComments = new ISScrollArea(this);
	ScrollAreaComments->widget()->setLayout(LayoutComments);
	layout()->addWidget(ScrollAreaComments);
}
//-----------------------------------------------------------------------------
ISTaskCommentsTab::~ISTaskCommentsTab()
{

}
//-----------------------------------------------------------------------------
void ISTaskCommentsTab::Update()
{
	while (Vector.count())
	{
		QWidget *Widget = Vector.takeFirst();
		delete Widget;
	}

	ISQuery qSelectComments(QS_COMMENTS);
	qSelectComments.BindValue(":TaskID", GetTaskID());
	if (qSelectComments.Execute())
	{
		SetCount(qSelectComments.GetCountResultRows());
		while (qSelectComments.Next())
		{
			QDateTime CreationDate = qSelectComments.ReadColumn("tcom_creationdate").toDateTime();
			QString UserFullName = qSelectComments.ReadColumn("userfullname").toString();
			QString Comment = qSelectComments.ReadColumn("tcom_comment").toString();

			ISTaskCommentWidget *TaskCommentWidget = new ISTaskCommentWidget(CreationDate, UserFullName, Comment, ScrollAreaComments);
			LayoutComments->insertWidget(LayoutComments->count() - 1, TaskCommentWidget);

			QFrame *HorizontalLine = ISControls::CreateHorizontalLine(ScrollAreaComments);
			LayoutComments->insertWidget(LayoutComments->count() - 1, HorizontalLine);

			Vector.append(TaskCommentWidget);
			Vector.append(HorizontalLine);
		}
	}

	Change();
}
//-----------------------------------------------------------------------------
