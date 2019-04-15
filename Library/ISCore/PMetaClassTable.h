#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "PMetaClass.h"
#include "PMetaClassField.h"
#include "PMetaClassEscort.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT PMetaClassTable : public PMetaClass
{
	Q_OBJECT

	Q_PROPERTY(QString Alias READ GetAlias WRITE SetAlias)
	Q_PROPERTY(QString LocalName READ GetLocalName WRITE SetLocalName)
	Q_PROPERTY(QString LocalListName READ GetLocalListName WRITE SetLocalListName)
	Q_PROPERTY(QString TitleName READ GetTitleName WRITE SetTitleName)
	Q_PROPERTY(bool UseRoles READ GetUseRoles WRITE SetUseRoles)
	Q_PROPERTY(QString ClassFilter READ GetClassFilter WRITE SetClassFilter)
	Q_PROPERTY(QString ClassFilterField READ GetClassFilterField WRITE SetClassFilterField)
	Q_PROPERTY(QString ObjectForm READ GetObjectForm WRITE SetObjectForm)
	Q_PROPERTY(bool ShowOnly READ GetShowOnly WRITE SetShowOnly)
	Q_PROPERTY(bool IsSystem READ GetIsSystem WRITE SetIsSystem)
	Q_PROPERTY(QString SqlModel READ GetSqlModel WRITE SetSqlModel)
	
	Q_PROPERTY(QString Parent READ GetParent WRITE SetParent)
	Q_PROPERTY(QString Where READ GetWhere WRITE SetWhere)
	Q_PROPERTY(QString OrderField READ GetOrderField WRITE SetOrderField)

public:
	PMetaClassTable(QObject *parent = 0);
	virtual ~PMetaClassTable();

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
	QString Alias; //���������
	QString LocalName; //��������� ��� (� ������������ �����)
	QString LocalListName; //��������� ��� (� ������������� �����)
	QString TitleName; //
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
