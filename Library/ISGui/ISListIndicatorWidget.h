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

    void SetVisibleAnimation(bool Visible); //�������� �������� � � ���������
    void SetIcon(const QIcon &Icon); //�������� ��������
    void SetText(const QString &Text); //�������� �����

private:
    ISWaitWidget *WaitWidget;
    QLabel *LabelImage;
    QLabel *LabelText;
};
//-----------------------------------------------------------------------------
