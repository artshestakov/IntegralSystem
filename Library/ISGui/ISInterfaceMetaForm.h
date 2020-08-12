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

	QString GetParentFilterField() const;
	void SetParentFilterField(const QString &parent_filter_field);

private:
	QString ParentTableName;
	int ParentObjectID;
	QString ParentFilterField;
};
//-----------------------------------------------------------------------------
