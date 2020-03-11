#pragma once
#ifndef _PMETACLASS_H_INCLUDED
#define _PMETACLASS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISUuid.h"
#include "ISNamespace.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
struct PMetaClass
{
	PMetaClass(const QString &type_object) : TypeObject(type_object.isEmpty() ? "Unknown" : type_object) { }

	QString TypeObject; //Тип объекта
};
//-----------------------------------------------------------------------------
struct PMetaClassIndex : public PMetaClass
{
	PMetaClassIndex(bool unique, const QString &alias, const QString &table_name, const QString &field_name) 
		: PMetaClass("Index"), Unique(unique), Alias(alias), TableName(table_name), FieldName(field_name) { }

	bool Unique;
	QString Alias;
	QString TableName;
	QString FieldName;
	ISVectorString Fields;
};
//-----------------------------------------------------------------------------
struct PMetaClassForeign : public PMetaClass
{
	PMetaClassForeign() : PMetaClass("Foreign") { }

	QString Field; //Поле, на которое устанавливается внешний ключ
	QString ForeignClass; //На какую таблицу ссылкается внешний ключ
	QString ForeignField; //На какое поле ссылкается внешний ключ
	QString ForeignViewNameField; //Какое поле (поля) отображать в запросе на выборку
	QString OrderField; //Поле по которому будет происходить сортировка

	QString TableName; //Таблица, содержащая поле, на которое устанавливается внешний ключ
	QString SqlQuery; //Запрос на выбору по внешнему ключу
};
//-----------------------------------------------------------------------------
struct PMetaClassEscort : public PMetaClass
{
	PMetaClassEscort() : PMetaClass("Escort") { }

	QString LocalName;
	QString TableName;
	QString ClassName;
	QString ClassFilter;
};
//-----------------------------------------------------------------------------
struct PMetaClassField : public PMetaClass
{
	PMetaClassField() : PMetaClass("Field"),
		Sequence(false),
		Type(ISNamespace::FT_Unknown),
		Size(0),
		Upper(false),
		Lower(false),
		NotNull(false),
		ReadOnly(false),
		HideFromObject(false),
		HideFromList(false),
		NotSearch(false),
		IsSystem(false),
		Index(nullptr),
		Foreign(nullptr)
	{

	}

	bool IsFieldID() const { return Name.toLower() == "id"; }
	bool IsFieldUID() const { return Name.toLower() == "uid"; }

	ISUuid UID;
	ISNamespace::FieldType Type; //Тип
	QString Name; //Название
	int Size; //Размер
	bool Upper; //Только верхний регистр
	bool Lower; //Только нижний регистр
	QVariant DefaultValue; //Значение по умолчанию для базы
	QVariant DefaultValueWidget; //Значение по умолчанию для виджета
	QString LabelName; //Имя поля на форме объекта
	QString LocalListName; //Имя поля в списке
	bool NotNull; //Поле не должно быть пустым
	bool ReadOnly; //Редактирование поля запрещено
	bool HideFromObject; //Поле должно быть скрыто в форме объекта
	bool HideFromList; //Поле не должно учавствовать в запросе
	bool NotSearch; //Участие поля в поиске
	QString Hint; //Подсказка для поля
	QString PlaceholderText; //Placeholder текст для виджета
	QString ControlWidget; //Наименование виджета-редактора значения
	QString RegExp; //Регулярное выражение для поля
	bool IsSystem; //Статус системного поля

	QString QueryText; //Текст подзапроса

	PMetaClassIndex *Index; //Индекс
	PMetaClassForeign *Foreign; //Внешний ключ

	bool Sequence; //Последовательность поля
	QString LayoutName; //Наименование компоновщика (для горизонтального размещения поля)
	QString SeparatorName; //Наименование вкладки
};
//-----------------------------------------------------------------------------
struct PMetaClassTable : public PMetaClass
{
	PMetaClassTable() : PMetaClass("Table"), UseRoles(true), ShowOnly(false), IsSystem(false) { }
	PMetaClassTable(const QString &type_object) : PMetaClass(type_object), UseRoles(true), ShowOnly(false), IsSystem(false) { }

	PMetaClassField* GetField(const QString &FieldName) //Получить поле по имени
	{
		for (PMetaClassField *MetaField : AllFields)
		{
			if (MetaField->Name.toLower() == FieldName.toLower())
			{
				return MetaField;
			}
		}
		return nullptr;
	}

	PMetaClassField* GetField(int Index) //Получить поле по индексу
	{
		if (!AllFields.empty())
		{
			return AllFields[Index];
		}
		return nullptr;
	}

	PMetaClassField* GetFieldID() //Получить поле "Код"
	{
		if (!SystemFields.empty())
		{
			return SystemFields.front();
		}
		return nullptr;
	}

	int GetFieldIndex(const QString &FieldName) const //Получить индекс поля по его имени
	{
		for (int i = 0, c = AllFields.size(); i < c; ++i)
		{
			PMetaClassField *MetaField = AllFields[i];
			if (MetaField->Name.toLower() == FieldName.toLower())
			{
				return i;
			}
		}
		return -1;
	}

	bool ContainsField(const QString &FieldName)
	{
		for (PMetaClassField *MetaField : AllFields)
		{
			if (MetaField->Name.toLower() == FieldName.toLower())
			{
				return true;
			}
		}
		return false;
	}

	QString Name; //Название таблицы
	ISUuid UID; //Идентификатор
	QString Alias; //Псевдоним
	QString LocalName; //Локальное имя (в единственном числе)
	QString LocalListName; //Локальное имя (в множественном числе)
	QString TitleName;
	bool UseRoles;
	QString ClassFilter; //Фильтр таблицы
	QString ClassFilterField;
	QString ObjectForm; //Наименование класса формы объекта
	bool ShowOnly; //Только просмотр таблицы
	bool IsSystem;

	QString SqlModel; //Наименование класса модели

	QString Parent;
	QString Where;
	QString OrderField;

	std::vector<PMetaClassEscort*> Escorts; //Эскортные таблицы
	std::vector<PMetaClassField*> Fields; //Поля
	std::vector<PMetaClassField*> SystemFields; //Системные поля
	std::vector<PMetaClassField*> AllFields; //Все поля

	std::vector<PMetaClassField*> SystemFieldsVisible; //Отображаемые системные поля
	ISVectorString Joins;
};
//-----------------------------------------------------------------------------
struct PMetaClassFunction : public PMetaClass
{
	PMetaClassFunction() : PMetaClass("Function") { }

	QString Name; //Имя функции
	QString Text; //Текст функции
};
//-----------------------------------------------------------------------------
struct PMetaClassQuery : public PMetaClassTable
{
	PMetaClassQuery() : PMetaClassTable("Query") { }

	QString From;
	QString Where;
	QString Order;
	QString OrderType;
	ISVectorString Joins;
};
//-----------------------------------------------------------------------------
struct PMetaClassResource : public PMetaClass
{
	PMetaClassResource() : PMetaClass("Resource") { }

	void AddField(const QString &FieldName, const QString &Value) //Добавить параметр и его значение в ресурс
	{
		if (FieldName.toLower() != "uid")
		{
			Parameters.emplace(FieldName, Value);
		}
	}

	QString TableName;
	ISUuid UID;
	ISStringMap Parameters;
};
//-----------------------------------------------------------------------------
#endif
