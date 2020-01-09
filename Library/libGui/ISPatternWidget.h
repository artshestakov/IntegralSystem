#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISQLabel.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISPatternWidget : public QFrame
{
	Q_OBJECT

public:
	ISPatternWidget(QWidget *parent = 0);
	virtual ~ISPatternWidget();

	void SetIcon(const QIcon &Icon);
	void SetPattern(const QString &Pattern);
	void SetBackgroundColor(const QString &BackgroundColor);

private:
	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);

private:
	QLabel *LabelIcon;
	QLabel *LabelPattern;
};
//-----------------------------------------------------------------------------
