#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
#include "ISUuid.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class ISSystem
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
	static bool IsConfigurationDebug(); //Является ли текущая конфигурация отладочной
	static void ClearDirRecursive(const QString &DirPath); //Очистить папку рекурсиво (с подпапками)
    static bool LoadModule(const QString &ModulePath, QString &ErrorString); //Загрузить модуль в память
	static void RemoveLastSymbolFromString(QString &String, int CountSymbols = 1); //Удалить последние символы из строки
	static bool OpenFolder(const QString &FolderPath); //Открыть дикекторию
	static void RepaintWidget(QWidget *Widget, bool adjust_size = true); //Перерисовать указанный в параметре виджет
	static void ProcessEvents();
	static void MoveWidgetToDesktop(QWidget *Widget, ISNamespace::MoveWidgetDesktop MovePosition); //Перемещение виджета по рабочему столу
	static void SetWaitGlobalCursor(bool Wait); //Изменить глобальный курсор для ожидания
	static ISNamespace::ApplicationType GetApplicationType(); //Получить тип запущенного приложения
	static ISUuid GenerateUuid(); //Сгенерировать новый UID
	static QString PrepareLongToolTip(QString ToolTipText, int MaxWidth = 250); //Правка текста высплывающей подсказки (для переноса слов на новую строку)
	static void BeginSymbolToUpper(QString &String); //Преобразование первого символа строки в верхний регистр
	static QString GetCurrentOSName(); //Получить наименование текущей операционной системы
	static ISNamespace::OSType GetCurrentOSType(); //Получить тип текущей операционной системы
	static QString GetLibraryExtension(); //Получить расширение динамической библиотеки для текущей операционной системы
	static bool CheckExistSlot(QObject *Object, const QString &SlotName);
	static bool LoadResource(); //Загрузка библиотеки ресурсов в память
	static void ExecLoop(unsigned long Milliseconds); //Остановка потока событий
	static bool AddressIsList(const QString &AddressString); //Проверка строки адреса на ввод по справочнику
	static bool CheckPressCapsLook(); //Проверить активность кнопки CapsLook
	static bool IsStringUrl(const QString &Url); //Проверить является ли строка ссылкой
	static QString GetCurrentLayoutName(); //Получить наименование текущей раскладки клавиатуры

//ДАТА И ВРЕМЯ
	static int TimeFromMinutes(const QTime &Time); //Перевести время в минуты
	static QString MillisecondsToString(int Milliseconds); //Перевести милисекунды в строку

//ШРИФТЫ
	static void SetFontWidgetUnderline(QWidget *Widget, bool Underline);
	static void SetFontWidgetItalic(QWidget *Widget, bool Italic);
	static void SetFontWidgetBold(QWidget *Widget, bool Bold);

//КОНВЕРТАЦИЯ
	static QString StringToBase64(const QString &String); //Конвертация строки в Base64
	static QString Base64ToString(const QString &Base64); //Конвертация Base64 в строку
	static QFont StringToFont(const QString &FontText); //Конвертация строки в шрифт, если передается пустая строка, возвращается глобальный шрифт приложения
	static QByteArray IconToByteArray(const QIcon &Icon); //Конвертация икноки в массив байт
	static QIcon ByteArrayToIcon(const QByteArray &ByteArray); //Конвертация массива байт в икноку
	static QByteArray PixmapToByteArray(const QPixmap &Pixmap); //Конвертация изображения Pixmap в массив байтов
	static QPixmap ByteArrayToPixmap(const QByteArray &ByteArray); //Конвертация массива байтов в изображение Pixmap
	static QString SizeToString(const QSize &Size); //Конвертация размера в строку
	static QColor StringToColor(const QString &String); //Конвертация строки в цвет
	static double MillisecondToSecond(int Milliseconds); //Конвертация милисекунд в секунды
	static QVariantMap JsonStringToVariantMap(const QString &JsonString); //Конвертация JSON-строки в список ключ-значение
	static QStringMap JsonStringToStringMap(const QString &JsonString); //Конвертация JSON-строки в список ключ-значение
	static QString VariantMapToJsonString(const QVariantMap &VariantMap); //Конвертация списка ключ-значение в JSON-строку
	static QString StringFromMD5(const QString &String); //Получить MD5 строки

//ФАЙЛЫ
	static qint64 GetFileSize(const QString &FilePath); //Получить размер файла
	static QString FileSizeFromString(qint64 FileSize); //Получить размер файла
	static QString GetCompleteBaseFileName(const QString &FilePath); //Получить имя файла
	static QString GetFileName(const QString &FilePath); //Получить полное имя файла с расширением
	static bool OpenUrl(const QString &Url); //Открыть ссылку в браузере по умолчанию
	static bool OpenFile(const QString &FilePath); //Открыть файл приложением по умолчанию
	static QIcon GetIconFile(const QString &FilePath); //Получить иконку файла
	static QByteArray GetFileMD5(const QString &FilePath); //Получить хэш файла
	static QDateTime GetCreatedDateFile(const QString &FilePath); //Получить дату и время создания файла
	static QDateTime GetLastModifiedFile(const QString &FilePath); //Получить дату и время последнего изменения файла
};
//-----------------------------------------------------------------------------
