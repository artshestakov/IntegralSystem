#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISInterfaceMetaForm : public ISInterfaceForm
{
	Q_OBJECT

signals:
	void AddFormFromTab(QWidget *ObjectForm);

public:
	ISInterfaceMetaForm(QWidget *parent = 0);
	virtual ~ISInterfaceMetaForm();

	virtual void LoadData() = 0;

	QString GetParentTableName() const;
	void SetParentTableName(const QString &table_name);

	int GetParentObjectID() const;
	void SetParentObjectID(int parent_object_id);

	void SetUID(const QString &sub_system_uid); //Изменить идентификатор подсистемы/таблицы
	ISUuid GetUID() const; //Получить идентификатор подсистемы/таблицы

private:
	QString ParentTableName;
	int ParentObjectID;
	ISUuid UID; //Идентификатор подсистемы/таблицы
};
//-----------------------------------------------------------------------------
