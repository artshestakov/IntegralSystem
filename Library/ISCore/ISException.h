#pragma once
#ifndef _ISEXCEPTION_H_INCLUDED
#define _ISEXCEPTION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
//Базовый класс обработки исключений
class ISCORE_EXPORT ISExceptionBase : public std::exception
{
public:
    ISExceptionBase(const QString &ExceptionString = QString());
    virtual ~ISExceptionBase();

    QString What() const;

private:
    QString Description;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//Класс для обработки исключений потери соединения с БД
class ISCORE_EXPORT ISExceptionConnectionDB : public ISExceptionBase
{
public:
    ISExceptionConnectionDB(const QString &ExceptionString = QString());
    virtual ~ISExceptionConnectionDB();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//Класс для обработки исключения синтаксиса SQL-запросов
class ISCORE_EXPORT ISExceptionSqlSyntax : public ISExceptionBase
{
public:
    ISExceptionSqlSyntax(const QString &ExceptionString = QString());
    virtual ~ISExceptionSqlSyntax();
};
//-----------------------------------------------------------------------------
#endif
