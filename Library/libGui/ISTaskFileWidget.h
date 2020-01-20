#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISTaskFileWidget : public QWidget
{
	Q_OBJECT

public:
	ISTaskFileWidget(const QDateTime &DateTime, const QString &UserName, const QString &FileName, const QString &Size, const QByteArray &Icon, const QString &Note, QWidget *parent = 0);
	virtual ~ISTaskFileWidget();
};
//-----------------------------------------------------------------------------
