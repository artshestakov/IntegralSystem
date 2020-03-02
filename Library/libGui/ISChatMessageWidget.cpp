#include "ISChatMessageWidget.h"
#include "ISDefinesGui.h"
#include "ISBuffer.h"
#include "ISLabels.h"
#include "ISQuery.h"
#include "ISSystem.h"
#include "ISLocalization.h"
#include "ISFileDialog.h"
#include "ISMessageBox.h"
#include "ISButtons.h"
#include "ISButtons.h"
#include "ISMetaData.h"
#include "ISGui.h"
#include "ISButtons.h"
#include "ISNotificationService.h"
#include "ISConstants.h"
#include "ISDefinesCore.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_CHAT_MESSAGE = PREPARE_QUERY("SELECT "
											   "chat_creationdate, "
											   "userfullname(chat_user), "
											   "useronline(userlogin(chat_user)), "
											   "chat_message, "
											   "length(chat_image) AS chat_image, "
											   "chat_filename, "
											   "chat_tablename, "
											   "chat_objectid "
											   "FROM _chatmessages "
											   "WHERE chat_id = :MessageID");
//-----------------------------------------------------------------------------
static QString QS_MESSAGE_IMAGE = PREPARE_QUERY("SELECT chat_image FROM _chatmessages WHERE chat_id = :MessageID");
//-----------------------------------------------------------------------------
static QString QS_MESSAGE_FILE = PREPARE_QUERY("SELECT chat_file, chat_filename FROM _chatmessages WHERE chat_id = :MessageID");
//-----------------------------------------------------------------------------
ISChatMessageWidget::ISChatMessageWidget(int message_id, QWidget *parent) : QWidget(parent)
{
	MessageID = message_id;

	ISQuery qSelectMessage(QS_CHAT_MESSAGE);
	qSelectMessage.BindValue(":MessageID", MessageID);
	qSelectMessage.ExecuteFirst();
	
	QDateTime DateTime = qSelectMessage.ReadColumn("chat_creationdate").toDateTime();
	QString UserFullName = qSelectMessage.ReadColumn("userfullname").toString();
	bool UserOnline = qSelectMessage.ReadColumn("useronline").toBool();
	QString Message = qSelectMessage.ReadColumn("chat_message").toString();
	int ImageSize = qSelectMessage.ReadColumn("chat_image").toInt();
	QString FileName = qSelectMessage.ReadColumn("chat_filename").toString();
	QString TableName = qSelectMessage.ReadColumn("chat_tablename").toString();
	int ObjectID = qSelectMessage.ReadColumn("chat_objectid").toInt();

	QVBoxLayout *MainLayout = new QVBoxLayout();
	MainLayout->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_4_PX);
	setLayout(MainLayout);

	QHBoxLayout *LayoutTop = new QHBoxLayout();
	MainLayout->addLayout(LayoutTop);

	ISServiceButton *ButtonIcon = new ISServiceButton(this);
	ButtonIcon->setToolTip(LANG("MessageID").arg(MessageID));
	ButtonIcon->setIcon(BUFFER_ICONS("Chat.Message"));
	ButtonIcon->setIconSize(ISDefines::Gui::SIZE_16_16);
	ButtonIcon->setFlat(true);
	ButtonIcon->setCursor(CURSOR_POINTING_HAND);
	ButtonIcon->setMenu(new QMenu(ButtonIcon));
	LayoutTop->addWidget(ButtonIcon);

	ButtonIcon->menu()->addAction(LANG("CiteCurrentMessage"), this, &ISChatMessageWidget::Cite);
	ButtonIcon->menu()->addAction(LANG("SelectMessage"), this, &ISChatMessageWidget::SelectMessage);
	ButtonIcon->menu()->addAction(LANG("CopyMessage"), this, &ISChatMessageWidget::CopyMessage);

	QLabel *LabelOnline = new QLabel(this);
	LayoutTop->addWidget(LabelOnline);

	if (UserOnline)
	{
		LabelOnline->setPixmap(BUFFER_ICONS("ChatUser.Online").pixmap(ISDefines::Gui::SIZE_10_10));
		LabelOnline->setToolTip(LANG("ChatForm.User.Online"));
	}
	else
	{
		LabelOnline->setPixmap(BUFFER_ICONS("ChatUser.Offline").pixmap(ISDefines::Gui::SIZE_10_10));
		LabelOnline->setToolTip(LANG("ChatForm.User.Offline"));
	}

	QLabel *LabelUser = new QLabel(UserFullName + ':', this);
	LabelUser->setFont(ISDefines::Gui::FONT_APPLICATION_BOLD);
	LayoutTop->addWidget(LabelUser);

	LayoutTop->addStretch();

	QLabel *LabelDateTime = new QLabel(this);
	LabelDateTime->setText(ISGui::ConvertDateTimeToString(DateTime, DATE_FORMAT_V1, TIME_FORMAT_V3));
	LayoutTop->addWidget(LabelDateTime);
	
	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	MainLayout->addLayout(LayoutBottom);

	if (Message.length()) //Если есть текст сообщения
	{
		int Pos = 0;
		if (ISGui::IsStringUrl(Message))
		{
			LabelMessage = new ISLabelLink(this);
			LabelMessage->setCursor(CURSOR_POINTING_HAND);
			LabelMessage->setToolTip(LANG("ClickFromOpenUrlInBrowser"));
			connect(dynamic_cast<ISLabelLink*>(LabelMessage), &ISLabelLink::Clicked, this, &ISChatMessageWidget::OpenUrlLink);
		}
		else
		{
			LabelMessage = new QLabel(this);
			LabelMessage->setTextInteractionFlags(Qt::TextSelectableByMouse);
			LabelMessage->setCursor(CURSOR_I_BEAM);
			LabelMessage->setFont(ISDefines::Gui::FONT_TAHOMA_10);
		}
		
		LabelMessage->setText(Message);
		LabelMessage->setWordWrap(true);
		LayoutBottom->addWidget(LabelMessage);
	}

	if (ImageSize) //Если к сообщению прикреплено изображение
	{
		ISToolButton *ButtonImage = new ISToolButton(this);
		ButtonImage->setText(LANG("Image"));
		ButtonImage->setToolTip(LANG("ClickFromOpenImage"));
		ButtonImage->setPopupMode(QToolButton::MenuButtonPopup);
		connect(ButtonImage, &ISToolButton::clicked, this, &ISChatMessageWidget::OpenImage);
		MainLayout->addWidget(ButtonImage, 0, Qt::AlignLeft);

		QMenu *Menu = new QMenu(ButtonImage);
		Menu->addAction(BUFFER_ICONS("Save"), LANG("Save"), this, &ISChatMessageWidget::SaveImage);
		ButtonImage->setMenu(Menu);
	}

	if (FileName.length()) //Если к сообщению прикреплен файл
	{
		ISToolButton *ButtonFile = new ISToolButton(this);
		ButtonFile->setText(LANG("File") + ": " + FileName);
		ButtonFile->setToolTip(LANG("ClickFromOpenFile")); //ClickFromSaveFile
		ButtonFile->setPopupMode(QToolButton::MenuButtonPopup);
		connect(ButtonFile, &ISToolButton::clicked, this, &ISChatMessageWidget::OpenFile);
		MainLayout->addWidget(ButtonFile, 0, Qt::AlignLeft);

		QMenu *Menu = new QMenu(ButtonFile);
		QAction *ActionSave = Menu->addAction(BUFFER_ICONS("Save"), LANG("Save"), this, &ISChatMessageWidget::SaveFile);
		ActionSave->setData(FileName);
		ButtonFile->setMenu(Menu);
	}

	if (TableName.length() && ObjectID) //Если к сообщению прикреплена ссылка на запись
	{
		ISPushButton *ButtonLink = new ISPushButton(this);
		ButtonLink->setText(LANG("LinkToRecordInTable").arg(ISMetaData::GetInstanse().GetMetaTable(TableName)->LocalListName));
		ButtonLink->setToolTip(LANG("ClickFromOpenRecord"));
		ButtonLink->setCursor(CURSOR_POINTING_HAND);
		ButtonLink->setProperty("TableName", TableName);
		ButtonLink->setProperty("ObjectID", ObjectID);
		connect(ButtonLink, &ISPushButton::clicked, this, &ISChatMessageWidget::OpenLink);
		MainLayout->addWidget(ButtonLink, 0, Qt::AlignLeft);
	}
}
//-----------------------------------------------------------------------------
ISChatMessageWidget::~ISChatMessageWidget()
{

}
//-----------------------------------------------------------------------------
QString ISChatMessageWidget::GetMessage() const
{
	return LabelMessage->text();
}
//-----------------------------------------------------------------------------
void ISChatMessageWidget::OpenImage()
{
	ISGui::SetWaitGlobalCursor(true);

	ISQuery qSelectImage(QS_MESSAGE_IMAGE);
	qSelectImage.BindValue(":MessageID", MessageID);
	if (qSelectImage.ExecuteFirst())
	{
		QPixmap Pixmap;
		if (Pixmap.loadFromData(qSelectImage.ReadColumn("chat_image").toByteArray()))
		{
			ISGui::ShowImageForm(Pixmap);
		}
	}

	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISChatMessageWidget::SaveImage()
{
	QString ImagePath = ISFileDialog::GetSaveFileNameImage(this);
	if (ImagePath.length())
	{
		ISGui::SetWaitGlobalCursor(true);

		ISQuery qSelectImage(QS_MESSAGE_IMAGE);
		qSelectImage.BindValue(":MessageID", MessageID);
		if (qSelectImage.ExecuteFirst())
		{
			QPixmap Pixmap;
			if (Pixmap.loadFromData(qSelectImage.ReadColumn("chat_image").toByteArray()))
			{
				if (Pixmap.save(ImagePath))
				{
					ISGui::SetWaitGlobalCursor(false);
					ISMessageBox::ShowInformation(nullptr, LANG("Message.Information.FileSaved"), ImagePath);
				}
			}
		}

		ISGui::SetWaitGlobalCursor(false);
	}
}
//-----------------------------------------------------------------------------
void ISChatMessageWidget::OpenFile()
{
	if (ISMessageBox::ShowQuestion(this, LANG("Message.Question.OpenFileInChat"), LANG("OpeningLargeFilesMayTakeSomeTime")))
	{
		ISGui::ProcessEvents();
		ISGui::SetWaitGlobalCursor(true);

		ISQuery qSelectFile(QS_MESSAGE_FILE);
		qSelectFile.BindValue(":MessageID", MessageID);
		if (qSelectFile.ExecuteFirst())
		{
			QByteArray ByteArray = qSelectFile.ReadColumn("chat_file").toByteArray();
			QString FileName = qSelectFile.ReadColumn("chat_filename").toString();

			QFile File(ISDefines::Core::PATH_TEMP_DIR + '/' + ISSystem::GenerateUuid() + SYMBOL_POINT + QFileInfo(FileName).suffix());
			if (File.open(QIODevice::WriteOnly))
			{
				File.write(ByteArray);
				File.close();

				ISGui::OpenFile(File.fileName());
			}
		}

		ISGui::SetWaitGlobalCursor(false);
	}
}
//-----------------------------------------------------------------------------
void ISChatMessageWidget::SaveFile()
{
	QAction *Action = dynamic_cast<QAction*>(sender());
	if (Action)
	{
		QString FilePath = ISFileDialog::GetSaveFileName(this, QString(), Action->data().toString());
		if (FilePath.length())
		{
			ISGui::SetWaitGlobalCursor(true);

			ISQuery qSelectFile(QS_MESSAGE_FILE);
			qSelectFile.BindValue(":MessageID", MessageID);
			if (qSelectFile.ExecuteFirst())
			{
				QByteArray ByteFile = qSelectFile.ReadColumn("chat_file").toByteArray();

				QFile File(FilePath);
				if (File.open(QIODevice::WriteOnly))
				{
					File.write(ByteFile);
					File.close();

					ISGui::SetWaitGlobalCursor(false);
					if (ISMessageBox::ShowQuestion(nullptr, LANG("Message.Question.FileSaved")))
					{
						ISGui::OpenFile(FilePath);
					}
				}
			}

			ISGui::SetWaitGlobalCursor(false);
		}
	}
}
//-----------------------------------------------------------------------------
void ISChatMessageWidget::OpenLink()
{
	ISGui::CreateObjectForm(ISNamespace::OFT_Edit, sender()->property("TableName").toString(), sender()->property("ObjectID").toInt())->show();
}
//-----------------------------------------------------------------------------
void ISChatMessageWidget::OpenUrlLink()
{
	ISGui::OpenUrl(dynamic_cast<QLabel*>(sender())->text());
}
//-----------------------------------------------------------------------------
void ISChatMessageWidget::Cite()
{
	emit CiteMessage(MessageID);
}
//-----------------------------------------------------------------------------
void ISChatMessageWidget::SelectMessage()
{
	LabelMessage->setSelection(0, LabelMessage->text().length());
	LabelMessage->setFocus();
}
//-----------------------------------------------------------------------------
void ISChatMessageWidget::CopyMessage()
{
	QApplication::clipboard()->setText(LabelMessage->text());
	ISNotificationService::ShowNotification(LANG("MessageCopyToClipboard"));
}
//-----------------------------------------------------------------------------
