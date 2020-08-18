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
#include "ISAlgorithm.h"
#include "ISFileDialog.h"
#include "ISProgressForm.h"
#include "ISSystem.h"
#include "ISListBaseForm.h"
#include "ISDatabase.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
static QString QS_TASK = PREPARE_QUERY("SELECT "
									   "t.task_name, "
									   "t.task_description, "
									   "userfullname(t.task_executor) AS task_executor, "
									   "tt.tstp_name AS task_type, "
									   "ts.tsst_uid AS task_status_uid, "
									   "ts.tsst_name AS task_status_name, "
									   "tp.tspr_uid AS task_priority_uid, "
									   "tp.tspr_name AS task_priority_name, "
									   "userfullname(t.task_owner) AS task_owner, "
									   "t.task_important, "
									   "t.task_creationdate, "
									   "t.task_updationdate, "
									   "t.task_parent AS task_parent_id, "
									   "p.task_name AS task_parent_name "
									   "FROM _task t "
									   "LEFT JOIN _tasktype tt ON tt.tstp_id = t.task_type "
									   "LEFT JOIN _taskstatus ts ON ts.tsst_id = t.task_status "
									   "LEFT JOIN _taskpriority tp ON tp.tspr_id = t.task_priority "
									   "LEFT JOIN _task p ON p.task_id = t.task_parent "
									   "WHERE t.task_id = :TaskID");
//-----------------------------------------------------------------------------
static QString QS_SUBTASK = PREPARE_QUERY("SELECT task_id, task_name, task_description, tsst_uid AS task_status_uid "
										  "FROM _task "
										  "LEFT JOIN _taskstatus ON tsst_id = task_status "
										  "WHERE task_parent = :TaskID "
										  "AND NOT task_isdeleted "
										  "ORDER BY task_id");
//-----------------------------------------------------------------------------
static QString QS_STATUSES = PREPARE_QUERY("SELECT tsst_uid, tsst_name, tsst_stylesheet "
										   "FROM _taskstatus "
										   "WHERE NOT tsst_isdeleted "
										   "ORDER BY tsst_order");
//-----------------------------------------------------------------------------
static QString QU_NAME = PREPARE_QUERY("UPDATE _task SET "
									   "task_name = :TaskName, "
									   "task_updationdate = now() "
									   "WHERE task_id = :TaskID "
									   "RETURNING task_updationdate");
//-----------------------------------------------------------------------------
static QString QU_DESCRIPTION = PREPARE_QUERY("UPDATE _task SET "
											  "task_description = :TaskDescription, "
											  "task_updationdate = now() "
											  "WHERE task_id = :TaskID "
											  "RETURNING task_updationdate");
//-----------------------------------------------------------------------------
static QString QU_CONVERT_TO_TASK = PREPARE_QUERY("UPDATE _task SET "
												  "task_parent = NULL "
												  "WHERE task_id = :TaskID");
//-----------------------------------------------------------------------------
static QString QS_SUBTASK_COUNT = PREPARE_QUERY("SELECT COUNT(*) FROM _task WHERE task_parent = :TaskID AND NOT task_isdeleted");
//-----------------------------------------------------------------------------
static QString QU_CONVERT_TO_SUBTASK = PREPARE_QUERY("UPDATE _task SET "
													 "task_parent = :TaskParentID "
													 "WHERE task_id = :TaskID");
//-----------------------------------------------------------------------------
static QString QU_STATUS = PREPARE_QUERY("UPDATE _task SET "
										 "task_status = (SELECT tsst_id FROM _taskstatus WHERE tsst_uid = :StatusUID) "
										 "WHERE task_id = :TaskID "
										 "RETURNING "
										 "task_status, "
										 "(SELECT tsst_name FROM _taskstatus WHERE tsst_uid = :StatusUID), "
										 "(SELECT tsst_stylesheet FROM _taskstatus WHERE tsst_uid = :StatusUID)");
//-----------------------------------------------------------------------------
static QString QI_STATUS_HISTORY = PREPARE_QUERY("INSERT INTO _taskstatushistory(tshr_task, tshr_status) "
												 "VALUES(:TaskID, :StatusID)");
//-----------------------------------------------------------------------------
static QString QS_FILE = PREPARE_QUERY("SELECT tfls_id, tfls_creationdate, tfls_name, tfls_extension, tfls_size, tfls_icon, userfullname(tfls_user) "
									   "FROM _taskfile "
									   "WHERE NOT tfls_isdeleted "
									   "AND tfls_task = :TaskID "
									   "ORDER BY tfls_id");
//-----------------------------------------------------------------------------
static QString QI_FILE = PREPARE_QUERY("INSERT INTO _taskfile(tfls_task, tfls_name, tfls_extension, tfls_data, tfls_size, tfls_icon) "
									   "VALUES(:TaskID, :Name, :Extension, :Data, :Size, :Icon)");
//-----------------------------------------------------------------------------
static QString QS_FILE_DATA = PREPARE_QUERY("SELECT tfls_data "
											"FROM _taskfile "
											"WHERE tfls_id = :TaskFileID");
//-----------------------------------------------------------------------------
static QString QD_FILE = PREPARE_QUERY("DELETE FROM _taskfile WHERE tfls_id = :TaskFileID");
//-----------------------------------------------------------------------------
static QString QS_LINK = PREPARE_QUERY("SELECT tlnk_id, "
									   "task_id, "
									   "task_name, "
									   "task_description, "
									   "userfullname(tlnk_user), "
									   "tlnk_creationdate, "
									   "tsst_uid AS task_status_uid, "
									   "tsst_name AS task_status_name, "
									   "tsst_icon AS task_status_icon "
									   "FROM _tasklink "
									   "LEFT JOIN _task ON tlnk_link = task_id "
									   "LEFT JOIN _taskstatus ON task_status = tsst_id "
									   "WHERE NOT tlnk_isdeleted "
									   "AND tlnk_task = :TaskID "
									   "ORDER BY tlnk_id");
//-----------------------------------------------------------------------------
static QString QI_LINK = PREPARE_QUERY("INSERT INTO _tasklink(tlnk_task, tlnk_link) "
									   "VALUES(:CurrentTaskID, :LinkTaskID)");
