#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISSplashScreen : public QWidget
{
    Q_OBJECT

public:
    ISSplashScreen(const QString &Message = QString());
    virtual ~ISSplashScreen();

    void SetMessage(const QString &Message); //Изменить текст сообщения

private:
    QLabel *LabelText;
};
//-----------------------------------------------------------------------------
