#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISLabels.h"
//-----------------------------------------------------------------------------
class ISChatMessageWidget : public QWidget
{
	Q_OBJECT

signals:
	void CiteMessage(int ID);

public:
	ISChatMessageWidget(int message_id, QWidget *parent = 0);
	virtual ~ISChatMessageWidget();

	QString GetMessage() const;

protected:
	void OpenImage();
	void SaveImage();
	void OpenFile();
	void SaveFile();
	void OpenLink();
	void OpenUrlLink();
	void Cite(); //Цитировать сообщение
	void SelectMessage(); //Выделить сообщение
	void CopyMessage(); //Скопировать сообщение

private:
	int MessageID;
	ISQLabel *LabelMessage;
};
//-----------------------------------------------------------------------------
