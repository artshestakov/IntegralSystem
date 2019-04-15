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

	void SetAlias(const QString &alias); //Изменить псевдоним
	QString GetAlias() const; //Получить псевдоним

	void SetLocalName(const QString &local_name); //Изменить локальное наименование в единственном числе
	QString GetLocalName() const; //Получить локальное наименование в единственном числе

	void SetLocalListName(const QString &local_list_name); //Изменить локальное наименование в множественном числе
	QString GetLocalListName() const; //Получить локальное наименование в множественном числе

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

	void AddField(PMetaClassField *Field); //Добавить поле в список пользовательских полей
	void AddSystemField(PMetaClassField *SystemField); //Добавить поле в список системных полей
	void AddAllField(PMetaClassField *Field); //Добавить поле в общий список всех полей
	void AddEscort(PMetaClassEscort *EscortObject); //Добавить эскортную таблицу
	void AddVisibleSystemField(PMetaClassField *MetaField); //Добавить отображаемое системное поле
	void AddJoin(const QString &JoinText);
	
	QVector<PMetaClassField*> GetFields(); //Получить список пользовательских полей
	QVector<PMetaClassField*> GetSystemFields(); //Получить список системных полей
	QVector<PMetaClassField*> GetAllFields(); //Получить список всех полей
	QVector<PMetaClassEscort*> GetEscorts(); //Получить список эскортных таблиц
	QVector<PMetaClassField*> GetVisibleSystemFields(); //Получить список отображаемых системных полей
	QVectorString GetJoins();

	PMetaClassField* GetField(const QString &FieldName); //Получить поле по имени
	PMetaClassField* GetField(int Index); //Получить поле по индексу
	PMetaClassField* GetFieldID(); //Получить поле "Код"

	int GetFieldIndex(const QString &FieldName) const; //Получить индекс поля по его имени

private:
	QString Alias; //Псевдоним
	QString LocalName; //Локальное имя (в единственном числе)
	QString LocalListName; //Локальное имя (в множественном числе)
	QString TitleName; //
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
