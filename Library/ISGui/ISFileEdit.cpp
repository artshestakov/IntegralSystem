#include "ISFileEdit.h"
#include "ISConstants.h"
#include "ISFileDialog.h"
#include "ISMessageBox.h"
#include "ISLocalization.h"
#include "ISSystem.h"
#include "ISBuffer.h"
#include "ISGui.h"
#include "ISInputDialog.h"
#include "ISDefinesGui.h"
#include "ISSettingsDatabase.h"
//-----------------------------------------------------------------------------
ISFileEdit::ISFileEdit(QWidget *parent) : ISFieldEditBase(parent)
{
	SetSizePolicyHorizontal(QSizePolicy::Maximum);
	CreateButtonClear();

	ButtonFile = new ISPushButton(this);
	ButtonFile->setCursor(CURSOR_POINTING_HAND);
	ButtonFile->setText(LANG("FileNotSelected"));
	ButtonFile->setToolTip(LANG("ClickFromSelectFile"));
	connect(ButtonFile, &ISPushButton::clicked, this, &ISFileEdit::SelectFile);
	AddWidgetEdit(ButtonFile, this);

	Menu = new QMenu(this);
	Menu->addAction(BUFFER_ICONS("Select"), LANG("Overview"), this, &ISFileEdit::SelectFile);
	ActionSave = Menu->addAction(BUFFER_ICONS("Save"), LANG("SaveToDisk"), this, &ISFileEdit::Save);
	ActionRename = Menu->addAction(BUFFER_ICONS("Edit"), LANG("Rename"), this, &ISFileEdit::Rename);

	ActionSave->setEnabled(false);
	ActionRename->setEnabled(false);
}
//-----------------------------------------------------------------------------
ISFileEdit::~ISFileEdit()
{

}
//-----------------------------------------------------------------------------
void ISFileEdit::SetValue(const QVariant &value)
{
	VariantMap = ISSystem::JsonStringToVariantMap(value.toString());
	ButtonFile->setMenu(Menu);
	ButtonFile->setText(VariantMap[FILE_EDIT_PROPERTY_NAME].toString());
	ActionSave->setEnabled(true);
	ActionRename->setEnabled(true);

	QPixmap Pixmap;
	if (Pixmap.loadFromData(QByteArray::fromBase64(VariantMap[FILE_EDIT_PROPERTY_LOGO].toString().toUtf8())))
	{
		ButtonFile->setIcon(QIcon(Pixmap));
	}
}
//-----------------------------------------------------------------------------
QVariant ISFileEdit::GetValue() const
{
	return VariantMap.isEmpty() ? QVariant() : ISSystem::VariantMapToJsonString(VariantMap, QJsonDocument::Compact).simplified();
}
//-----------------------------------------------------------------------------
void ISFileEdit::Clear()
{
	VariantMap.clear();
	ButtonFile->setMenu(nullptr);
	ButtonFile->setIcon(QIcon());
	ButtonFile->setText(LANG("FileNotSelected"));
	ActionSave->setEnabled(false);
	ActionRename->setEnabled(false);
	emit ValueChanged();
}
//-----------------------------------------------------------------------------
void ISFileEdit::SelectFile()
{
	QString FilePath = ISFileDialog::GetOpenFileName(this);
	if (!FilePath.isEmpty())
	{
		QFileInfo FileInfo(FilePath);
		QFile File(FilePath);
		if (File.size() > (((1000 * 1024) * SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_STORAGEFILEMAXSIZE))))
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.InsertingFileSizeVeryBig").arg(FileInfo.fileName()).arg(SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_STORAGEFILEMAXSIZE)));
			return;
		}

		if (File.open(QIODevice::ReadOnly))
		{
			ISGui::SetWaitGlobalCursor(true);
			QIcon IconFile = ISGui::GetIconFile(FilePath);
			VariantMap[FILE_EDIT_PROPERTY_NAME] = FileInfo.fileName();
			VariantMap[FILE_EDIT_PROPERTY_LOGO] = ISGui::PixmapToByteArray(IconFile.pixmap(ISDefines::Gui::SIZE_32_32)).toBase64();
			VariantMap[FILE_EDIT_PROPERTY_DATA] = File.readAll().toBase64();
			File.close();
			ButtonFile->setMenu(Menu);
			ButtonFile->setIcon(IconFile);
			ButtonFile->setText(FileInfo.fileName());
			ActionSave->setEnabled(true);
			ActionRename->setEnabled(true);
			emit ValueChanged();
			ISGui::SetWaitGlobalCursor(false);
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.NotOpenedFile").arg(FileInfo.fileName()), File.errorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISFileEdit::Save()
{
	QString FilePath = ISFileDialog::GetSaveFileName(this, LANG("File.Filter.File").arg(QFileInfo(VariantMap[FILE_EDIT_PROPERTY_NAME].toString()).suffix()), QFileInfo(VariantMap[FILE_EDIT_PROPERTY_NAME].toString()).baseName());
	if (!FilePath.isEmpty())
	{
		QFile File(FilePath);
		if (File.open(QIODevice::WriteOnly))
		{
			File.write(QByteArray::fromBase64(VariantMap[FILE_EDIT_PROPERTY_DATA].toString().toUtf8()));
			File.close();

			if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.FileSaved")))
			{
				if (!ISGui::OpenFile(FilePath))
				{
					ISMessageBox::ShowCritical(this, LANG("Message.Error.NotOpenedFile").arg(FilePath));
				}
			}
		}
		else
		{
			ISMessageBox::ShowCritical(this, LANG("Message.Error.NotOpenedFile").arg(File.fileName()), File.errorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISFileEdit::Rename()
{
	QFileInfo FileInfo(VariantMap[FILE_EDIT_PROPERTY_NAME].toString());
	QString FileName = ISInputDialog::GetString(LANG("Renaming"), LANG("EnterFileName") + ':', FileInfo.baseName());
	if (!FileName.isEmpty())
	{
		FileName += SYMBOL_POINT + FileInfo.suffix();
		ButtonFile->setText(FileName);
		VariantMap[FILE_EDIT_PROPERTY_NAME] = FileName;
		emit ValueChanged();
	}
}
//-----------------------------------------------------------------------------
