#include "ISFileEdit.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISFileDialog.h"
#include "ISMessageBox.h"
#include "ISSystem.h"
#include "ISInputDialog.h"
#include "ISQuery.h"
#include "ISGui.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_FILE = PREPARE_QUERY("SELECT file_name, file_extension, file_icon "
									   "FROM _file "
									   "WHERE file_id = :FileID");
//-----------------------------------------------------------------------------
static QString QU_FILE = PREPARE_QUERY("UPDATE _file SET "
									   "file_name = :Name, "
									   "file_extension = :Extension, "
									   "file_icon = :Icon, "
									   "file_data = :Data "
									   "WHERE file_id = :FileID");
//-----------------------------------------------------------------------------
static QString QI_FILE = PREPARE_QUERY("INSERT INTO _file(file_name, file_extension, file_icon, file_data) "
									   "VALUES(:Name, :Extension, :Icon, :Data) "
									   "RETURNING file_id");
//-----------------------------------------------------------------------------
static QString QU_FILE_NAME = PREPARE_QUERY("UPDATE _file SET "
											"file_name = :Name "
											"WHERE file_id = :FileID");
//-----------------------------------------------------------------------------
static QString QS_FILE_OPEN = PREPARE_QUERY("SELECT file_extension, file_data FROM _file WHERE file_id = :FileID");
//-----------------------------------------------------------------------------
static QString QS_FILE_SAVE = PREPARE_QUERY("SELECT file_name, file_extension FROM _file WHERE file_id = :FileID");
//-----------------------------------------------------------------------------
static QString QS_FILE_DATA = PREPARE_QUERY("SELECT file_data FROM _file WHERE file_id = :FileID");
//-----------------------------------------------------------------------------
ISFileEdit::ISFileEdit(QWidget *parent) : ISFieldEditBase(parent)
{
	CreateButtonClear();

	ButtonFile = new ISButtonFile(this);
	ButtonFile->setFlat(true);
	ButtonFile->setText(LANG("FileNotSelected"));
	ButtonFile->setToolTip(LANG("ClickOverviewFromSelectFile"));
	ButtonFile->setCursor(CURSOR_POINTING_HAND);
	connect(ButtonFile, &ISButtonFile::DragAndDropFile, this, &ISFileEdit::HandlingFile);
	AddWidgetEdit(ButtonFile, this);

	MenuFile = new QMenu(ButtonFile);
	MenuFile->addAction(LANG("Open"), this, &ISFileEdit::Open);
	MenuFile->addAction(LANG("Rename"), this, &ISFileEdit::Rename);
	MenuFile->addAction(LANG("SaveToDisk"), this, &ISFileEdit::Save);

	ISPushButton *ButtonSelect = new ISPushButton(this);
	ButtonSelect->setText(LANG("Overview") + "...");
	ButtonSelect->setToolTip(LANG("SelectFile"));
	ButtonSelect->setCursor(CURSOR_POINTING_HAND);
	connect(ButtonSelect, &ISPushButton::clicked, this, &ISFileEdit::SelectFile);
	AddWidgetToRight(ButtonSelect);
}
//-----------------------------------------------------------------------------
ISFileEdit::~ISFileEdit()
{

}
//-----------------------------------------------------------------------------
void ISFileEdit::SetValue(const QVariant &value)
{
	if (value.isValid())
	{
		FileID = value;
		ISQuery qSelectFile(QS_FILE);
		qSelectFile.BindValue(":FileID", FileID);
		if (qSelectFile.ExecuteFirst())
		{
			QString FileName = qSelectFile.ReadColumn("file_name").toString();
			QString FileExtension = qSelectFile.ReadColumn("file_extension").toString();
			QIcon FileIcon = ISGui::ByteArrayToIcon(qSelectFile.ReadColumn("file_icon").toByteArray());

			ButtonFile->setMenu(MenuFile);
			ButtonFile->setText(FileName + SYMBOL_POINT + FileExtension);
			ButtonFile->setToolTip(QString());
			ButtonFile->setIcon(FileIcon);
		}
	}
}
//-----------------------------------------------------------------------------
QVariant ISFileEdit::GetValue() const
{
	return FileID;
}
//-----------------------------------------------------------------------------
void ISFileEdit::Clear()
{
	ISGui::SetWaitGlobalCursor(true);
	if (FileID.isValid())
	{
		FileID.clear();
		ButtonFile->setMenu(nullptr);
		ButtonFile->setText(LANG("FileNotSelected"));
		ButtonFile->setToolTip(LANG("ClickOverviewFromSelectFile"));
		ButtonFile->setIcon(QIcon());
		ValueChanged();
	}
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISFileEdit::SetVisibleClear(bool Visible)
{

}
//-----------------------------------------------------------------------------
void ISFileEdit::SelectFile()
{
	QString FilePath = ISFileDialog::GetOpenFileName(this);
	if (FilePath.length()) //Пользователь не выбрал файл
	{
		HandlingFile(FilePath);
	}
}
//-----------------------------------------------------------------------------
void ISFileEdit::HandlingFile(const QString &FilePath)
{
	QFileInfo FileInfo(FilePath);
	if (FileInfo.size() > MAX_FILE_SIZE_FIELD) //Если размер файла больше установленного ограничения
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.FileSizeExceedsLimit").arg(MAX_FILE_SIZE_FIELD / 1024 / 1000));
		return;
	}

	QFile File(FilePath);
	if (!File.open(QIODevice::ReadOnly)) //Если файл не удалось открыть на чтение
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Error.NotOpenedFile").arg(FilePath));
		return;
	}

	QString FileName = FileInfo.baseName();
	QString FileExtension = FileInfo.suffix();
	QIcon FileIcon = ISGui::GetIconFile(FilePath);
	QByteArray FileData = File.readAll();
	File.close();

	if (!FileData.size())
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Error.FileEmptyOrInvalid").arg(FilePath));
		return;
	}

	ISGui::SetWaitGlobalCursor(true);
	ButtonFile->setText(LANG("Adding") + "...");
	ISGui::RepaintWidget(ButtonFile);

	if (FileID.isValid())
	{
		if (UpdateFile(FileName, FileExtension, FileIcon, FileData))
		{
			ButtonFile->setMenu(MenuFile);
			ButtonFile->setText(FileInfo.fileName());
			ButtonFile->setToolTip(QString());
			ButtonFile->setIcon(FileIcon);
			ValueChanged();
		}
	}
	else
	{
		if (InsertFile(FileName, FileExtension, FileIcon, FileData))
		{
			ButtonFile->setMenu(MenuFile);
			ButtonFile->setText(FileInfo.fileName());
			ButtonFile->setToolTip(QString());
			ButtonFile->setIcon(FileIcon);
			ValueChanged();
		}
	}
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISFileEdit::Open()
{
	ISGui::SetWaitGlobalCursor(true);

	ISQuery qSelect(QS_FILE_OPEN);
	qSelect.BindValue(":FileID", FileID);
	if (qSelect.ExecuteFirst())
	{
		QString Extension = qSelect.ReadColumn("file_extension").toString();
		QByteArray Data = qSelect.ReadColumn("file_data").toByteArray();
		QString FilePathTemp = ISDefines::Core::PATH_TEMP_DIR + '/' + ISSystem::GenerateUuid() + SYMBOL_POINT + Extension;
		QFile FileTemp(FilePathTemp);
		if (FileTemp.open(QIODevice::WriteOnly))
		{
			FileTemp.write(Data);
			FileTemp.close();

			ISGui::OpenFile(FilePathTemp);
		}
	}

	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISFileEdit::Rename()
{
	QFileInfo FileInfo(ButtonFile->text());
	QVariant NewName = ISInputDialog::GetString(this, LANG("Renaming"), LANG("NewFileName"), FileInfo.baseName());
	if (NewName.isValid())
	{
		ISGui::SetWaitGlobalCursor(true);

		ISQuery qUpdateName(QU_FILE_NAME);
		qUpdateName.BindValue(":Name", NewName);
		qUpdateName.BindValue(":FileID", FileID);
		if (qUpdateName.Execute())
		{
			ButtonFile->setText(NewName.toString() + SYMBOL_POINT + FileInfo.suffix());
			ValueChanged();
		}

		ISGui::SetWaitGlobalCursor(false);
	}
}
//-----------------------------------------------------------------------------
void ISFileEdit::Save()
{
	ISQuery qSelectFile(QS_FILE_SAVE);
	qSelectFile.BindValue(":FileID", FileID);
	if (qSelectFile.ExecuteFirst())
	{
		QString Name = qSelectFile.ReadColumn("file_name").toString();
		QString Extension = qSelectFile.ReadColumn("file_extension").toString();
		QString FilePath = ISFileDialog::GetSaveFileName(this, LANG("File.Filter.File").arg(Extension), Name);
		if (FilePath.length())
		{
			QFile File(FilePath);
			if (File.exists())
			{
				if (!File.remove())
				{
					ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotDeletedFile").arg(FilePath));
				}
			}

			ISQuery qSelectData(QS_FILE_DATA);
			qSelectData.BindValue(":FileID", FileID);
			if (qSelectData.ExecuteFirst())
			{
				if (File.open(QIODevice::WriteOnly))
				{
					File.write(qSelectData.ReadColumn("file_data").toByteArray());
					File.close();
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
bool ISFileEdit::UpdateFile(const QString &FileName, const QString &FileExtension, const QIcon &FileIcon, const QByteArray &FileData) const
{
	ISQuery qUpdateFile(QU_FILE);
	qUpdateFile.BindValue(":Name", FileName);
	qUpdateFile.BindValue(":Extension", FileExtension);
	qUpdateFile.BindValue(":Icon", ISGui::IconToByteArray(FileIcon));
	qUpdateFile.BindValue(":Data", FileData);
	qUpdateFile.BindValue(":FileID", FileID);
	return qUpdateFile.Execute();
}
//-----------------------------------------------------------------------------
bool ISFileEdit::InsertFile(const QString &FileName, const QString &FileExtension, const QIcon &FileIcon, const QByteArray &FileData)
{
	ISQuery qInsertFile(QI_FILE);
	qInsertFile.BindValue(":Name", FileName);
	qInsertFile.BindValue(":Extension", FileExtension);
	qInsertFile.BindValue(":Icon", ISGui::IconToByteArray(FileIcon));
	qInsertFile.BindValue(":Data", FileData);
	bool Result = qInsertFile.ExecuteFirst();
	if (Result)
	{
		FileID = qInsertFile.ReadColumn("file_id");
	}

	return Result;
}
//-----------------------------------------------------------------------------
