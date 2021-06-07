#pragma once
#ifndef _ISGUI_H_INCLUDED
#define _ISGUI_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISFieldEditBase.h"
#include "ISObjectFormBase.h"
#include "ISComboSearchWidgets.h"
#include "ISImageViewerForm.h"
#include "ISSplashScreen.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------

//Обработка дробного значения
#define DOUBLE_PREPAREN(d, n) QString::number(d, 'f', n)
#define DOUBLE_PREPARE(d) DOUBLE_PREPAREN(d, SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA))

//Обработка денежного значения (кол-во знаков после запятой должно быть 2)
#define DOUBLE_PREPAREM(d) QString::number(d, 'f', 2)

//-----------------------------------------------------------------------------
class ISGui
{
public:
    static bool Startup(QString &ErrorString);
    static bool Startup(ISSplashScreen *SplashScreen);
    static void Shutdown(ISSplashScreen *SplashScreen);

    static bool CheckAdminRole(); //Проверка наличия прав администратора
    static void RepaintWidget(QWidget *Widget, bool AdjustSize = true); //Перерисовать указанный в параметре виджет
    static void MoveWidgetToDesktop(QWidget *Widget, ISNamespace::MoveWidgetDesktop MovePosition); //Перемещение виджета по рабочему столу (вызывать только после вызова метода show() у виджета)
    static void SetWaitGlobalCursor(bool Wait); //Изменить глобальный курсор для ожидания
    static void SetFontWidgetUnderline(QWidget *Widget, bool Underline);
    static void SetFontWidgetItalic(QWidget *Widget, bool Italic);
    static void SetFontWidgetBold(QWidget *Widget, bool Bold);
    static void SetFontWidgetStrikeOut(QWidget *Widget, bool StrikeOut);
    static void SetFontListWidgetItemStrikeOut(QListWidgetItem *ListWidgetItem, bool StrikeOut);
    static QFont StringToFont(const QString &FontText); //Конвертация строки в шрифт, если передается пустая строка, возвращается глобальный шрифт приложения
    static QByteArray IconToByteArray(const QIcon &Icon); //Конвертация икноки в массив байт
    static QIcon ByteArrayToIcon(const QByteArray &ByteArray); //Конвертация массива байт в икноку
    static QByteArray PixmapToByteArray(const QPixmap &Pixmap); //Конвертация изображения Pixmap в массив байтов
    static QPixmap ByteArrayToPixmap(const QByteArray &ByteArray); //Конвертация массива байтов в изображение Pixmap
    static QIcon GetIconFile(const QString &FilePath); //Получить иконку файла
    static bool OpenUrl(const QString &Url); //Открыть ссылку в браузере по умолчанию
    static bool OpenFile(const QString &FilePath); //Открыть файл приложением по умолчанию
    static QString PrepareLongToolTip(QString ToolTipText, int MaxWidth = 250); //Правка текста высплывающей подсказки (для переноса слов на новую строку)
    static int GetStringWidth(const QString &String, const QFont &Font); //Получить размер строки в пикселях для шрифта
    static QColor StringToColor(const QString &String); //Конвертация строки в цвет
    static bool OpenFolder(const QString &FolderPath); //Открыть дикекторию
    static QString ConvertDateTimeToString(const QDateTime &DateTime, const QString &DateFormat, const QString &TimeFormat); //Конвертировать дату и время в строку
    static QString ConvertDateToString(const QDate &Date, const QString &DateFormat); //Конвертировать дату в строку
    static bool RecordsDelete(const QString &TableName, const ISVectorUInt &ObjectsID, QString &ErrorString); //Удалить записи
    static void FavoriteObject(const QString &TableName, unsigned int ObjectID, bool &IsExist); //Добавить/удалить объект из избранного

    static void ExitApplication(); //Выход из программы
    static void ExecLoop(unsigned long Milliseconds); //Остановка потока событий

    static ISFieldEditBase* CreateColumnForField(QWidget *ParentWidget, PMetaField *MetaField);
    static ISFieldEditBase* CreateColumnForField(QWidget *ParentWidget, ISNamespace::FieldType DataType, const QString &ControlWidget = QString());
    static ISObjectFormBase* CreateObjectForm(ISNamespace::ObjectFormType FormType, const QString &TableName, int ObjectID = 0, QWidget *parent = 0);

