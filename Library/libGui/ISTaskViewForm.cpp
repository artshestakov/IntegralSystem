#include "StdAfx.h"
#include "ISTaskViewForm.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
#include "ISQuery.h"
#include "ISControls.h"
#include "ISMetaData.h"
#include "ISTaskObjectForm.h"
#include "ISSystem.h"
#include "ISCore.h"
#include "ISGui.h"
#include "ISMessageBox.h"
#include "ISInputDialog.h"
#include "ISTaskFileInsertForm.h"
#include "ISTaskDoneForm.h"
#include "ISIntegralSystem.h"
#include "ISLabelLink.h"
#include "ISNotifySender.h"
#include "ISMetaUser.h"
//-----------------------------------------------------------------------------
static QString QS_TASK = PREPARE_QUERY("SELECT task_name, task_important, task_description, task_executor "
									   "FROM _task "
									   "WHERE task_id = :TaskID");
//-----------------------------------------------------------------------------
static QString QI_COMMENT = PREPARE_QUERY("INSERT INTO _taskcomment(tcom_task, tcom_user, tcom_comment) "
										  "VALUES(:TaskID, currentuserid(), :Comment)");
//-----------------------------------------------------------------------------
static QString QS_TASK_LINKED = PREPARE_QUERY("SELECT task_id, task_name, userfullname(tlnk_userlink) "
											  "FROM _tasklink "
											  "LEFT JOIN _task ON task_id = tlnk_linktask "
											  "WHERE tlnk_task = :TaskID "
											  "ORDER BY tlnk_id");
//-----------------------------------------------------------------------------
static QString QS_TASK_LINK = PREPARE_QUERY("SELECT COUNT(*) "
											"FROM _tasklink "
											"WHERE tlnk_task = :TaskID "
											"AND tlnk_linktask = :LinkTaskID");
//-----------------------------------------------------------------------------
static QString QI_TASK_LINK = PREPARE_QUERY("INSERT INTO _tasklink(tlnk_task, tlnk_linktask) "
											"VALUES(:Task, :LinkTask) "
											"RETURNING "
											"(SELECT task_name AS task FROM _task WHERE task_id = tlnk_task), "
											"(SELECT task_name AS linktask FROM _task WHERE task_id = tlnk_linktask)");
//-----------------------------------------------------------------------------
static QString QU_TASK_NAME = PREPARE_QUERY("UPDATE _task SET "
											"task_name = :Name "
											"WHERE task_id = :TaskID");
