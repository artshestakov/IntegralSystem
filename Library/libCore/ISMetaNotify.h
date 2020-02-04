#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISMetaNotify
{
public:
	ISMetaNotify();
	virtual ~ISMetaNotify();

	void SetUID(const ISUuid &uid);
	ISUuid GetUID() const;

	void SetName(const QString &name);
	QString GetName() const;

	void SetSoundFileName(const QString &sourd_file_name);
	QString GetSoundFileName() const;

	void SetMessageNotify(const QString &message_notify);
	QString GetMessageNotify() const;

	void SetSignalName(const QString &signal_name);
	QString GetSignalName() const;

	void SetShowPopup(bool show_popup);
	bool GetShowPopup() const;

private:
	ISUuid UID;
	QString Name;
	QString SoundFileName;
	QString MessageNotify;
	QString SignalName;
	bool ShowPopup;
};
//-----------------------------------------------------------------------------
