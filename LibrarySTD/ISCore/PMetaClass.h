#pragma once
#ifndef _PMETACLASS_H_INCLUDED
#define _PMETACLASS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
struct PMetaBase
{
    PMetaBase(const std::string &type_object)
        : TypeObject(type_object.empty() ? "Unknown" : type_object) { }

    std::string TypeObject; //Тип объекта
};
//-----------------------------------------------------------------------------
struct PMetaField : public PMetaBase
{
    PMetaField() : PMetaBase("Field"),
        Type(ISNamespace::FieldType::Unknown),
        Size(0),
        Upper(false),
        Lower(false),
        NotNull(false),
        ReadOnly(false),
        HideFromObject(false),
        HideFromList(false),
        NotSearch(false),
        IsSystem(false),
        //Index(nullptr),
        //Foreign(nullptr),
        Sequence(false),
        PrimaryKey(false)
    {

    }

    std::string UID;
    ISNamespace::FieldType Type; //Тип
    std::string Name; //Название
    int Size; //Размер
    bool Upper; //Только верхний регистр
    bool Lower; //Только нижний регистр
    std::string DefaultValue; //Значение по умолчанию для базы
    std::string DefaultValueWidget; //Значение по умолчанию для виджета
    std::string LabelName; //Имя поля на форме объекта
    std::string LocalListName; //Имя поля в списке
    bool NotNull; //Поле не должно быть пустым
    bool ReadOnly; //Редактирование поля запрещено
    bool HideFromObject; //Поле должно быть скрыто в форме объекта
    bool HideFromList; //Поле не должно учавствовать в запросе
    bool NotSearch; //Участие поля в поиске
    std::string Hint; //Подсказка для поля
    std::string PlaceholderText; //Placeholder текст для виджета
    std::string ControlWidget; //Наименование виджета-редактора значения
    std::string RegExp; //Регулярное выражение для поля
    bool IsSystem; //Статус системного поля

    std::string QueryText; //Текст подзапроса

    //PMetaIndex *Index; //Индекс
    //PMetaForeign *Foreign; //Внешний ключ

    bool Sequence; //Последовательность поля
    bool PrimaryKey; //Первичный ключ
    std::string LayoutName; //Наименование компоновщика (для горизонтального размещения поля)
    std::string SeparatorName; //Наименование вкладки
};
//-----------------------------------------------------------------------------
struct PMetaTable : public PMetaBase
{
    PMetaTable() : PMetaBase("Table")
    {

    }

    std::string TableName;
    std::string UID;
    std::string Alias;
    std::string LocalName;
    std::string LocalListName;

    std::vector<PMetaField*> Fields;
};
//-----------------------------------------------------------------------------
#endif
