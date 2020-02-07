#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISParagraphBaseForm.h"
#include "ISTextEdit.h"
#include "ISNamespace.h"
#include "ISButtons.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISChatForm : public ISParagraphBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISChatForm(QWidget *parent = 0);
	virtual ~ISChatForm();

	void Invoke() override;

protected:
	void LoadMessages(int Limit); //Загрузка сообщений
	void SendMessage(); //Отправка сообщения
	void AttachImage(); //Прикрепление изображения к сообщению
	void AttachFile(); //Прикрепление файла к сообщению
	void ClearAttach(); //Очитска прикрпленного объекта
	void CiteMessage(int MessageID); //Цитировать сообщение
	void CreateItemWidget(int MessageID);
	QString RemoveLastSpace(const QString &Message) const; //Удаление пробелов в конце сообщения
	
	QVariant GetImage() const;
	QVariant GetFile() const;
	QString GetFileName() const;

	void NewMessage(const QVariantMap &VariantMap);

private:
	QButtonGroup *ButtonGroup;
	ISListWidget *ListWidget;
	ISTextEdit *TextEdit;
	QLabel *LabelLoading;
	QLabel *LabelAttachPath;

	QAction *ActionAttachImage;
	QAction *ActionAttachFile;
	QAction *ActionClearAttach;
	ISPushButton *ButtonSend;

	ISNamespace::AttachChatType CurrentAttach;
	int CountMessage;

	QIcon IconChat;
	QIcon IconNewMessage;
};
//-----------------------------------------------------------------------------
