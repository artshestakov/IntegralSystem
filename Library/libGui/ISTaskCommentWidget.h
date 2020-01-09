#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISTaskCommentWidget : public QWidget
{
	Q_OBJECT

public:
	ISTaskCommentWidget(const QDateTime &CreationDate, const QString &UserFullName, const QString &Comment, QWidget *parent = 0);
	virtual ~ISTaskCommentWidget();
};
//-----------------------------------------------------------------------------
