#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISMetaSetting
{
public:
	ISMetaSetting();
	~ISMetaSetting();

	void SetUID(const ISUuid &uid);
	ISUuid GetUID() const;

	void SetName(const QString &name);
	QString GetName() const;

	void SetType(ISNamespace::FieldType setting_type);
	ISNamespace::FieldType GetType() const;

	void SetWidgetEditName(const QString &widget_edit_name);
	QString GetWidgetEditName() const;

	void SetLocalName(const QString &local_name);
	QString GetLocalName() const;

	void SetHint(const QString &hint);
	QString GetHint() const;

	void SetDefaultValue(const QVariant &default_value);
	QVariant GetDefaultValue() const;

	void SetValue(const QVariant &value);
	QVariant GetValue() const;

private:
	ISUuid UID;
	QString Name;
	ISNamespace::FieldType SettingType;
	QString WidgetEditName;
	QString LocalName;
	QString Hint;
	QVariant DefaultValue;
	QVariant Value;
};
//-----------------------------------------------------------------------------
