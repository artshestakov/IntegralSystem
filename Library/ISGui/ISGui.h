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
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISGui
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

    static ISFieldEditBase* CreateColumnForField(QWidget *ParentWidget, PMetaField *MetaField);
    static ISFieldEditBase* CreateColumnForField(QWidget *ParentWidget, ISNamespace::FieldType DataType, const QString &ControlWidget = QString());
    static ISObjectFormBase* CreateObjectForm(ISNamespace::ObjectFormType FormType, const QString &TableName, int ObjectID = 0, QWidget *parent = 0);

    static ISObjectPair SelectObject(const QString &TableName, int SelectObjectID = 0); //Выбор одной записи из таблицы
    static ISVectorUInt SelectObjects(const QString &TableName); //Выбор нескольких записей из таблицы

    static void ShowSettingsForm(const QString &SettingGroupUID = QString()); //Открыть форму настроек
    static void ShowUserPasswordForm(unsigned int UserID, const QString &UserFIO, const QString &UserLogin); //Открыть форму изменения пароля для пользователя
    static void ShowUserPasswordForm(unsigned int UserID, const QString &UserFIO, const QString &UserLogin, bool &Result); //Открыть форму изменения пароля для пользователя
    static void ShowObjectForm(QWidget *ObjectForm); //Показать форму объекта
    static void ShowRecordInfoForm(QWidget *parent, const QString &TableName, unsigned int ObjectID); //ПОказать форму информации о записи

    static ISImageViewerForm* ShowImageForm(const QPixmap &Pixmap); //Показать изображение
    static ISImageViewerForm* ShowImageForm(const QString &FilePath); //Показать изображение
    static ISImageViewerForm* ShowImageForm(const QByteArray &ByteArray); //Показать изображение

    static void ShowNoteObject(QWidget *parent, const QString &TableName, int ObjectID);
    static void ShowFavoritesForm(const QString &TableName = QString());
    static void ShowHistoryForm();

    //static void ShowTaskViewForm(int TaskID); //Показать форму просмотра задачи по идентификатору
    //static void ShowTaskViewForm(QWidget *TaskViewForm); //Показать форму просмотра задач по виджету

private:
    static ISFieldEditBase* CreateFieldEditBase(QWidget *ParentWidget, PMetaField *MetaField = nullptr, ISNamespace::FieldType DataType = ISNamespace::FieldType::Unknown, const QString &ControlWidget = QString());

private:
    static void RegisterMetaType(); //Регистрация мета-типов
};
//-----------------------------------------------------------------------------
#endif
