#pragma once
#ifndef _PMETACLASSFIELD_H_INCLUDED
#define _PMETACLASSFIELD_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
#include "ISNamespace.h"
#include "PMetaClassIndex.h"
#include "PMetaClassForeign.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT PMetaClassField : public PMetaClass
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

	bool IsFieldID() const
	{
		return Name.toLower() == "id";
	}

	bool IsFieldUID() const
	{
		return Name.toLower() == "uid";
	}

	ISUuid UID;
	ISNamespace::FieldType Type; //Тип
	QString Name; //Название
	int Size; //Размер
	bool Upper; //Только верхний регистр
	bool Lower; //Только нижний регистр
	QVariant DefaultValue; //Значение по умолчанию для базы
	QVariant DefaultValueWidget; //Значение по умолчанию
	QString LabelName; //Имя поля на форме объекта
	QString LocalListName; //Имя поля в списке
	bool NotNull; //Поле не должно быть пустым
	bool ReadOnly; //Редактирование поля запрещено
	bool HideFromObject; //Поле должно быть скрыто в форме объекта
	bool HideFromList; //Поле не должно учавствовать в запросе
	bool NotSearch; //Участие поля в поиске
	QString Hint; //Подсказка для поля
	QString PlaceholderText;
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
#endif