//-----------------------------------------------------------------------------
static QString QD_LINK = PREPARE_QUERY("DELETE FROM _tasklink "
									   "WHERE tlnk_id = :LinkID");
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
	TaskID(task_id),
	TaskParentID(0)
{
	setWindowIcon(BUFFER_ICONS("Task"));
	resize(800, 600);
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	//Запрашиваем информацию по задаче
	ISQuery qSelect(QS_TASK);
	qSelect.BindValue(":TaskID", TaskID);
	if (!qSelect.ExecuteFirst()) //Ошибка запроса
	{
		ISMessageBox::ShowCritical(nullptr, LANG("Message.Error.SelectTask").arg(TaskID), qSelect.GetErrorString());
		return;
	}

	TaskName = qSelect.ReadColumn("task_name").toString();
	TaskDescription = qSelect.ReadColumn("task_description").toString();
	TaskExecutor = qSelect.ReadColumn("task_executor").toString();
	TaskType = qSelect.ReadColumn("task_type").toString();
	TaskStatusUID = qSelect.ReadColumn("task_status_uid");
	TaskStatusName = qSelect.ReadColumn("task_status_name").toString();
	TaskPriorityUID = qSelect.ReadColumn("task_priority_uid");
	TaskPriorityName = qSelect.ReadColumn("task_priority_name").toString();
	TaskOwner = qSelect.ReadColumn("task_owner").toString();
	TaskImportant = qSelect.ReadColumn("task_important").toBool();
	TaskCreationDate = ISGui::ConvertDateTimeToString(qSelect.ReadColumn("task_creationdate").toDateTime(), FORMAT_DATE_V2, FORMAT_TIME_V1);
	TaskUpdationDate = ISGui::ConvertDateTimeToString(qSelect.ReadColumn("task_updationdate").toDateTime(), FORMAT_DATE_V2, FORMAT_TIME_V1);
	TaskParentID = qSelect.ReadColumn("task_parent_id").toInt();
	TaskParentName = qSelect.ReadColumn("task_parent_name").toString();

	setWindowTitle(LANG(TaskParentID ? "Task.ViewFormTitle.SubTask" : "Task.ViewFormTitle.Task").arg(TaskID).arg(TaskName));

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutTitle);

	ButtonMenu = new ISPushButton(BUFFER_ICONS("Menu"), LANG("Menu"), this);
	ButtonMenu->setFlat(true);
	ButtonMenu->setCursor(CURSOR_POINTING_HAND);
	ButtonMenu->setSizePolicy(QSizePolicy::Maximum, ButtonMenu->sizePolicy().verticalPolicy());
	ButtonMenu->setMenu(new QMenu(ButtonMenu));
	ButtonMenu->menu()->addAction(BUFFER_ICONS("Update"), LANG("Task.ReopenTaskViewForm"), this, &ISTaskViewForm::Reopen, QKeySequence(Qt::Key_F5));
	ButtonMenu->menu()->addAction(BUFFER_ICONS("Edit"), LANG("Task.EditTask"), this, &ISTaskViewForm::Edit, QKeySequence(Qt::Key_F2));
	ButtonMenu->menu()->addSeparator();
	ButtonMenu->menu()->addAction(LANG("Task.Rename"), this, &ISTaskViewForm::Rename);
	ButtonMenu->menu()->addAction(LANG("Task.SetDescription"), this, &ISTaskViewForm::SetDescription);
	ButtonMenu->menu()->addAction(BUFFER_ICONS("Copy"), LANG(TaskParentID ? "Task.CloneSubTask" : "Task.CloneTask"), this, &ISTaskViewForm::CloneTask);
	ButtonMenu->menu()->addSeparator();
	if (TaskParentID)
	{
		ButtonMenu->menu()->addAction(LANG("Task.ConvertToTask"), this, &ISTaskViewForm::ConvertThisToTask);
	}
	else
	{
		ButtonMenu->menu()->addAction(LANG("Task.ConvertToSubTask"), this, &ISTaskViewForm::ConvertToSubTask);
		ButtonMenu->menu()->addSeparator();
		ButtonMenu->menu()->addAction(BUFFER_ICONS("Add"), LANG("Task.CreateSubTask"), this, &ISTaskViewForm::SubTaskCreate);
	}
	LayoutTitle->addWidget(ButtonMenu);

	ButtonProcess = new ISPushButton(BUFFER_ICONS("Task.Process"), this);
	ButtonProcess->setToolTip(LANG("Task.Process.ToolTip"));
	ButtonProcess->setCursor(CURSOR_POINTING_HAND);
	ButtonProcess->setSizePolicy(QSizePolicy::Maximum, ButtonProcess->sizePolicy().verticalPolicy());
	ButtonProcess->setMenu(new QMenu(ButtonProcess));
	LayoutTitle->addWidget(ButtonProcess);

	ISQuery qSelectStatuses(QS_STATUSES);
	if (qSelectStatuses.Execute())
	{
		while (qSelectStatuses.Next())
		{
			ISUuid StatusUID = qSelectStatuses.ReadColumn("tsst_uid");
			QString StatusName = qSelectStatuses.ReadColumn("tsst_name").toString();
			
			QAction *ActionStatus = ButtonProcess->menu()->addAction(StatusName, this, &ISTaskViewForm::TaskStatusClicked);
			ActionStatus->setProperty("StatusUID", StatusUID);
			if (StatusUID == TaskStatusUID)
			{
				ButtonProcess->setText(StatusName);
				ButtonProcess->setStyleSheet(STYLE_SHEET(qSelectStatuses.ReadColumn("tsst_stylesheet").toString()));
				ActionStatus->setIcon(BUFFER_ICONS("Task.Status.Current"));
				ActionStatus->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
			}
		}
	}
	else
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.SelectTaskStatuses"), qSelectStatuses.GetErrorString());
	}

	ButtonProcess->menu()->addSeparator();
	ButtonProcess->menu()->addAction(LANG("Task.Process.History"), this, &ISTaskViewForm::ShowStatusHistory);

	if (TaskParentID)
	{
		ISQLabel *LabelParent = new ISQLabel(QString("#%1").arg(TaskParentID), this);
		LabelParent->setToolTip(LANG("Task.SubTask.ToolTip").arg(TaskParentName));
		LabelParent->SetIsLinked(true);
		LabelParent->setStyleSheet(STYLE_SHEET("QLabel.Color.Gray"));
		LabelParent->setSizePolicy(QSizePolicy::Maximum, LabelParent->sizePolicy().verticalPolicy());
		LabelParent->setFont(ISDefines::Gui::FONT_TAHOMA_12_BOLD);
		connect(LabelParent, &ISQLabel::Clicked, this, &ISTaskViewForm::SubTaskOpenParent);
		LayoutTitle->addWidget(LabelParent);
	}

	LabelName = new ISLabelElided((TaskParentID ? " / " : QString()) + QString("#%1: %2").arg(TaskID).arg(TaskName), this);
	LabelName->SetColorText(ISDefines::Gui::COLOR_DARK_GRAY);
	LabelName->SetElidedToolTip(true);
	LabelName->setFont(ISDefines::Gui::FONT_TAHOMA_12_BOLD);
	LayoutTitle->addWidget(LabelName);

	if (TaskImportant) //Задача является важной - добавляем соответствующую надпись
	{
		ISLabelPixmapText *LabelImportant = new ISLabelPixmapText(BUFFER_ICONS("Task.Important.Checked").pixmap(ISDefines::Gui::SIZE_22_22), LANG("Task.ThisIsImportantTask"), this);
		LabelImportant->setSizePolicy(QSizePolicy::Maximum, LabelImportant->sizePolicy().verticalPolicy());
		ISGui::SetFontWidgetUnderline(LabelImportant->GetLabelText(), true);
		LayoutTitle->addWidget(LabelImportant);
	}

	LayoutHorizontal = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutHorizontal);

	LayoutLeft = new QVBoxLayout();
	LayoutHorizontal->addLayout(LayoutLeft);

	QGroupBox *GroupBoxDescription = new QGroupBox(LANG("Task.Description"), this);
	GroupBoxDescription->setLayout(new QVBoxLayout());
	GroupBoxDescription->layout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	LayoutLeft->addWidget(GroupBoxDescription, 0, Qt::AlignTop);

	TextEdit = new ISTextEdit(GroupBoxDescription);
	TextEdit->SetReadOnly(true);
	TextEdit->SetPlaceholderText(LANG("Task.Description.Empty"));
	TextEdit->SetFrameShape(QFrame::NoFrame);
	TextEdit->SetValue(TaskDescription);
	GroupBoxDescription->layout()->addWidget(TextEdit);

	GroupBoxSubTask = new QGroupBox(LANG("Task.SubTask.List").arg(0), this);
	GroupBoxSubTask->setLayout(new QVBoxLayout());
	GroupBoxSubTask->layout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_1_PX);
	GroupBoxSubTask->setSizePolicy(GroupBoxSubTask->sizePolicy().horizontalPolicy(), QSizePolicy::Maximum);
	LayoutLeft->addWidget(GroupBoxSubTask);

	ProgressBarSubTask = new QProgressBar(GroupBoxSubTask);
	ProgressBarSubTask->setMaximum(100);
	ProgressBarSubTask->setAlignment(Qt::AlignJustify | Qt::AlignVCenter);
	ProgressBarSubTask->setFormat(LANG("Task.SubTask.Progress"));
	GroupBoxSubTask->layout()->addWidget(ProgressBarSubTask);

	ListWidgetSubTask = new ISListWidget(GroupBoxSubTask);
	ListWidgetSubTask->setAlternatingRowColors(true);
	ListWidgetSubTask->setFrameShape(QFrame::NoFrame);
	ListWidgetSubTask->setContextMenuPolicy(Qt::ActionsContextMenu);
	ListWidgetSubTask->SetMaxVisibleItems(6);
	GroupBoxSubTask->layout()->addWidget(ListWidgetSubTask);
	connect(ListWidgetSubTask, &ISListWidget::itemDoubleClicked, this, &ISTaskViewForm::SubTaskOpen);
	SubTaskLoadList();

	QAction *ActionConvertToTask = new QAction(LANG("Task.ConvertToTask"), ListWidgetSubTask);
	ActionConvertToTask->setEnabled(false);
	connect(ActionConvertToTask, &QAction::triggered, this, &ISTaskViewForm::ConvertListSubTaskToTask);
	ListWidgetSubTask->addAction(ActionConvertToTask);

	connect(ListWidgetSubTask, &ISListWidget::itemSelectionChanged, [=]
	{
		if (ListWidgetSubTask->count())
		{
			ActionConvertToTask->setEnabled(ListWidgetSubTask->currentItem());
		}
		else
		{
			ActionConvertToTask->setEnabled(false);
		}
	});

	TabWidget = new QTabWidget(this);
	TabWidget->setTabsClosable(true);
	TabWidget->setStyleSheet(STYLE_SHEET("QTabWidgetTask"));
	TabWidget->tabBar()->setStyleSheet(STYLE_SHEET("QTabBarTask"));
	LayoutLeft->addWidget(TabWidget);

	TreeWidgetComment = new QTreeWidget(TabWidget);
	TreeWidgetComment->setHeaderHidden(true);
	TreeWidgetComment->setRootIsDecorated(false);
	TreeWidgetComment->setAlternatingRowColors(true);
	TreeWidgetComment->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	TreeWidgetComment->setFrameShape(QFrame::NoFrame);
	TabWidget->addTab(TreeWidgetComment, BUFFER_ICONS("Document"), LANG("Task.Comments").arg(0));
	CommentLoadList();

	ListWidgetLinks = new ISListWidget(TabWidget);
	ListWidgetLinks->setAlternatingRowColors(true);
	ListWidgetLinks->setContextMenuPolicy(Qt::ActionsContextMenu);
	ListWidgetLinks->setFrameShape(QFrame::NoFrame);
	connect(ListWidgetLinks, &ISListWidget::itemDoubleClicked, this, &ISTaskViewForm::LinkOpen);
	TabWidget->addTab(ListWidgetLinks, BUFFER_ICONS("Document"), LANG("Task.Files").arg(0));
	LinkLoadList();

	QAction *ActionLinkDelete = new QAction(BUFFER_ICONS("Delete"), LANG("Task.Link.Delete"), ListWidgetLinks);
	ActionLinkDelete->setEnabled(false);
	connect(ActionLinkDelete, &QAction::triggered, this, &ISTaskViewForm::LinkDelete);
	ListWidgetLinks->addAction(ActionLinkDelete);

	connect(ListWidgetLinks, &ISListWidget::itemSelectionChanged, [=]
	{
		if (ListWidgetLinks->count())
		{
			ActionLinkDelete->setEnabled(ListWidgetLinks->currentItem());
		}
		else
		{
			ActionLinkDelete->setEnabled(false);
		}
	});

	ListWidgetFiles = new ISListWidget(TabWidget);
	ListWidgetFiles->setLayout(new QVBoxLayout());
	ListWidgetFiles->setFrameShape(QFrame::NoFrame);
	TabWidget->addTab(ListWidgetFiles, BUFFER_ICONS("Document"), LANG("Task.LinkTask").arg(0));
	FileLoadList();

	QToolButton *ButtonAddComment = CreateAddButton(LANG("Task.AddComment"));
	connect(ButtonAddComment, &QToolButton::clicked, this, &ISTaskViewForm::CommentAdd);
	TabWidget->tabBar()->setTabButton(TabWidget->indexOf(TreeWidgetComment), QTabBar::RightSide, ButtonAddComment);

	QToolButton *ButtonAddLink = CreateAddButton(LANG("Task.AddLink"));
	connect(ButtonAddLink, &QToolButton::clicked, this, &ISTaskViewForm::LinkAdd);
	TabWidget->tabBar()->setTabButton(TabWidget->indexOf(ListWidgetLinks), QTabBar::RightSide, ButtonAddLink);

	QToolButton *ButtonAddFile = CreateAddButton(LANG("Task.AddFile"));
	connect(ButtonAddFile, &QToolButton::clicked, this, &ISTaskViewForm::FileAdd);
	TabWidget->tabBar()->setTabButton(TabWidget->indexOf(ListWidgetFiles), QTabBar::RightSide, ButtonAddFile);

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

	LayoutRight->addWidget(new QLabel(LANG("Task.Right.Status") + ':', GroupBoxDetails));

	LabelStatus = new QLabel(TaskStatusName, GroupBoxDetails);
	ISGui::SetFontWidgetBold(LabelStatus, true);
	LayoutRight->addWidget(LabelStatus);

	LayoutRight->addWidget(ISControls::CreateHorizontalLine(GroupBoxDetails));

	LayoutRight->addWidget(new QLabel(LANG("Task.Right.Priority") + ':', GroupBoxDetails));

	ISLabelPixmapText *LabelPriority = new ISLabelPixmapText(TaskPriorityName, GroupBoxDetails);
	LabelPriority->SetDirection(QBoxLayout::RightToLeft);
	LabelPriority->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	ISGui::SetFontWidgetBold(LabelPriority->GetLabelText(), true);
	LayoutRight->addWidget(LabelPriority);

	//Устанавливаем иконку в зависимости от приоритета
	if (TaskPriorityUID == CONST_UID_TASK_PRIORITY_LOW)
	{
		LabelPriority->SetPixmap(BUFFER_ICONS("Task.Priority.Low").pixmap(ISDefines::Gui::SIZE_25_25));
	}
	else if (TaskPriorityUID == CONST_UID_TASK_PRIORITY_AVERAGE)
	{
		LabelPriority->SetPixmap(BUFFER_ICONS("Task.Priority.Average").pixmap(ISDefines::Gui::SIZE_25_25));
	}
	else if (TaskPriorityUID == CONST_UID_TASK_PRIORITY_HIGH)
	{
		LabelPriority->SetPixmap(BUFFER_ICONS("Task.Priority.Tall").pixmap(ISDefines::Gui::SIZE_25_25));
	}

	LayoutRight->addWidget(ISControls::CreateHorizontalLine(GroupBoxDetails));

	LayoutRight->addWidget(new QLabel(LANG("Task.Right.CreationDate") + ':', GroupBoxDetails));

	QLabel *LabelCreationDate = new QLabel(TaskCreationDate, GroupBoxDetails);
	ISGui::SetFontWidgetBold(LabelCreationDate, true);
	LayoutRight->addWidget(LabelCreationDate);

	LayoutRight->addWidget(ISControls::CreateHorizontalLine(GroupBoxDetails));

	LayoutRight->addWidget(new QLabel(LANG("Task.Right.UpdationDate") + ':', GroupBoxDetails));

	LabelUpdationDate = new QLabel(TaskUpdationDate.isEmpty() ? LANG("Task.Right.UpdationDate.Empty") : TaskUpdationDate, GroupBoxDetails);
	ISGui::SetFontWidgetBold(LabelUpdationDate, true);
	LayoutRight->addWidget(LabelUpdationDate);

	LayoutRight->addWidget(ISControls::CreateHorizontalLine(GroupBoxDetails));

	LayoutRight->addStretch();
}
//-----------------------------------------------------------------------------
ISTaskViewForm::~ISTaskViewForm()
{

}
//-----------------------------------------------------------------------------
void ISTaskViewForm::keyReleaseEvent(QKeyEvent *KeyEvent)
{
	ISInterfaceForm::keyReleaseEvent(KeyEvent);
	if (KeyEvent->key() == Qt::Key_Alt)
	{
		ButtonMenu->animateClick();
	}
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
void ISTaskViewForm::Edit()
{
	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "_Task", TaskID);
	connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISTaskViewForm::Reopen);
	ISGui::ShowObjectForm(ObjectFormBase);
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::Rename()
{
	QString NewName = ISInputDialog::GetString(LANG("Renaming"), LANG("Task.Rename.LabelText"), TaskName);
	if (NewName.isEmpty())
	{
		return;
	}

	if (NewName != TaskName)
	{
		ISQuery qRenameTask(QU_NAME);
		qRenameTask.BindValue(":TaskName", NewName);
		qRenameTask.BindValue(":TaskID", TaskID);
		if (qRenameTask.ExecuteFirst())
		{
			TaskUpdationDate = qRenameTask.ReadColumn("task_updationdate").toDateTime().toString(FORMAT_DATE_TIME_V2);
			TaskName = NewName;

			LabelName->SetText(QString("#%1: %2").arg(TaskID).arg(TaskName));
			LabelUpdationDate->setText(TaskUpdationDate);
			emit Renamed(TaskName);
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.RenameTask"), qRenameTask.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::SetDescription()
{
	QString NewDescription = ISInputDialog::GetText(LANG("Task.SetDescription.Title"), LANG("Task.SetDescription.LabelText"), TaskDescription);
	if (NewDescription != TaskDescription)
	{
		ISQuery qSetDescription(QU_DESCRIPTION);
		qSetDescription.BindValue(":TaskDescription", NewDescription);
		qSetDescription.BindValue(":TaskID", TaskID);
		if (qSetDescription.ExecuteFirst())
		{
			TaskUpdationDate = qSetDescription.ReadColumn("task_updationdate").toDateTime().toString(FORMAT_DATE_TIME_V2);
			TaskDescription = NewDescription;

			TextEdit->SetValue(TaskDescription);
			LabelUpdationDate->setText(TaskUpdationDate);
			emit DescriptionChanged(TaskDescription);
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.SetDescriptionTask"), qSetDescription.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::CloneTask()
{
	if (ISMessageBox::ShowQuestion(this, LANG(TaskParentID ? "Message.Question.CloneSubTask" : "Message.Question.CloneTask")))
	{
		QString Fields;
		PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable("_Task");
		for (PMetaField *MetaField : MetaTable->Fields)
		{
			Fields += MetaTable->Alias + '_' + MetaField->Name + ',';
		}
		Fields.chop(1);

		ISQuery qInsertClone(QString("INSERT INTO _task(%1) SELECT %1 FROM _task WHERE task_id = :TaskID RETURNING task_id").arg(Fields));
		qInsertClone.BindValue(":TaskID", TaskID);
		if (qInsertClone.ExecuteFirst())
		{
			ISGui::ShowTaskViewForm(qInsertClone.ReadColumn("task_id").toInt());
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.CloneTask"), qInsertClone.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::ConvertThisToTask()
{
	if (ConvertToTask(TaskID))
	{
		Reopen();
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::ConvertListSubTaskToTask()
{
	if (ConvertToTask(ListWidgetSubTask->currentItem()->data(Qt::UserRole).toInt()))
	{
		SubTaskLoadList();
	}
}
//-----------------------------------------------------------------------------
bool ISTaskViewForm::ConvertToTask(int task_id)
{
	bool Result = ISMessageBox::ShowQuestion(this, LANG("Message.Question.ConvertToTask"));
	if (Result)
	{
		ISQuery qConvertToTask(QU_CONVERT_TO_TASK);
		qConvertToTask.BindValue(":TaskID", task_id);
		Result = qConvertToTask.Execute();
		if (Result)
		{
			emit ConvertedToTask();
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.ConvertToTask"), qConvertToTask.GetErrorString());
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::ConvertToSubTask()
{
	//Проверка количества подзадач
	ISQuery qSelectSubTask(QS_SUBTASK_COUNT);
	qSelectSubTask.BindValue(":TaskID", TaskID);
	if (qSelectSubTask.ExecuteFirst())
	{
		if (qSelectSubTask.ReadColumn("count").toInt() > 0)
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.CurrentTaskHaveSubTasks"));
			return;
		}
	}
	else
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.SelectSubTaskCount"), qSelectSubTask.GetErrorString());
	}

	int SelectedParentID = ISGui::SelectObject("_Task");
	if (SelectedParentID)
	{
		//Проверка выбранной задачи на факт подзадачи
		int ParentID = ISDatabase::Instance().GetValue("_Task", "Parent", SelectedParentID).toInt();
		if (ParentID)
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.SelectedTaskIsSubTasks"));
			return;
		}

		//Преобразование в подзадачу
		ISQuery qConvertToSubTask(QU_CONVERT_TO_SUBTASK);
		qConvertToSubTask.BindValue(":TaskParentID", SelectedParentID);
		qConvertToSubTask.BindValue(":TaskID", TaskID);
		if (qConvertToSubTask.Execute())
		{
			Reopen();
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.ConvertToSubTask"), qConvertToSubTask.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::TaskStatusClicked()
{
	ISUuid NewTaskStatusUID = sender()->property("StatusUID");
	if (NewTaskStatusUID == TaskStatusUID)
	{
		return;
	}

	ISQuery qUpdateStatus(QU_STATUS);
	qUpdateStatus.BindValue(":StatusUID", NewTaskStatusUID);
	qUpdateStatus.BindValue(":TaskID", TaskID);
	if (qUpdateStatus.ExecuteFirst())
	{
		TaskStatusUID = NewTaskStatusUID;
		TaskStatusName = qUpdateStatus.ReadColumn("tsst_name").toString();

		for (QAction *ActionStatus : ButtonProcess->menu()->actions())
		{
			bool IsCurrent = ActionStatus->property("StatusUID") == TaskStatusUID;
			ActionStatus->setIcon(IsCurrent ? BUFFER_ICONS("Task.Status.Current") : QIcon());
			ActionStatus->setFont(IsCurrent ? ISDefines::Gui::FONT_APPLICATION_BOLD : ISDefines::Gui::FONT_APPLICATION);
		}
		ButtonProcess->setText(TaskStatusName);
		ButtonProcess->setStyleSheet(STYLE_SHEET(qUpdateStatus.ReadColumn("tsst_stylesheet").toString()));
		LabelStatus->setText(TaskStatusName);
		emit StatusChanged(TaskStatusUID);

		ISQuery qInsertStatusHistory(QI_STATUS_HISTORY);
		qInsertStatusHistory.BindValue(":TaskID", TaskID);
		qInsertStatusHistory.BindValue(":StatusID", qUpdateStatus.ReadColumn("task_status").toInt());
		if (!qInsertStatusHistory.Execute())
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.InsertTaskStatusHistory"), qInsertStatusHistory.GetErrorString());
		}
	}
	else
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.UpdateTaskStatus"), qUpdateStatus.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::ShowStatusHistory()
{
	ISListBaseForm *ListBaseForm = new ISListBaseForm("_TaskStatusHistory");
	ListBaseForm->setWindowTitle(LANG("Task.Process.History.Title").arg(TaskName));
	ListBaseForm->resize(800, 600);
	ListBaseForm->show();
	ListBaseForm->LoadData();
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::SubTaskLoadList()
{
	ListWidgetSubTask->Clear();

	ISQuery qSelectSubTask(QS_SUBTASK);
	qSelectSubTask.BindValue(":TaskID", TaskID);
	if (qSelectSubTask.Execute())
	{
		//Общее количество вернутых строк и количество выполненных подзадач
		int Rows = qSelectSubTask.GetCountResultRows(), IsDoned = 0;
		while (qSelectSubTask.Next())
		{
			int SubTaskID = qSelectSubTask.ReadColumn("task_id").toInt();
			QString SubTaskName = qSelectSubTask.ReadColumn("task_name").toString();
			QString SubTaskDescription = qSelectSubTask.ReadColumn("task_description").toString();
			ISUuid SubTaskStatusUID = qSelectSubTask.ReadColumn("task_status_uid");
			bool IsDone = SubTaskStatusUID == CONST_UID_TASK_STATUS_DONE || SubTaskStatusUID == CONST_UID_TASK_STATUS_CLOSE;

			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidgetSubTask);
			ListWidgetItem->setText(QString("#%1: %2").arg(SubTaskID).arg(SubTaskName));
			ListWidgetItem->setToolTip(SubTaskDescription);
			ListWidgetItem->setData(Qt::UserRole, SubTaskID);
			ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 25));
			ISGui::SetFontListWidgetItemStrikeOut(ListWidgetItem, IsDone);
			IsDoned += IsDone ? 1 : 0;
		}
		GroupBoxSubTask->setTitle(LANG("Task.SubTask.List").arg(Rows));
		GroupBoxSubTask->setVisible(Rows);
		ProgressBarSubTask->setValue(IsDoned ? (IsDoned * 100 / Rows) : 0);
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::SubTaskCreate()
{
	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_New, "_Task");
	ObjectFormBase->SetFieldValue("Parent", TaskID);
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISTaskViewForm::SubTaskLoadList);
	ISGui::ShowObjectForm(ObjectFormBase);
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::SubTaskOpen(QListWidgetItem *ListWidgetItem)
{
	ISTaskViewForm *TaskViewForm = new ISTaskViewForm(ListWidgetItem->data(Qt::UserRole).toInt());
	connect(TaskViewForm, &ISTaskViewForm::Renamed, this, &ISTaskViewForm::SubTaskLoadList);
	connect(TaskViewForm, &ISTaskViewForm::Renamed, this, &ISTaskViewForm::LinkLoadList);
	connect(TaskViewForm, &ISTaskViewForm::DescriptionChanged, this, &ISTaskViewForm::SubTaskLoadList);
	connect(TaskViewForm, &ISTaskViewForm::DescriptionChanged, this, &ISTaskViewForm::LinkLoadList);
	connect(TaskViewForm, &ISTaskViewForm::StatusChanged, this, &ISTaskViewForm::SubTaskLoadList);
	connect(TaskViewForm, &ISTaskViewForm::StatusChanged, this, &ISTaskViewForm::LinkLoadList);
	connect(TaskViewForm, &ISTaskViewForm::ConvertedToTask, this, &ISTaskViewForm::SubTaskLoadList);
	ISGui::ShowTaskViewForm(TaskViewForm);
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::SubTaskOpenParent()
{
	ISGui::ShowTaskViewForm(TaskParentID);
}
//-----------------------------------------------------------------------------
QToolButton* ISTaskViewForm::CreateAddButton(const QString &ToolTip)
{
	QToolButton *ToolButton = new QToolButton(TabWidget->tabBar());
	ToolButton->setToolTip(ToolTip);
	ToolButton->setIcon(BUFFER_ICONS("Add"));
	ToolButton->setAutoRaise(true);
	ToolButton->setCursor(CURSOR_POINTING_HAND);
	ToolButton->setFixedSize(ISDefines::Gui::SIZE_25_25);
	return ToolButton;
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::FileLoadList()
{
	ListWidgetFiles->Clear();

	ISQuery qSelectFiles(QS_FILE);
	qSelectFiles.BindValue(":TaskID", TaskID);
	if (qSelectFiles.Execute())
	{
		while (qSelectFiles.Next())
		{
			int ID = qSelectFiles.ReadColumn("tfls_id").toInt();
			QString CreationDate = ISGui::ConvertDateTimeToString(qSelectFiles.ReadColumn("tfls_creationdate").toDateTime(), FORMAT_DATE_V2, FORMAT_TIME_V1);
			QString Name = qSelectFiles.ReadColumn("tfls_name").toString();
			QString Extension = qSelectFiles.ReadColumn("tfls_extension").toString();
			qint64 Size = qSelectFiles.ReadColumn("tfls_size").toLongLong();
			QByteArray Icon = qSelectFiles.ReadColumn("tfls_icon").toByteArray();
			QString UserFullName = qSelectFiles.ReadColumn("userfullname").toString();

			QWidget *Widget = FileCreateWidget(ISGui::ByteArrayToPixmap(Icon).scaled(ISDefines::Gui::SIZE_32_32), Name, ID, Extension, Size, UserFullName, CreationDate);
			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidgetFiles);
			ListWidgetItem->setSizeHint(Widget->sizeHint());
			ListWidgetFiles->setItemWidget(ListWidgetItem, Widget);
		}
		TabWidget->setTabText(TabWidget->indexOf(ListWidgetFiles), LANG("Task.Files").arg(qSelectFiles.GetCountResultRows()));
	}
}
//-----------------------------------------------------------------------------
QWidget* ISTaskViewForm::FileCreateWidget(const QPixmap &Pixmap, const QString &Name, int FileID, const QString &Extension, qint64 Size, const QString &UserFullName, const QString &CreationDate)
{
	QHBoxLayout *LayoutWidget = new QHBoxLayout();
	LayoutWidget->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_4_PX);

	QWidget *Widget = new QWidget(ListWidgetFiles);
	Widget->setLayout(LayoutWidget);

	QLabel *LabelPixmap = new QLabel(Widget);
	LabelPixmap->setPixmap(Pixmap);
	LayoutWidget->addWidget(LabelPixmap);

	QVBoxLayout *Layout = new QVBoxLayout();
	LayoutWidget->addLayout(Layout);

	QLabel *LabelName = new QLabel(Name, Widget);
	LabelName->setStyleSheet(STYLE_SHEET("QLabel.Color.Gray"));
	ISGui::SetFontWidgetBold(LabelName, true);
	Layout->addWidget(LabelName);
	
	Layout->addWidget(new QLabel(LANG("Task.File.Size").arg(ISSystem::FileSizeFromString(Size)), Widget));

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	Layout->addLayout(LayoutBottom);

	ISQLabel *LabelSave = new ISQLabel(LANG("Save"), Widget);
	LabelSave->setProperty("ID", FileID);
	LabelSave->setProperty("Name", Name);
	LabelSave->setProperty("Extension", Extension);
	LabelSave->SetIsLinked(true);
	connect(LabelSave, &ISQLabel::Clicked, this, &ISTaskViewForm::FileSave);
	LayoutBottom->addWidget(LabelSave);

	ISQLabel *LabelDelete = new ISQLabel(LANG("Delete"), Widget);
	LabelDelete->setProperty("ID", FileID);
	LabelDelete->SetIsLinked(true);
	connect(LabelDelete, &ISQLabel::Clicked, this, &ISTaskViewForm::FileDelete);
	LayoutBottom->addWidget(LabelDelete);

	LayoutBottom->addWidget(ISControls::CreateVerticalLine(Widget));
	LayoutBottom->addWidget(new QLabel(LANG("Task.File.UserFullName").arg(UserFullName), Widget));
	LayoutBottom->addWidget(ISControls::CreateVerticalLine(Widget));
	LayoutBottom->addWidget(new QLabel(LANG("Task.File.CreationDate").arg(CreationDate), Widget));

	LayoutWidget->addStretch();

	return Widget;
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::FileAdd()
{
	QStringList FileList = ISFileDialog::GetOpenFileNames(this);
	if (!FileList.isEmpty())
	{
		int Inserted = 0;
		ISProgressForm ProgressForm(FileList.size(), QString(), nullptr, "TitleText");
		ProgressForm.show();
		for (int i = 0; i < FileList.size(); ++i)
		{
			QString FilePath = FileList[i], FileName = QFileInfo(FilePath).fileName(), FileExtension = QFileInfo(FilePath).suffix();
			ProgressForm.IncrementValue(LANG("Task.AddFile.LabelText").arg(FileName));

			QFile File(FileList[i]);
			if (File.open(QIODevice::ReadOnly))
			{
				QByteArray FileData = File.readAll();
				qint64 FileSize = File.size();
				QByteArray FileIcon = ISGui::IconToByteArray(ISGui::GetIconFile(FilePath));
				File.close();

				ISQuery qInsertFile(QI_FILE);
				qInsertFile.BindValue(":TaskID", TaskID);
				qInsertFile.BindValue(":Name", FileName);
				qInsertFile.BindValue(":Extension", FileExtension);
				qInsertFile.BindValue(":Data", FileData);
				qInsertFile.BindValue(":Size", FileSize);
				qInsertFile.BindValue(":Icon", FileIcon);
				if (qInsertFile.Execute())
				{
					++Inserted;
				}
				else
				{
					ISMessageBox::ShowCritical(this, LANG("Message.Error.InsertTaskFile").arg(FilePath), qInsertFile.GetErrorString());
				}
			}
			else
			{
				ISMessageBox::ShowCritical(this, LANG("Message.Error.NotOpenedFile").arg(FilePath), File.errorString());
			}

			if (ProgressForm.WasCanceled())
			{

			}
		}

		if (Inserted)
		{
			FileLoadList();
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::FileSave()
{
	int ID = sender()->property("ID").toInt();
	QString Name = sender()->property("Name").toString();
	QString Extension = sender()->property("Extension").toString();

	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.TaskFileSave")))
	{
		QString FilePath = ISFileDialog::GetSaveFileName(this, Extension.isEmpty() ? QString() : LANG("File.Filter.File").arg(Extension), Name);
		if (!FilePath.isEmpty())
		{
			QFile File(FilePath);
			if (File.open(QIODevice::WriteOnly))
			{
				ISQuery qSelectFileData(QS_FILE_DATA);
				qSelectFileData.BindValue(":TaskFileID", ID);
				if (qSelectFileData.ExecuteFirst())
				{
					File.write(qSelectFileData.ReadColumn("tfls_data").toByteArray());
				}
				else
				{
					ISMessageBox::ShowCritical(this, LANG("Message.Error.SelectTaskFileData"), qSelectFileData.GetErrorString());
				}
				File.close();
			}
			else
			{
				ISMessageBox::ShowWarning(this, LANG("Message.Error.NotOpenedFile"), File.errorString());
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::FileDelete()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteTaskFile")))
	{
		ISQuery qDeleteFile(QD_FILE);
		qDeleteFile.BindValue(":TaskFileID", sender()->property("ID"));
		if (qDeleteFile.Execute())
		{
			FileLoadList();
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.DeleteTaskFile"), qDeleteFile.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::LinkLoadList()
{
	ListWidgetLinks->Clear();

	ISQuery qSelectLink(QS_LINK);
	qSelectLink.BindValue(":TaskID", TaskID);
	if (qSelectLink.Execute())
	{
		while (qSelectLink.Next())
		{
			int LinkID = qSelectLink.ReadColumn("tlnk_id").toInt();
			int LinkTaskID = qSelectLink.ReadColumn("task_id").toInt();
			QString LinkTaskName = qSelectLink.ReadColumn("task_name").toString();
			QString LinkTaskDescription = qSelectLink.ReadColumn("task_description").toString();
			QString LinkUser = qSelectLink.ReadColumn("userfullname").toString();
			QString LinkCreationDate = ISGui::ConvertDateTimeToString(qSelectLink.ReadColumn("tlnk_creationdate").toDateTime(), FORMAT_DATE_V2, FORMAT_TIME_V1);
			ISUuid TaskStatusUID = qSelectLink.ReadColumn("task_status_uid");
			QString TaskStatusName = qSelectLink.ReadColumn("task_status_name").toString();
			QString TaskStatusIcon = qSelectLink.ReadColumn("task_status_icon").toString();
			
			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidgetLinks);
			ListWidgetItem->setData(Qt::UserRole, LinkID);
			ListWidgetItem->setData(Qt::UserRole * 2, LinkTaskID);
			ListWidgetItem->setIcon(BUFFER_ICONS(TaskStatusIcon));
			ListWidgetItem->setText(QString("#%1: %2").arg(LinkTaskID).arg(LinkTaskName));
			ListWidgetItem->setToolTip(LANG("Task.LinkToolTip").arg(TaskStatusName).arg(LinkTaskDescription.isEmpty() ? LANG("Task.Description.Empty") : LinkTaskDescription).arg(LinkUser).arg(LinkCreationDate));
			ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 35));
			ISGui::SetFontListWidgetItemStrikeOut(ListWidgetItem, TaskStatusUID == CONST_UID_TASK_STATUS_DONE || TaskStatusUID == CONST_UID_TASK_STATUS_CLOSE);
		}
		TabWidget->setTabText(TabWidget->indexOf(ListWidgetLinks), LANG("Task.LinkTask").arg(qSelectLink.GetCountResultRows()));
	}
	else
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.LoadTaskLink"), qSelectLink.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::LinkAdd()
{
	int LinkTaskID = ISGui::SelectObject("_Task");
	if (LinkTaskID)
	{
		if (LinkTaskID == TaskID)
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.TaskLinkToByMyself"));
			LinkAdd();
		}
		else
		{
			//Проверяем, есть ли уже связь с выбранной задачей
			for (int i = 0; i < ListWidgetLinks->count(); ++i)
			{
				if (ListWidgetLinks->item(i)->data(Qt::UserRole * 2).toInt() == LinkTaskID)
				{
					ISMessageBox::ShowWarning(this, LANG("Message.Warning.TaskLinkAlreadyExist"));
					LinkAdd();
					return;
				}
			}

			ISQuery qInsertLink(QI_LINK);
			qInsertLink.BindValue(":CurrentTaskID", TaskID);
			qInsertLink.BindValue(":LinkTaskID", LinkTaskID);
			if (qInsertLink.Execute())
			{
				LinkLoadList();
			}
			else
			{
				ISMessageBox::ShowCritical(this, LANG("Message.Error.InsertTaskLink"));
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::LinkOpen(QListWidgetItem *ListWidgetItem)
{
	ISTaskViewForm *TaskViewForm = new ISTaskViewForm(ListWidgetItem->data(Qt::UserRole * 2).toInt());
	connect(TaskViewForm, &ISTaskViewForm::Renamed, this, &ISTaskViewForm::LinkLoadList);
	connect(TaskViewForm, &ISTaskViewForm::Renamed, this, &ISTaskViewForm::SubTaskLoadList);
	connect(TaskViewForm, &ISTaskViewForm::DescriptionChanged, this, &ISTaskViewForm::LinkLoadList);
	connect(TaskViewForm, &ISTaskViewForm::DescriptionChanged, this, &ISTaskViewForm::SubTaskLoadList);
	connect(TaskViewForm, &ISTaskViewForm::StatusChanged, this, &ISTaskViewForm::LinkLoadList);
	connect(TaskViewForm, &ISTaskViewForm::StatusChanged, this, &ISTaskViewForm::SubTaskLoadList);
	connect(TaskViewForm, &ISTaskViewForm::ConvertedToTask, this, &ISTaskViewForm::SubTaskLoadList);
	ISGui::ShowTaskViewForm(TaskViewForm);
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::LinkDelete()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteTaskLink")))
	{
		ISQuery qDeleteLink(QD_LINK);
		qDeleteLink.BindValue(":LinkID", ListWidgetLinks->currentItem()->data(Qt::UserRole));
		if (qDeleteLink.Execute())
		{
			LinkLoadList();
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.DeleteTaskLink"), qDeleteLink.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::CommentLoadList()
{
	ISGui::SetWaitGlobalCursor(true);
	while (TreeWidgetComment->topLevelItemCount())
	{
		QTreeWidgetItem *TreeWidgetItem = TreeWidgetComment->takeTopLevelItem(0);
		delete TreeWidgetComment->itemWidget(TreeWidgetItem, 0);
		delete TreeWidgetItem;
	}

	ISQuery qSelectComments(QS_COMMENT);
	qSelectComments.BindValue(":TaskID", TaskID);
	if (qSelectComments.Execute())
	{
		while (qSelectComments.Next())
		{
			int CommentID = qSelectComments.ReadColumn("tcom_id").toInt();
			QPixmap UserPhoto = ISGui::ByteArrayToPixmap(qSelectComments.ReadColumn("userphoto").toByteArray());
			QString UserFullName = qSelectComments.ReadColumn("userfullname").toString();
			bool IsUserOwner = qSelectComments.ReadColumn("is_user_owner").toBool();
			QString Comment = qSelectComments.ReadColumn("tcom_comment").toString();
			QDateTime CreationDate = qSelectComments.ReadColumn("tcom_creationdate").toDateTime();

			QTreeWidgetItem *TreeItemWidget = new QTreeWidgetItem(TreeWidgetComment);
			TreeWidgetComment->setItemWidget(TreeItemWidget, 0, CommentCreateWidget(CommentID, UserPhoto, IsUserOwner ? LANG("Task.CommentUserOwner").arg(UserFullName) : UserFullName, Comment, CreationDate));
		}
		TabWidget->setTabText(TabWidget->indexOf(TreeWidgetComment), LANG("Task.Comments").arg(qSelectComments.GetCountResultRows()));
	}
	else
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowCritical(this, LANG("Message.Error.LoadTaskComments"), qSelectComments.GetErrorString());
	}
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
QWidget* ISTaskViewForm::CommentCreateWidget(int CommentID, const QPixmap &UserPhoto, const QString &UserFullName, const QString &Comment, const QDateTime &DateTime)
{
	QVBoxLayout *LayoutWidget = new QVBoxLayout();
	LayoutWidget->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_4_PX);

	QWidget *Widget = new QWidget(TreeWidgetComment);
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
	LabelComment->setWordWrap(true);
	LayoutWidget->addWidget(LabelComment);

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	LayoutBottom->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	QWidget *WidgetBottom = new QWidget(Widget);
	WidgetBottom->setLayout(LayoutBottom);
	LayoutWidget->addWidget(WidgetBottom);

	ISQLabel *LabelEdit = new ISQLabel(LANG("Edit"), WidgetBottom);
	LabelEdit->setProperty("Comment", Comment);
	LabelEdit->setProperty("CommentID", CommentID);
	LabelEdit->SetIsLinked(true);
	connect(LabelEdit, &ISQLabel::Clicked, this, &ISTaskViewForm::CommentEdit);
	LayoutBottom->addWidget(LabelEdit);

	ISQLabel *LabelDelete = new ISQLabel(LANG("Delete"), WidgetBottom);
	LabelDelete->setProperty("CommentID", CommentID);
	LabelDelete->SetIsLinked(true);
	connect(LabelDelete, &ISQLabel::Clicked, this, &ISTaskViewForm::CommentDelete);
	LayoutBottom->addWidget(LabelDelete);

	LayoutBottom->addWidget(new QLabel(ISGui::ConvertDateTimeToString(DateTime, FORMAT_DATE_V4, FORMAT_TIME_V3), WidgetBottom));

	LayoutBottom->addStretch();

	return Widget;
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::CommentAdd()
{
	QString Comment = ISInputDialog::GetText(LANG("Task.Comment"), LANG("Task.InputComment"));
	if (!Comment.isEmpty())
	{
		ISQuery qInsertComment(QI_COMMENT);
		qInsertComment.BindValue(":TaskID", TaskID);
		qInsertComment.BindValue(":Comment", Comment);
		if (qInsertComment.Execute())
		{
			CommentLoadList();
			TreeWidgetComment->scrollToItem(TreeWidgetComment->topLevelItem(TreeWidgetComment->topLevelItemCount() - 1), QAbstractItemView::ScrollHint::PositionAtBottom);
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.InsertTaskComment"), qInsertComment.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::CommentEdit()
{
	QString Comment = sender()->property("Comment").toString();
	QString NewComment = ISInputDialog::GetText(LANG("Task.Comment"), LANG("Task.InputComment"), Comment);
	if (!NewComment.isEmpty() && NewComment != Comment)
	{
		ISQuery qUpdateComment(QU_COMMENT);
		qUpdateComment.BindValue(":Comment", NewComment);
		qUpdateComment.BindValue(":CommentID", sender()->property("CommentID"));
		if (qUpdateComment.Execute())
		{
			CommentLoadList();
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.UpdateTaskComment"), qUpdateComment.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::CommentDelete()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteTaskComment")))
	{
		ISQuery qDeleteComment(QD_COMMENT);
		qDeleteComment.BindValue(":CommentID", sender()->property("CommentID"));
		if (qDeleteComment.Execute())
		{
			CommentLoadList();
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.DeleteTaskComment"), qDeleteComment.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
