#include "StdAfx.h"
#include "ISAsteriskRecordWaitForm.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISButtonClose.h"
#include "ISSystem.h"
#include "ISDebug.h"
#include "ISSettingsDatabase.h"
//-----------------------------------------------------------------------------
ISAsteriskRecordWaitForm::ISAsteriskRecordWaitForm(const QString &unique_id, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	Size = 0;
	UniqueID = unique_id;

	setWindowTitle(LANG("AsteriskRecord.Interface.Title"));
	GetMainLayout()->setContentsMargins(MARGINS_LAYOUT_10_PX);
	ForbidResize();

	TcpSocket = new QTcpSocket(this);
	connect(TcpSocket, &QTcpSocket::connected, this, &ISAsteriskRecordWaitForm::Connected);
	connect(TcpSocket, static_cast<void(QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &ISAsteriskRecordWaitForm::Error);
	connect(TcpSocket, &QTcpSocket::readyRead, this, &ISAsteriskRecordWaitForm::ReadyRead);

	Label = new QLabel(this);
	Label->setText(LANG("AsteriskRecord.Interface.Label.Prepare"));
	GetMainLayout()->addWidget(Label, 0, Qt::AlignLeft);

	ProgressBar = new QProgressBar(this);
	ProgressBar->setRange(0, 0);
	ProgressBar->setTextVisible(false);
	GetMainLayout()->addWidget(ProgressBar);

	ISButtonClose *ButtonClose = new ISButtonClose(this);
	connect(ButtonClose, &ISButtonClose::clicked, this, &ISAsteriskRecordWaitForm::close);
	GetMainLayout()->addWidget(ButtonClose, 0, Qt::AlignRight);
}
//-----------------------------------------------------------------------------
ISAsteriskRecordWaitForm::~ISAsteriskRecordWaitForm()
{
	
}
//-----------------------------------------------------------------------------
QString ISAsteriskRecordWaitForm::GetFilePath() const
{
	return FilePath;
}
//-----------------------------------------------------------------------------
void ISAsteriskRecordWaitForm::closeEvent(QCloseEvent *e)
{
	ISInterfaceDialogForm::closeEvent(e);
	if (TcpSocket->isOpen())
	{
		TcpSocket->close();
	}
}
//-----------------------------------------------------------------------------
void ISAsteriskRecordWaitForm::AfterShowEvent()
{
	ISInterfaceDialogForm::AfterShowEvent();
	QTimer::singleShot(500, this, &ISAsteriskRecordWaitForm::Connect);
}
//-----------------------------------------------------------------------------
void ISAsteriskRecordWaitForm::Connect()
{
	setCursor(CURSOR_BUSY);
	Label->setText(LANG("AsteriskRecord.Interface.Label.Connecting"));
	TcpSocket->connectToHost(SETTING_DATABASE_VALUE_STRING(CONST_UID_DATABASE_SETTING_ASTERISK_SERVER), SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_ASTERISK_RECORDS_PORT));
}
//-----------------------------------------------------------------------------
void ISAsteriskRecordWaitForm::Connected()
{
	Label->setText(LANG("AsteriskRecord.Interface.Label.Connected"));
	TcpSocket->write(UniqueID.toUtf8());
}
//-----------------------------------------------------------------------------
void ISAsteriskRecordWaitForm::Error(QAbstractSocket::SocketError ErrorHost)
{
	ProgressBar->setRange(0, 100);
	Label->setText(TcpSocket->errorString());
	setCursor(CURSOR_ARROW);
}
//-----------------------------------------------------------------------------
void ISAsteriskRecordWaitForm::ReadyRead()
{
	QByteArray ByteArray = TcpSocket->readAll();
	if (ByteArray.contains("start")) //Начало загрузки файла
	{
		QStringList StringList = QString(ByteArray).split(':');
		Size = StringList.at(1).toInt();

		Label->setText(LANG("AsteriskRecord.Interface.Label.Downloading"));
		ProgressBar->setValue(0);
		ProgressBar->setMaximum(Size);
		ProgressBar->setTextVisible(true);
		setCursor(CURSOR_BUSY);
	}
	else if (ByteArray.contains("message")) //Сообщение от сервера
	{
		QStringList StringList = QString(ByteArray).split(':');
		Label->setText(LANG(StringList.at(1)));
		ProgressBar->setRange(0, 100);
		setCursor(CURSOR_ARROW);
		TcpSocket->close();
	}
	else if (ByteArray.contains(QString("end:%1").arg(UniqueID).toUtf8())) //Окончание загрузки файла
	{
		TcpSocket->close();
		Label->setText(LANG("AsteriskRecord.Interface.Label.Downloaded"));

		QFile FileRecord(PATH_TEMP_DIR + '/' + ISSystem::GenerateUuid() + SYMBOL_POINT + EXTENSION_WAV);
		if (FileRecord.open(QIODevice::WriteOnly))
		{
			FileRecord.write(Buffer);
			FileRecord.close();
			Buffer.clear();
			FilePath = FileRecord.fileName();
			SetResult(true);
			close();
		}
	}
	else //Загрузка файла
	{
		ProgressBar->setValue(ProgressBar->value() + ByteArray.size());
		Buffer.append(ByteArray);
	}
}
//-----------------------------------------------------------------------------