//-----------------------------------------------------------------------------
ISTaskViewForm::ISTaskViewForm(int task_id, QWidget *parent) : ISInterfaceForm(parent)
{
	TaskID = task_id;

	SelectTask();
	setWindowTitle(LOCALIZATION("Task.CardTask") + ": " + TaskName);
	setWindowIcon(BUFFER_ICONS("Task"));
	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_10_PX);

	CreateTitle();

	LayoutCenter = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutCenter);

	CreateLeft();
	CreateRight();
}
//-----------------------------------------------------------------------------
ISTaskViewForm::~ISTaskViewForm()
{

}
//-----------------------------------------------------------------------------
void ISTaskViewForm::SelectTask()
{
	ISQuery qSelectTask(QS_TASK);
	qSelectTask.BindValue(":TaskID", TaskID);
	if (qSelectTask.ExecuteFirst())
	{
		TaskName = qSelectTask.ReadColumn("task_name").toString();
		TaskImportant = qSelectTask.ReadColumn("task_important").toBool();
		TaskDescription = qSelectTask.ReadColumn("task_description").toString();
 		TaskExecutorID = qSelectTask.ReadColumn("task_executor").toInt();

		if (!TaskDescription.length())
		{
			TaskDescription = LOCALIZATION("Task.Description.Empty");
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::AfterShowEvent()
{
	ISInterfaceForm::AfterShowEvent();
	UpdateActions();
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::EscapeClicked()
{
	close();
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::CreateTitle()
{
	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutTitle);

	GetMainLayout()->addWidget(ISControls::CreateHorizontalLine(this));

	if (TaskImportant)
	{
		QLabel *LabelImportant = new QLabel(this);
		LabelImportant->setPixmap(BUFFER_ICONS("Task.Important.Checked").pixmap(SIZE_32_32));
		LabelImportant->setToolTip(LOCALIZATION("Task.Important.ToolTip"));
		LabelImportant->setCursor(CURSOR_WHATS_THIS);
		LayoutTitle->addWidget(LabelImportant);
	}

	LabelName = new QLabel(this);
	LabelName->setText("#" + QString::number(TaskID) + SYMBOL_SPACE + TaskName);
	LabelName->setFont(FONT_TAHOMA_15_BOLD);
	LabelName->setStyleSheet(STYLE_SHEET("QLabel.Color.Gray"));
	LayoutTitle->addWidget(LabelName);

	LayoutTitle->addStretch();

	ISPushButton *ButtonRename = new ISPushButton(this);
	ButtonRename->setText(LOCALIZATION("Task.Rename"));
	ButtonRename->setIcon(BUFFER_ICONS("Edit"));
	ButtonRename->setIconSize(SIZE_24_24);
	ButtonRename->setFlat(true);
	ButtonRename->setFixedHeight(30);
	ButtonRename->setFont(FONT_TAHOMA_10);
	connect(ButtonRename, &ISPushButton::clicked, this, &ISTaskViewForm::Rename);
	LayoutTitle->addWidget(ButtonRename);

	ISPushButton *ButtonCreate = new ISPushButton(this);
	ButtonCreate->setText(LOCALIZATION("Task.CreateTask"));
	ButtonCreate->setIcon(BUFFER_ICONS("Add"));
	ButtonCreate->setIconSize(SIZE_24_24);
	ButtonCreate->setFlat(true);
	ButtonCreate->setFixedHeight(30);
	ButtonCreate->setFont(FONT_TAHOMA_10);
	connect(ButtonCreate, &ISPushButton::clicked, this, &ISTaskViewForm::CreateTask);
	LayoutTitle->addWidget(ButtonCreate);
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::CreateLeft()
{
	QVBoxLayout *Layout = new QVBoxLayout();
	LayoutCenter->addLayout(Layout);

	QToolBar *ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	Layout->addWidget(ToolBar);

	ActionDone = new QAction(ToolBar);
	ActionDone->setText(LOCALIZATION("Task.Done"));
	ActionDone->setToolTip(LOCALIZATION("Task.Done"));
	ActionDone->setIcon(BUFFER_ICONS("Task.Done"));
	connect(ActionDone, &QAction::triggered, this, &ISTaskViewForm::DoneTask);
	ToolBar->addAction(ActionDone);

	ActionClose = new QAction(ToolBar);
	ActionClose->setText(LOCALIZATION("Task.Close"));
	ActionClose->setToolTip(LOCALIZATION("Task.Close"));
	ActionClose->setIcon(BUFFER_ICONS("Task.Close"));
	connect(ActionClose, &QAction::triggered, this, &ISTaskViewForm::CloseTask);
	ToolBar->addAction(ActionClose);

	ActionProcess = new QAction(ToolBar);
	ActionProcess->setText(LOCALIZATION("Task.Process"));
	ActionProcess->setToolTip(LOCALIZATION("Task.Process"));
	ActionProcess->setIcon(BUFFER_ICONS("Task.Process"));
	ActionProcess->setMenu(new QMenu(ToolBar));
	ToolBar->addAction(ActionProcess);
	dynamic_cast<QToolButton*>(ToolBar->widgetForAction(ActionProcess))->setPopupMode(QToolButton::InstantPopup);
	dynamic_cast<QToolButton*>(ToolBar->widgetForAction(ActionProcess))->setStyleSheet(STYLE_SHEET("QToolButtonMenu"));

	QAction *ActionClone = new QAction(ToolBar);
	ActionClone->setText(LOCALIZATION("Task.CloneTask"));
	ActionClone->setToolTip(LOCALIZATION("Task.CloneTask"));
	ActionClone->setIcon(BUFFER_ICONS("Task.Clone"));
	connect(ActionClone, &QAction::triggered, this, &ISTaskViewForm::CloneTask);
	ActionProcess->menu()->addAction(ActionClone);

	QAction *ActionDuplicate = new QAction(ToolBar);
	ActionDuplicate->setText(LOCALIZATION("Task.Duplicate"));
	ActionDuplicate->setToolTip(LOCALIZATION("Task.Duplicate"));
	ActionDuplicate->setIcon(BUFFER_ICONS("Task.Duplicate"));
	connect(ActionDuplicate, &QAction::triggered, this, &ISTaskViewForm::Duplicate);
	ActionProcess->menu()->addAction(ActionDuplicate);

	QAction *ActionLink = new QAction(ToolBar);
	ActionLink->setText(LOCALIZATION("Task.Link"));
	ActionLink->setToolTip(LOCALIZATION("Task.Link"));
	connect(ActionLink, &QAction::triggered, this, &ISTaskViewForm::Link);
	ActionProcess->menu()->addAction(ActionLink);

	ActionProcess->menu()->addSeparator();

	QAction *ActionComment = new QAction(ToolBar);
	ActionComment->setText(LOCALIZATION("Task.CreateComment"));
	ActionComment->setToolTip(LOCALIZATION("Task.CreateComment"));
	ActionComment->setIcon(BUFFER_ICONS("Task.Comments"));
	connect(ActionComment, &QAction::triggered, this, &ISTaskViewForm::AddComment);
	ActionProcess->menu()->addAction(ActionComment);

	QAction *ActionFile = new QAction(ToolBar);
	ActionFile->setText(LOCALIZATION("Task.CreateFile"));
	ActionFile->setToolTip(LOCALIZATION("Task.CreateFile"));
	ActionFile->setIcon(BUFFER_ICONS("Task.CreateFile"));
	connect(ActionFile, &QAction::triggered, this, &ISTaskViewForm::AddFile);
	ActionProcess->menu()->addAction(ActionFile);

	QAction *ActionReRead = ISControls::CreateActionUpdate(ToolBar);
	ActionReRead->setText(LOCALIZATION("Task.ReRead"));
	ActionReRead->setToolTip(LOCALIZATION("Task.ReRead.ToolTip"));
	ActionReRead->setShortcut(QKeySequence(Qt::Key_F5));
	connect(ActionReRead, &QAction::triggered, this, &ISTaskViewForm::ReRead);
	ToolBar->addAction(ActionReRead);

	QGroupBox *GroupBoxDescription = new QGroupBox(this);
	GroupBoxDescription->setTitle(LOCALIZATION("Task.Description"));
	GroupBoxDescription->setLayout(new QVBoxLayout());
	GroupBoxDescription->layout()->setContentsMargins(LAYOUT_MARGINS_2_PX);
	GroupBoxDescription->setSizePolicy(GroupBoxDescription->sizePolicy().horizontalPolicy(), QSizePolicy::Maximum);
	Layout->addWidget(GroupBoxDescription);

	QTextEdit *TextEdit = new QTextEdit(GroupBoxDescription);
	TextEdit->setReadOnly(true);
	TextEdit->setText(TaskDescription);
	TextEdit->setFrameShape(QFrame::NoFrame);
	TextEdit->setFixedHeight(100);
	TextEdit->setSizePolicy(TextEdit->sizePolicy().horizontalPolicy(), QSizePolicy::Maximum);
	GroupBoxDescription->layout()->addWidget(TextEdit);

	ISQuery qSelectLinked(QS_TASK_LINKED);
	qSelectLinked.BindValue(":TaskID", TaskID);
	if (qSelectLinked.Execute())
	{
		if (qSelectLinked.GetCountResultRows())
		{
			QFormLayout *FormLayout = new QFormLayout();
			QGroupBox *GroupBoxLinked = new QGroupBox(this);
			GroupBoxLinked->setTitle(LOCALIZATION("Task.Linked"));
			GroupBoxLinked->setLayout(FormLayout);
			GroupBoxLinked->setSizePolicy(GroupBoxLinked->sizePolicy().horizontalPolicy(), QSizePolicy::Maximum);
			Layout->addWidget(GroupBoxLinked);

			while (qSelectLinked.Next())
			{
				int LinkTaskID = qSelectLinked.ReadColumn("task_id").toInt();
				QString LinkTaskName = qSelectLinked.ReadColumn("task_name").toString();
				QString LinkUser = qSelectLinked.ReadColumn("userfullname").toString();

				ISLabelLink *LabelLink = new ISLabelLink(GroupBoxLinked);
				LabelLink->setCursor(CURSOR_POINTING_HAND);
				LabelLink->setProperty("LinkTaskID", LinkTaskID);
				LabelLink->setText(LinkTaskName);
				LabelLink->setToolTip(LOCALIZATION("Task.LinkedUser").arg(LinkUser));
				LabelLink->setSizePolicy(QSizePolicy::Maximum, LabelLink->sizePolicy().verticalPolicy());
				connect(LabelLink, &ISLabelLink::Clicked, this, &ISTaskViewForm::OpenLinkTask);
				FormLayout->addRow("#" + QString::number(LinkTaskID) + ":", LabelLink);
			}
		}
	}

	QTabWidget *TabWidget = new QTabWidget(this);
	TabWidget->setSizePolicy(TabWidget->sizePolicy().horizontalPolicy(), QSizePolicy::Minimum);
	Layout->addWidget(TabWidget);

	TaskCommentsTab = new ISTaskCommentsTab(TaskID, TabWidget);
	TabWidget->addTab(TaskCommentsTab, BUFFER_ICONS("Task.Comments"), LOCALIZATION("Task.Comments").arg(0));
	connect(TaskCommentsTab, &ISTaskCommentsTab::TabTextChanged, [=](int Count) 
	{ 
		TabWidget->setTabText(TabWidget->indexOf(TaskCommentsTab), LOCALIZATION("Task.Comments").arg(Count));
	});
	TaskCommentsTab->Update();

	TaskFilesTab = new ISTaskFilesTab(TaskID, TabWidget);
	TabWidget->addTab(TaskFilesTab, BUFFER_ICONS("Task.Files"), LOCALIZATION("Task.Files").arg(0));
	connect(TaskFilesTab, &ISTaskFilesTab::TabTextChanged, [=](int Count)
	{
		TabWidget->setTabText(TabWidget->indexOf(TaskFilesTab), LOCALIZATION("Task.Files").arg(Count));
	});
	TaskFilesTab->Update();

	TaskDuplicatesTab = new ISTaskDuplicatesTab(TaskID, TabWidget);
	TabWidget->addTab(TaskDuplicatesTab, BUFFER_ICONS("Task.Duplicate"), LOCALIZATION("Task.Duplicates").arg(0));
	connect(TaskDuplicatesTab, &ISTaskDuplicatesTab::TabTextChanged, [=](int Count)
	{
		TabWidget->setTabText(TabWidget->indexOf(TaskDuplicatesTab), LOCALIZATION("Task.Duplicates").arg(Count));
	});
	TaskDuplicatesTab->Update();

	TaskHistoryTab = new ISTaskHistoryTab(TaskID, TabWidget);
	TabWidget->addTab(TaskHistoryTab, BUFFER_ICONS("Task.History"), LOCALIZATION("Task.History"));
	TaskHistoryTab->Update();
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::CreateRight()
{
	TaskInfoWidget = new ISTaskInfoWidget(TaskID, this);
	LayoutCenter->addWidget(TaskInfoWidget);
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::Rename()
{
	QVariant NewName = ISInputDialog::GetString(this, LOCALIZATION("Renaming"), LOCALIZATION("Task.Rename.Description") + ":", TaskName);
	if (NewName.isValid())
	{
		ISQuery qRename(QU_TASK_NAME);
		qRename.BindValue(":Name", NewName);
		qRename.BindValue(":TaskID", TaskID);
		if (qRename.Execute())
		{
			ISCore::TaskInsertHistory(TaskID, CONST_UID_TASK_HISTORY_RENAME, TaskName + " -> " + NewName.toString());

			TaskName = NewName.toString();
			LabelName->setText("#" + QString::number(TaskID) + SYMBOL_SPACE + TaskName);
			TaskHistoryTab->Update();
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::CreateTask()
{
	ISIntegralSystem::ShowTaskObjectForm(ISNamespace::OFT_New);
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::UpdateActions()
{
	TaskInfoWidget->Update();

	ISUuid TaskStatusUID = ISCore::TaskGetStatusUID(TaskID);
	if (TaskStatusUID == CONST_UID_TASK_STATUS_IN_WORK)
	{
		ActionDone->setEnabled(true);
		ActionClose->setEnabled(false);
	}
	else if (TaskStatusUID == CONST_UID_TASK_STATUS_DONE)
	{
		ActionDone->setEnabled(false);
		ActionClose->setEnabled(true);
	}
	else
	{
		ActionDone->setEnabled(false);
		ActionClose->setEnabled(false);
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::DoneTask()
{
	if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.TaskDone")))
	{
		ISTaskDoneForm TaskDoneForm(TaskID);
		if (TaskDoneForm.Exec())
		{
			UpdateActions();
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::CloseTask()
{
	if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.TaskClose")))
	{
		ISCore::TaskSetStatus(TaskID, CONST_UID_TASK_STATUS_CLOSE);
		UpdateActions();
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::AddComment()
{
	QVariant Comment = ISInputDialog::GetText(this, LOCALIZATION("Task.CommentTask"), LOCALIZATION("Task.InputTheComment") + ":");
	if (Comment.isValid())
	{
		ISQuery qInsertComment(QI_COMMENT);
		qInsertComment.BindValue(":TaskID", TaskID);
		qInsertComment.BindValue(":Comment", Comment);
		if (qInsertComment.Execute())
		{
			ISCore::TaskInsertHistory(TaskID, CONST_UID_TASK_HISTORY_COMMENT, Comment.toString());
			TaskCommentsTab->Update();

			if (ISMetaUser::GetInstance().GetData()->ID != TaskExecutorID)
			{
				ISNotifySender::GetInstance().SendToUser(CONST_UID_NOTIFY_TASK_ADD_COMMENT, TaskExecutorID, QVariant(), TaskName, false);
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::AddFile()
{
	ISTaskFileInsertForm TaskFileInsertForm(TaskID);
	if (TaskFileInsertForm.Exec())
	{
		ISCore::TaskInsertHistory(TaskID, CONST_UID_TASK_HISTORY_FILE, TaskFileInsertForm.GetFileName());
		TaskFilesTab->Update();
		
		if (ISMetaUser::GetInstance().GetData()->ID != TaskExecutorID)
		{
			ISNotifySender::GetInstance().SendToUser(CONST_UID_NOTIFY_TASK_ADD_FILE, TaskExecutorID, QVariant(), TaskName, false);
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::CloneTask()
{
	if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.CloneThisTask")))
	{
		ISIntegralSystem::ShowTaskObjectForm(ISNamespace::OFT_Copy, TaskID);
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::Duplicate()
{
	if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Message.Question.CheckThisTaskAsDuplicate")))
	{
		int TaskOriginal = ISInputDialog::GetInteger(this, LOCALIZATION("Task.Duplicate"), LOCALIZATION("Task.NumberTask") + ":", 1, INTEGER_MAXIMUM).toInt();
		if (TaskOriginal)
		{
			if (ISCore::TaskCheckExist(TaskID)) //������ �������
			{
				if (ISCore::TaskIsDuplicate(TaskOriginal, TaskID)) //���� ������ ��� �������� ��� ��������
				{
					ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.TaskAlreadyCheckDuplicate").arg(TaskID).arg(TaskOriginal));
				}
				else //������ �� �������� ��� ��������
				{
					ISCore::TaskInsertDuplicate(TaskOriginal, TaskID);
					ISCore::TaskSetStatus(TaskID, CONST_UID_TASK_STATUS_DUPLICATE);
					UpdateActions();
				}
			}
			else //������ �� �������
			{
				ISMessageBox::ShowInformation(this, LOCALIZATION("Message.Information.TaskWithNumberNotFound").arg(TaskID));
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::Link()
{
	int ObjectID = ISGui::SelectObject("_Task");
	if (ObjectID)
	{
		if (ObjectID == TaskID)
		{
			ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.NotLinkTaskCurrentForCurrent"));
			return;
		}

		ISQuery qSelect(QS_TASK_LINK);
		qSelect.BindValue(":TaskID", TaskID);
		qSelect.BindValue(":LinkTaskID", ObjectID);
		if (qSelect.ExecuteFirst())
		{
			int Count = qSelect.ReadColumn("count").toInt();
			if (Count)
			{
				ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.TaskLinkAlreadyExist"));
			}
			else
			{
				ISQuery qInsert(QI_TASK_LINK);
				qInsert.BindValue(":Task", TaskID);
				qInsert.BindValue(":LinkTask", ObjectID);
				if (qInsert.ExecuteFirst())
				{
					QString TaskName = qInsert.ReadColumn("task").toString();
					QString LinkTaskName = qInsert.ReadColumn("linktask").toString();
					ISMessageBox::ShowInformation(this, LOCALIZATION("Message.Information.TaskLinkedDone").arg(LinkTaskName).arg(TaskName));
					ISCore::TaskInsertHistory(TaskID, CONST_UID_TASK_HISTORY_LINKED, LinkTaskName + " -> " + TaskName);
					ReRead();
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::ReRead()
{
	ISIntegralSystem::ShowTaskViewForm(TaskID);
	close();
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::OpenLinkTask()
{
	ISIntegralSystem::ShowTaskViewForm(sender()->property("LinkTaskID").toInt());
}
//-----------------------------------------------------------------------------
