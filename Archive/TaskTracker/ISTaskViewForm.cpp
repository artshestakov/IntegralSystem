#include "ISTaskViewForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
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
#include "ISUserRoleEntity.h"
//-----------------------------------------------------------------------------
static QString QS_TASK = PREPARE_QUERY("SELECT "
									   "t.task_name, "
									   "t.task_description, "
									   "t.task_deadline, "
									   "ue.usrs_fio AS task_executor_name, "
									   "userphotobyid(ue.usrs_id) AS task_executor_photo, "
									   "t.task_executor AS task_executor_id, "
									   "tt.tstp_name AS task_type, "
									   "ts.tsst_uid AS task_status_uid, "
									   "ts.tsst_name AS task_status_name, "
									   "ts.tsst_stylesheet AS task_status_stylesheet, "
									   "tp.tspr_uid AS task_priority_uid, "
									   "tp.tspr_name AS task_priority_name, "
									   //"userphotobyid(uo.usrs_id) AS task_owner_photo, "
									   //"uo.usrs_fio AS task_owner_name, "
									   //"t.task_creationuser AS task_owner_id, "
									   "t.task_important, "
									   "t.task_parent AS task_parent_id, "
									   "p.task_name AS task_parent_name "
									   "FROM _task t "
									   "LEFT JOIN _users ue ON ue.usrs_id = t.task_executor "
									   //"LEFT JOIN _users uo ON uo.usrs_id = t.task_creationuser "
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
										  "ORDER BY task_id");
//-----------------------------------------------------------------------------
static QString QS_STATUSES = PREPARE_QUERY("SELECT tsst_uid, tsst_buttontext, tsst_stylesheet, tsst_icon "
										   "FROM _taskstatus "
										   "ORDER BY tsst_order");
//-----------------------------------------------------------------------------
static QString QU_CONVERT_TO_TASK = PREPARE_QUERY("UPDATE _task SET "
												  "task_parent = NULL "
												  "WHERE task_id = :TaskID");
//-----------------------------------------------------------------------------
static QString QS_SUBTASK_COUNT = PREPARE_QUERY("SELECT COUNT(*) "
												"FROM _task "
												"WHERE task_parent = :TaskID");
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
static QString QS_FILE = PREPARE_QUERY("SELECT tfls_id, tfls_datetime, tfls_isimage, tfls_name, tfls_extension, tfls_size, tfls_icon, usrs_fio "
									   "FROM _taskfile "
									   "LEFT JOIN _users u ON u.usrs_id = tfls_user "
									   "WHERE tfls_task = :TaskID "
									   "ORDER BY tfls_id");
//-----------------------------------------------------------------------------
static QString QI_FILE = PREPARE_QUERY("INSERT INTO _taskfile(tfls_user, tfls_task, tfls_isimage, tfls_name, tfls_extension, tfls_data, tfls_size, tfls_icon) "
									   "VALUES(:UserID, :TaskID, :IsImage, :Name, :Extension, :Data, :Size, :Icon)");
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
									   "usrs_fio, "
									   "tlnk_datetime, "
									   "tsst_uid AS task_status_uid, "
									   "tsst_name AS task_status_name "
									   "FROM _tasklink "
									   "LEFT JOIN _task ON tlnk_link = task_id "
									   "LEFT JOIN _users ON usrs_id = tlnk_user "
									   "LEFT JOIN _taskstatus ON task_status = tsst_id "
									   "WHERE tlnk_task = :TaskID "
									   "ORDER BY tlnk_id");
//-----------------------------------------------------------------------------
static QString QI_LINK = PREPARE_QUERY("INSERT INTO _tasklink(tlnk_user, tlnk_task, tlnk_link) "
									   "VALUES(:UserID, :CurrentTaskID, :LinkTaskID)");
//-----------------------------------------------------------------------------
static QString QD_LINK = PREPARE_QUERY("DELETE FROM _tasklink "
									   "WHERE tlnk_id = :LinkID");
//-----------------------------------------------------------------------------
static QString QS_COMMENT = PREPARE_QUERY("SELECT "
										  "tcom_id, "
										  "tcom_parent, "
										  "tcom_comment, "
										  "tcom_datetime, "
										  "tcom_owner, "
										  "usrs_fio, "
										  "userphotobyid(tcom_owner) "
										  "FROM _taskcomment "
										  "LEFT JOIN _users ON usrs_id = tcom_owner "
										  "WHERE tcom_task = :TaskID "
										  "ORDER BY tcom_id");
//-----------------------------------------------------------------------------
static QString QU_COMMENT = PREPARE_QUERY("UPDATE _taskcomment SET "
										  "tcom_comment = :Comment "
										  "WHERE tcom_id = :CommentID");
//-----------------------------------------------------------------------------
static QString QD_COMMENT = PREPARE_QUERY("DELETE FROM _taskcomment WHERE tcom_id = :CommentID");
//-----------------------------------------------------------------------------
static QString QS_CHECK_LIST = PREPARE_QUERY("SELECT tchl_id, tchl_name, tchl_done "
											 "FROM _taskchecklist "
											 "WHERE tchl_task = :TaskID "
											 "ORDER BY tchl_id");
//-----------------------------------------------------------------------------
static QString QI_CHECK_LIST = PREPARE_QUERY("INSERT INTO _taskchecklist(tchl_task, tchl_name) "
											 "VALUES(:TaskID, :Name)");
//-----------------------------------------------------------------------------
static QString QU_CHECK_LIST = PREPARE_QUERY("UPDATE _taskchecklist SET "
											 "tchl_name = :Name "
											 "WHERE tchl_id = :CheckListID");
//-----------------------------------------------------------------------------
static QString QU_CHECK_LIST_DONE = PREPARE_QUERY("UPDATE _taskchecklist SET "
												  "tchl_done = :Done "
												  "WHERE tchl_id = :CheckListID");
