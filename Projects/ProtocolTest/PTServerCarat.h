#pragma once
#ifndef _PTSERVERCARAT_H_INCLUDED
#define _PTSERVERCARAT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTcpServerCarat.h"
//-----------------------------------------------------------------------------
class PTServerCarat : public ISTcpServerCarat
{
	Q_OBJECT

public:
	PTServerCarat(QObject *parent = 0);
	virtual ~PTServerCarat();

protected:
	bool StartWorker(QTcpSocket *TcpSocket, const QString &Port, const QString &Login, const QString &Password, const QString &Key) override;
};
//-----------------------------------------------------------------------------
#endif
