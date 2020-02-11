#pragma once
#ifndef _PMETACLASSTABLE_H_INCLUDED
#define _PMETACLASSTABLE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
#include "PMetaClassField.h"
#include "PMetaClassEscort.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT PMetaClassTable : public PMetaClass
{
public:
	PMetaClassTable();
	PMetaClassTable(const QString &type_object);
	~PMetaClassTable();

	void SetName(const QString &name);
	QString GetName() const;

	void SetUID(const ISUuid &uid);
	ISUuid GetUID() const;

	void SetAlias(const QString &alias); //�������� ���������
	QString GetAlias() const; //�������� ���������

	void SetLocalName(const QString &local_name); //�������� ��������� ������������ � ������������ �����
	QString GetLocalName() const; //�������� ��������� ������������ � ������������ �����

	void SetLocalListName(const QString &local_list_name); //�������� ��������� ������������ � ������������� �����
	QString GetLocalListName() const; //�������� ��������� ������������ � ������������� �����

	void SetTitleName(const QString &title_name);
	QString GetTitleName() const;

	void SetUseRoles(bool use_roles);
	bool GetUseRoles() const;

	void SetClassFilter(const QString &class_filter);
	QString GetClassFilter() const;

	void SetClassFilterField(const QString &class_filter_field);
	QString GetClassFilterField() const;

	void SetObjectForm(const QString &object_form);
	QString GetObjectForm() const;

	void SetShowOnly(bool show_only);
	bool GetShowOnly() const;

	void SetIsSystem(bool is_system);
	bool GetIsSystem() const;

	void SetSqlModel(const QString &sql_model);
	QString GetSqlModel() const;

	void SetParent(const QString &parent);
	QString GetParent() const;

	void SetWhere(const QString &where);
	QString GetWhere() const;

	void SetOrderField(const QString &order_field);
	QString GetOrderField() const;

	void AddField(PMetaClassField *Field); //�������� ���� � ������ ���������������� �����
	void AddSystemField(PMetaClassField *SystemField); //�������� ���� � ������ ��������� �����
	void AddAllField(PMetaClassField *Field); //�������� ���� � ����� ������ ���� �����
	void AddEscort(PMetaClassEscort *EscortObject); //�������� ��������� �������
	void AddVisibleSystemField(PMetaClassField *MetaField); //�������� ������������ ��������� ����
	void AddJoin(const QString &JoinText);
	
	QVector<PMetaClassField*> GetFields(); //�������� ������ ���������������� �����
	QVector<PMetaClassField*> GetSystemFields(); //�������� ������ ��������� �����
	QVector<PMetaClassField*> GetAllFields(); //�������� ������ ���� �����
	QVector<PMetaClassEscort*> GetEscorts(); //�������� ������ ��������� ������
	QVector<PMetaClassField*> GetVisibleSystemFields(); //�������� ������ ������������ ��������� �����
	QVectorString GetJoins();

	PMetaClassField* GetField(const QString &FieldName); //�������� ���� �� �����
	PMetaClassField* GetField(int Index); //�������� ���� �� �������
	PMetaClassField* GetFieldID(); //�������� ���� "���"

	int GetFieldIndex(const QString &FieldName) const; //�������� ������ ���� �� ��� �����

private:
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