//-----------------------------------------------------------------------------
static QString QD_CHECK_LIST = PREPARE_QUERY("DELETE FROM _taskchecklist WHERE tchl_id = :CheckListID");
//-----------------------------------------------------------------------------
ISTaskViewForm::ISTaskViewForm(int task_id, QWidget *parent)
	: ISInterfaceForm(parent),
	ButtonGroupStatus(new QButtonGroup(this)),
	TaskID(task_id),
	TaskParentID(0)
{
	setWindowIcon(BUFFER_ICONS("Task"));
	resize(800, 600);
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	//����������� ���������� �� ������
	ISQuery qSelect(QS_TASK);
	qSelect.BindValue(":TaskID", TaskID);
	if (!qSelect.ExecuteFirst()) //������ �������
	{
		ISMessageBox::ShowCritical(nullptr, LANG("Message.Error.SelectTask").arg(TaskID), qSelect.GetErrorString());
		return;
	}

	TaskName = qSelect.ReadColumn("task_name").toString();
	TaskDescription = qSelect.ReadColumn("task_description").toString();
	TaskDeadline = qSelect.ReadColumn("task_deadline").toDate();
	TaskExecutorName = qSelect.ReadColumn("task_executor_name").toString();
	TaskExecutroPhoto = ISGui::ByteArrayToPixmap(qSelect.ReadColumn("task_executor_photo").toByteArray()).scaled(40, 40);
	TaskExecutorID = qSelect.ReadColumn("task_executor_id").toUInt();
	TaskType = qSelect.ReadColumn("task_type").toString();
	TaskStatusUID = qSelect.ReadColumn("task_status_uid");
	TaskStatusName = qSelect.ReadColumn("task_status_name").toString();
	TaskStatusStyleSheet = qSelect.ReadColumn("task_status_stylesheet").toString();
	TaskPriorityUID = qSelect.ReadColumn("task_priority_uid");
	TaskPriorityName = qSelect.ReadColumn("task_priority_name").toString();
	//TaskOwnerPhoto = ISGui::ByteArrayToPixmap(qSelect.ReadColumn("task_owner_photo").toByteArray()).scaled(40, 40);
	//TaskOwner = qSelect.ReadColumn("task_owner_name").toString();
	//TaskOwnerID = qSelect.ReadColumn("task_owner_id").toUInt();
	TaskImportant = qSelect.ReadColumn("task_important").toBool();
	TaskParentID = qSelect.ReadColumn("task_parent_id").toInt();
	TaskParentName = qSelect.ReadColumn("task_parent_name").toString();

	setWindowTitle(TaskParentID ? LANG("Task.ViewFormTitle.SubTask").arg(TaskParentID).arg(TaskName) : LANG("Task.ViewFormTitle.Task").arg(TaskID).arg(TaskName));

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutTitle);

	LabelStatus = new QLabel(TaskStatusName, this);
	LabelStatus->setFont(ISDefines::Gui::FONT_TAHOMA_12_BOLD);
	LabelStatus->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_5_PX);
	LabelStatus->setAlignment(Qt::AlignCenter);
	LabelStatus->setStyleSheet(BUFFER_STYLE_SHEET(TaskStatusStyleSheet));
	LayoutTitle->addWidget(LabelStatus);

	LayoutTitle->addWidget(ISControls::CreateVerticalLine(this));

	if (TaskParentID)
	{
		ISQLabel *LabelParent = new ISQLabel(QString("#%1").arg(TaskParentID), this);
		LabelParent->setToolTip(LANG("Task.SubTask.ToolTip").arg(TaskParentName));
		LabelParent->SetIsLinked(true);
		LabelParent->setSizePolicy(QSizePolicy::Maximum, LabelParent->sizePolicy().verticalPolicy());
		LabelParent->setFont(ISDefines::Gui::FONT_TAHOMA_12_BOLD);
		connect(LabelParent, &ISQLabel::Clicked, this, &ISTaskViewForm::SubTaskOpenParent);
		LayoutTitle->addWidget(LabelParent, 0, Qt::AlignTop);
	}

	LabelName = new ISLabelElided((TaskParentID ? " \\ " : QString()) + QString("#%1: %2").arg(TaskID).arg(TaskName), this);
	LabelName->SetElidedToolTip(true);
	LabelName->setFont(ISDefines::Gui::FONT_TAHOMA_12_BOLD);
	LabelName->setSizePolicy(LabelName->sizePolicy().horizontalPolicy(), QSizePolicy::Minimum);
	LayoutTitle->addWidget(LabelName);

	if (TaskImportant) //������ �������� ������ - ��������� ��������������� �������
	{
		ISLabelPixmapText *LabelImportant = new ISLabelPixmapText(BUFFER_ICONS("Task.Important.Checked").pixmap(ISDefines::Gui::SIZE_22_22), LANG("Task.ThisIsImportantTask"), this);
		LabelImportant->setSizePolicy(QSizePolicy::Maximum, LabelImportant->sizePolicy().verticalPolicy());
		LayoutTitle->addWidget(LabelImportant);
	}

	LayoutHorizontal = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutHorizontal);

	LayoutLeft = new QVBoxLayout();
	LayoutHorizontal->addLayout(LayoutLeft);

	LayoutButtonStatus = new QHBoxLayout();
	LayoutLeft->addLayout(LayoutButtonStatus);
	ReloadStatusButtons();

	ButtonReopen = new ISPushButton(BUFFER_ICONS("Task.Reopen"), LANG("Task.ReopenStatus"), this);
	connect(ButtonReopen, &ISPushButton::clicked, this, &ISTaskViewForm::ReopenStatus);
	LayoutButtonStatus->addWidget(ButtonReopen);
	UpdateVisibleButtonReOpen();

	ButtonActions = new ISPushButton(BUFFER_ICONS("ArrowDown"), LANG("Task.Actions"), this);
	ButtonActions->setCursor(CURSOR_POINTING_HAND);
	ButtonActions->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	ButtonActions->setMenu(new QMenu(ButtonActions));
	ButtonActions->menu()->addAction(BUFFER_ICONS("Update"), LANG("Task.ReopenTaskViewForm"), this, &ISTaskViewForm::Reopen, QKeySequence(Qt::Key_F5));
	ButtonActions->menu()->addAction(BUFFER_ICONS("Edit"), LANG("Task.EditTask"), this, &ISTaskViewForm::Edit, QKeySequence(Qt::Key_F2));
	ButtonActions->menu()->addSeparator();
	ButtonActions->menu()->addAction(BUFFER_ICONS("Copy"), TaskParentID ? LANG("Task.CloneSubTask") : LANG("Task.CloneTask"), this, &ISTaskViewForm::CloneTask);
	ButtonActions->menu()->addAction(LANG("Task.Process.History"), this, &ISTaskViewForm::ShowStatusHistory);
	ButtonActions->menu()->addSeparator();
	if (TaskParentID)
	{
		ButtonActions->menu()->addAction(LANG("Task.ConvertToTask"), this, &ISTaskViewForm::ConvertThisToTask);
	}
	else
	{
		ButtonActions->menu()->addAction(LANG("Task.ConvertToSubTask"), this, &ISTaskViewForm::ConvertToSubTask);
		ButtonActions->menu()->addSeparator();
	}
	LayoutButtonStatus->addWidget(ButtonActions);

	LayoutButtonStatus->addStretch();

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

	TabWidget = new QTabWidget(this);
	TabWidget->setTabsClosable(true);
	TabWidget->setStyleSheet(BUFFER_STYLE_SHEET("QTabWidgetTask"));
	TabWidget->tabBar()->setStyleSheet(BUFFER_STYLE_SHEET("QTabBarTask"));
	LayoutLeft->addWidget(TabWidget);

	TreeWidgetComment = new QTreeWidget(TabWidget);
	TreeWidgetComment->setHeaderHidden(true);
	TreeWidgetComment->setAlternatingRowColors(true);
	TreeWidgetComment->setAnimated(true);
	TreeWidgetComment->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	TreeWidgetComment->setFrameShape(QFrame::NoFrame);
	TabWidget->addTab(TreeWidgetComment, BUFFER_ICONS("Document"), LANG("Task.Comments").arg(0));
	CommentLoadList();

	ListWidgetSubTask = new ISListWidget(TabWidget);
	ListWidgetSubTask->setAlternatingRowColors(true);
	ListWidgetSubTask->setFrameShape(QFrame::NoFrame);
	ListWidgetSubTask->setContextMenuPolicy(Qt::ActionsContextMenu);
	TabWidget->addTab(ListWidgetSubTask, BUFFER_ICONS("Document"), LANG("Task.SubTask.List"));
	connect(ListWidgetSubTask, &ISListWidget::itemDoubleClicked, this, &ISTaskViewForm::SubTaskOpen);
	SubTaskLoadList();

	QAction *ActionConvertToTask = new QAction(LANG("Task.ConvertToTask"), ListWidgetSubTask);
	ActionConvertToTask->setEnabled(false);
	connect(ActionConvertToTask, &QAction::triggered, this, &ISTaskViewForm::ConvertListSubTaskToTask);
	ListWidgetSubTask->AddAction(ActionConvertToTask, true);

	LayoutCheckList = new QVBoxLayout();
	LayoutCheckList->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	LayoutCheckList->setSpacing(0);
	LayoutCheckList->addStretch();

	ScrollCheckList = new ISScrollArea(TabWidget);
	ScrollCheckList->widget()->setLayout(LayoutCheckList);
	TabWidget->addTab(ScrollCheckList, BUFFER_ICONS("Document"), LANG("Task.CheckList").arg(0));
	CheckLoadList();

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
	ListWidgetLinks->AddAction(ActionLinkDelete, true);

	ListWidgetFiles = new ISListWidget(TabWidget);
	ListWidgetFiles->setLayout(new QVBoxLayout());
	ListWidgetFiles->setFrameShape(QFrame::NoFrame);
	TabWidget->addTab(ListWidgetFiles, BUFFER_ICONS("Document"), LANG("Task.LinkTask").arg(0));
	FileLoadList();

	QToolButton *ButtonAddComment = CreateAddButton(LANG("Task.AddComment"));
	connect(ButtonAddComment, &QToolButton::clicked, this, &ISTaskViewForm::CommentAdd);
	TabWidget->tabBar()->setTabButton(TabWidget->indexOf(TreeWidgetComment), QTabBar::RightSide, ButtonAddComment);

	QToolButton *ButtonAddSubTask = CreateAddButton(LANG("Task.CreateSubTask"));
	connect(ButtonAddSubTask, &QToolButton::clicked, this, &ISTaskViewForm::SubTaskCreate);
	TabWidget->tabBar()->setTabButton(TabWidget->indexOf(ListWidgetSubTask), QTabBar::RightSide, ButtonAddSubTask);

	QToolButton *ButtonAddCheck = CreateAddButton(LANG("Task.AddCheck"));
	connect(ButtonAddCheck, &QToolButton::clicked, this, &ISTaskViewForm::CheckAdd);
	TabWidget->tabBar()->setTabButton(TabWidget->indexOf(ScrollCheckList), QTabBar::RightSide, ButtonAddCheck);

	QToolButton *ButtonAddLink = CreateAddButton(LANG("Task.AddLink"));
	connect(ButtonAddLink, &QToolButton::clicked, this, &ISTaskViewForm::LinkAdd);
	TabWidget->tabBar()->setTabButton(TabWidget->indexOf(ListWidgetLinks), QTabBar::RightSide, ButtonAddLink);

	QToolButton *ButtonAddFile = CreateAddButton(LANG("Task.AddFile"));
	connect(ButtonAddFile, &QToolButton::clicked, this, &ISTaskViewForm::FileAdd);
	TabWidget->tabBar()->setTabButton(TabWidget->indexOf(ListWidgetFiles), QTabBar::RightSide, ButtonAddFile);

	QGroupBox *GroupBoxDetails = new QGroupBox(LANG("Task.Details"), this);
	GroupBoxDetails->setSizePolicy(QSizePolicy::Maximum, GroupBoxDetails->sizePolicy().verticalPolicy());
	GroupBoxDetails->setLayout(new QVBoxLayout());
	LayoutHorizontal->addWidget(GroupBoxDetails);

	LayoutRight = new QVBoxLayout();
	LayoutRight->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	ISScrollArea *ScrollRight = new ISScrollArea(GroupBoxDetails);
	ScrollRight->widget()->setLayout(LayoutRight);
	GroupBoxDetails->layout()->addWidget(ScrollRight);

	LayoutRight->addWidget(new QLabel(LANG("Task.Right.Deadline") + ':', GroupBoxDetails));
	
	//���� ���� ������ �������, ������ ��� ��� ������ ���������� - ������ ������-������� � �������
	QDate CurrentDate = QDate::currentDate();
	if (TaskDeadline == CurrentDate || TaskDeadline == CurrentDate.addDays(1) || TaskDeadline < CurrentDate && !TaskDeadline.isNull())
	{
		ISLabelPixmapText *LabelDeadline = new ISLabelPixmapText(GroupBoxDetails);
		LabelDeadline->SetFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
		if (TaskDeadline == CurrentDate)
		{
			LabelDeadline->SetPixmapText(BUFFER_ICONS("Task.Filter.Today").pixmap(40, 40), LANG("Today"));
		}
		else if (TaskDeadline == CurrentDate.addDays(1))
		{
			LabelDeadline->SetPixmapText(BUFFER_ICONS("Task.Filter.Tomorrow").pixmap(40, 40), LANG("Tomorrow"));
		}
		else if(TaskDeadline < CurrentDate)
		{
			LabelDeadline->SetPixmapText(BUFFER_ICONS("Task.Filter.Expired").pixmap(40, 40), LANG("Task.Filter.Expired"));
			LabelDeadline->setToolTip(ISGui::ConvertDateToString(TaskDeadline, FORMAT_DATE_V4));
			LabelDeadline->setCursor(CURSOR_WHATS_THIS);
		}
		else
		{
			LabelDeadline->SetText(TaskDeadline.isNull() ? LANG("Task.Right.Deadline.Empty") : ISGui::ConvertDateToString(TaskDeadline, FORMAT_DATE_V4));
		}
		LayoutRight->addWidget(LabelDeadline);
	}
	else //������ ������� ������-�������
	{
		QLabel *LabelDeadline = new QLabel(TaskDeadline.isNull() ? LANG("Task.Right.Deadline.Empty") : ISGui::ConvertDateToString(TaskDeadline, FORMAT_DATE_V4), GroupBoxDetails);
		ISGui::SetFontWidgetBold(LabelDeadline, true);
		LayoutRight->addWidget(LabelDeadline);
	}

	LayoutRight->addWidget(ISControls::CreateHorizontalLine(GroupBoxDetails));

	LayoutRight->addWidget(new QLabel(LANG("Task.Right.Executor") + ':', GroupBoxDetails));

	if (TaskExecutorName.isEmpty())
	{
		QLabel *LabelExecutor = new QLabel(LANG("Task.Right.Executor.Empty"));
		ISGui::SetFontWidgetBold(LabelExecutor, true);
		LayoutRight->addWidget(LabelExecutor);
	}
	else
	{
		ISLabelPixmapText *LabelExecutor = new ISLabelPixmapText(TaskExecutroPhoto, TaskExecutorID == CURRENT_USER_ID ? LANG("You") : TaskExecutorName, GroupBoxDetails);
		LabelExecutor->SetWordWrap(true);
		LabelExecutor->SetFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
		LayoutRight->addWidget(LabelExecutor);
	}

	LayoutRight->addWidget(ISControls::CreateHorizontalLine(GroupBoxDetails));

	LayoutRight->addWidget(new QLabel(LANG("Task.Right.Owner") + ':', GroupBoxDetails));

	ISLabelPixmapText *LabelOwner = new ISLabelPixmapText(/*TaskOwnerPhoto*/QPixmap(), TaskOwnerID == CURRENT_USER_ID ? LANG("You") : TaskOwner, GroupBoxDetails);
	LabelOwner->SetWordWrap(true);
	LabelOwner->SetFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	LayoutRight->addWidget(LabelOwner);

	LayoutRight->addWidget(ISControls::CreateHorizontalLine(GroupBoxDetails));

	LayoutRight->addWidget(new QLabel(LANG("Task.Right.Type") + ':', GroupBoxDetails));

	QLabel *LabelType = new QLabel(TaskType.isEmpty() ? LANG("Task.Right.Type.Empty") : TaskType, GroupBoxDetails);
	LabelType->setWordWrap(true);
	ISGui::SetFontWidgetBold(LabelType, true);
	LayoutRight->addWidget(LabelType);

	LayoutRight->addWidget(ISControls::CreateHorizontalLine(GroupBoxDetails));

	LayoutRight->addWidget(new QLabel(LANG("Task.Right.Priority") + ':', GroupBoxDetails));

	ISLabelPixmapText *LabelPriority = new ISLabelPixmapText(TaskPriorityName, GroupBoxDetails);
	LabelPriority->SetDirection(QBoxLayout::RightToLeft);
	LabelPriority->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	LabelPriority->SetFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	LayoutRight->addWidget(LabelPriority);

	//������������� ������ � ����������� �� ����������
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
	else if (TaskPriorityUID == CONST_UID_TASK_PRIORITY_CRITICAL)
	{
		LabelPriority->SetPixmap(BUFFER_ICONS("Task.Priority.Critical").pixmap(ISDefines::Gui::SIZE_25_25));
	}

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
		ButtonActions->animateClick();
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::EscapeClicked()
{
	close();
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::PasteClicked()
{
	QString ImageName;
	QPixmap Pixmap = QApplication::clipboard()->pixmap();
	if (Pixmap.isNull()) //����������� � ������ ����������� - ��������� �� ������� ���� � ����������� (����� � ���������� �������� ����)
	{
		QList<QUrl> Urls = QApplication::clipboard()->mimeData()->urls();
		if (Urls.isEmpty() || Urls.size() != 1) //���� ���� ����������� ��� � ������ �� ���� ���� - ����������
		{
			return;
		}
		
		QString FilePath = Urls.front().toLocalFile();
		QByteArray ImageFormat = QImageReader::imageFormat(FilePath);
		if (ImageFormat.isEmpty()) //���� �� �������� ������������ - �������
		{
			return;
		}

		QFile FileImage(FilePath);
		if (FileImage.open(QIODevice::ReadOnly)) //���� ������� ��������
		{
			QByteArray FileData = FileImage.readAll();
			FileImage.close();
			if (Pixmap.loadFromData(FileData, ImageFormat)) //�� ������� ��������� ������ ����� � �����������
			{
				ImageName = QFileInfo(FileImage).baseName();
			}
			else
			{
				ISMessageBox::ShowCritical(this, LANG("Message.Error.LoadPixmapFromBuffer"));
				return;
			}
		}
		else //������ �������� �����
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.NotOpenedFile").arg(FilePath), FileImage.errorString());
			return;
		}
	}

	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.PasteImageToTask")))
	{
		//���� ��� ����� �� ���� ���������� ����� - ������� ���
		ImageName = ImageName.isEmpty() ? ISInputDialog::GetString(QString(), LANG("EnterImageName") + ':') : ImageName;
		if (ImageName.isEmpty())
		{
			return;
		}
		QByteArray ByteArray = ISGui::PixmapToByteArray(Pixmap);

		ISQuery qInsertImage(QI_FILE);
		qInsertImage.BindValue(":UserID", CURRENT_USER_ID);
		qInsertImage.BindValue(":TaskID", TaskID);
		qInsertImage.BindValue(":IsImage", true);
		qInsertImage.BindValue(":Name", ImageName);
		qInsertImage.BindValue(":Extension", EXTENSION_PNG);
		qInsertImage.BindValue(":Data", ByteArray);
		qInsertImage.BindValue(":Size", ByteArray.size());
		qInsertImage.BindValue(":Icon", ISGui::PixmapToByteArray(Pixmap.scaled(ISDefines::Gui::SIZE_45_45)));
		if (qInsertImage.Execute())
		{
			FileLoadList();
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.PasteImageToTask"), qInsertImage.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::ReloadStatusButtons()
{
	while (!ButtonGroupStatus->buttons().isEmpty())
	{
		delete ButtonGroupStatus->buttons().takeLast();
	}

	//���� ������� ������ "�������" ��� "�� ����� ���������" �� �� ����� ������
	if (TaskStatusUID == CONST_UID_TASK_STATUS_NOT_DONE || TaskStatusUID == CONST_UID_TASK_STATUS_CLOSE)
	{
		return;
	}

	ISQuery qSelectStatuses(QS_STATUSES);
	if (qSelectStatuses.Execute())
	{
		int ButtonIndex = 0;
		while (qSelectStatuses.Next())
		{
			ISUuid StatusUID = qSelectStatuses.ReadColumn("tsst_uid");
			if (StatusUID == TaskStatusUID)
			{
				LabelStatus->setStyleSheet(BUFFER_STYLE_SHEET(qSelectStatuses.ReadColumn("tsst_stylesheet").toString()));
			}
			else
			{
				ISPushButton *ButtonStatus = new ISPushButton(BUFFER_ICONS(qSelectStatuses.ReadColumn("tsst_icon").toString()), qSelectStatuses.ReadColumn("tsst_buttontext").toString(), this);
				ButtonStatus->setProperty("StatusUID", StatusUID);
				connect(ButtonStatus, &ISPushButton::clicked, this, &ISTaskViewForm::TaskStatusClicked);
				LayoutButtonStatus->insertWidget(ButtonIndex++, ButtonStatus);
				ButtonGroupStatus->addButton(ButtonStatus);
			}
		}
	}
	else
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.SelectTaskStatuses"), qSelectStatuses.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::ReopenStatus()
{
	if (ISUserRoleEntity::Instance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_TASK_REOPEN))
	{
		SetStatus(CONST_UID_TASK_STATUS_OPEN);
	}
	else
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Special.TaskReopen"));
	}
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
	if (ISUserRoleEntity::Instance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_TASK_EDIT))
	{
		ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_Edit, "_Task", TaskID);
		connect(ObjectFormBase, &ISObjectFormBase::UpdateList, this, &ISTaskViewForm::Reopen);
		ISGui::ShowObjectForm(ObjectFormBase);
	}
	else
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Special.TaskEdit"));
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::CloneTask()
{
	SetVisibleShadow(true);
	bool Answer = ISMessageBox::ShowQuestion(this, TaskParentID ? LANG("Message.Question.CloneSubTask") : LANG("Message.Question.CloneTask"));
	SetVisibleShadow(false);
	if (Answer)
	{
		ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_Copy, "_Task", TaskID);
		connect(ObjectFormBase, &ISObjectFormBase::SavedObject, [=](int ObjectID) { ISGui::ShowTaskViewForm(ObjectID); });
		ISGui::ShowObjectForm(ObjectFormBase);
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::ConvertThisToTask()
{
	if (ISUserRoleEntity::Instance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_TASK_TRANSFORMATION))
	{
		if (ConvertToTask(TaskID))
		{
			Reopen();
		}
	}
	else
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Special.TaskTransformation"));
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
	SetVisibleShadow(true);
	bool Result = ISMessageBox::ShowQuestion(this, LANG("Message.Question.ConvertToTask"));
	SetVisibleShadow(false);
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
	if (!ISUserRoleEntity::Instance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_TASK_TRANSFORMATION))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Special.TaskTransformation"));
		return;
	}

	//�������� ���������� ��������
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

	SetVisibleShadow(true);
	int SelectedParentID = ISGui::SelectObject("_Task");
	SetVisibleShadow(false);
	if (SelectedParentID)
	{
		//�������� ��������� ������ �� ���� ���������
		int ParentID = ISDatabase::Instance().GetValue("_Task", "Parent", SelectedParentID).toInt();
		if (ParentID)
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.SelectedTaskIsSubTasks"));
			return;
		}

		//�������������� � ���������
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
	ISUuid StatusUID = sender()->property("StatusUID");
	if (StatusUID == CONST_UID_TASK_STATUS_CLOSE && !ISUserRoleEntity::Instance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_TASK_CLOSE))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Special.TaskClose"));
		return;
	}
	SetStatus(StatusUID);
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::SetStatus(const ISUuid &StatusUID)
{
	ISQuery qUpdateStatus(QU_STATUS);
	qUpdateStatus.BindValue(":StatusUID", StatusUID);
	qUpdateStatus.BindValue(":TaskID", TaskID);
	if (qUpdateStatus.ExecuteFirst())
	{
		//��������� ��������� ������� � �������
		ISQuery qInsertStatusHistory(QI_STATUS_HISTORY);
		qInsertStatusHistory.BindValue(":TaskID", TaskID);
		qInsertStatusHistory.BindValue(":StatusID", qUpdateStatus.ReadColumn("task_status").toInt());
		if (qInsertStatusHistory.Execute()) //���� ������� ������������� ��������� �������
		{
			//���������� ����� ������������� ������� � ��� ���
			TaskStatusUID = StatusUID;
			TaskStatusName = qUpdateStatus.ReadColumn("tsst_name").toString();

			//������ �������, � ����� � ��������� ������ ������������ ������
			LabelStatus->setText(TaskStatusName);
			LabelStatus->setStyleSheet(BUFFER_STYLE_SHEET(qUpdateStatus.ReadColumn("tsst_stylesheet").toString()));
			UpdateVisibleButtonReOpen();

			//������������� ������ �� ��������� � �������� ������ ��������� �������
			ReloadStatusButtons();
			emit StatusChanged(TaskStatusUID);
		}
		else
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
void ISTaskViewForm::UpdateVisibleButtonReOpen()
{
	ButtonReopen->setVisible(TaskStatusUID == CONST_UID_TASK_STATUS_CLOSE || TaskStatusUID == CONST_UID_TASK_STATUS_NOT_DONE); //�������� ��������� ������ ������������: ���� ������� ������ "�������" ��� "�� ����� ���������"
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::SubTaskLoadList()
{
	ListWidgetSubTask->Clear();

	ISQuery qSelectSubTask(QS_SUBTASK);
	qSelectSubTask.BindValue(":TaskID", TaskID);
	if (qSelectSubTask.Execute())
	{
		//����� ���������� �������� ����� � ���������� ����������� ��������
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
			ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 30));
			ISGui::SetFontListWidgetItemStrikeOut(ListWidgetItem, IsDone);
			IsDone ? ++IsDoned : IsDoned;
		}
		TabWidget->setTabText(TabWidget->indexOf(ListWidgetSubTask), Rows ? LANG("Task.SubTask.List").arg(IsDoned).arg(Rows) : LANG("Task.SubTask.List.Empty"));
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::SubTaskCreate()
{
	ISObjectFormBase *ObjectFormBase = ISGui::CreateObjectForm(ISNamespace::OFT_New, "_Task");
	ObjectFormBase->AddVirtualField("Parent", TaskID);
	connect(ObjectFormBase, &ISObjectFormBase::SavedObject, this, &ISTaskViewForm::SubTaskLoadList);
	ISGui::ShowObjectForm(ObjectFormBase);
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::SubTaskOpen(QListWidgetItem *ListWidgetItem)
{
	ISTaskViewForm *TaskViewForm = new ISTaskViewForm(ListWidgetItem->data(Qt::UserRole).toInt());
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
		int Rows = qSelectFiles.GetCountResultRows();
		while (qSelectFiles.Next())
		{
			int ID = qSelectFiles.ReadColumn("tfls_id").toInt();
			QDateTime DateTime = qSelectFiles.ReadColumn("tfls_datetime").toDateTime();
			bool IsImage = qSelectFiles.ReadColumn("tfls_isimage").toBool();
			QString Name = qSelectFiles.ReadColumn("tfls_name").toString();
			QString Extension = qSelectFiles.ReadColumn("tfls_extension").toString();
			qint64 Size = qSelectFiles.ReadColumn("tfls_size").toLongLong();
			QByteArray Icon = qSelectFiles.ReadColumn("tfls_icon").toByteArray();
			QString UserFIO = qSelectFiles.ReadColumn("usrs_fio").toString();

			QWidget *Widget = FileCreateWidget(ISGui::ByteArrayToPixmap(Icon).scaled(ISDefines::Gui::SIZE_45_45), IsImage, Name, ID, Extension, Size, UserFIO, DateTime);
			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidgetFiles);
			ListWidgetItem->setSizeHint(Widget->sizeHint());
			ListWidgetFiles->setItemWidget(ListWidgetItem, Widget);
		}
		TabWidget->setTabText(TabWidget->indexOf(ListWidgetFiles), LANG("Task.Files").arg(Rows));
		ListWidgetFiles->SetVisibleNoData(!Rows);
	}
}
//-----------------------------------------------------------------------------
QWidget* ISTaskViewForm::FileCreateWidget(const QPixmap &Pixmap, bool IsImage, const QString &Name, int FileID, const QString &Extension, qint64 Size, const QString &UserFullName, const QDateTime &DateTime)
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

	QLabel *LabelFileName = new QLabel((IsImage ? LANG("Task.File.Name.Image") : LANG("Task.File.Name.File")).arg(Name), Widget);
	LabelFileName->setStyleSheet(BUFFER_STYLE_SHEET("QLabel.Color.Gray"));
	ISGui::SetFontWidgetBold(LabelFileName, true);
	Layout->addWidget(LabelFileName);
	
	Layout->addWidget(new QLabel(LANG("Task.File.Size").arg(ISSystem::FileSizeFromString(Size)), Widget));

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	Layout->addLayout(LayoutBottom);

	if (IsImage)
	{
		ISQLabel *LabelShow = new ISQLabel(LANG("ViewImage"), Widget);
		LabelShow->setProperty("ID", FileID);
		LabelShow->SetIsLinked(true);
		connect(LabelShow, &ISQLabel::Clicked, this, &ISTaskViewForm::FileShow);
		LayoutBottom->addWidget(LabelShow);
	}

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

	QLabel *LabelDateTime = new QLabel(LANG("Task.File.CreationDate").arg(ISGui::ConvertDateTimeToString(DateTime, FORMAT_DATE_V2, FORMAT_TIME_V1)), Widget);
	LabelDateTime->setToolTip(DateTime.toString(FORMAT_DATE_TIME_V10));
	LabelDateTime->setCursor(CURSOR_WHATS_THIS);
	LayoutBottom->addWidget(LabelDateTime);

	LayoutWidget->addStretch();

	return Widget;
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::FileAdd()
{
	SetVisibleShadow(true);
	QStringList FileList = ISFileDialog::GetOpenFileNames(this);
	SetVisibleShadow(false);
	if (!FileList.isEmpty())
	{
		int Inserted = 0;
		ISProgressForm ProgressForm(FileList.size(), QString(), nullptr, "TitleText");
		ProgressForm.show();
		for (int i = 0, c = FileList.size(); i < c; ++i)
		{
			QString FilePath = FileList[i], FileName = QFileInfo(FilePath).fileName(), FileExtension = QFileInfo(FilePath).suffix();
			ProgressForm.IncrementValue(LANG("Task.AddFile.LabelText").arg(FileName));

			QFile File(FileList[i]);
			if (File.open(QIODevice::ReadOnly))
			{
				QByteArray ImageFormat = QImageReader::imageFormat(File.fileName());
				bool IsImage = !ImageFormat.isEmpty();
				QByteArray FileData = File.readAll();
				qint64 FileSize = File.size();
				QByteArray FileIcon = IsImage ? QByteArray() : ISGui::IconToByteArray(ISGui::GetIconFile(FilePath));
				File.close();

				if (IsImage) //���� ���� �������� ������������
				{
					FileIcon = ISGui::PixmapToByteArray(ISGui::ByteArrayToPixmap(FileData).scaled(ISDefines::Gui::SIZE_45_45));
				}

				ISQuery qInsertFile(QI_FILE);
				qInsertFile.BindValue(":UserID", CURRENT_USER_ID);
				qInsertFile.BindValue(":TaskID", TaskID);
				qInsertFile.BindValue(":IsImage", IsImage);
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

			if (ProgressForm.WasCanceled() && ISMessageBox::ShowQuestion(this, LANG("Message.Question.CloseInsetingTaskFile"), LANG("Message.Question.CloseInsetingTaskFile.DetailedText").arg(Inserted).arg(c - Inserted).arg(c)))
			{
				break;
			}
			else
			{
				ProgressForm.SetCanceled(false);
			}
		}

		if (Inserted)
		{
			FileLoadList();
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::FileShow()
{
	ISQuery qSelectImage(QS_FILE_DATA);
	qSelectImage.BindValue(":TaskFileID", sender()->property("ID"));
	if (qSelectImage.ExecuteFirst())
	{
		ISGui::ShowImageForm(qSelectImage.ReadColumn("tfls_data").toByteArray());
	}
	else
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.SelectTaskFileData"), qSelectImage.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::FileSave()
{
	int ID = sender()->property("ID").toInt();
	QString Name = sender()->property("Name").toString();
	QString Extension = sender()->property("Extension").toString();

	SetVisibleShadow(true);
	bool Answer = ISMessageBox::ShowQuestion(this, LANG("Message.Question.TaskFileSave"));
	SetVisibleShadow(false);
	if (Answer)
	{
		SetVisibleShadow(true);
		QString FilePath = ISFileDialog::GetSaveFileName(this, Extension.isEmpty() ? QString() : LANG("File.Filter.File").arg(Extension), Name);
		SetVisibleShadow(false);
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
	SetVisibleShadow(true);
	bool Answer = ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteTaskFile"));
	SetVisibleShadow(false);
	if (Answer)
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
		int Rows = qSelectLink.GetCountResultRows();
		while (qSelectLink.Next())
		{
			int LinkID = qSelectLink.ReadColumn("tlnk_id").toInt();
			int LinkTaskID = qSelectLink.ReadColumn("task_id").toInt();
			QString LinkTaskName = qSelectLink.ReadColumn("task_name").toString();
			QString LinkTaskDescription = qSelectLink.ReadColumn("task_description").toString();
			QString LinkUser = qSelectLink.ReadColumn("usrs_fio").toString();
			QString LinkDateTime = ISGui::ConvertDateTimeToString(qSelectLink.ReadColumn("tlnk_datetime").toDateTime(), FORMAT_DATE_V2, FORMAT_TIME_V1);
			ISUuid TaskLinkStatusUID = qSelectLink.ReadColumn("task_status_uid");
			QString TaskLinkStatusName = qSelectLink.ReadColumn("task_status_name").toString();
			
			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidgetLinks);
			ListWidgetItem->setData(Qt::UserRole, LinkID);
			ListWidgetItem->setData(Qt::UserRole * 2, LinkTaskID);
			ListWidgetItem->setText(QString("#%1: %2").arg(LinkTaskID).arg(LinkTaskName));
			ListWidgetItem->setToolTip(LANG("Task.LinkToolTip").arg(TaskLinkStatusName).arg(LinkTaskDescription.isEmpty() ? LANG("Task.Description.Empty") : LinkTaskDescription).arg(LinkUser).arg(LinkDateTime));
			ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 35));
			ISGui::SetFontListWidgetItemStrikeOut(ListWidgetItem, TaskLinkStatusUID == CONST_UID_TASK_STATUS_DONE || TaskLinkStatusUID == CONST_UID_TASK_STATUS_CLOSE);
		}
		TabWidget->setTabText(TabWidget->indexOf(ListWidgetLinks), LANG("Task.LinkTask").arg(Rows));
		ListWidgetLinks->SetVisibleNoData(!Rows);
	}
	else
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.LoadTaskLink"), qSelectLink.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::LinkAdd()
{
	SetVisibleShadow(true);
	int LinkTaskID = ISGui::SelectObject("_Task");
	SetVisibleShadow(false);
	if (LinkTaskID)
	{
		if (LinkTaskID == TaskID)
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.TaskLinkToByMyself"));
			LinkAdd();
		}
		else
		{
			//���������, ���� �� ��� ����� � ��������� �������
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
			qInsertLink.BindValue(":UserID", CURRENT_USER_ID);
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
	connect(TaskViewForm, &ISTaskViewForm::StatusChanged, this, &ISTaskViewForm::LinkLoadList);
	connect(TaskViewForm, &ISTaskViewForm::StatusChanged, this, &ISTaskViewForm::SubTaskLoadList);
	connect(TaskViewForm, &ISTaskViewForm::ConvertedToTask, this, &ISTaskViewForm::SubTaskLoadList);
	ISGui::ShowTaskViewForm(TaskViewForm);
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::LinkDelete()
{
	SetVisibleShadow(true);
	bool Answer = ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteTaskLink"));
	SetVisibleShadow(false);
	if (Answer)
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
	TreeWidgetComment->clear();
	MapComment.clear();

	ISQuery qSelectComments(QS_COMMENT);
	qSelectComments.BindValue(":TaskID", TaskID);
	if (qSelectComments.Execute())
	{
		while (qSelectComments.Next())
		{
			int CommentID = qSelectComments.ReadColumn("tcom_id").toInt();
			int ParentID = qSelectComments.ReadColumn("tcom_parent").toInt();
			QString Comment = qSelectComments.ReadColumn("tcom_comment").toString();
			QDateTime DateTime = qSelectComments.ReadColumn("tcom_datetime").toDateTime();
			int UserID = qSelectComments.ReadColumn("tcom_owner").toInt();
			QString UserFIO = qSelectComments.ReadColumn("usrs_fio").toString();
			QPixmap UserPhoto = ISGui::ByteArrayToPixmap(qSelectComments.ReadColumn("userphotobyid").toByteArray());

			QWidget *Widget = CommentCreateWidget(ParentID, CommentID, UserID, UserPhoto, UserFIO, Comment, DateTime);
			if (ParentID)
			{
				QTreeWidgetItem *TreeWidgetItem = new QTreeWidgetItem(MapComment[ParentID]);
				TreeWidgetComment->setItemWidget(TreeWidgetItem, 0, Widget);
			}
			else
			{
				QTreeWidgetItem *TreeWidgetItem = new QTreeWidgetItem(TreeWidgetComment);
				TreeWidgetComment->setItemWidget(TreeWidgetItem, 0, Widget);
				MapComment[CommentID] = TreeWidgetItem;
			}
		}
		TabWidget->setTabText(TabWidget->indexOf(TreeWidgetComment), LANG("Task.Comments").arg(qSelectComments.GetCountResultRows()));
	}
	else
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.LoadTaskComments"), qSelectComments.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
QWidget* ISTaskViewForm::CommentCreateWidget(bool IsParent, int CommentID, unsigned int UserID, const QPixmap &UserPhoto, const QString &UserFIO, const QString &Comment, const QDateTime &DateTime)
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

	QLabel *LabelUser = new QLabel(UserFIO, WidgetTitle);
	LabelUser->setStyleSheet(BUFFER_STYLE_SHEET("QLabel.Color.Gray"));
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

	if (!IsParent)
	{
		ISQLabel *LabelAnswer = new ISQLabel(LANG("Task.Comment.Answer"), WidgetBottom);
		LabelAnswer->SetIsLinked(true);
		LabelAnswer->setProperty("ParentID", CommentID);
		connect(LabelAnswer, &ISQLabel::Clicked, this, &ISTaskViewForm::CommentAdd);
		LayoutBottom->addWidget(LabelAnswer);
	}

	if (UserID == CURRENT_USER_ID) //���� ������� ������������ �������� ������� ����������� - ��������� ��� ������������� � ������� �����������
	{
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
	}

	QLabel *LabelDateTime = new QLabel(LANG("Task.Comment.DateTime").arg(ISGui::ConvertDateTimeToString(DateTime, FORMAT_DATE_V1, FORMAT_TIME_V1)), WidgetBottom);
	LabelDateTime->setToolTip(DateTime.toString(FORMAT_DATE_TIME_V10));
	LabelDateTime->setCursor(CURSOR_WHATS_THIS);
	LayoutBottom->addWidget(LabelDateTime);

	LayoutBottom->addStretch();

	return Widget;
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::CommentAdd()
{
	SetVisibleShadow(true);
	QString Comment = ISInputDialog::GetText(LANG("Task.Comment"), LANG("Task.InputComment") + ':');
	SetVisibleShadow(false);
	
	ISTcpQuery qTaskCommentAdd(API_TASK_COMMENT_ADD);
	qTaskCommentAdd.BindValue("TaskID", TaskID);
	qTaskCommentAdd.BindValue("Parent", sender()->property("ParentID"));
	qTaskCommentAdd.BindValue("Comment", Comment);
	if (qTaskCommentAdd.Execute())
	{
		CommentLoadList();
		TreeWidgetComment->scrollToBottom();
	}
	else
	{
		ISMessageBox::ShowCritical(this, qTaskCommentAdd.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::CommentEdit()
{
	QString Comment = sender()->property("Comment").toString();
	SetVisibleShadow(true);
	QString NewComment = ISInputDialog::GetText(LANG("Task.Comment"), LANG("Task.InputComment") + ':', Comment);
	SetVisibleShadow(false);
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
	SetVisibleShadow(true);
	bool Answer = ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteTaskComment"));
	SetVisibleShadow(false);
	if (Answer)
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
void ISTaskViewForm::CheckLoadList()
{
	while (!VectorCheckList.empty())
	{
		delete ISAlgorithm::VectorTakeBack(VectorCheckList);
	}

	ISQuery qSelectCheckList(QS_CHECK_LIST);
	qSelectCheckList.BindValue(":TaskID", TaskID);
	if (qSelectCheckList.Execute())
	{
		int Index = 0, Order = 0;
		while (qSelectCheckList.Next())
		{
			QWidget *Widget = CreateCheckListWidget(
				qSelectCheckList.ReadColumn("tchl_id").toInt(),
				++Order,
				qSelectCheckList.ReadColumn("tchl_name").toString(),
				qSelectCheckList.ReadColumn("tchl_done").toBool());
			LayoutCheckList->insertWidget(Index++, Widget);
			VectorCheckList.emplace_back(Widget);
		}
		TabWidget->setTabText(TabWidget->indexOf(ScrollCheckList), LANG("Task.CheckList").arg(qSelectCheckList.GetCountResultRows()));
	}
	else
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.LoadTaskCheckList"), qSelectCheckList.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
QWidget* ISTaskViewForm::CreateCheckListWidget(int CheckListID, int Order, const QString &Name, bool Done)
{
	QHBoxLayout *LayoutWidget = new QHBoxLayout();

	QWidget *Widget = new QWidget(ScrollCheckList);
	Widget->setLayout(LayoutWidget);

	ISServiceButton *ButtonEditCheck = new ISServiceButton(BUFFER_ICONS("Edit"), LANG("Edit"), Widget);
	ButtonEditCheck->setFlat(true);
	ButtonEditCheck->setProperty("CheckListID", CheckListID);
	ButtonEditCheck->setProperty("CheckListName", Name);
	connect(ButtonEditCheck, &ISServiceButton::clicked, this, &ISTaskViewForm::CheckEdit);
	LayoutWidget->addWidget(ButtonEditCheck);

	ISServiceButton *ButtonDeleteCheck = new ISServiceButton(BUFFER_ICONS("Delete"), LANG("Delete"), Widget);
	ButtonDeleteCheck->setFlat(true);
	ButtonDeleteCheck->setProperty("CheckListID", CheckListID);
	connect(ButtonDeleteCheck, &ISServiceButton::clicked, this, &ISTaskViewForm::CheckDelete);
	LayoutWidget->addWidget(ButtonDeleteCheck);

	ISCheckEdit *CheckEdit = new ISCheckEdit(Widget);
	CheckEdit->SetCheckableStrikeOut(true);
	CheckEdit->SetText(QString("%1. %2").arg(Order).arg(Name));
	CheckEdit->SetValue(Done);
	CheckEdit->setProperty("CheckListID", CheckListID);
	connect(CheckEdit, &ISCheckEdit::ValueChange, this, &ISTaskViewForm::CheckClicked);
	LayoutWidget->addWidget(CheckEdit);

	LayoutWidget->addStretch();
	return Widget;
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::CheckAdd()
{
	SetVisibleShadow(true);
	QString ItemName = ISInputDialog::GetString(LANG("Task.Check"), LANG("Task.Check.InputName"));
	SetVisibleShadow(false);
	if (!ItemName.isEmpty())
	{
		ISQuery qInsertCheck(QI_CHECK_LIST);
		qInsertCheck.BindValue(":TaskID", TaskID);
		qInsertCheck.BindValue(":Name", ItemName);
		if (qInsertCheck.Execute())
		{
			CheckLoadList();
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.InsertTaskCheckList"), qInsertCheck.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::CheckEdit()
{
	QString CurrentName = sender()->property("CheckListName").toString();
	SetVisibleShadow(true);
	QString NewName = ISInputDialog::GetString(LANG("Task.Check"), LANG("Task.Check.InputName"), CurrentName);
	SetVisibleShadow(false);
	if (!NewName.isEmpty() && NewName != CurrentName)
	{
		ISQuery qUpdateCheck(QU_CHECK_LIST);
		qUpdateCheck.BindValue(":Name", NewName);
		qUpdateCheck.BindValue(":CheckListID", sender()->property("CheckListID"));
		if (qUpdateCheck.Execute())
		{
			CheckLoadList();
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.UpdateTaskCheckList"), qUpdateCheck.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::CheckDelete()
{
	SetVisibleShadow(true);
	bool Answer = ISMessageBox::ShowQuestion(this, LANG("Message.Question.TaskCheckListDelete"));
	SetVisibleShadow(false);
	if (Answer)
	{
		ISQuery qDeleteCheck(QD_CHECK_LIST);
		qDeleteCheck.BindValue(":CheckListID", sender()->property("CheckListID"));
		if (qDeleteCheck.Execute())
		{
			CheckLoadList();
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.DeleteTaskCheckList"), qDeleteCheck.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::CheckClicked(const QVariant &Value)
{
	ISQuery qUpdate(QU_CHECK_LIST_DONE);
	qUpdate.BindValue(":Done", Value);
	qUpdate.BindValue(":CheckListID", sender()->property("CheckListID"));
	if (!qUpdate.Execute())
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.UpdateDoneTaskCheckList"), qUpdate.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
