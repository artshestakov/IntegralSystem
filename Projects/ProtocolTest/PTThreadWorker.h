#pragma once
#ifndef _PTTHREADWORKER_H_INCLUDED
#define _PTTHREADWORKER_H_INCLUDED
//-----------------------------------------------------------------------------
#include <QtCore/QThread>
//-----------------------------------------------------------------------------
class PTThreadWorker : public QThread
{
	Q_OBJECT

signals:
	void StateChanged(bool);

public:
	PTThreadWorker(quint16 port, const QString &token, const QString &login, const QString &password, QObject *parent = 0);
	virtual ~PTThreadWorker();

	QString GetErrorString() const;
	void Start(quint16 port);

protected:
	void run() override;

private:
	QString ErrorString;
	quint16 Port;
	QString Token;
	QString Login;
	QString Password;
};
//-----------------------------------------------------------------------------
#endif
