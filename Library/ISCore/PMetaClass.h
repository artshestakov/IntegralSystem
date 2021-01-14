#pragma once
#ifndef _PMETACLASS_H_INCLUDED
#define _PMETACLASS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISUuid.h"
#include "ISNamespace.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
struct PMetaBase
{
	PMetaBase(const QString &type_object) : TypeObject(type_object.isEmpty() ? "Unknown" : type_object) { }

	QString TypeObject; //Тип объекта
};
//-----------------------------------------------------------------------------
struct PMetaIndex : public PMetaBase
{
	PMetaIndex(bool unique, const QString &alias, const QString &table_name, const QString &field_name) 
		: PMetaBase("Index"),
		Unique(unique), Alias(alias), TableName(table_name), FieldName(field_name) { }

	QString GetName() //Получить имя индекса
	{
		QString IndexName;
		if (!Fields.empty()) //Если индекс составной
		{
			IndexName += TableName + '_';
			for (const QString &String : Fields)
			{
				IndexName += String + '_';
			}
			IndexName.chop(1);
		}
		else
		{
			IndexName = TableName + '_' + FieldName;
		}
		return IndexName.toLower();
	}

	bool Unique;
	QString Alias;
	QString TableName;
	QString FieldName;
	ISVectorString Fields;
};
//-----------------------------------------------------------------------------
struct PMetaForeign : public PMetaBase
{
	PMetaForeign(const QString &field, const QString &foreign_class, const QString &foreign_field, const QString &foreign_view_name_field, const QString &table_name = QString())
		: PMetaBase("Foreign"),
		Field(field), ForeignClass(foreign_class), ForeignField(foreign_field), ForeignViewNameField(foreign_view_name_field), TableName(table_name) { }

	QString GetName() const //Получить имя внешнего ключа
	{
		return (TableName + '_' + Field).toLower();
	}

	QString Field; //Поле, на которое устанавливается внешний ключ
	QString ForeignClass; //На какую таблицу ссылкается внешний ключ
	QString ForeignField; //На какое поле ссылается внешний ключ
	QString ForeignViewNameField; //Какое поле (поля) отображать в запросе на выборку

	QString TableName; //Таблица, содержащая поле, на которое устанавливается внешний ключ
};
//-----------------------------------------------------------------------------
struct PMetaEscort : public PMetaBase
{
	PMetaEscort() : PMetaBase("Escort") { }

	QString LocalName;
	QString TableName;
	QString FilterField;
	QString ClassName;
};
//-----------------------------------------------------------------------------
struct PMetaField : public PMetaBase
{
	PMetaField() : PMetaBase("Field"),
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
        Foreign(nullptr),
        Sequence(false),
		PrimaryKey(false)
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

	PMetaIndex *Index; //Индекс
	PMetaForeign *Foreign; //Внешний ключ

	bool Sequence; //Последовательность поля
	bool PrimaryKey; //Первичный ключ
	QString LayoutName; //Наименование компоновщика (для горизонтального размещения поля)
	QString SeparatorName; //Наименование вкладки
};
//-----------------------------------------------------------------------------
struct PMetaTable : public PMetaBase
{
	PMetaTable() : PMetaBase("Table"), UseRoles(true), ShowOnly(false), IsSystem(false) { }
	PMetaTable(const QString &type_object) : PMetaBase(type_object), UseRoles(true), ShowOnly(false), IsSystem(false) { }

	PMetaField* GetField(const QString &FieldName) //Получить поле по имени
	{
		for (PMetaField *MetaField : Fields)
		{
			if (MetaField->Name.toLower() == FieldName.toLower())
			{
				return MetaField;
			}
		}
		return nullptr;
	}

	PMetaField* GetField(int Index) //Получить поле по индексу
	{
		if (!Fields.empty())
		{
			return Fields[Index];
		}
		return nullptr;
	}

	PMetaField* GetFieldID() //Получить поле "Код"
	{
		if (!Fields.empty())
		{
			return Fields.front();
		}
		return nullptr;
	}

	int GetFieldIndex(const QString &FieldName) const //Получить индекс поля по его имени
	{
		for (size_t i = 0, c = Fields.size(); i < c; ++i)
		{
			PMetaField *MetaField = Fields[i];
			if (MetaField->Name.toLower() == FieldName.toLower())
			{
				return (int)i;
			}
		}
		return -1;
	}

	bool ContainsField(const QString &FieldName)
	{
		for (PMetaField *MetaField : Fields)
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
	QString ObjectForm; //Наименование класса формы объекта
	bool ShowOnly; //Только просмотр таблицы
	bool IsSystem;

	std::vector<PMetaEscort*> Escorts; //Эскортные таблицы
	std::vector<PMetaField*> Fields; //Поля
	std::vector<PMetaIndex*> IndexesCompound; //Составные индексы таблицы
};
//-----------------------------------------------------------------------------
struct PMetaFunction : public PMetaBase
{
	PMetaFunction() : PMetaBase("Function") { }

	QString Name; //Имя функции
	QString Comment; //Комментарий функции
	QString Text; //Текст функции
};
//-----------------------------------------------------------------------------
struct PMetaResource : public PMetaBase
{
	PMetaResource() : PMetaBase("Resource") { }

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
