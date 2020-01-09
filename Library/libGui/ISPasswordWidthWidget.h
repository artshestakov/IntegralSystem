#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISPasswordWidthWidget : public QWidget
{
	Q_OBJECT

public:
	ISPasswordWidthWidget(QWidget *parent = 0);
	virtual ~ISPasswordWidthWidget();

	void SetPassword(const QVariant &Password);

private:
	QLabel *LabelWidth;
	QProgressBar *ProgressBar;
	QPropertyAnimation *PropertyAnimation;
};
//-----------------------------------------------------------------------------
