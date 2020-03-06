#pragma once
#ifndef _ISEXCEPTION_H_INCLUDED
#define _ISEXCEPTION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
//������� ����� ��������� ����������
class LIBCORE_EXPORT ISExceptionBase : public std::exception
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
//����� ��� ��������� ���������� ������ ���������� � ��
class LIBCORE_EXPORT ISExceptionConnectionDB : public ISExceptionBase
{
public:
	ISExceptionConnectionDB(const QString &ExceptionString = QString());
	virtual ~ISExceptionConnectionDB();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//����� ��� ��������� ���������� ���������� SQL-��������
class LIBCORE_EXPORT ISExceptionSqlSyntax : public ISExceptionBase
{
public:
	ISExceptionSqlSyntax(const QString &ExceptionString = QString());
	virtual ~ISExceptionSqlSyntax();
};
//-----------------------------------------------------------------------------
#endif
