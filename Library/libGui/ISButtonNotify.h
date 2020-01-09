#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISButtonNotify : public QToolButton
{
	Q_OBJECT

public:
	ISButtonNotify(QWidget *parent = 0);
	virtual ~ISButtonNotify();

	void SetCountNotify(int Count);

protected:
	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);

	void Clicked();
	void NotifyEvent(const QVariantMap &VariantMap);
	void Timeout();

private:
	int NotifyCount;
	QIcon IconNull;
	QIcon IconNullActive;
	QIcon IconNew;
	QIcon IconNewActive;
	QTimer *Timer;
	bool IsBlink;
	QString StyleWhite;
	QString StyleBlink;
};
//-----------------------------------------------------------------------------
