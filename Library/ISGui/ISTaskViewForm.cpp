#include "ISTaskViewForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
#include "ISGui.h"
#include "ISControls.h"
#include "ISInputDialog.h"
#include "ISLabels.h"
#include "ISAlgorithm.h"
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
static QString QS_COMMENT = PREPARE_QUERY("SELECT "
										  "tcom_id, "
										  "userphoto(tcom_user), "
										  "userfullname(tcom_user), "
										  "(SELECT task_owner = tcom_user AS is_user_owner FROM _task WHERE task_id = tcom_task), "
										  "tcom_comment, "
										  "tcom_creationdate "
										  "FROM _taskcomment "
										  "WHERE tcom_task = :TaskID "
										  "ORDER BY tcom_id");
//-----------------------------------------------------------------------------
static QString QI_COMMENT = PREPARE_QUERY("INSERT INTO _taskcomment(tcom_task, tcom_comment) "
										  "VALUES(:TaskID, :Comment)");
//-----------------------------------------------------------------------------
static QString QU_COMMENT = PREPARE_QUERY("UPDATE _taskcomment SET "
										  "tcom_comment = :Comment "
										  "WHERE tcom_id = :CommentID");
//-----------------------------------------------------------------------------
static QString QD_COMMENT = PREPARE_QUERY("DELETE FROM _taskcomment WHERE tcom_id = :CommentID");
//-----------------------------------------------------------------------------
ISTaskViewForm::ISTaskViewForm(int task_id, QWidget *parent)
	: ISInterfaceForm(parent),
	TaskID(task_id)
{
	setWindowIcon(BUFFER_ICONS("Task"));
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	//����������� ���������� �� ������
	ISQuery qSelect(QS_TASK);
	qSelect.BindValue(":TaskID", TaskID);
	if (!qSelect.ExecuteFirst()) //������ �������
	{
		ISMessageBox::ShowCritical(nullptr, qSelect.GetErrorString());
		return;
	}

	TaskName = qSelect.ReadColumn("task_name").toString();
	TaskDescription = qSelect.ReadColumn("task_description").toString();
	TaskExecutor = qSelect.ReadColumn("task_executor").toString();
	TaskType = qSelect.ReadColumn("task_type").toString();
	TaskStatus = qSelect.ReadColumn("task_status").toString();
	TaskPriority = qSelect.ReadColumn("task_priority").toString();
	TaskOwner = qSelect.ReadColumn("task_owner").toString();
	TaskImportant = qSelect.ReadColumn("task_important").toBool();

	setWindowTitle(LANG("Task.ViewFormTitle").arg(TaskID).arg(TaskName));

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutTitle);

	ISPushButton *ButtonMenu = new ISPushButton(BUFFER_ICONS("Menu"), LANG("Menu"), this);
	ButtonMenu->setFlat(true);
	ButtonMenu->setMenu(new QMenu(ButtonMenu));
	ButtonMenu->menu()->addAction(BUFFER_ICONS("Add"), LANG("Task.AddComment"), this, &ISTaskViewForm::AddComment);
	ButtonMenu->menu()->addSeparator();
	ButtonMenu->menu()->addAction(BUFFER_ICONS("Update"), LANG("Task.ReopenTaskViewForm"), this, &ISTaskViewForm::Reopen);
	LayoutTitle->addWidget(ButtonMenu);

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

	GroupBoxComments = new QGroupBox(LANG("Task.Comments").arg(0), this);
	GroupBoxComments->setLayout(new QVBoxLayout());
	LayoutLeft->addWidget(GroupBoxComments);

	LayoutComments = new QVBoxLayout();
	LayoutComments->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	LayoutComments->addStretch();

	ScrollAreaComments = new ISScrollArea(GroupBoxComments);
	ScrollAreaComments->widget()->setLayout(LayoutComments);
	GroupBoxComments->layout()->addWidget(ScrollAreaComments);
	LoadComments();

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

	LayoutRight->addWidget(new QLabel(LANG("Task.Right.Owner") + ':', GroupBoxDetails));

	QLabel *LabelOwner = new QLabel(TaskOwner.isEmpty() ? LANG("Task.Right.Owner.Empty") : TaskOwner, GroupBoxDetails);
	ISGui::SetFontWidgetBold(LabelOwner, true);
	LayoutRight->addWidget(LabelOwner);

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
void ISTaskViewForm::EscapeClicked()
{
	close();
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::Reopen()
{
	ISGui::ShowTaskViewForm(TaskID);
	close();
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::LoadComments()
{
	while (!VectorComments.empty())
	{
		delete ISAlgorithm::VectorTakeBack(VectorComments);
	}

	ISQuery qSelectComments(QS_COMMENT);
	qSelectComments.BindValue(":TaskID", TaskID);
	if (qSelectComments.Execute())
	{
		int Rows = qSelectComments.GetCountResultRows(), Index = 0;
		while (qSelectComments.Next())
		{
			int CommentID = qSelectComments.ReadColumn("tcom_id").toInt();
			QPixmap UserPhoto = ISGui::ByteArrayToPixmap(qSelectComments.ReadColumn("userphoto").toByteArray());
			QString UserFullName = qSelectComments.ReadColumn("userfullname").toString();
			bool IsUserOwner = qSelectComments.ReadColumn("is_user_owner").toBool();
			QString Comment = qSelectComments.ReadColumn("tcom_comment").toString();
			QDateTime CreationDate = qSelectComments.ReadColumn("tcom_creationdate").toDateTime();

			QWidget *WidgetComment = CreateCommentWidget(CommentID, UserPhoto, IsUserOwner ? LANG("Task.CommentUserOwner").arg(UserFullName) : UserFullName, Comment, CreationDate);
			LayoutComments->insertWidget(LayoutComments->count() - 1, WidgetComment);
			VectorComments.push_back(WidgetComment);

			if (Index != Rows - 1)
			{
				QFrame *FrameSeparator = ISControls::CreateHorizontalLine(ScrollAreaComments);
				LayoutComments->insertWidget(LayoutComments->count() - 1, FrameSeparator);
				VectorComments.push_back(FrameSeparator);
			}
			++Index;
		}
		GroupBoxComments->setTitle(LANG("Task.Comments").arg(Rows));
	}
}
//-----------------------------------------------------------------------------
QWidget* ISTaskViewForm::CreateCommentWidget(int CommentID, const QPixmap &UserPhoto, const QString &UserFullName, const QString &Comment, const QDateTime &DateTime)
{
	QVBoxLayout *LayoutWidget = new QVBoxLayout();
	LayoutWidget->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	QWidget *Widget = new QWidget(ScrollAreaComments);
	Widget->setLayout(LayoutWidget);

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	LayoutTitle->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	QWidget *WidgetTitle = new QWidget(Widget);
	WidgetTitle->setLayout(LayoutTitle);
	LayoutWidget->addWidget(WidgetTitle);

	QLabel *LabelAvatar = new QLabel(WidgetTitle);
	LabelAvatar->setPixmap(UserPhoto.isNull() ? BUFFER_ICONS("User").pixmap(ISDefines::Gui::SIZE_45_45) : UserPhoto.scaled(ISDefines::Gui::SIZE_45_45, Qt::KeepAspectRatio));
	LayoutTitle->addWidget(LabelAvatar);

	QLabel *LabelUser = new QLabel(UserFullName, WidgetTitle);
	LabelUser->setStyleSheet(STYLE_SHEET("QLabel.Color.Gray"));
	ISGui::SetFontWidgetBold(LabelUser, true);
	LayoutTitle->addWidget(LabelUser);

	LayoutTitle->addStretch();

	QLabel *LabelComment = new QLabel(Comment, Widget);
	LabelComment->setTextInteractionFlags(Qt::TextSelectableByMouse);
	LabelComment->setWordWrap(true);
	LayoutWidget->addWidget(LabelComment);

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	LayoutBottom->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	QWidget *WidgetBottom = new QWidget(Widget);
	WidgetBottom->setLayout(LayoutBottom);
	LayoutWidget->addWidget(WidgetBottom);

	ISLabelLink *LabelEdit = new ISLabelLink(LANG("Edit"), WidgetBottom);
	LabelEdit->setProperty("Comment", Comment);
	LabelEdit->setProperty("CommentID", CommentID);
	connect(LabelEdit, &ISLabelLink::Clicked, this, &ISTaskViewForm::EditComment);
	LayoutBottom->addWidget(LabelEdit);

	ISLabelLink *LabelDelete = new ISLabelLink(LANG("Delete"), WidgetBottom);
	LabelDelete->setProperty("CommentID", CommentID);
	connect(LabelDelete, &ISLabelLink::Clicked, this, &ISTaskViewForm::DeleteComment);
	LayoutBottom->addWidget(LabelDelete);

	LayoutBottom->addWidget(new QLabel(DateTime.toString(FORMAT_DATE_TIME_V3), WidgetBottom));

	LayoutBottom->addStretch();

	return Widget;
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::AddComment()
{
	QString Comment = ISInputDialog::GetText(LANG("Task.Comment"), LANG("Task.InputComment"));
	if (!Comment.isEmpty())
	{
		ISQuery qInsertComment(QI_COMMENT);
		qInsertComment.BindValue(":TaskID", TaskID);
		qInsertComment.BindValue(":Comment", Comment);
		if (qInsertComment.Execute())
		{
			LoadComments();
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.InsertTaskComment"), qInsertComment.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::EditComment()
{
	QString Comment = sender()->property("Comment").toString();
	QString NewComment = ISInputDialog::GetText(LANG("Task.Comment"), LANG("Task.InputComment"), Comment);
	if (NewComment != Comment)
	{
		ISQuery qUpdateComment(QU_COMMENT);
		qUpdateComment.BindValue(":Comment", NewComment);
		qUpdateComment.BindValue(":CommentID", sender()->property("CommentID"));
		if (qUpdateComment.Execute())
		{
			LoadComments();
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.UpdateTaskComment"), qUpdateComment.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::DeleteComment()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteTaskComment")))
	{
		ISQuery qDeleteComment(QD_COMMENT);
		qDeleteComment.BindValue(":CommentID", sender()->property("CommentID"));
		if (qDeleteComment.Execute())
		{
			LoadComments();
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.DeleteTaskComment"), qDeleteComment.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
