#pragma once
#ifndef _ISSYSTEM_H_INCLUDED
#define _ISSYSTEM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISNamespace.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISSystem
{
public:

//ОСНОВНЫЕ
    static QDomElement GetDomElement(QFile &File); //Получить элемент файла схемы из файла
	static QDomElement GetDomElement(const QString &Content); //Получить элемент файла схемы из контента
	static bool CreateDir(const QString &DirPath);
	static bool CreateDir(const QString &DirPath, QString &ErrorString);

//КОНВЕРТАЦИЯ
    static QVariantMap JsonStringToVariantMap(const QString &JsonString, QJsonParseError &JsonParseError); //Конвертация JSON-строки в список ключ-значение
	static QByteArray VariantMapToJsonString(const QVariantMap &VariantMap, QJsonDocument::JsonFormat Format = QJsonDocument::Indented); //Конвертация списка ключ-значение в JSON-строку
	static QByteArray VariantListToJsonString(const QVariantList &VariantList, QJsonDocument::JsonFormat Format = QJsonDocument::Indented); //Конвертация списка в JSON-строку
};
//-----------------------------------------------------------------------------
#endif
