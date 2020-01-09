#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISPopupMessage : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(float PopupOpacity READ GetPopupOpacity WRITE SetPopupOpacity)

signals:
	void Clicked();

public:
	explicit ISPopupMessage(QWidget *parent = 0);
	virtual ~ISPopupMessage();

	void SetPopupOpacity(float Opacity);
	float GetPopupOpacity() const;

	void SetTitleAlignment(Qt::Alignment Alignment);
	void SetMessageAlignment(Qt::Alignment Alignment);

public slots:
	void SetTitle(const QString &Text);
	void SetMessage(const QString &Message);
	void show();

protected:
	void paintEvent(QPaintEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);

private slots:
	void HideAnimation();
	void hide();

private:
	QLabel *LabelTitle;
	QLabel *LabelMessage;
	QGridLayout *GridLayout;
	QPropertyAnimation *PropertyAnimation;
	float PopupOpacity;
	QTimer *Timer;
};
//-----------------------------------------------------------------------------
