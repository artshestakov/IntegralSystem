#include "ISChatForm.h"
#include "ISQuery.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMetaUser.h"
#include "ISChatMessageWidget.h"
#include "ISSystem.h"
#include "ISFileDialog.h"
#include "ISMessageBox.h"
#include "ISConstants.h"
#include "ISNotifySender.h"
#include "ISNotifyRecipient.h"
#include "ISParagraphEntity.h"
#include "ISMediaPlayer.h"
#include "ISMemoryObjects.h"
#include "ISMainWindow.h"
#include "ISUserRoleEntity.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
static QString QS_CHAT_MESSAGES = PREPARE_QUERY("SELECT chat_id "
												"FROM _chatmessages "
												"WHERE NOT chat_isdeleted "
												"AND chat_id > ((SELECT max(chat_id) FROM _chatmessages WHERE NOT chat_isdeleted) - :Limit) "
												"ORDER BY chat_id");
//-----------------------------------------------------------------------------
static QString QS_COUNT_MESSAGES = PREPARE_QUERY("SELECT COUNT(*) FROM _chatmessages WHERE NOT chat_isdeleted");
//-----------------------------------------------------------------------------
static QString QI_CHAT_MESSAGE = PREPARE_QUERY("INSERT INTO _chatmessages(chat_message, chat_image, chat_file, chat_filename) "
											   "VALUES(:Message, :Image, :File, :FileName) "
											   "RETURNING chat_id");
//-----------------------------------------------------------------------------
static QString QS_CHAT_MESSAGE_CITE = PREPARE_QUERY("SELECT chat_message "
													"FROM _chatmessages "
													"WHERE chat_id = :MessageID");
