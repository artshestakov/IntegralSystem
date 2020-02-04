#include "PMetaClassQuery.h"
//-----------------------------------------------------------------------------
PMetaClassQuery::PMetaClassQuery(QObject *parent) : PMetaClassTable(parent)
{
	
}
//-----------------------------------------------------------------------------
PMetaClassQuery::~PMetaClassQuery()
{

}
//-----------------------------------------------------------------------------
void PMetaClassQuery::SetFrom(const QString &from)
{
	From = from;
}
//-----------------------------------------------------------------------------
QString PMetaClassQuery::GetFrom() const
{
	return From;
}
//-----------------------------------------------------------------------------
void PMetaClassQuery::SetWhere(const QString &where)
{
	Where = where;
}
//-----------------------------------------------------------------------------
QString PMetaClassQuery::GetWhere() const
{
	return Where;
}
//-----------------------------------------------------------------------------
void PMetaClassQuery::SetOrder(const QString &order)
{
	Order = order;
}
//-----------------------------------------------------------------------------
QString PMetaClassQuery::GetOrder() const
{
	return Order;
}
//-----------------------------------------------------------------------------
void PMetaClassQuery::SetOrderType(const QString &order_type)
{
	OrderType = order_type;
}
//-----------------------------------------------------------------------------
QString PMetaClassQuery::GetOrderType() const
{
	return OrderType;
}
//-----------------------------------------------------------------------------
void PMetaClassQuery::AddJoin(const QString &join)
{
	Joins.append(join);
}
//-----------------------------------------------------------------------------
QVectorString PMetaClassQuery::GetJoins()
{
	return Joins;
}
//-----------------------------------------------------------------------------
