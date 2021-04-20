#pragma once
#ifndef _ISINTERFACEFORM_H_INCLUDED
#define _ISINTERFACEFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isgui_global.h"
//-----------------------------------------------------------------------------
//!Базовый класс формы интерфейса
class ISGUI_EXPORT ISInterfaceForm : public QWidget
{
    Q_OBJECT

        Q_PROPERTY(QString FormUID READ GetFormUID)

public:
    ISInterfaceForm(QWidget *parent = 0, QFlags<Qt::WindowType> Flags = 0);
    virtual ~ISInterfaceForm();

    void ForbidResize(); //Запретить изменять размер формы
    void SetVisibleShadow(bool Visible); //Изменить видимость тени
    QString GetFormUID() const; //Получить уникальный идентификатор формы

protected:
    virtual void showEvent(QShowEvent *e);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void keyPressEvent(QKeyEvent *KeyEvent);

    QVBoxLayout* GetMainLayout(); //Получить указатель на главный компоновщик формы
    bool IsShowed() const;

    void FlashingStart(int Interval, const QColor &Color); //Запуск анимации заднего фона формы
    void FlashingTimeout(); //Событие изменения цвета анимации заднего фона формы
    void FlashingStop(); //Остановка анимации заднего фона формы

    virtual void AfterShowEvent();
    virtual void SetWindowState();
    virtual void EscapeClicked();
    virtual void EnterClicked();
    virtual void ControlEnterClicked();
    virtual void PasteClicked();

private:
    QVBoxLayout *MainLayout;
    QLabel *LabelShadow;
    QTimer *FlashingTimer;
    bool ShowedFlag;
    QString FormUID;
};
//-----------------------------------------------------------------------------
#endif
