#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISMetaNotify
{
public:
	ISMetaNotify();
	~ISMetaNotify();

	ISUuid UID;
	QString Name;
	QString SoundFileName;
	QString MessageNotify;
	QString SignalName;
	bool ShowPopup;
};
//-----------------------------------------------------------------------------
