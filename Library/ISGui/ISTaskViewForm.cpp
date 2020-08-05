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
//-----------------------------------------------------------------------------
static QString QS_TASK = PREPARE_QUERY("SELECT "
									   "task_name, "
									   "task_description, "
									   "userfullname(task_executor) AS task_executor, "
									   "tstp_name AS task_type, "
									   "tsst_name AS task_status, "
									   "tspr_name AS task_priority, "
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
static QString QS_LINK = PREPARE_QUERY("SELECT tlnk_id, task_id, task_name, task_description, userfullname(tlnk_user), tlnk_creationdate "
									   "FROM _tasklink "
									   "LEFT JOIN _task ON tlnk_link = task_id "
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
	TaskStatus = qSelect.ReadColumn("task_status").toString();
	TaskPriority = qSelect.ReadColumn("task_priority").toString();
	TaskOwner = qSelect.ReadColumn("task_owner").toString();
	TaskImportant = qSelect.ReadColumn("task_important").toBool();
	TaskCreationDate = qSelect.ReadColumn("task_creationdate").toDateTime().toString(FORMAT_DATE_TIME_V2);
	TaskUpdationDate = qSelect.ReadColumn("task_updationdate").toDateTime().toString(FORMAT_DATE_TIME_V2);

	setWindowTitle(LANG("Task.ViewFormTitle").arg(TaskID).arg(TaskName));

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutTitle);

	ISPushButton *ButtonMenu = new ISPushButton(BUFFER_ICONS("Menu"), LANG("Menu"), this);
	ButtonMenu->setFlat(true);
	ButtonMenu->setMenu(new QMenu(ButtonMenu));
	ButtonMenu->menu()->addAction(BUFFER_ICONS("Update"), LANG("Task.ReopenTaskViewForm"), this, &ISTaskViewForm::Reopen, QKeySequence(Qt::Key_F5));
	ButtonMenu->menu()->addSeparator();
	ButtonMenu->menu()->addAction(LANG("Task.Rename"), this, &ISTaskViewForm::Rename);
	ButtonMenu->menu()->addAction(LANG("Task.SetDescription"), this, &ISTaskViewForm::SetDescription);
	ButtonMenu->menu()->addSeparator();
	ButtonMenu->menu()->addAction(BUFFER_ICONS("Add"), LANG("Task.AddFile"), this, &ISTaskViewForm::AddFile);
	ButtonMenu->menu()->addAction(BUFFER_ICONS("Add"), LANG("Task.AddLink"), this, &ISTaskViewForm::AddLink);
	ButtonMenu->menu()->addAction(BUFFER_ICONS("Add"), LANG("Task.AddComment"), this, &ISTaskViewForm::AddComment);
	LayoutTitle->addWidget(ButtonMenu);

	LabelName = new ISLabelSelectionText(QString("#%1: %2").arg(TaskID).arg(TaskName), this);
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
	connect(ActionFileSave, &QAction::triggered, this, &ISTaskViewForm::SaveFile);
	ListWidgetFiles->addAction(ActionFileSave);

	QAction *ActionFileDelete = ISControls::CreateActionDelete(ListWidgetFiles);
	ActionFileDelete->setEnabled(false);
	connect(ActionFileDelete, &QAction::triggered, this, &ISTaskViewForm::DeleteFile);
	ListWidgetFiles->addAction(ActionFileDelete);
	
	connect(ListWidgetFiles, &ISListWidget::itemSelectionChanged, [=]
	{
		bool is_enabled = ListWidgetFiles->currentItem();
		ActionFileSave->setEnabled(is_enabled);
		ActionFileDelete->setEnabled(is_enabled);
	});

	LoadFiles();

	GroupBoxLinkTask = new QGroupBox(LANG("Task.LinkTask"), this);
	GroupBoxLinkTask->setLayout(new QVBoxLayout());
	LayoutLeft->addWidget(GroupBoxLinkTask);
	LoadLinks();

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
void ISTaskViewForm::LoadFiles()
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
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::AddFile()
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
			LoadFiles();
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::SaveFile()
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
void ISTaskViewForm::DeleteFile()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteTaskFile")))
	{
		ISQuery qDeleteFile(QD_FILE);
		qDeleteFile.BindValue(":TaskFileID", ListWidgetFiles->currentItem()->data(Qt::UserRole));
		if (qDeleteFile.Execute())
		{
			LoadFiles();
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.DeleteTaskFile"), qDeleteFile.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::LoadLinks()
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
			
			ISLabelLink *LabelLink = new ISLabelLink(QString("#%1: %2").arg(LinkTaskID).arg(LinkTaskName), GroupBoxLinkTask);
			LabelLink->setProperty("TaskID", LinkTaskID);
			LabelLink->setProperty("LinkID", LinkID);
			LabelLink->setToolTip(LANG("Task.LinkToolTip").arg(LinkTaskDescription.isEmpty() ? LANG("Task.Description.Empty") : LinkTaskDescription).arg(LinkUser).arg(LinkCreationDate));
			LabelLink->setWordWrap(true);
			LabelLink->setContextMenuPolicy(Qt::ActionsContextMenu);
			connect(LabelLink, &ISLabelLink::Clicked, this, &ISTaskViewForm::OpenLink);
			GroupBoxLinkTask->layout()->addWidget(LabelLink);
			VectorLinks.push_back(LabelLink);

			QAction *ActionDelete = ISControls::CreateActionDelete(LabelLink);
			connect(ActionDelete, &QAction::triggered, this, &ISTaskViewForm::DeleteLink);
			LabelLink->addAction(ActionDelete);
		}
	}
	else
	{
		ISMessageBox::ShowCritical(this, LANG("Message.Error.LoadTaskLink"), qSelectLink.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::AddLink()
{
	int LinkTaskID = ISGui::SelectObject("_Task");
	if (LinkTaskID)
	{
		if (LinkTaskID == TaskID)
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.TaskLinkToByMyself"));
		}
		else
		{
			//Проверяем, есть ли уже связь с выбранной задачей
			for (ISLabelLink *LabelLink : VectorLinks)
			{
				if (LabelLink->property("TaskID").toInt() == LinkTaskID)
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
				LoadLinks();
			}
			else
			{
				ISMessageBox::ShowCritical(this, LANG("Message.Error.InsertTaskLink"));
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::OpenLink()
{
	ISGui::ShowTaskViewForm(sender()->property("TaskID").toInt());
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::DeleteLink()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.DeleteTaskLink")))
	{
		ISQuery qDeleteLink(QD_LINK);
		qDeleteLink.BindValue(":LinkID", sender()->parent()->property("LinkID"));
		if (qDeleteLink.Execute())
		{
			//Если вызывать напрямую обновление списка связей, то происходит падение. Причина неизвестна.
			QTimer::singleShot(10, this, &ISTaskViewForm::LoadLinks);
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.DeleteTaskLink"), qDeleteLink.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISTaskViewForm::LoadComments()
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
	else
	{
		ISGui::SetWaitGlobalCursor(false);
		ISMessageBox::ShowCritical(this, LANG("Message.Error.LoadTaskComments"), qSelectComments.GetErrorString());
	}
	ISGui::SetWaitGlobalCursor(false);
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

	ISLabelSelectionText *LabelComment = new ISLabelSelectionText(Comment, Widget);
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

	LayoutBottom->addWidget(new QLabel(DateTime.toString(FORMAT_DATE_TIME_V10), WidgetBottom));

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
void ISTaskViewForm::EditComment()
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
			QTimer::singleShot(10, this, &ISTaskViewForm::LoadComments);
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
			//Если вызывать напрямую обновление списка комментариев, то происходит падение. Причина неизвестна.
			QTimer::singleShot(10, this, &ISTaskViewForm::LoadComments);
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.DeleteTaskComment"), qDeleteComment.GetErrorString());
		}
	}
}
//-----------------------------------------------------------------------------
