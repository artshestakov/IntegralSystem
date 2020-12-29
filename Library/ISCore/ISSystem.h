#pragma once
#ifndef _ISSYSTEM_H_INCLUDED
#define _ISSYSTEM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISNamespace.h"
#include "ISUuid.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISSystem
{
public:

//ОСНОВНЫЕ
    static QDomElement GetDomElement(QFile &File); //Получить элемент файла схемы из файла
	static QDomElement GetDomElement(const QString &Content); //Получить элемент файла схемы из контента
	static QString GetSizeDir(const QString &DirPath); //Получить размер файлов в папке
	static QString GetDayOfWeekName(Qt::DayOfWeek Day); //Получить название дня
	static void ClearDirRecursive(const QString &DirPath); //Очистить папку рекурсивно (с подпапками)
	static void BeginSymbolToUpper(QString &String); //Преобразование первого символа строки в верхний регистр
	static bool CheckExistSlot(QObject *Object, const QString &SlotName); //Проверить наличие слота в объекте
	static void ExecLoop(unsigned long Milliseconds); //Остановка потока событий
	static bool CreateDir(const QString &DirPath);
	static bool CreateDir(const QString &DirPath, QString &ErrorString);

//ДАТА И ВРЕМЯ
	static int TimeFromMinutes(const QTime &Time); //Перевести время в минуты
	static QString MillisecondsToString(int Milliseconds); //Перевести милисекунды в строку

//КОНВЕРТАЦИЯ
	static QString StringToBase64(const QString &String); //Конвертация строки в Base64
	static QString Base64ToString(const QString &Base64); //Конвертация Base64 в строку
    static QVariantMap JsonStringToVariantMap(const QString &JsonString, QJsonParseError &JsonParseError); //Конвертация JSON-строки в список ключ-значение
	static QByteArray VariantMapToJsonString(const QVariantMap &VariantMap, QJsonDocument::JsonFormat Format = QJsonDocument::Indented); //Конвертация списка ключ-значение в JSON-строку
	static QByteArray VariantListToJsonString(const QVariantList &VariantList, QJsonDocument::JsonFormat Format = QJsonDocument::Indented); //Конвертация списка в JSON-строку
	static QString StringToMD5(const QString &String); //Получить MD5 строки
	static QString StringToSha256(const QString &String); //Получить sha256 строки

//ФАЙЛЫ
	static QString FileSizeFromString(qint64 FileSize); //Получить размер файла
};
//-----------------------------------------------------------------------------
#endif
