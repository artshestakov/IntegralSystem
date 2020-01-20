#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISMetaParagraph : QObject
{
	Q_OBJECT

public:
    ISMetaParagraph(const ISUuid &uid, const QString &name, const QString &local_name, const QString &tool_tip, const QString &icon, const QString &class_name, bool default_paragraph, QObject *parent = 0);
	virtual ~ISMetaParagraph();

	ISUuid GetUID() const;
	QString GetName() const;
	QString GetLocalName() const;
	QString GetToolTip() const;
	QString GetIcon() const;
	QString GetClassName() const;
	bool GetDefault() const;

private:
	ISUuid UID;
	QString Name;
	QString LocalName;
	QString ToolTip;
	QString Icon;
	QString ClassName;
	bool Default;
};
//-----------------------------------------------------------------------------
