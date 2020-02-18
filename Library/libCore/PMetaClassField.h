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
	ISNamespace::FieldType Type; //���
	QString Name; //��������
	int Size; //������
	bool Upper; //������ ������� �������
	bool Lower; //������ ������ �������
	QVariant DefaultValue; //�������� �� ��������� ��� ����
	QVariant DefaultValueWidget; //�������� �� ���������
	QString LabelName; //��� ���� �� ����� �������
	QString LocalListName; //��� ���� � ������
	bool NotNull; //���� �� ������ ���� ������
	bool ReadOnly; //�������������� ���� ���������
	bool HideFromObject; //���� ������ ���� ������ � ����� �������
	bool HideFromList; //���� �� ������ ������������ � �������
	bool NotSearch; //������� ���� � ������
	QString Hint; //��������� ��� ����
	QString PlaceholderText;
	QString ControlWidget; //������������ �������-��������� ��������
	QString RegExp; //���������� ��������� ��� ����
	bool IsSystem; //������ ���������� ����

	QString QueryText; //����� ����������

	PMetaClassIndex *Index; //������
	PMetaClassForeign *Foreign; //������� ����

	bool Sequence; //������������������ ����
	QString LayoutName; //������������ ������������ (��� ��������������� ���������� ����)
	QString SeparatorName; //������������ �������
};
//-----------------------------------------------------------------------------
#endif
