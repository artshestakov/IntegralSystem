#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISMetaExternalTool : public QObject
{
	Q_OBJECT

public:
	ISMetaExternalTool(QObject *parent = 0);
	virtual ~ISMetaExternalTool();

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
