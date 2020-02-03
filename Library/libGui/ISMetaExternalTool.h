#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISMetaExternalTool
{
public:
	ISMetaExternalTool();
	~ISMetaExternalTool();

	void SetLocalName(const QString &local_name);
	QString GetLocalName() const;

	void SetCommand(const QString &command);
	QString GetCommand() const;

	void SetUID(const ISUuid &uid);
	ISUuid GetUID() const;

	void SetIcon(const QIcon &icon);
	QIcon GetIcon() const;

private:
	QString LocalName;
	QString Command;
	ISUuid UID;
	QIcon Icon;
};
//-----------------------------------------------------------------------------
