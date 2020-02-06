#pragma once
#ifndef _ISPROTOCOL_H_INCLUDED
#define _ISPROTOCOL_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"

//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISProtocol
{
public:
	static void EnterApplication(); //Вход в программу
	static void ExitApplication(); //Выход из программы

	static void OpenSubSystem(const QString &TableName, const QString &LocalListName); //Открытие подсистемы

	static void CreateObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &ObjectName); //Создание записи
	static void CreateCopyObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &ObjectName); //Создание копии записи
	static void EditObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &ObjectName); //Редактирование записи
	static void ShowObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &ObjectName); //Просмотр записи
	
	static void DeleteObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID); //Удаление записи
	static void RecoveryObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID); //Восстановление записи
	static void DeleteCascadeObject(const QString &TableName, const QString &LocalListName, const QVariant &ObjectID); //Каскадное удаление записи

	static int Insert(bool Thread, const QString &TypeUID, const QString &TableName, const QString &LocalListName, const QVariant &ObjectID, const QString &Information = QString());
};
//-----------------------------------------------------------------------------
#endif
