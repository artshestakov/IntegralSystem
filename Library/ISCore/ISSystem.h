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
	static void SetObjectProperty(QObject *Object, const char *PropertyName, const QVariant &PropertyValue); //Изменить значние свойства объекта
    static QDomElement GetDomElement(QFile &File); //Получить элемент файла схемы из файла
	static QDomElement GetDomElement(const QString &Content); //Получить элемент файла схемы из контента
	static void SleepMilliseconds(unsigned long Milliseconds); //Задержка потока (в миллисекундах)
	static void SleepSeconds(unsigned long Seconds); //Задержка потока (в секундах)
	static QString GetSizeDir(const QString &DirPath); //Получить размер файлов в папке
	static QString FormatQFuncInfo(const QString &QFuncInfo, ISNamespace::FunctionNameFormat Format); //Форматировать результат макроса Q_FUNC_INFO
	static QString GetCurrentDayOfWeekName(); //Получить название текущего дня недели
	static QString GetDayOfWeekName(Qt::DayOfWeek Day); //Получить название дня
	static QString GetConfigurationName(); //Получить текущую конфигурацию
	static void ClearDirRecursive(const QString &DirPath); //Очистить папку рекурсивно (с подпапками)
	static void RemoveLastSymbolLoop(QString &String, char Char); //Удалить символ в конце если существует
	static ISUuid GenerateUuid(); //Сгенерировать новый UID
	static void BeginSymbolToUpper(QString &String); //Преобразование первого символа строки в верхний регистр
	static bool CheckExistSlot(QObject *Object, const QString &SlotName); //Проверить наличие слота в объекте
	static void ExecLoop(unsigned long Milliseconds); //Остановка потока событий
	static bool AddressIsList(const QString &AddressString); //Проверка строки адреса на ввод по справочнику
	static bool CreateDir(const QString &DirPath);
	static bool CreateDir(const QString &DirPath, QString &ErrorString);

//ДАТА И ВРЕМЯ
	static int TimeFromMinutes(const QTime &Time); //Перевести время в минуты
	static QString MillisecondsToString(int Milliseconds); //Перевести милисекунды в строку

//КОНВЕРТАЦИЯ
	static QString StringToBase64(const QString &String); //Конвертация строки в Base64
	static QString Base64ToString(const QString &Base64); //Конвертация Base64 в строку
	static QString SizeToString(const QSize &Size); //Конвертация размера в строку
	static double MillisecondToSecond(int Milliseconds); //Конвертация милисекунд в секунды
    static QVariantMap JsonStringToVariantMap(const QString &JsonString, QString *ErrorString = nullptr); //Конвертация JSON-строки в список ключ-значение
    static ISStringMap JsonStringToStringMap(const QString &JsonString, QString *ErrorString = nullptr); //Конвертация JSON-строки в список ключ-значение
	static QString VariantMapToJsonString(const QVariantMap &VariantMap); //Конвертация списка ключ-значение в JSON-строку
	static QString StringToMD5(const QString &String); //Получить MD5 строки

//ФАЙЛЫ
	static qint64 GetFileSize(const QString &FilePath); //Получить размер файла
	static QString FileSizeFromString(qint64 FileSize); //Получить размер файла
	static QString GetCompleteBaseFileName(const QString &FilePath); //Получить имя файла
	static QString GetFileName(const QString &FilePath); //Получить полное имя файла с расширением
	static QByteArray GetFileMD5(const QString &FilePath); //Получить хэш файла
	static QDateTime GetCreatedDateFile(const QString &FilePath); //Получить дату и время создания файла
	static QDateTime GetLastModifiedFile(const QString &FilePath); //Получить дату и время последнего изменения файла
};
//-----------------------------------------------------------------------------
#endif
