#include "ISMetaNotify.h"
//-----------------------------------------------------------------------------
ISMetaNotify::ISMetaNotify()
	: ShowPopup(true)
{
	
}
//-----------------------------------------------------------------------------
ISMetaNotify::~ISMetaNotify()
{

}
//-----------------------------------------------------------------------------
void ISMetaNotify::SetUID(const ISUuid &uid)
{
	UID = uid;
}
//-----------------------------------------------------------------------------
ISUuid ISMetaNotify::GetUID() const
{
	return UID;
}
//-----------------------------------------------------------------------------
void ISMetaNotify::SetName(const QString &name)
{
	Name = name;
}
//-----------------------------------------------------------------------------
QString ISMetaNotify::GetName() const
{
	return Name;
}
//-----------------------------------------------------------------------------
void ISMetaNotify::SetSoundFileName(const QString &sound_file_name)
{
	SoundFileName = sound_file_name;
}
//-----------------------------------------------------------------------------
QString ISMetaNotify::GetSoundFileName() const
{
	return SoundFileName;
}
//-----------------------------------------------------------------------------
void ISMetaNotify::SetMessageNotify(const QString &message_notify)
{
	MessageNotify = message_notify;
}
//-----------------------------------------------------------------------------
QString ISMetaNotify::GetMessageNotify() const
{
	return MessageNotify;
}
//-----------------------------------------------------------------------------
void ISMetaNotify::SetSignalName(const QString &signal_name)
{
	SignalName = signal_name;
}
//-----------------------------------------------------------------------------
QString ISMetaNotify::GetSignalName() const
{
	return SignalName;
}
//-----------------------------------------------------------------------------
void ISMetaNotify::SetShowPopup(bool show_popup)
{
	ShowPopup = show_popup;
}
//-----------------------------------------------------------------------------
bool ISMetaNotify::GetShowPopup() const
{
	return ShowPopup;
}
//-----------------------------------------------------------------------------
