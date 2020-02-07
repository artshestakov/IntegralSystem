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
	void LoadMessages(int Limit); //�������� ���������
	void SendMessage(); //�������� ���������
	void AttachImage(); //������������ ����������� � ���������
	void AttachFile(); //������������ ����� � ���������
	void ClearAttach(); //������� ������������� �������
	void CiteMessage(int MessageID); //���������� ���������
	void CreateItemWidget(int MessageID);
	QString RemoveLastSpace(const QString &Message) const; //�������� �������� � ����� ���������
	
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
