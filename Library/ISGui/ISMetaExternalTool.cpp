#include "StdAfx.h"
#include "ISMetaExternalTool.h"
//-----------------------------------------------------------------------------
ISMetaExternalTool::ISMetaExternalTool(QObject *parent) : QObject(parent)
{

}
//-----------------------------------------------------------------------------
ISMetaExternalTool::~ISMetaExternalTool()
{

}
//-----------------------------------------------------------------------------
void ISMetaExternalTool::SetLocalName(const QString &local_name)
{
	LocalName = local_name;
}
//-----------------------------------------------------------------------------
QString ISMetaExternalTool::GetLocalName() const
{
	return LocalName;
}
//-----------------------------------------------------------------------------
void ISMetaExternalTool::SetCommand(const QString &command)
{
	Command = command;
}
//-----------------------------------------------------------------------------
QString ISMetaExternalTool::GetCommand() const
{
	return Command;
}
//-----------------------------------------------------------------------------
void ISMetaExternalTool::SetUID(const ISUuid &uid)
{
	UID = uid;
}
//-----------------------------------------------------------------------------
ISUuid ISMetaExternalTool::GetUID() const
{
	return UID;
}
//-----------------------------------------------------------------------------
void ISMetaExternalTool::SetIcon(const QIcon &icon)
{
	Icon = icon;
}
//-----------------------------------------------------------------------------
QIcon ISMetaExternalTool::GetIcon() const
{
	return Icon;
}
//-----------------------------------------------------------------------------