//-----------------------------------------------------------------------------
ISChatForm::ISChatForm(QWidget *parent) : ISParagraphBaseForm(parent)
{
	CurrentAttach = ISNamespace::ACT_NoAttach;
	CountMessage = 0;
	connect(&ISNotifyRecipient::GetInstance(), &ISNotifyRecipient::ChatMessage, this, &ISChatForm::NewMessage);

	QHBoxLayout *MainLayout = new QHBoxLayout();
	setLayout(MainLayout);

	QVBoxLayout *LayoutCenter = new QVBoxLayout();
	MainLayout->addLayout(LayoutCenter);

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	LayoutCenter->addLayout(LayoutTitle);

	LayoutTitle->addWidget(new QLabel(LANG("ChatForm.ShowMessages") + ':', this));

	QRadioButton *ButtonShowAll = new QRadioButton(LANG("ChatForm.AllMessages"), this);
	LayoutTitle->addWidget(ButtonShowAll);

	QRadioButton *ButtonLast10 = new QRadioButton(LANG("ChatForm.Last10Messages"), this);
	ButtonLast10->setChecked(true);
	LayoutTitle->addWidget(ButtonLast10);

	QRadioButton *ButtonLast20 = new QRadioButton(LANG("ChatForm.Last20Messages"), this);
	LayoutTitle->addWidget(ButtonLast20);

	QRadioButton *ButtonLast100 = new QRadioButton(LANG("ChatForm.Last100Messages"), this);
	LayoutTitle->addWidget(ButtonLast100);

	LayoutTitle->addStretch();

	ButtonGroup = new QButtonGroup(this);
	ButtonGroup->addButton(ButtonShowAll, 0);
	ButtonGroup->addButton(ButtonLast10, 10);
	ButtonGroup->addButton(ButtonLast20, 20);
	ButtonGroup->addButton(ButtonLast100, 100);
	connect(ButtonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &ISChatForm::LoadMessages);

	QVBoxLayout *LayouGroupBox = new QVBoxLayout();
	LayouGroupBox->setContentsMargins(QMargins(0, 1, 0, 1));

	QGroupBox *GroupBox = new QGroupBox(LANG("ChatForm.GroupBox.Title"), this);
	GroupBox->setLayout(LayouGroupBox);
	LayoutCenter->addWidget(GroupBox);

	ListWidget = new ISListWidget(this);
	ListWidget->setFrameShape(QFrame::NoFrame);
	ListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	ListWidget->setAlternatingRowColors(true);
	ListWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
	GroupBox->layout()->addWidget(ListWidget);

	QHBoxLayout *LayoutBar = new QHBoxLayout();
	LayoutCenter->addLayout(LayoutBar);
	
	LabelLoading = new QLabel(this);
	LabelLoading->setText(LANG("ChatForm.LoadingMessages") + "...");
	LabelLoading->setVisible(false);
	LayoutBar->addWidget(LabelLoading);
	
	LayoutBar->addStretch();

	LabelAttachPath = new QLabel(this);
	LayoutBar->addWidget(LabelAttachPath);

	ISPushButton *ButtonAttach = new ISPushButton(this);
	ButtonAttach->setText(LANG("ChatForm.Attach"));
	ButtonAttach->setMenu(new QMenu(ButtonAttach));
	LayoutBar->addWidget(ButtonAttach);

	ActionAttachImage = new QAction(ButtonAttach->menu());
	ActionAttachImage->setText(LANG("Image"));
	connect(ActionAttachImage, &QAction::triggered, this, &ISChatForm::AttachImage);
	ButtonAttach->menu()->addAction(ActionAttachImage);

	ActionAttachFile = new QAction(ButtonAttach->menu());
	ActionAttachFile->setText(LANG("File"));
	connect(ActionAttachFile, &QAction::triggered, this, &ISChatForm::AttachFile);
	ButtonAttach->menu()->addAction(ActionAttachFile);

	ButtonAttach->menu()->addSeparator();

	ActionClearAttach = new QAction(ButtonAttach->menu());
	ActionClearAttach->setText(LANG("ChatForm.ClearAttach"));
	ActionClearAttach->setEnabled(false);
	ButtonAttach->menu()->addAction(ActionClearAttach);
	connect(ActionClearAttach, &QAction::triggered, this, &ISChatForm::ClearAttach);

	ButtonSend = new ISPushButton(this);
	ButtonSend->setText(LANG("ChatForm.Send"));
	ButtonSend->setToolTip(LANG("ChatForm.Send.ToolTip"));
	ButtonSend->setIcon(BUFFER_ICONS("Chat.SendMessage"));
	ButtonSend->setCursor(CURSOR_POINTING_HAND);
	connect(ButtonSend, &ISPushButton::clicked, this, &ISChatForm::SendMessage);
	LayoutBar->addWidget(ButtonSend);

	TextEdit = new ISTextEdit(this);
	TextEdit->SetPlaceholderText(LANG("ChatForm.InputMessage") + "...");
	TextEdit->SetExecuteEnter(false);
	TextEdit->setMaximumHeight(80);
	connect(TextEdit, &ISTextEdit::KeyPressEnter, this, &ISChatForm::SendMessage);
	LayoutCenter->addWidget(TextEdit);

	LoadMessages(10);
}
//-----------------------------------------------------------------------------
ISChatForm::~ISChatForm()
{

}
//-----------------------------------------------------------------------------
void ISChatForm::Invoke()
{
	ISParagraphBaseForm::Invoke();

	CountMessage = 0;
	GetButtonParagraph()->SetText(QString());
	GetButtonParagraph()->SetToolTip(ISParagraphEntity::GetInstance().GetParagraph(CONST_UID_PARAGRAPH_CHAT)->ToolTip);
	GetButtonParagraph()->SetCursor(CURSOR_POINTING_HAND);

	QTimer::singleShot(50, TextEdit, &ISTextEdit::SetFocus);
}
//-----------------------------------------------------------------------------
void ISChatForm::LoadMessages(int Limit)
{
	ListWidget->Clear();
	LabelLoading->setVisible(true);
	ISGui::RepaintWidget(LabelLoading);
	ISGui::SetWaitGlobalCursor(true);

	ISQuery qSelectMessages(QS_CHAT_MESSAGES);

	if (Limit)
	{
		qSelectMessages.BindValue(":Limit", Limit);
	}
	else
	{
		ISQuery qCountMessages(QS_COUNT_MESSAGES);
		qCountMessages.ExecuteFirst();
		qSelectMessages.BindValue(":Limit", qCountMessages.ReadColumn("count"));
	}

	if (qSelectMessages.Execute())
	{
		while (qSelectMessages.Next())
		{
			int MessageID = qSelectMessages.ReadColumn("chat_id").toInt();
			CreateItemWidget(MessageID);
		}
	}

	LabelLoading->setVisible(false);
	ISGui::RepaintWidget(LabelLoading);
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISChatForm::SendMessage()
{
	if (!ISUserRoleEntity::GetInstance().CheckAccessSpecial(CONST_UID_GROUP_ACCESS_SPECIAL_SEND_MESSAGE_TO_CHAT))
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.NotAccess.Special.SendMessageToChat"));
		return;
	}

	QString Message = TextEdit->GetValue().toString();
	Message = RemoveLastSpace(Message);

	if (Message.length() || CurrentAttach == ISNamespace::ACT_Image || CurrentAttach == ISNamespace::ACT_File)
	{
		ButtonSend->setText(LANG("ChatForm.Sending") + "...");
		ISGui::RepaintWidget(ButtonSend);
		ISGui::SetWaitGlobalCursor(true);

		QVariant VariantImage = GetImage();
		QVariant VariantFile = GetFile();
		QString FileName = GetFileName();

		ISQuery qInsertMessage(QI_CHAT_MESSAGE);
		qInsertMessage.BindValue(":Message", Message);
		qInsertMessage.BindValue(":Image", VariantImage);
		qInsertMessage.BindValue(":File", VariantFile);
		qInsertMessage.BindValue(":FileName", FileName);
		if (qInsertMessage.ExecuteFirst())
		{
			int MessageID = qInsertMessage.ReadColumn("chat_id").toInt();
			CreateItemWidget(MessageID);
			ISNotifySender::GetInstance().SendToAll(CONST_UID_NOTIFY_NEW_CHAT_MESSAGE, MessageID, ISMetaUser::Instance().UserData->FullName + ": " + Message, false);

			TextEdit->Clear();
			ClearAttach();

			ISMediaPlayer::GetInstance().Play(BUFFER_AUDIO("Chat.SendMessage"));
		}

		ButtonSend->setText(LANG("ChatForm.Send"));
		ISGui::SetWaitGlobalCursor(false);
	}
	else
	{
		TextEdit->SetFocus();
	}
}
//-----------------------------------------------------------------------------
void ISChatForm::AttachImage()
{
	QString ImagePath = ISFileDialog::GetOpenFileNameImage(this);
	if (ImagePath.length())
	{
		qint64 Kbytes = ISSystem::GetFileSize(ImagePath) / 1024;
		if (Kbytes > 5120) //5mb
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.ChatImageSizeConstraint"));
		}
		else
		{
			LabelAttachPath->setText(ImagePath);
			ActionClearAttach->setEnabled(true);

			CurrentAttach = ISNamespace::ACT_Image;
		}
	}
}
//-----------------------------------------------------------------------------
void ISChatForm::AttachFile()
{
	QString FilePath = ISFileDialog::GetOpenFileName(this);
	if (FilePath.length())
	{
		qint64 Kbytes = ISSystem::GetFileSize(FilePath) / 1024;
		if (Kbytes > 15360) //15mb
		{
			ISMessageBox::ShowWarning(this, LANG("Message.Warning.ChatFileSizeConstraint"));
		}
		else
		{
			LabelAttachPath->setText(FilePath);
			ActionClearAttach->setEnabled(true);

			CurrentAttach = ISNamespace::ACT_File;
		}
	}
}
//-----------------------------------------------------------------------------
void ISChatForm::ClearAttach()
{
	LabelAttachPath->clear();
	ActionClearAttach->setEnabled(false);

	CurrentAttach = ISNamespace::ACT_NoAttach;
}
//-----------------------------------------------------------------------------
void ISChatForm::CiteMessage(int MessageID)
{
	ISGui::SetWaitGlobalCursor(true);

	ISQuery qSelect(QS_CHAT_MESSAGE_CITE);
	qSelect.BindValue(":MessageID", MessageID);
	if (qSelect.ExecuteFirst())
	{
		TextEdit->SetValue('[' + qSelect.ReadColumn("chat_message").toString() + ']');
		TextEdit->SetFocus();
	}

	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISChatForm::CreateItemWidget(int MessageID)
{
	QListWidgetItem *ListWidgetItem = new QListWidgetItem(ListWidget);

	ISChatMessageWidget *ChatMessageWidget = new ISChatMessageWidget(MessageID, ListWidget);
	connect(ChatMessageWidget, &ISChatMessageWidget::CiteMessage, this, &ISChatForm::CiteMessage);
	ListWidget->setItemWidget(ListWidgetItem, ChatMessageWidget);

	ChatMessageWidget->adjustSize();
	ListWidgetItem->setSizeHint(ChatMessageWidget->sizeHint());
	ListWidgetItem->setSelected(true);
	ListWidget->scrollToBottom();
}
//-----------------------------------------------------------------------------
QString ISChatForm::RemoveLastSpace(const QString &Message) const
{
	QString Result = Message;
	if (Result.length())
	{
		while (true)
		{
			QChar LastSymbol = Result.at(Result.count() - 1);
			if (LastSymbol == SYMBOL_SPACE)
			{
				ISSystem::RemoveLastSymbolFromString(Result);
			}
			else
			{
				break;
			}
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
QVariant ISChatForm::GetImage() const
{
	if (CurrentAttach == ISNamespace::ACT_Image)
	{
		QString ImagePath = LabelAttachPath->text();

		QPixmap Pixmap(ImagePath);
		QByteArray ByteArray;
		QBuffer Buffer(&ByteArray);
		Buffer.open(QIODevice::WriteOnly);
		Pixmap.save(&Buffer, QImageReader(ImagePath).format());
		return ByteArray;
	}

	return QVariant();
}
//-----------------------------------------------------------------------------
QVariant ISChatForm::GetFile() const
{
	if (CurrentAttach == ISNamespace::ACT_File)
	{
		QFile File(LabelAttachPath->text());
		if (File.open(QIODevice::ReadOnly))
		{
			QByteArray ByteArray = File.readAll();
			return ByteArray;
		}
	}

	return QVariant();
}
//-----------------------------------------------------------------------------
QString ISChatForm::GetFileName() const
{
	if (CurrentAttach == ISNamespace::ACT_File)
	{
		return ISSystem::GetFileName(LabelAttachPath->text());
	}

	return QString();
}
//-----------------------------------------------------------------------------
void ISChatForm::NewMessage(const QVariantMap &VariantMap)
{
	CreateItemWidget(VariantMap.value("Payload").toInt());
	
	ISMainWindow *MainWindow = dynamic_cast<ISMainWindow*>(ISMemoryObjects::GetInstance().GetMainWindow());
	if (MainWindow->GetCurrentParagraphUID() != CONST_UID_PARAGRAPH_CHAT)
	{
		++CountMessage;
		GetButtonParagraph()->SetText(QString("(+%1)").arg(CountMessage));
		GetButtonParagraph()->SetToolTip(LANG("ChatForm.UnreadMessages"));
		GetButtonParagraph()->SetCursor(CURSOR_WHATS_THIS);
	}
}
//-----------------------------------------------------------------------------
