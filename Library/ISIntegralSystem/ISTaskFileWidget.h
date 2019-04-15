#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISTaskFileWidget : public QWidget
{
	Q_OBJECT

public:
	ISTaskFileWidget(const QDateTime &DateTime, const QString &UserName, const QString &FileName, const QString &Size, const QByteArray &Icon, const QString &Note, QWidget *parent = 0);
	virtual ~ISTaskFileWidget();
};
//-----------------------------------------------------------------------------
