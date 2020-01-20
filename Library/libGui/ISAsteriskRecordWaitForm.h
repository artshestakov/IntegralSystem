#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISAsteriskRecordWaitForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISAsteriskRecordWaitForm(const QString &unique_id, QWidget *parent = 0);
	virtual ~ISAsteriskRecordWaitForm();

	QString GetFilePath() const;

protected:
	void closeEvent(QCloseEvent *e);
	void AfterShowEvent() override;
	void Connect();
	void Connected();
	void Error(QAbstractSocket::SocketError ErrorHost);
	void ReadyRead();

private:
	QTcpSocket *TcpSocket;
	QByteArray Buffer;
	int Size;
	QString UniqueID;
	QString FilePath;

	QLabel *Label;
	QProgressBar *ProgressBar;

};
//-----------------------------------------------------------------------------
