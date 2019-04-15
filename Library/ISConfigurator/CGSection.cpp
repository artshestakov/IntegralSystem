#include "StdAfx.h"
#include "CGSection.h"
//-----------------------------------------------------------------------------
CGSection::CGSection(QObject *parent) : QObject(parent)
{

}
//-----------------------------------------------------------------------------
CGSection::~CGSection()
{

}
//-----------------------------------------------------------------------------
void CGSection::SetName(const QString &name)
{
	Name = name;
}
//-----------------------------------------------------------------------------
QString CGSection::GetName() const
{
	return Name;
}
//-----------------------------------------------------------------------------
void CGSection::SetClassName(const QString &class_name)
{
	ClassName = class_name;
}
//-----------------------------------------------------------------------------
QString CGSection::GetClassName() const
{
	return ClassName;
}
//-----------------------------------------------------------------------------
void CGSection::AddItem(CGSectionItem *SectionItem)
{
	Items.append(SectionItem);
}
//-----------------------------------------------------------------------------
QVector<CGSectionItem*> CGSection::GetItems()
{
	return Items;
}
//-----------------------------------------------------------------------------
