#pragma once
#ifndef _ISGUI_H_INCLUDED
#define _ISGUI_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISFieldEditBase.h"
#include "ISObjectFormBase.h"
#include "ISComboSearchWidgets.h"
#include "ISImageViewerForm.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISGui
{
public:
	static bool Startup(QString &ErrorString);

	static bool CheckAdminRole(); //Проверка наличия прав администратора
	static bool CheckPressCapsLook(); //Проверить активность кнопки CapsLook
	static QString GetCurrentLayoutName(); //Получить наименование текущей раскладки клавиатуры
	static void RepaintWidget(QWidget *Widget, bool AdjustSize = true); //Перерисовать указанный в параметре виджет
	static void ProcessEvents();
	static void MoveWidgetToDesktop(QWidget *Widget, ISNamespace::MoveWidgetDesktop MovePosition); //Перемещение виджета по рабочему столу
	static void SetWaitGlobalCursor(bool Wait); //Изменить глобальный курсор для ожидания
	static void SetFontWidgetUnderline(QWidget *Widget, bool Underline);
	static void SetFontWidgetItalic(QWidget *Widget, bool Italic);
	static void SetFontWidgetBold(QWidget *Widget, bool Bold);
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
	static bool IsStringUrl(const QString &Url); //Проверить является ли строка ссылкой
	static bool OpenFolder(const QString &FolderPath); //Открыть дикекторию
	static QString ConvertDateTimeToString(const QDateTime &DateTime, const QString &DateFormat, const QString &TimeFormat); //Конвертировать дату и время в строку
	static QString ConvertDateToString(const QDate &Date, const QString &DateFormat); //Конвертировать дату в строку

	static void ChangeUser(); //Смена пользователя
	static void RestartApplication(); //Перезапуск программы
	static void ExitApplication(); //Выход из программы
	static bool DeleteOrRecoveryObject(ISNamespace::DeleteRecoveryObject DeleteOrRecovery, const QString &TableName, const QString &TableAlias, int ID, const QString &LocalListName); //Удалить/Восстановить запись
	static bool DeleteCascadeObject(const QString &TableName, const QString &TableAlias, int ObjectID); //Удалить объект каскадом

	//Календарь
	static int CalendarInsert(const QDateTime &DateTime, const QString &Name, const QVariant &Text, const QString &TableName = QString(), int ObjectID = 0);

	static ISFieldEditBase* CreateColumnForField(QWidget *ParentWidget, PMetaField *MetaField);
	static ISFieldEditBase* CreateColumnForField(QWidget *ParentWidget, ISNamespace::FieldType DataType, const QString &ControlWidget = QString());
	static ISObjectFormBase* CreateObjectForm(ISNamespace::ObjectFormType FormType, const QString &TableName, int ObjectID = 0, QWidget *parent = 0);
	static ISComboSearchBase* CreateSearchOperator(QWidget *parent, ISNamespace::FieldType DataType, PMetaForeign *MetaForeign);

	static int SelectObject(const QString &TableName, int SelectObjectID = 0); //Выбор одной записи из таблицы
	static ISVectorInt SelectObjects(const QString &TableName); //Выбор нескольких записей из таблицы

	static void ShowSettingsForm(const QString &SettingGroupUID = QString()); //Открыть форму настроек
	static bool ShowUserPasswordForm(int UserID); //Открыть форму изменения пароля для пользователя
	static void ShowSystemInfoRecord(PMetaTable *MetaTable, int ObjectID); //Просмотр системной информации о записи
	static void ShowDatabaseSettings(); //Показать настройки базы данных
	static void ShowObjectForm(QWidget *ObjectForm); //Показать форму объекта
	
	static ISImageViewerForm* ShowImageForm(const QPixmap &Pixmap); //Показать изображение
	static ISImageViewerForm* ShowImageForm(const QString &FilePath); //Показать изображение
	static ISImageViewerForm* ShowImageForm(const QByteArray &ByteArray); //Показать изображение

	static void ShowTaskViewForm(int TaskID); //Показать карточку просмотра задачи
	static void ShowTaskObjectForm(ISNamespace::ObjectFormType FormType, int TaskID = 0); //Показать форму объекта задачи

private:
	static ISFieldEditBase* CreateFieldEditBase(QWidget *ParentWidget, PMetaField *MetaField = nullptr, ISNamespace::FieldType DataType = ISNamespace::FT_Unknown, const QString &ControlWidget = QString());
};
//-----------------------------------------------------------------------------
#endif