    static ISObjectPair SelectObject(const QString &TableName, const QString &TableLocalName, int SelectObjectID = 0); //Выбор одной записи из таблицы
    static ISVectorUInt SelectObjects(const QString &TableName, const QString &TableLocalName); //Выбор нескольких записей из таблицы

    static void ShowSettingsForm(const QString &SettingGroupUID = QString()); //Открыть форму настроек
    static void ShowUserPasswordForm(unsigned int UserID, const QString &UserFIO, const QString &UserLogin); //Открыть форму изменения пароля для пользователя
    static void ShowUserPasswordForm(unsigned int UserID, const QString &UserFIO, const QString &UserLogin, bool &Result); //Открыть форму изменения пароля для пользователя
    static void ShowObjectForm(QWidget *ObjectForm); //Показать форму объекта
    static void ShowRecordInfoForm(QWidget *parent, const QString &TableName, unsigned int ObjectID); //ПОказать форму информации о записи

    static ISImageViewerForm* ShowImageForm(const QPixmap &Pixmap); //Показать изображение
    static ISImageViewerForm* ShowImageForm(const QString &FilePath); //Показать изображение
    static ISImageViewerForm* ShowImageForm(const QByteArray &ByteArray); //Показать изображение

    static void ShowNoteObject(QWidget *parent, const QString &TableName, int ObjectID);
    static void ShowFavoritesForm(const QString &TableName = QString(), const QString &TableLocalName = QString());
    static void ShowHistoryForm();

    //static void ShowTaskViewForm(int TaskID); //Показать форму просмотра задачи по идентификатору
    //static void ShowTaskViewForm(QWidget *TaskViewForm); //Показать форму просмотра задач по виджету

    static bool CreateDir(const QString &DirPath);
    static bool CreateDir(const QString &DirPath, QString &ErrorString);

    static QVariantMap JsonStringToVariantMap(const QString &JsonString, QJsonParseError &JsonParseError); //Конвертация JSON-строки в список ключ-значение
    static QByteArray VariantMapToJsonString(const QVariantMap &VariantMap, QJsonDocument::JsonFormat Format = QJsonDocument::Indented); //Конвертация списка ключ-значение в JSON-строку
    static QByteArray VariantListToJsonString(const QVariantList &VariantList, QJsonDocument::JsonFormat Format = QJsonDocument::Indented); //Конвертация списка в JSON-строку

    static QDomElement GetDomElement(QFile &File); //Получить элемент файла схемы из файла
    static QDomElement GetDomElement(const QString &Content); //Получить элемент файла схемы из контента

    //Получить указатель на класс по его имени
    template <typename Class> static Class CreatePointer(const QString &ClassName,
        QGenericArgument val0 = QGenericArgument(nullptr),
        QGenericArgument val1 = QGenericArgument(),
        QGenericArgument val2 = QGenericArgument(),
        QGenericArgument val3 = QGenericArgument(),
        QGenericArgument val4 = QGenericArgument(),
        QGenericArgument val5 = QGenericArgument(),
        QGenericArgument val6 = QGenericArgument(),
        QGenericArgument val7 = QGenericArgument(),
        QGenericArgument val8 = QGenericArgument(),
        QGenericArgument val9 = QGenericArgument())
    {
        Class Pointer = nullptr;
        int ObjectType = QMetaType::type((ClassName + '*').toLocal8Bit().constData());
        const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
        if (ObjectType && MetaObject)
        {
            Pointer = dynamic_cast<Class>(MetaObject->newInstance(val0, val1, val2, val3, val4, val5, val6, val7, val8, val9));
        }

        if (!Pointer)
        {
            IS_ASSERT(Pointer, QString("Not created pointer with class name \"" + ClassName + "\"").toStdString());
        }
        return Pointer;
    }

private:
    static ISFieldEditBase* CreateFieldEditBase(QWidget *ParentWidget, PMetaField *MetaField = nullptr, ISNamespace::FieldType DataType = ISNamespace::FieldType::Unknown, const QString &ControlWidget = QString());

private:
    static void RegisterMetaType(); //Регистрация мета-типов
};
//-----------------------------------------------------------------------------
#endif
