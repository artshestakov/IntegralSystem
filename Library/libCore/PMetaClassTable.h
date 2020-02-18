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
	
	PMetaClassField* GetField(const QString &FieldName) //�������� ���� �� �����
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

	PMetaClassField* GetField(int Index) //�������� ���� �� �������
	{
		if (!AllFields.isEmpty())
		{
			return AllFields[Index];
		}
		return nullptr;
	}

	PMetaClassField* GetFieldID() //�������� ���� "���"
	{
		if (!SystemFields.isEmpty())
		{
			return SystemFields.front();
		}
		return nullptr;
	}

	int GetFieldIndex(const QString &FieldName) const //�������� ������ ���� �� ��� �����
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

	QString Name; //�������� �������
	ISUuid UID; //�������������
	QString Alias; //���������
	QString LocalName; //��������� ��� (� ������������ �����)
	QString LocalListName; //��������� ��� (� ������������� �����)
	QString TitleName;
	bool UseRoles;
	QString ClassFilter; //������ �������
	QString ClassFilterField;
	QString ObjectForm; //������������ ������ ����� �������
	bool ShowOnly; //������ �������� �������
	bool IsSystem;

	QString SqlModel; //������������ ������ ������
	
	QString Parent;
	QString Where;
	QString OrderField;

	QVector<PMetaClassEscort*> Escorts; //��������� �������
	QVector<PMetaClassField*> Fields; //����
	QVector<PMetaClassField*> SystemFields; //��������� ����
	QVector<PMetaClassField*> AllFields; //��� ����

	QVector<PMetaClassField*> SystemFieldsVisible; //������������ ��������� ����
	QVectorString Joins;
};
//-----------------------------------------------------------------------------
#endif
