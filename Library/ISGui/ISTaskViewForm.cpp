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
//-----------------------------------------------------------------------------
static QString QS_TASK = PREPARE_QUERY("SELECT "
									   "task_name, "
									   "task_description, "
									   "userfullname(task_executor) AS task_executor, "
									   "tstp_name AS task_type, "
									   "tsst_uid AS task_status_uid, "
									   "tsst_name AS task_status_name, "
									   "tspr_uid AS task_priority_uid, "
									   "tspr_name AS task_priority_name, "
									   "userfullname(task_owner) AS task_owner, "
									   "task_important, "
									   "task_creationdate, "
									   "task_updationdate "
									   "FROM _task "
									   "LEFT JOIN _tasktype ON tstp_id = task_type "
									   "LEFT JOIN _taskstatus ON tsst_id = task_status "
									   "LEFT JOIN _taskpriority ON tspr_id = task_priority "
									   "WHERE task_id = :TaskID");
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
	TaskCreationDate = qSelect.ReadColumn("task_creationdate").toDateTime().toString(FORMAT_DATE_TIME_V2);
	TaskUpdationDate = qSelect.ReadColumn("task_updationdate").toDateTime().toString(FORMAT_DATE_TIME_V2);

	setWindowTitle(LANG("Task.ViewFormTitle").arg(TaskID).arg(TaskName));

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutTitle);

	ISPushButton *ButtonMenu = new ISPushButton(BUFFER_ICONS("Menu"), LANG("Menu"), this);
	ButtonMenu->setFlat(true);
	ButtonMenu->setCursor(CURSOR_POINTING_HAND);
	ButtonMenu->setSizePolicy(QSizePolicy::Maximum, ButtonMenu->sizePolicy().verticalPolicy());
	ButtonMenu->setMenu(new QMenu(ButtonMenu));
	ButtonMenu->menu()->addAction(BUFFER_ICONS("Update"), LANG("Task.ReopenTaskViewForm"), this, &ISTaskViewForm::Reopen, QKeySequence(Qt::Key_F5));
	ButtonMenu->menu()->addSeparator();
	ButtonMenu->menu()->addAction(LANG("Task.Rename"), this, &ISTaskViewForm::Rename);
	ButtonMenu->menu()->addAction(LANG("Task.SetDescription"), this, &ISTaskViewForm::SetDescription);
	ButtonMenu->menu()->addSeparator();
	ButtonMenu->menu()->addAction(BUFFER_ICONS("Add"), LANG("Task.AddFile"), this, &ISTaskViewForm::FileAdd);
	ButtonMenu->menu()->addAction(BUFFER_ICONS("Add"), LANG("Task.AddLink"), this, &ISTaskViewForm::LinkAdd);
	ButtonMenu->menu()->addAction(BUFFER_ICONS("Add"), LANG("Task.AddComment"), this, &ISTaskViewForm::CommentAdd);
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
				ButtonProcess->setText(LANG("Task.Process").arg(StatusName));
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

	LabelName = new ISLabelSelectionText(QString("#%1: %2").arg(TaskID).arg(TaskName), this);
	LabelName->setWordWrap(true);
	LabelName->setFont(ISDefines::Gui::FONT_TAHOMA_12_BOLD);
	LabelName->setStyleSheet(STYLE_SHEET("QLabel.Color.Gray"));
	LabelName->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
	LayoutTitle->addWidget(LabelName);

	if (TaskImportant) //Задача является важной - добавляем соответствующую надпись
	{
		ISLabelPixmapText *LabelImportant = new ISLabelPixmapText(BUFFER_ICONS("Task.Important.Checked").pixmap(ISDefines::Gui::SIZE_32_32), LANG("Task.ThisIsImportantTask"), this);
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
	GroupBoxDescription->setSizePolicy(GroupBoxDescription->sizePolicy().horizontalPolicy(), QSizePolicy::Maximum);
	LayoutLeft->addWidget(GroupBoxDescription, 0, Qt::AlignTop);

	LabelDescription = new ISLabelSelectionText(TaskDescription.isEmpty() ? LANG("Task.Description.Empty") : TaskDescription, GroupBoxDescription);
	LabelDescription->setWordWrap(true);
	GroupBoxDescription->layout()->addWidget(LabelDescription);

	GroupBoxFiles = new QGroupBox(LANG("Task.Files"), this);
	LayoutLeft->addWidget(GroupBoxFiles);

	QVBoxLayout *LayoutFiles = new QVBoxLayout();
	LayoutFiles->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	GroupBoxFiles->setLayout(LayoutFiles);

	ListWidgetFiles = new ISListWidget(GroupBoxFiles);
	ListWidgetFiles->setSizePolicy(ListWidgetFiles->sizePolicy().horizontalPolicy(), QSizePolicy::Maximum);
	ListWidgetFiles->setContextMenuPolicy(Qt::ActionsContextMenu);
	LayoutFiles->addWidget(ListWidgetFiles);

	QAction *ActionFileSave = ISControls::CreateActionSave(ListWidgetFiles);
	ActionFileSave->setEnabled(false);
	connect(ActionFileSave, &QAction::triggered, this, &ISTaskViewForm::FileSave);
	ListWidgetFiles->addAction(ActionFileSave);

	QAction *ActionFileDelete = new QAction(BUFFER_ICONS("Delete"), LANG("Delete"), ListWidgetFiles);
	ActionFileDelete->setEnabled(false);
	connect(ActionFileDelete, &QAction::triggered, this, &ISTaskViewForm::FileDelete);
	ListWidgetFiles->addAction(ActionFileDelete);
	
	connect(ListWidgetFiles, &ISListWidget::itemSelectionChanged, [=]
	{
		bool is_enabled = ListWidgetFiles->currentItem();
		ActionFileSave->setEnabled(is_enabled);
		ActionFileDelete->setEnabled(is_enabled);
	});

	FileLoadList();

	GroupBoxLinkTask = new QGroupBox(LANG("Task.LinkTask"), this);
	GroupBoxLinkTask->setLayout(new QVBoxLayout());
	LayoutLeft->addWidget(GroupBoxLinkTask);
	LinkLoadList();

	GroupBoxComments = new QGroupBox(LANG("Task.Comments").arg(0), this);
	GroupBoxComments->setLayout(new QVBoxLayout());
	LayoutLeft->addWidget(GroupBoxComments);

	LayoutComments = new QVBoxLayout();
	LayoutComments->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);
	LayoutComments->addStretch();

	ScrollAreaComments = new ISScrollArea(GroupBoxComments);
	ScrollAreaComments->widget()->setLayout(LayoutComments);
	GroupBoxComments->layout()->addWidget(ScrollAreaComments);
	CommentLoadList();

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
void ISTaskViewForm::Rename()
{
	QString NewName = ISInputDialog::GetString(LANG("Renaming"), LANG("Task.Rename.LabelText"), TaskName);
	if (NewName.isEmpty())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NewTaskNameIsEmpty"));
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

			LabelName->setText(QString("#%1: %2").arg(TaskID).arg(TaskName));
			LabelUpdationDate->setText(TaskUpdationDate);
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

			LabelDescription->setText(TaskDescription.isEmpty() ? LANG("Task.Description.Empty") : TaskDescription);
			LabelUpdationDate->setText(TaskUpdationDate);
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.SetDescriptionTask"), qSetDescription.GetErrorString());
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
		ButtonProcess->setText(LANG("Task.Process").arg(TaskStatusName));
		ButtonProcess->setStyleSheet(STYLE_SHEET(qUpdateStatus.ReadColumn("tsst_stylesheet").toString()));
		LabelStatus->setText(TaskStatusName);

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
			QString CreationDate = qSelectFiles.ReadColumn("tfls_creationdate").toDateTime().toString(FORMAT_DATE_TIME_V2);
			QString Name = qSelectFiles.ReadColumn("tfls_name").toString();
			QString Extension = qSelectFiles.ReadColumn("tfls_extension").toString();
			qint64 Size = qSelectFiles.ReadColumn("tfls_size").toLongLong();
			QByteArray Icon = qSelectFiles.ReadColumn("tfls_icon").toByteArray();
			QString UserFullname = qSelectFiles.ReadColumn("userfullname").toString();

			QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidgetFiles);
			ListWidgetItem->setText(Name);
			ListWidgetItem->setIcon(ISGui::ByteArrayToIcon(Icon));
			ListWidgetItem->setToolTip(LANG("Task.FileToolTip").arg(UserFullname).arg(CreationDate).arg(ISSystem::FileSizeFromString(Size)));
			ListWidgetItem->setData(Qt::UserRole, ID);
			ListWidgetItem->setData(Qt::UserRole * 2, Extension);
			ListWidgetItem->setSizeHint(QSize(ListWidgetItem->sizeHint().width(), 25));
		}
		GroupBoxFiles->setVisible(qSelectFiles.GetCountResultRows());
	}
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
	QListWidgetItem *ListWidgetItem = ListWidgetFiles->currentItem();
	int ID = ListWidgetItem->data(Qt::UserRole).toInt();
	QString Name = ListWidgetItem->text();
	QString Extension = ListWidgetItem->data(Qt::UserRole * 2).toString();

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
		qDeleteFile.BindValue(":TaskFileID", ListWidgetFiles->currentItem()->data(Qt::UserRole));
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
	while (!VectorLinks.empty())
	{
		delete ISAlgorithm::VectorTakeBack(VectorLinks);
	}

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
			QString LinkCreationDate = qSelectLink.ReadColumn("tlnk_creationdate").toDateTime().toString(FORMAT_DATE_TIME_V2);
			ISUuid TaskStatusUID = qSelectLink.ReadColumn("task_status_uid");
			QString TaskStatusName = qSelectLink.ReadColumn("task_status_name").toString();
			QString TaskStatusIcon = qSelectLink.ReadColumn("task_status_icon").toString();
			
			ISLabelPixmapText *LabelLink = new ISLabelPixmapText(BUFFER_ICONS(TaskStatusIcon).pixmap(ISDefines::Gui::SIZE_16_16), QString("#%1: %2").arg(LinkTaskID).arg(LinkTaskName), GroupBoxLinkTask);
			LabelLink->GetLabelText()->SetIsLinked(true);
			LabelLink->GetLabelText()->setProperty("TaskID", LinkTaskID);
			LabelLink->GetLabelText()->setProperty("LinkID", LinkID);
			LabelLink->GetLabelText()->setToolTip(LANG("Task.LinkToolTip").arg(TaskStatusName).arg(LinkTaskDescription.isEmpty() ? LANG("Task.Description.Empty") : LinkTaskDescription).arg(LinkUser).arg(LinkCreationDate));
			LabelLink->GetLabelText()->setWordWrap(true);
			LabelLink->GetLabelText()->setContextMenuPolicy(Qt::ActionsContextMenu);
			connect(LabelLink->GetLabelText(), &ISQLabel::Clicked, this, &ISTaskViewForm::LinkOpen);
			GroupBoxLinkTask->layout()->addWidget(LabelLink);
			VectorLinks.push_back(LabelLink);

			QAction *ActionDelete = new QAction(BUFFER_ICONS("Delete"), LANG("Delete"), LabelLink->GetLabelText());
			connect(ActionDelete, &QAction::triggered, this, &ISTaskViewForm::LinkDelete);
			LabelLink->GetLabelText()->addAction(ActionDelete);

			if (TaskStatusUID == CONST_UID_TASK_STATUS_DONE || TaskStatusUID == CONST_UID_TASK_STATUS_CLOSE)
			{
				ISGui::SetFontWidgetStrikeOut(LabelLink, true);
			}
		}
		GroupBoxLinkTask->setVisible(qSelectLink.GetCountResultRows());
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
			for (ISLabelPixmapText *LabelLink : VectorLinks)
			{
				if (LabelLink->GetLabelText()->property("TaskID").toInt() == LinkTaskID)
				{
					ISMessageBox::ShowWarning(this, LANG("Message.Warning.TaskLinkAlreadyExist"));
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
void ISTaskViewForm::LinkOpen()
{
	ISGui::ShowTaskViewForm(sender()->property("TaskID").toInt());
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::LinkDelete()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteTaskLink")))
	{
		ISQuery qDeleteLink(QD_LINK);
		qDeleteLink.BindValue(":LinkID", sender()->parent()->property("LinkID"));
		if (qDeleteLink.Execute())
		{
			//Если вызывать напрямую обновление списка связей, то происходит падение. Причина неизвестна.
			QTimer::singleShot(10, this, &ISTaskViewForm::LinkLoadList);
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

			QWidget *WidgetComment = CommentCreateWidget(CommentID, UserPhoto, IsUserOwner ? LANG("Task.CommentUserOwner").arg(UserFullName) : UserFullName, Comment, CreationDate);
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

	ISLabelSelectionText *LabelComment = new ISLabelSelectionText(Comment, Widget);
	LabelComment->setWordWrap(true);
	LayoutWidget->addWidget(LabelComment);

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	LayoutBottom->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_NULL);

	QWidget *WidgetBottom = new QWidget(Widget);
	WidgetBottom->setLayout(LayoutBottom);
	LayoutWidget->addWidget(WidgetBottom);

	ISQLabel *LabelEdit = new ISQLabel(LANG("Edit"), true, WidgetBottom);
	LabelEdit->setProperty("Comment", Comment);
	LabelEdit->setProperty("CommentID", CommentID);
	connect(LabelEdit, &ISQLabel::Clicked, this, &ISTaskViewForm::CommentEdit);
	LayoutBottom->addWidget(LabelEdit);

	ISQLabel *LabelDelete = new ISQLabel(LANG("Delete"), true, WidgetBottom);
	LabelDelete->setProperty("CommentID", CommentID);
	connect(LabelDelete, &ISQLabel::Clicked, this, &ISTaskViewForm::CommentDelete);
	LayoutBottom->addWidget(LabelDelete);

	LayoutBottom->addWidget(new QLabel(DateTime.toString(FORMAT_DATE_TIME_V10), WidgetBottom));

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
			//После загрузки списка комментариев вызываем прокрутку в самый низ
			QTimer::singleShot(50, [=]() { ScrollAreaComments->verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum); });
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
			//Если вызывать напрямую обновление списка комментариев, то происходит падение. Причина неизвестна.
			QTimer::singleShot(10, this, &ISTaskViewForm::CommentLoadList);
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
			//Если вызывать напрямую обновление списка комментариев, то происходит падение. Причина неизвестна.
			QTimer::singleShot(10, this, &ISTaskViewForm::CommentLoadList);
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.DeleteTaskComment"), qDeleteComment.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
