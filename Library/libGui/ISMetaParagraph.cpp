#include "StdAfx.h"
#include "ISMetaParagraph.h"
//-----------------------------------------------------------------------------
ISMetaParagraph::ISMetaParagraph(const ISUuid &uid, const QString &name, const QString &local_name, const QString &tool_tip, const QString &icon, const QString &class_name, bool default_paragraph, QObject *parent) : QObject(parent)
{
	UID = uid; 
	Name = name;
	LocalName = local_name;
	ToolTip = tool_tip;
	Icon = icon;
	ClassName = class_name;
    Default = default_paragraph;
}
//-----------------------------------------------------------------------------
ISMetaParagraph::~ISMetaParagraph()
{

}
//-----------------------------------------------------------------------------
ISUuid ISMetaParagraph::GetUID() const
{
	return UID;
}
//-----------------------------------------------------------------------------
QString ISMetaParagraph::GetName() const
{
	return Name;
}
//-----------------------------------------------------------------------------
QString ISMetaParagraph::GetLocalName() const
{
	return LocalName;
}
//-----------------------------------------------------------------------------
QString ISMetaParagraph::GetToolTip() const
{
	return ToolTip;
}
//-----------------------------------------------------------------------------
QString ISMetaParagraph::GetIcon() const
{
	return Icon;
}
//-----------------------------------------------------------------------------
QString ISMetaParagraph::GetClassName() const
{
	return ClassName;
}
//-----------------------------------------------------------------------------
bool ISMetaParagraph::GetDefault() const
{
	return Default;
}
//-----------------------------------------------------------------------------
