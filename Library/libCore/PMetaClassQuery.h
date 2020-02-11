#pragma once
#ifndef _PMETACLASSQUERY_H_INCLUDED
#define _PMETACLASSQUERY_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClassTable.h"
#include "PMetaClassField.h"
//-----------------------------------------------------------------------------
class PMetaClassQuery : public PMetaClassTable
{
public:
	PMetaClassQuery();
	~PMetaClassQuery();

	void SetFrom(const QString &from);
	QString GetFrom() const;

	void SetWhere(const QString &where);
	QString GetWhere() const;

	void SetOrder(const QString &order);
	QString GetOrder() const;

	void SetOrderType(const QString &order_type);
	QString GetOrderType() const;

	void AddJoin(const QString &join);
	QVectorString GetJoins();

private:
	QString From;
	QString Where;
	QString Order;
	QString OrderType;
	QVectorString Joins;
};
//-----------------------------------------------------------------------------
#endif
