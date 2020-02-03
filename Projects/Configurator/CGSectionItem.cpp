#include "CGSectionItem.h"
//-----------------------------------------------------------------------------
CGSectionItem::CGSectionItem()
{

}
//-----------------------------------------------------------------------------
CGSectionItem::~CGSectionItem()
{

}
//-----------------------------------------------------------------------------
void CGSectionItem::SetFunction(const QString &function)
{
	Function = function;
}
//-----------------------------------------------------------------------------
QString CGSectionItem::GetFunction() const
{
	return Function;
}
//-----------------------------------------------------------------------------
void CGSectionItem::SetLocalName(const QString &local_name)
{
	LocalName = local_name;
}
//-----------------------------------------------------------------------------
QString CGSectionItem::GetLocalName() const
{
	return LocalName;
}
//-----------------------------------------------------------------------------
void CGSectionItem::SetDescription(const QString &description)
{
	Description = description;
}
//-----------------------------------------------------------------------------
QString CGSectionItem::GetDescription() const
{
	return Description;
}
//-----------------------------------------------------------------------------
