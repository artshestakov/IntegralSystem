#pragma once
//-----------------------------------------------------------------------------
#include "isconfigurator_global.h"
#include "CGSectionItem.h"
//-----------------------------------------------------------------------------
class ISCONFIGURATOR_EXPORT CGSection : public QObject
{
	Q_OBJECT

public:
	CGSection(QObject *parent = 0);
	virtual ~CGSection();

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
