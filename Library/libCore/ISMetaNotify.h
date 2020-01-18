#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISMetaNotify : public QObject
{
	Q_OBJECT

public:
	ISMetaNotify(QObject *parent);
	virtual ~ISMetaNotify();

	void SetUID(const ISUuid &uid);
	ISUuid GetUID() const;

	void SetName(const QString &name);
	QString GetName() const;

	void SetSoundFileName(const QString &sourd_file_name);
	QString GetSoundFileName() const;

	void SetMessage(const QString &message);
	QString GetMessage() const;

	void SetSignalName(const QString &signal_name);
	QString GetSignalName() const;

	void SetShowPopup(bool show_popup);
	bool GetShowPopup() const;

private:
	ISUuid UID;
	QString Name;
	QString SoundFileName;
	QString Message;
	QString SignalName;
	bool ShowPopup;
};
//-----------------------------------------------------------------------------
