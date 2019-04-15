#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "PMetaClassTable.h"
#include "PMetaClassField.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT PMetaClassQuery : public PMetaClassTable
{
	Q_OBJECT

	Q_PROPERTY(QString From READ GetFrom WRITE SetFrom)
	Q_PROPERTY(QString Where READ GetWhere WRITE SetWhere)
	Q_PROPERTY(QString Order READ GetOrder WRITE SetOrder)
	Q_PROPERTY(QString OrderType READ GetOrderType WRITE SetOrderType)

public:
	PMetaClassQuery(QObject *parent = 0);
	virtual ~PMetaClassQuery();

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
