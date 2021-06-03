#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISPopupMessage : public QWidget
{
    Q_OBJECT

        Q_PROPERTY(float PopupOpacity READ GetPopupOpacity WRITE SetPopupOpacity)

        signals:
    void Clicked();

public:
    ISPopupMessage(const QString &Title, const QString &Message, QWidget *parent = 0);
    ISPopupMessage(const QString &Message, QWidget *parent = 0);
    ISPopupMessage(QWidget *parent = 0);
    virtual ~ISPopupMessage();

    static void ShowNotification(const QString &Title, const QString &Message);
    static void ShowNotification(const QString &Mesage);

    void SetPopupOpacity(float Opacity);
    float GetPopupOpacity() const;

    void SetTitleAlignment(Qt::Alignment Alignment);
    void SetMessageAlignment(Qt::Alignment Alignment);

    void SetTitle(const QString &Text);
    void SetMessage(const QString &Message);
    void show();

protected:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *MouseEvent);
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
