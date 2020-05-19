#pragma once
#ifndef _PTTHREADCARAT_H_INCLUDED
#define _PTTHREADCARAT_H_INCLUDED
//-----------------------------------------------------------------------------
#include <QtCore/QThread>
//-----------------------------------------------------------------------------
class PTThreadCarat : public QThread
{
	Q_OBJECT

signals:
	void StateChanged(bool);

public:
	PTThreadCarat(QObject *parent = 0);
	virtual ~PTThreadCarat();

	QString GetErrorString() const;
	void Start(quint16 port);

protected:
	void run() override;

private:
	QString ErrorString;
	quint16 Port;
};
//-----------------------------------------------------------------------------
#endif
