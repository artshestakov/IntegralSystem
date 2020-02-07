#include "ISMetaParagraph.h"
//-----------------------------------------------------------------------------
ISMetaParagraph::ISMetaParagraph(const ISUuid &uid, const QString &name, const QString &local_name, const QString &tool_tip, const QString &icon, const QString &class_name, bool default_paragraph)
	: UID(uid),
	Name(name),
	LocalName(local_name),
	ToolTip(tool_tip),
	Icon(icon),
	ClassName(class_name),
	Default(default_paragraph)
{
	
}
//-----------------------------------------------------------------------------
ISMetaParagraph::~ISMetaParagraph()
{

}
//-----------------------------------------------------------------------------
