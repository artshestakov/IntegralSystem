#pragma once
#ifndef _PMETACLASSTABLE_H_INCLUDED
#define _PMETACLASSTABLE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
#include "PMetaClassField.h"
#include "PMetaClassEscort.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT PMetaClassTable : public PMetaClass
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
		if (!AllFields.isEmpty())
		{
			return AllFields[Index];
		}
		return nullptr;
	}

	PMetaClassField* GetFieldID() //Получить поле "Код"
	{
		if (!SystemFields.isEmpty())
		{
			return SystemFields.front();
		}
		return nullptr;
	}

	int GetFieldIndex(const QString &FieldName) const //Получить индекс поля по его имени
	{
		for (int i = 0; i < AllFields.count(); ++i)
		{
			PMetaClassField *MetaField = AllFields.at(i);
			if (MetaField->Name == FieldName)
			{
				return i;
			}
		}
		return -1;
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

	QVector<PMetaClassEscort*> Escorts; //Эскортные таблицы
	QVector<PMetaClassField*> Fields; //Поля
	QVector<PMetaClassField*> SystemFields; //Системные поля
	QVector<PMetaClassField*> AllFields; //Все поля

	QVector<PMetaClassField*> SystemFieldsVisible; //Отображаемые системные поля
	QVectorString Joins;
};
//-----------------------------------------------------------------------------
#endif
