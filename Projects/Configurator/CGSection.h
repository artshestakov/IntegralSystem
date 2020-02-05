#pragma once
#ifndef _CGSECTION_H_INCLUDED
#define _CGSECTION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "CGSectionItem.h"
//-----------------------------------------------------------------------------
class CGSection
{
public:
	CGSection();
	~CGSection();

	void SetName(const QString &name);
	QString GetName() const;

	void SetClassName(const QString &class_name);
	QString GetClassName() const;

	void AddItem(CGSectionItem *SectionItem);
	QVector<CGSectionItem*> GetItems();

private:
	QString Name;
	QString ClassName;
	QVector<CGSectionItem*> Items;
};
//-----------------------------------------------------------------------------
#endif
