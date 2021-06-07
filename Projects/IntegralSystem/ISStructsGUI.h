#pragma once
#ifndef _ISSTRUCTSGUI_H_INCLUDED
#define _ISSTRUCTSGUI_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISAlgorithm.h"
#include "ISTypedefsGUI.h"
//-----------------------------------------------------------------------------
struct ISCurrentUserInfo
{
    ISCurrentUserInfo() : System(false), ID(0), GroupID(0), GroupFullAccess(false) { }

    bool System; //Системный
    unsigned int ID; //Идентификатор пользователя
    QString FIO; //ФИО
    QString Login; //Логин
    QString Password; //Пароль (временно)

    unsigned int GroupID; //Идентификатор группы в которой состоит пользователь
    bool GroupFullAccess; //Полный доступ
};
//-----------------------------------------------------------------------------
struct ISMetaSetting
{
    ISMetaSetting() : Type(ISNamespace::FieldType::Unknown) { }

    QString UID;
    QString Name;
    ISNamespace::FieldType Type;
    QString WidgetEditName;
    QString LocalName;
    QString Hint;
    QVariant Default;
    QVariant Value;
};
//-----------------------------------------------------------------------------
struct ISMetaSettingsGroup
{
    ISMetaSettingsGroup() : System(true) { }
    ~ISMetaSettingsGroup()
    {
        while (!Settings.empty())
        {
            delete ISAlgorithm::VectorTakeBack(Settings);
        }
    }

    QString Name;
    QString UID;
    QString LocalName;
    QString IconName;
    bool System;
    QString Hint;
    std::vector<ISMetaSetting*> Settings;
};
//-----------------------------------------------------------------------------
struct ISMetaParagraph
{
    QString UID;
    QString Name;
    QString LocalName;
    QString ToolTip;
    QString Icon;
    QString ClassName;
};
//-----------------------------------------------------------------------------
struct ISMetaSubSystem
{
    QString UID; //Идентификатор подсистемы
    QString LocalName; //Локальное имя подсистемы
    QString IconName; //Имя иконки подсистемы
    QString TableName; //Имя таблицы подсистемы
    QString ClassName; //Имя класса подсистемы
    QString Hint; //Подсказка

    QString SystemUID; //Идентификатор системы к которой относится эта подсистема
    QString SystemLocalName; //Имя родительской системы
};
//-----------------------------------------------------------------------------
struct ISMetaSystem
{
    ISMetaSystem() : IsSystem(false) { }

    ~ISMetaSystem()
    {
        while (!SubSystems.empty())
        {
            delete ISAlgorithm::VectorTakeBack(SubSystems);
        }
    }

    bool IsSystem; //Движковая система (если нет - значит пользовательская)
    QString UID; //Идентификатор
    QString LocalName; //Имя системы
    QString IconName; //Имя иконки
    QString Hint; //Всплывающая подсказка
    std::vector<ISMetaSubSystem*> SubSystems; //Список подсистем
};
//-----------------------------------------------------------------------------
struct ISMessageBoxButton
{
    ISMessageBoxButton(int id, const QString& text, bool _default = false, const QIcon& icon = QIcon())
        : ID(id),
        Text(text),
        Default(_default),
        Icon(icon)
    {

    }

    int ID; //Идентификатор кнопки
    QString Text; //Надпись
    bool Default; //На кнопке будет установлен фокус
    QIcon Icon; //Иконка
};
//-----------------------------------------------------------------------------
struct ISModelField
{
    ISModelField() : Index(0), Type(ISNamespace::FieldType::Unknown), IsForeign(false), IsSystem(false) { }

    size_t Index; //Индекс
    QString Name; //Имя
    QString LocalName; //Локальное имя
    ISNamespace::FieldType Type; //Тип
    bool IsForeign; //Флаг внешнего ключа
    bool IsSystem; //Флаг системного поля
};
//-----------------------------------------------------------------------------
struct ISModelRecord
{
    ISModelRecord() : ID(0) { }

    unsigned int ID; //Идентификатор записи
    ISVectorVariantQt Values; //Список значений
};
//-----------------------------------------------------------------------------
#endif
