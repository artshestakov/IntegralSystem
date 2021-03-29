#pragma once
#ifndef _ISNAMESPACE_H_INCLUDED
#define _ISNAMESPACE_H_INCLUDED
//-----------------------------------------------------------------------------
namespace ISNamespace
{
    enum class LogMessageType //Типы сообщений логгера
    {
        Unknown, //Неизвестное сообщение
        Assert, //Ассерт
        Critical, //Критическая ошибка
        Error, //Ошибка
        Warning, //Предупреждение
        Info, //Информация
        Debug, //Отладка
        Trace //Трассировка
    };
};
//-----------------------------------------------------------------------------
#endif
