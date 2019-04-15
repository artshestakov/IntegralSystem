#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISTelephony : public QObject
{
	Q_OBJECT

public:
    ISTelephony();
    virtual ~ISTelephony();

    static bool CheckSetUp(); //Проверка настроена ли телефония
};
//-----------------------------------------------------------------------------
