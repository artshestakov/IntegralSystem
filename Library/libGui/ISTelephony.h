#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISTelephony : public QObject
{
	Q_OBJECT

public:
    ISTelephony();
    virtual ~ISTelephony();

    static bool CheckSetUp(); //Проверка настроена ли телефония
};
//-----------------------------------------------------------------------------
