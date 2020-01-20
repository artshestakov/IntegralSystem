#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISMetaSubSystem : public QObject
{
	Q_OBJECT

public:
	ISMetaSubSystem(QObject *parent = 0);
	virtual ~ISMetaSubSystem();

	void SetID(int id);
	int GetID() const;

	void SetUID(const ISUuid &uid);
	ISUuid GetUID() const;

	void SetLocalName(const QString &local_name);
	QString GetLocalName() const;

	void SetOrderID(int order_id);
	int GetOrderID() const;

	void SetIconName(const QString &icon_name);
	QString GetIconName() const;

	void SetTableName(const QString &table_name);
	QString GetTableName() const;

	void SetClassName(const QString &class_name);
	QString GetClassName() const;

	void SetHint(const QString &hint);
	QString GetHint() const;

	void SetSystemUID(const QString &system_uid);
	QString GetSystemUID() const;

	void SetSystemID(int system_id);
	int GetSystemID() const;

	void SetSystemLocalName(const QString &system_local_name);
	QString GetSystemLocalName() const;

private:
	int ID; //Идентификатор подсистемы
	ISUuid UID; //Идентификатор подсистемы
	QString LocalName; //Локальное имя подсистемы
	int OrderID; //Порядок подсистемы
	QString IconName; //Имя иконки подсистемы
	QString TableName; //Имя таблицы подсистемы
	QString ClassName; //Имя класса подсистемы
	QString Hint; //Подсказка

	QString SystemUID; //Идентификатор системы к которой относится эта подсистема
	int SystemID; //Идентификатор родительской системы
	QString SystemLocalName; //Имя родительской системы
};
//-----------------------------------------------------------------------------
