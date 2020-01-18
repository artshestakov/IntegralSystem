#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISMetaSetting.h"
//-----------------------------------------------------------------------------
class ISMetaSettingsGroup : public QObject
{
	Q_OBJECT

public:
	ISMetaSettingsGroup(QObject *parent = 0);
	virtual ~ISMetaSettingsGroup();

	void SetName(const QString &name);
	QString GetName() const;

	void SetUID(const ISUuid &uid);
	ISUuid GetUID() const;

	void SetLocalName(const QString &local_name);
	QString GetLocalName() const;

	void SetIconName(const QString &icon_name);
	QString GetIconName() const;

	void SetSystem(bool system);
	bool GetSystem() const;

	void SetHint(const QString &hint);
	QString GetHint() const;

	void AppendSetting(ISMetaSetting *Setting);
	QVector<ISMetaSetting*> GetSettings();

private:
	QString Name;
	ISUuid UID;
	QString LocalName;
	QString IconName;
	bool System;
	QString Hint;
	QVector<ISMetaSetting*> Settings;
};
//-----------------------------------------------------------------------------
