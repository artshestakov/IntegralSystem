#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISWaitWidget.h"
//-----------------------------------------------------------------------------
class ISListIndicatorWidget : public QWidget
{
	Q_OBJECT

public:
	ISListIndicatorWidget(QWidget *parent);
	virtual ~ISListIndicatorWidget();

	void hideEvent(QHideEvent *e);

	void SetVisibleAnimation(bool Visible); //Изменить анимацию и её видимость
	void SetPixmap(const QPixmap &Pixmap); //Изменить картинку
	void SetText(const QString &Text); //Изменить текст

private:
	ISWaitWidget *WaitWidget;
	QLabel *LabelImage;
	QLabel *LabelText;
};
//-----------------------------------------------------------------------------
