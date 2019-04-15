#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISWaitWidget.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISListIndicatorWidget : public QWidget
{
	Q_OBJECT

public:
	ISListIndicatorWidget(QWidget *parent);
	virtual ~ISListIndicatorWidget();

	void hideEvent(QHideEvent *e);

	void SetVisibleAnimation(bool Visible); //�������� �������� � � ���������
	void SetPixmap(const QPixmap &Pixmap); //�������� ��������
	void SetText(const QString &Text); //�������� �����

private:
	ISWaitWidget *WaitWidget;
	QLabel *LabelImage;
	QLabel *LabelText;
};
//-----------------------------------------------------------------------------
